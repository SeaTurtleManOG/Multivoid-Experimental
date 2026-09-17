#if __has_include("coop/props/keyed_destroy_gate.h")
#include "coop/props/keyed_destroy_gate.h"
#else
// The pinned pre-fix policy, retained only so this same test source can prove its
// pre-fix negative before the production policy header exists. It is the seam's
// behaviour before the fix: the sole outbound refusal was
// `if (keyless && !hasEid) return;` in prop_destroy_seam.cpp, so everything keyed
// went on the wire regardless of role, Element row, or who else held the key.
namespace coop::props {
inline bool ShouldBroadcastKeyedDestroy(bool /*isHost*/, bool keyless, bool hasEid,
                                        bool /*anotherLiveLocalActorHoldsKey*/) {
    return keyless ? hasEid : true;
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

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    using coop::props::ShouldBroadcastKeyedDestroy;
    bool ok = true;

    // Positive controls: the shapes that broadcast before the fix must still broadcast
    // after it, or the gate has taken a genuine destroy off the wire.
    ok &= Check(ShouldBroadcastKeyedDestroy(kHost, kKeyed, kHasEid, kOtherHolder),
                "host keyed destroy with an Element row broadcasts even with another key holder");
    ok &= Check(ShouldBroadcastKeyedDestroy(kHost, kKeyed, kHasEid, kNoOtherHolder),
                "host keyed destroy with an Element row broadcasts (the measured eid=5974 pickup)");
    ok &= Check(ShouldBroadcastKeyedDestroy(kHost, kKeyless, kHasEid, kNoOtherHolder),
                "keyless destroy with an Element row broadcasts (the trash-clump shape)");
    ok &= Check(!ShouldBroadcastKeyedDestroy(kHost, kKeyless, kNoEid, kNoOtherHolder),
                "keyless destroy with no Element row is not addressable and is refused");

    // The defect, as measured in a game session before the fix: the host's in-hand husk is
    // unenrolled, so it stamps eid=0, and the placed prop it dies behind still holds the
    // key. The pre-fix arm broadcasts this and the receiver destroys the placed mirror.
    // This line is the fix: FAIL under the pinned pre-fix policy, PASS under the header.
    ok &= Check(!ShouldBroadcastKeyedDestroy(kHost, kKeyed, kNoEid, kOtherHolder),
                "DEFECT: host key-only destroy is refused while another live local actor "
                "holds the key");

    // The gate is a duplicate-key discriminator, not an eid-presence one: a genuine
    // departure and a client's normal pickup both keep the wire.
    ok &= Check(ShouldBroadcastKeyedDestroy(kHost, kKeyed, kNoEid, kNoOtherHolder),
                "host key-only destroy still broadcasts when no other live actor holds the key");
    ok &= Check(ShouldBroadcastKeyedDestroy(kClient, kKeyed, kNoEid, kOtherHolder),
                "client key-only destroy broadcasts (the normal pickup shape; host-only gate)");

    return ok ? 0 : 1;
}
