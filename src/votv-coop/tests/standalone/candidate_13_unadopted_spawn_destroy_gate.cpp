// candidate_13 -- the host must not broadcast the key-only destroy of an actor
// that is still in the FinishSpawn pending-adopt queue, because no peer has ever been told that
// actor exists and the receiver resolves a key-only destroy against its OWN live actor.
//
// Measured incident: a host log's "[PROP-DROP] HOST spawned client-placed prop
// key='0zlKei78xf4Ek_bPrYWV0A'" immediately followed by "HOST broadcasting DESTROY
// actor=000002C888D0D980 key='0zlKei78xf4Ek_bPrYWV0A' eid=0", and the client log's
// lines where that destroy deleted the client's own just-dropped floppy.
//
// The PRE-FIX policy pinned below is NOT the seam's original one -- it is the keyed-destroy gate
// exactly as it stands before this refinement, so the one failing assertion in the pre-fix
// arm measures the refinement alone and nothing else.

#if __has_include("coop/props/keyed_destroy_gate.h")
#include "coop/props/keyed_destroy_gate.h"
#else
namespace coop::props {
inline bool ShouldBroadcastKeyedDestroy(bool isHost, bool keyless, bool hasEid,
                                        bool anotherLiveLocalActorHoldsKey) {
    if (keyless) return hasEid;
    if (hasEid) return true;
    return isHost ? !anotherLiveLocalActorHoldsKey : true;
}
// The pre-fix refinement is no refinement at all: the publication state was not consulted.
inline bool ShouldBroadcastKeyedDestroyOncePublished(bool isHost, bool keyless, bool hasEid,
                                                     bool anotherLiveLocalActorHoldsKey,
                                                     bool /*pendingUnadoptedSpawn*/) {
    return ShouldBroadcastKeyedDestroy(isHost, keyless, hasEid, anotherLiveLocalActorHoldsKey);
}
}  // namespace coop::props
#endif

#include <cstdio>

namespace {

constexpr bool kHost = true;
constexpr bool kClient = false;
constexpr bool kKeyless = true;
constexpr bool kKeyed = false;
constexpr bool kHasEid = true;
constexpr bool kNoEid = false;
constexpr bool kOtherHolder = true;
constexpr bool kNoOtherHolder = false;
constexpr bool kPending = true;
constexpr bool kNotPending = false;

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    using coop::props::ShouldBroadcastKeyedDestroy;
    using coop::props::ShouldBroadcastKeyedDestroyOncePublished;
    bool ok = true;

    // ---- 1. The non-contradiction property, over the WHOLE input space (2^5 = 32 rows).
    // The new predicate may only ever turn a broadcast into a refusal. If it re-admitted anything
    // the base gate refuses, the two policies would disagree and this fix would be a regression of
    // that one. Asserted exhaustively rather than on the interesting rows, because "it does not
    // contradict the existing gate" is a claim about every input, not about six of them.
    int rows = 0;
    bool implication = true;
    bool everDiffers = false;
    for (int m = 0; m < 32; ++m) {
        const bool isHost  = (m & 1) != 0;
        const bool keyless = (m & 2) != 0;
        const bool hasEid  = (m & 4) != 0;
        const bool other   = (m & 8) != 0;
        const bool pending = (m & 16) != 0;
        const bool base = ShouldBroadcastKeyedDestroy(isHost, keyless, hasEid, other);
        const bool ex   = ShouldBroadcastKeyedDestroyOncePublished(isHost, keyless, hasEid, other,
                                                                   pending);
        if (ex && !base) implication = false;   // re-admitted something the base gate refuses
        if (ex != base) everDiffers = true;
        ++rows;
    }
    ok &= Check(rows == 32, "the exhaustive sweep really visited all 32 input rows");
    ok &= Check(implication,
                "refinement: every input the C01 gate refuses is still refused (no contradiction)");

    // ---- 2. The base gate's husk row is untouched by this fix: still refused, pending or not.
    ok &= Check(!ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kNoEid, kOtherHolder,
                                                          kNotPending),
                "C01 preserved: host key-only destroy refused while another live actor holds the key");
    ok &= Check(!ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kNoEid, kOtherHolder,
                                                          kPending),
                "C01 preserved: same refusal when the dying actor is also pending-unadopted");

    // ---- 3. Positive controls. These are the shapes that MUST stay on the wire; the queue state
    // must not touch any of them. Each is asserted with pending BOTH ways, so a fix that keyed off
    // the queue alone (ignoring the eid and the role) would fail here rather than pass by luck.
    ok &= Check(ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kHasEid, kNoOtherHolder,
                                                         kPending),
                "an adopted host prop broadcasts even while queued (the eid=12361 garbage-bag case)");
    ok &= Check(ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kHasEid, kOtherHolder,
                                                         kPending),
                "an adopted host prop broadcasts with an eid regardless of key holders or queue");
    ok &= Check(ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyless, kHasEid, kNoOtherHolder,
                                                         kPending),
                "the keyless trash-clump eid-only destroy is never touched by the queue test");
    ok &= Check(!ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyless, kNoEid, kNoOtherHolder,
                                                          kNotPending),
                "keyless with no Element row is not addressable and is still refused");
    ok &= Check(ShouldBroadcastKeyedDestroyOncePublished(kClient, kKeyed, kNoEid, kNoOtherHolder,
                                                         kPending),
                "a CLIENT key-only destroy still broadcasts (the normal pickup shape; host-only gate)");

    // ---- 4. THE REGRESSION CONTROL, and the reason the queue test cannot be replaced by a plain
    // "refuse host key-only destroys". A source trace measured 3057 of the host's 6717 destroy broadcasts
    // in one session carrying eid=0; those are save-loaded world props whose finish spawn was many
    // ticks ago, so the queue was cleared long before. They must still reach the wire.
    ok &= Check(ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kNoEid, kNoOtherHolder,
                                                         kNotPending),
                "the save-loaded host pickup still broadcasts its key-only destroy (3057/session)");

    // ---- 5. THE DEFECT. The host's own one-tick-old copy of a client-placed prop: keyed, no
    // Element row, the sole holder of its key here, still in the pending-adopt queue. This is the
    // only assertion that discriminates between the pre-fix and the fixed policy.
    ok &= Check(!ShouldBroadcastKeyedDestroyOncePublished(kHost, kKeyed, kNoEid, kNoOtherHolder,
                                                          kPending),
                "DEFECT: host key-only destroy of a still-pending, never-published spawn is refused");

    // A pre-fix arm that never differs from the base gate is exactly the pre-fix policy; a fixed
    // arm that never differs would mean the refinement is dead code. Printed, not asserted, since
    // it must legitimately read 0 on one arm and 1 on the other.
    std::printf("INFO: the refinement changes the answer on at least one of the 32 rows: %d\n",
                everDiffers ? 1 : 0);
    return ok ? 0 : 1;
}
