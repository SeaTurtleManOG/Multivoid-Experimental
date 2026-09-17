// candidate_14 -- a reused Session must not open holding the dead session's DELIVERED RELIABLES.
//
// What this test IS: a single translation unit that drives the REAL session-boundary policy
// (coop/net/session_watermark_reset.h, whose RetireReceiveQueueForNewSession is the only route by
// which src/coop/net/session_status.cpp empties reliableInbox_ at a session start) through a
// transcription of the four per-connection clear sites, of Session::Stop, and of the authority
// gates event_feed applies to a drained message.
//
// What this test IS NOT: it does not instantiate coop::net::Session. Session's receive path drags
// GameNetworkingSockets and the ue_wrap engine layer, which no single TU can compile. Every model
// below quotes the source lines it transcribes so a reviewer can diff them. The GAP that leaves is
// stated here rather than hidden: this proves the SEAM
// POLICY and the SLOT-AUTHORITY consequence, not that the shipped binary wires them together.
//
// Why the pre-fix arm is exact for THIS fix. For candidate_t3's pose
// block, hiding the header was not literally the pre-fix tree, because the pose lane had a second
// clearer (ResetPeerRemoteState). The reliable inbox has NO second clearer at the session seam --
// its only four clear sites are per-connection close edges -- so removing the header and falling
// back to the no-op below reproduces the pre-fix tree for this queue exactly.
//
// Both arms are built from this one source. The fixed arm compiles against the include tree as
// shipped; the pre-fix arm compiles against a copy with coop/net/session_watermark_reset.h removed,
// so __has_include selects the pinned pre-fix policy below. The pre-fix arm MUST fail.

#if __has_include("coop/net/session_watermark_reset.h")
#include "coop/net/session_watermark_reset.h"
#else
namespace coop::net {
// The pinned PRE-FIX policy, retained only so this same source can prove its pre-fix negative. It
// is the seam's behaviour as measured on the tree before the fix: NOTHING happened to
// reliableInbox_ at a session boundary. Session::Start (session_start.cpp) never named the
// deque; Session::Stop (session_start.cpp) never named it either, and retired only the SEND
// side (backlog_.FreeSlot(i), "queued state dies with the session"); and the four clear sites
// (in session_status.cpp) were all per-connection close edges, which
// a teardown through Stop never reaches -- Stop closes every peer itself, and "GNS delivers no
// status callback for a connection we close" (session_status.cpp).
template <class Queue>
inline void RetireReceiveQueueForNewSession(Queue&) {}
}  // namespace coop::net
#endif

#include <array>
#include <cstdio>
#include <deque>

namespace {

constexpr int kMaxPeers = 4;  // net::kMaxPeers

// The reliable kinds this test needs. The two groups are the two authority shapes event_feed
// applies, and nothing else about a kind matters here.
enum class Kind {
    // Host-authored: `if (msg.senderPeerSlot != 0) { UE_LOGW(...); break; }`
    // -- RestoreVitals (event_feed.cpp:197), PlayerDamage (:210), WispGrab (:229), WispTear (:248),
    //    BalanceSync (:266), TeleportClient (:291), ChatLine (event_dispatch_world.cpp:240).
    RestoreVitals,
    PlayerDamage,
    ChatLine,
    // Client-authored: `msg.senderPeerSlot >= 1 && msg.senderPeerSlot < net::kMaxPeers`
    // -- SaveTransferRequest (event_feed.cpp:159-160), ClientWorldReady (:172-173).
    Join,
    PropSpawn,
};

// Session::ReliableMessage (session.h:93-98) as it bears on this test: the ONLY thing naming the
// origin is a bare slot index. No epoch, no generation, no connection handle.
struct Msg {
    Kind kind = Kind::Join;
    int  senderPeerSlot = -1;
    int  sessionTag = 0;  // NOT on the wire; the test's own label for which session minted it
};

// What a drain did, so an assertion can name the consequence rather than the queue depth.
struct Applied {
    int hostAuthored = 0;                  // rows that passed a senderPeerSlot != 0 gate
    int chatLinesIntoTheRecord = 0;        // rows that reached chat_sync::OnChatLine
    std::array<int, kMaxPeers> perSlot{};  // client-authored rows billed to a slot
    int fromDeadSession = 0;               // of all the above, how many carried an old sessionTag
};

// The Session model. Every member transcribes the named source lines and nothing else.
struct SessionModel {
    std::array<unsigned, kMaxPeers> peerConns{};
    std::array<bool, kMaxPeers> lanesConfigured{};
    std::deque<Msg> inbox;
    bool running = false;
    bool isHost = false;
    int  tag = 0;

    // session_status.cpp:349-357
    //     if (peerConns_[i].load() != 0 && peerLanesConfigured_[i].load()) ++n;
    int connectedPeerCount() const {
        int n = 0;
        for (int i = 0; i < kMaxPeers; ++i) if (peerConns[i] != 0 && lanesConfigured[i]) ++n;
        return n;
    }

    // session_status.cpp:241-247
    //     for (int i = 1; i < kMaxPeers; ++i) if (peerConns_[i].load() == 0) return i;
    int FindFreePeerSlotForClient() const {
        for (int i = 1; i < kMaxPeers; ++i) if (peerConns[i] == 0) return i;
        return -1;
    }

    void Seat(int slot, unsigned hConn) { peerConns[slot] = hConn; lanesConfigured[slot] = true; }

    // session.cpp:543-546 -- m.senderPeerSlot = routeSlot;
    void Enqueue(Kind k, int routeSlot) { inbox.push_back(Msg{k, routeSlot, tag}); }

    // session.cpp:51-57 -- NO ConnState gate, correctly: a FIFO gate would defer, not discard.
    bool TryGetReliable(Msg& out) {
        if (inbox.empty()) return false;
        out = inbox.front();
        inbox.pop_front();
        return true;
    }

    // The per-slot erase + the aggregate clear, session_status.cpp:546-549 and :561-567:
    //     for (auto it = reliableInbox_.begin(); it != reliableInbox_.end();) {
    //         if (it->senderPeerSlot == slot) it = reliableInbox_.erase(it); else ++it; }
    //     if (connectedPeerCount() == 0) { ... reliableInbox_.clear(); }
    void CloseEdgeForSlot(int slot) {
        peerConns[slot] = 0;
        lanesConfigured[slot] = false;
        for (auto it = inbox.begin(); it != inbox.end();) {
            if (it->senderPeerSlot == slot) it = inbox.erase(it); else ++it;
        }
        if (connectedPeerCount() == 0) inbox.clear();
    }
    // KickClaimed (session_status.cpp:675-680, :688-692) is the same two steps in the same order.
    void KickClaimed(int slot) { CloseEdgeForSlot(slot); }

    // session_start.cpp:374-434. Note what is and is not here: peerConns_ is exchanged to 0 BEFORE
    // CloseConnection (:395 then :400), so a racing callback's FindPeerSlotForConn returns -1; the
    // SEND side is retired (backlog_.FreeSlot(i), :397); the inbox is not named at all; and no
    // terminal status callback follows, because "GNS delivers no status callback for a connection we
    // close" (session_status.cpp:671).
    void Stop() {
        if (!running) return;
        running = false;
        for (int i = 0; i < kMaxPeers; ++i) { peerConns[i] = 0; lanesConfigured[i] = false; }
    }

    // session_start.cpp:98-207; the seam call is :122,
    //     { std::lock_guard<std::mutex> lk(remoteMutex_); ResetRemoteStreamStateForNewSession(); }
    // whose last act is the retirement under test.
    void Start(bool host, int newTag) {
        isHost = host;
        tag = newTag;
        coop::net::RetireReceiveQueueForNewSession(inbox);
        running = true;
    }
};

// coop::event_feed::Update's drain (event_feed.cpp:149-150) with the two authority gates. It runs
// on every pump tick of a RUNNING session and gates on nothing else -- net_pump::Tick is called
// under `if (running)` at session_runtime.cpp:640-642, so between Stop and Start it does not run at
// all, and the first tick of the NEXT session sees whatever crossed the seam.
Applied Drain(SessionModel& s, int liveTag) {
    Applied a{};
    Msg m;
    while (s.TryGetReliable(m)) {
        const bool stale = (m.sessionTag != liveTag);
        switch (m.kind) {
        case Kind::RestoreVitals:
        case Kind::PlayerDamage:
            if (m.senderPeerSlot != 0) break;  // "from non-host senderPeerSlot=%d -- dropping"
            ++a.hostAuthored;
            if (stale) ++a.fromDeadSession;
            break;
        case Kind::ChatLine:
            if (m.senderPeerSlot != 0) break;  // "only the host authors chat; dropping"
            ++a.hostAuthored;
            ++a.chatLinesIntoTheRecord;
            if (stale) ++a.fromDeadSession;
            break;
        case Kind::Join:
        case Kind::PropSpawn:
            if (m.senderPeerSlot >= 1 && m.senderPeerSlot < kMaxPeers) {
                ++a.perSlot[static_cast<size_t>(m.senderPeerSlot)];
                if (stale) ++a.fromDeadSession;
            }
            break;
        }
    }
    return a;
}

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    bool ok = true;

    // --- Positive controls. These must PASS in BOTH arms, or the test discriminates on something
    // --- other than the session boundary.
    {   // The reader is untouched by the fix: FIFO, once each.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u);
        s.Enqueue(Kind::Join, 1); s.Enqueue(Kind::PropSpawn, 1);
        const Applied a = Drain(s, 1);
        ok &= Check(a.perSlot[1] == 2 && s.inbox.empty(),
                    "within one session both rows are delivered exactly once");
    }
    {   // The per-slot erase at the close edge, with another peer still seated.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u); s.Seat(2, 0x22u);
        s.Enqueue(Kind::PropSpawn, 1); s.Enqueue(Kind::PropSpawn, 2);
        s.CloseEdgeForSlot(1);
        const Applied a = Drain(s, 1);
        ok &= Check(a.perSlot[1] == 0 && a.perSlot[2] == 1,
                    "a peer's close erases that peer's queued rows and leaves the other peer's");
    }
    {   // The aggregate clear when the last peer goes.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u);
        s.Enqueue(Kind::PropSpawn, 1); s.Enqueue(Kind::ChatLine, 0);
        s.CloseEdgeForSlot(1);
        ok &= Check(s.inbox.empty(),
                    "the last peer's close clears the whole inbox, host-authored rows included");
    }
    {   // KickClaimed, same two steps, other peer unaffected.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u); s.Seat(2, 0x22u);
        s.Enqueue(Kind::PropSpawn, 1); s.Enqueue(Kind::PropSpawn, 2);
        s.KickClaimed(2);
        const Applied a = Drain(s, 1);
        ok &= Check(a.perSlot[2] == 0 && a.perSlot[1] == 1,
                    "a kick erases the kicked slot's rows only, while another peer remains");
    }
    {   // Slot reuse is real and is not what the fix changes.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u); s.Seat(2, 0x22u);
        s.Stop(); s.Start(true, 2);
        ok &= Check(s.FindFreePeerSlotForClient() == 1,
                    "after a Stop the next joiner deterministically takes slot 1 again");
    }
    {   // The fix cannot narrow the live path: a row minted by the NEW session still lands.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u);
        s.Enqueue(Kind::PropSpawn, 1);
        s.Stop();
        s.Start(true, 2); s.Seat(1, 0x99u);
        s.Enqueue(Kind::PropSpawn, 1);
        const Applied a = Drain(s, 2);
        ok &= Check(a.perSlot[1] >= 1, "a row sent inside the new session is still delivered");
    }

    // --- The defect assertions. Each must FAIL pre-fix and PASS fixed.
    {   // DEFECT A: nothing crosses the seam.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u);
        s.Enqueue(Kind::PropSpawn, 1); s.Enqueue(Kind::Join, 1); s.Enqueue(Kind::ChatLine, 0);
        s.Stop();
        s.Start(true, 2);
        const size_t retained = s.inbox.size();
        ok &= Check(retained == 0,
                    "DEFECT A: a reused Session does not open holding the dead session's reliables");
        std::printf("       retained=%zu\n", retained);
    }
    {   // DEFECT B: slot aliasing. Session A's slot-1 peer queues rows; Stop; Start; the next
        // joiner takes slot 1 (the control above) and the rows are billed to THAT player.
        SessionModel s; s.Start(true, 1); s.Seat(1, 0x11u);
        s.Enqueue(Kind::PropSpawn, 1); s.Enqueue(Kind::Join, 1);
        s.Stop();
        s.Start(true, 2);
        const int seat = s.FindFreePeerSlotForClient();
        s.Seat(seat, 0x99u);
        const Applied a = Drain(s, 2);
        const int billed = a.perSlot[static_cast<size_t>(seat)];
        ok &= Check(billed == 0,
                    "DEFECT B: the new session's slot-1 peer is not billed rows it never sent");
        std::printf("       billed to slot %d from a dead session = %d\n", seat, billed);
    }
    {   // DEFECT C: the role flip. A CLIENT retains host-authored rows stamped senderPeerSlot 0;
        // the same process then HOSTS, where 0 means "me, the authority", so every
        // `senderPeerSlot != 0` gate passes.
        SessionModel s; s.Start(/*host*/false, 1); s.Seat(0, 0xA0u);
        s.Enqueue(Kind::RestoreVitals, 0);
        s.Enqueue(Kind::PlayerDamage, 0);
        s.Stop();
        s.Start(/*host*/true, 2);
        const Applied a = Drain(s, 2);
        ok &= Check(a.hostAuthored == 0,
                    "DEFECT C: the previous host's authority rows are not applied on the new host");
        std::printf("       host-authored rows applied from a dead session = %d\n",
                    a.fromDeadSession);
    }
    {   // DEFECT D: the chat seam. event_feed::OnSessionStart calls chat_sync::Reset()
        // (event_feed.cpp:111) so that "a re-host in one process would otherwise seed the new lobby
        // with the old talk" -- and it runs at session_runtime.cpp:362, BEFORE g_session.Start at
        // :420. A retained ChatSpeaker/ChatLine pair re-seeds the record anyway.
        SessionModel s; s.Start(/*host*/false, 1); s.Seat(0, 0xA0u);
        s.Enqueue(Kind::ChatLine, 0); s.Enqueue(Kind::ChatLine, 0);
        s.Stop();
        s.Start(/*host*/true, 2);
        const Applied a = Drain(s, 2);
        ok &= Check(a.chatLinesIntoTheRecord == 0,
                    "DEFECT D: the dead lobby's chat does not enter the new lobby's record");
        std::printf("       dead-lobby lines admitted = %d\n", a.chatLinesIntoTheRecord);
    }

    std::printf("%s\n", ok ? "CANDIDATE_14_PASS" : "CANDIDATE_14_FAIL");
    return ok ? 0 : 1;
}
