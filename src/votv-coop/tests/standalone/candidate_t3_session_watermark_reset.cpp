// candidate_t3 -- a reused Session must not open holding the dead session's receive watermarks.
//
// What this test IS: a single translation unit that drives the REAL session-boundary policy
// (coop/net/session_watermark_reset.h, which src/coop/net/session_status.cpp calls for every
// watermark ResetPeerRemoteState does not own) through the newest-wins gate that the receive
// stores implement, and asserts what the next session accepts.
//
// What this test IS NOT: it does not instantiate coop::net::Session. Session's receive path drags
// GameNetworkingSockets and the ue_wrap engine layer, which no single TU can compile. The gate
// below is a TRANSCRIPTION of session_streams.cpp:201-207, quoted in Store() so a reviewer can
// diff it against the source, not a call into it. The GAP that leaves is stated here rather
// than hidden: this proves the POLICY and the
// ARITHMETIC, not that the shipped binary wires them together.
//
// Both arms are built from this one source. The fixed arm compiles against the include tree as
// shipped; the pre-fix arm compiles against a copy with coop/net/session_watermark_reset.h removed,
// so __has_include selects the pinned pre-fix policy below. The pre-fix arm MUST fail.

#if __has_include("coop/net/session_watermark_reset.h")
#include "coop/net/session_watermark_reset.h"
#else
#include <cstdint>
namespace coop::net {
// The pinned PRE-FIX policy, retained only so this same source can prove its pre-fix negative.
// It is the seam's behaviour as measured on the tree before the fix: NOTHING ran at the session
// boundary. Session::Start (session_start.cpp:98) cleared expectedEpoch_, peerGenBySlot_ and the
// hasLocal* publish flags and stopped there; Session::Stop (session_start.cpp:366) called no reset
// at all; and the only clearer in the tree, Session::ResetPeerRemoteState
// (session_status.cpp:255), was reachable only from a per-connection close edge -- which a Stop
// route never reaches, because Stop closes every peer itself and exchanges peerConns_[i] to 0
// before doing so. So a reused Session opened holding every watermark the dead session left.
inline void ResetWatermarkForNewSession(bool&, uint32_t&) {}
}  // namespace coop::net
#endif

#include <cstdint>
#include <cstdio>

namespace {

// One receive watermark, in the shape Session holds it: parallel `has` and `lastSeq` per stream
// (session.h:628-629 for the player pose lane, and eleven siblings).
struct Watermark {
    bool has = false;
    uint32_t lastSeq = 0;
};

// The receive store's newest-wins gate, transcribed from session_streams.cpp:201-207:
//
//     if (hasRemote_[routeSlot] &&
//         static_cast<int32_t>(seq - lastRemoteSeq_[routeSlot]) <= 0) {
//         break;  // stale/duplicate for this origin slot
//     }
//     remotePoses_[routeSlot] = pkt.pose;
//     lastRemoteSeq_[routeSlot] = seq;
//     hasRemote_[routeSlot] = true;
//
// Returns true when the packet is ACCEPTED (the payload is stored and, on a host, billed to
// movement_ledger::OnClientPose, which is why a stale drop leaves no ledger row).
bool Store(Watermark& w, uint32_t seq) {
    if (w.has && static_cast<int32_t>(seq - w.lastSeq) <= 0) return false;  // stale/duplicate
    w.lastSeq = seq;
    w.has = true;
    return true;
}

// The session boundary: what Session::Start does to this watermark through
// Session::ResetRemoteStreamStateForNewSession.
void SessionStart(Watermark& w) {
    coop::net::ResetWatermarkForNewSession(w.has, w.lastSeq);
}

// The field numbers, so the arithmetic under test is the arithmetic that happened.
// Client session B's own send counter at its last log line before the tear
// (the client log, "net stats: state=2 sent=344223" at 00:03:31); the host's matching
// recv=344324 at 00:03:34. The session-C client was a FRESH PROCESS, so its sendSeq_
// (session.h:730, {0}, assigned nowhere else) restarted at 0, and it reached only sent=18210
// across the whole 5 min 07 s window.
constexpr uint32_t kPrevSessionTerminalSeq = 344223;
constexpr uint32_t kNextSessionPacketCount = 200;

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    bool ok = true;

    // --- Positive controls: the gate itself must be unchanged by the fix. These must PASS in
    // --- BOTH arms, or the test discriminates on something other than the session boundary.
    {
        Watermark w;
        ok &= Check(Store(w, 0), "a virgin watermark accepts the first packet, seq 0 included");
        ok &= Check(Store(w, 1), "the next sequence is accepted");
        ok &= Check(!Store(w, 1), "a duplicate is still dropped");
        ok &= Check(!Store(w, 0), "an older packet is still dropped");
        ok &= Check(Store(w, 2), "and the stream resumes");
    }
    {
        // Wrapping comparison: a sender that runs past 2^32 keeps working. int32 difference, not
        // a plain `<`, is what makes that true, and the fix must not disturb it.
        Watermark w;
        ok &= Check(Store(w, 0xFFFFFFFEu), "a watermark near the uint32 wrap accepts");
        ok &= Check(Store(w, 1u), "and a wrapped sequence is newer, not stale");
        ok &= Check(!Store(w, 0xFFFFFFFDu), "while a genuinely older one across the wrap is stale");
    }
    {
        // The reconnect that ALREADY WORKS: a peer that reconnects WITHOUT restarting its process
        // keeps climbing its own sendSeq_, so it never trips the retained watermark. The fix only
        // widens what is accepted, so this arm must pass identically before and after.
        Watermark w;
        ok &= Check(Store(w, kPrevSessionTerminalSeq), "the previous session's terminal pose lands");
        SessionStart(w);
        ok &= Check(Store(w, kPrevSessionTerminalSeq + 1),
                    "an in-process reconnect whose sendSeq_ kept climbing is accepted "
                    "(unchanged by the fix)");
    }

    // --- The defect. A Session object outlives its sessions (the harness holds one file-scope
    // --- g_session and calls Start on it again), so the next session opens on this same
    // --- watermark. The peer is a RESTARTED PROCESS, so its sendSeq_ is back at 0.
    {
        Watermark w;
        ok &= Check(Store(w, kPrevSessionTerminalSeq),
                    "session B's terminal pose sets the watermark to 344223");

        SessionStart(w);  // the in-process re-host

        // ASSERT A -- the isolated statement of the defect, no sequence arithmetic needed: after
        // the boundary the slot must not still be holding a pose. Pre-fix the watermark survives,
        // which is what made the puppet auto-spawn off the dead session's stored pose BEFORE a
        // single packet of the new session had been parsed (the field log's spawn-before-latch
        // inversion at host lines 101667 / 101706).
        ok &= Check(!w.has,
                    "DEFECT A: the reused session does not open holding the dead session's pose");

        // ASSERT B -- the observable the players saw. Every pose of the new session is fed in
        // order from a fresh sender. Pre-fix all 200 are silently dropped at the gate, because
        // (int32_t)(seq - 344223) <= 0 for each of them.
        uint32_t accepted = 0;
        for (uint32_t seq = 1; seq <= kNextSessionPacketCount; ++seq) {
            if (Store(w, seq)) ++accepted;
        }
        ok &= Check(accepted == kNextSessionPacketCount,
                    "DEFECT B: every pose of the next session from a peer whose sendSeq_ "
                    "restarted at 0 is accepted");
        std::printf("       accepted=%u of %u\n", accepted, kNextSessionPacketCount);

        // ASSERT C -- the mechanism, not just the symptom: the count of packets the gate ate.
        // Pre-fix this is 200 and nothing anywhere logs or counts it.
        const uint32_t dropped = kNextSessionPacketCount - accepted;
        ok &= Check(dropped == 0,
                    "DEFECT C: no packet of the next session is discarded as stale");
        std::printf("       staleDrops=%u\n", dropped);
    }

    // --- The same seam on a stream ResetPeerRemoteState never owned. This one maps EXACTLY onto
    // --- production: hasRemoteHostClock_/lastRemoteHostClockSeq_ (session.h:657-659) are written
    // --- only by the ClockPose receive store (session_streams.cpp:364-372) and cleared only by
    // --- the call this header now serves. The stream is client-side by its own
    // --- `if (cfg_.role == Role::Host) break;`, so this is a client that leaves one host and
    // --- joins another in the same process.
    {
        Watermark hostClock;
        ok &= Check(Store(hostClock, 530816),
                    "the first host's terminal clock snapshot sets the client's watermark");
        SessionStart(hostClock);
        ok &= Check(Store(hostClock, 7),
                    "DEFECT D: the NEXT host's first clock snapshot is accepted, so the client's "
                    "world clock is not frozen for the session");
    }

    return ok ? 0 : 1;
}
