// candidate_16 -- the drone sack respawn policy table (coop/props/destroy_respawn_policy.h).
//
// It proves the POLICY TABLE only. It does not prove that DestroyResolvedLocalActor_ in
// remote_prop_destroy.cpp consults the predicate, nor that the takenByDrone write happens before
// that function's K2_DestroyActor call; those are read in the source, and the behaviour (a sack
// count per peer) is an in-game measurement.

#include <cstdint>
#include <cstdio>

#if __has_include("coop/props/destroy_respawn_policy.h")
#include "coop/props/destroy_respawn_policy.h"
#else
// The pinned pre-fix policy, retained only so this same source can prove its pre-fix negative:
// before the fix no destroy application marked the sack taken, so a client applying a remote
// destroy of the sack always ran the sack's own respawn.
namespace coop::props {
enum class DestroyRole : uint8_t { NoSession, Host, Client };
enum class DestroyPath : uint8_t { NativeDestroy, RemoteApply };
inline bool ForceNoRespawnOnDestroy(DestroyRole, DestroyPath, bool) { return false; }
}  // namespace coop::props
#endif

namespace {

using coop::props::DestroyPath;
using coop::props::DestroyRole;
using coop::props::ForceNoRespawnOnDestroy;

constexpr bool kSack = true;
constexpr bool kOtherClass = false;

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    bool ok = true;

    // The defect: a client applying another peer's destroy of the sack ran the sack's own respawn,
    // leaving a sack no other peer knew about. FAIL under the pinned pre-fix policy.
    ok &= Check(ForceNoRespawnOnDestroy(DestroyRole::Client, DestroyPath::RemoteApply, kSack),
                "DEFECT: client + remote apply + drone sack forces the no-respawn branch");

    // What must not change.
    ok &= Check(!ForceNoRespawnOnDestroy(DestroyRole::Host, DestroyPath::RemoteApply, kSack),
                "host + remote apply + drone sack keeps the native respawn (the authoritative one)");
    ok &= Check(!ForceNoRespawnOnDestroy(DestroyRole::Client, DestroyPath::RemoteApply, kOtherClass),
                "client + remote apply + any other class is untouched");
    ok &= Check(!ForceNoRespawnOnDestroy(DestroyRole::Client, DestroyPath::NativeDestroy, kSack),
                "client + native destroy of the sack is not routed through the force");
    ok &= Check(!ForceNoRespawnOnDestroy(DestroyRole::Host, DestroyPath::NativeDestroy, kSack),
                "host + native destroy of the sack is not routed through the force");
    ok &= Check(!ForceNoRespawnOnDestroy(DestroyRole::NoSession, DestroyPath::RemoteApply, kSack),
                "no session role: never forces");

    // The whole table: exactly one of the twelve input combinations forces.
    const DestroyRole roles[] = {DestroyRole::NoSession, DestroyRole::Host, DestroyRole::Client};
    const DestroyPath paths[] = {DestroyPath::NativeDestroy, DestroyPath::RemoteApply};
    const bool classes[] = {kOtherClass, kSack};
    int forced = 0;
    int combinations = 0;
    for (DestroyRole role : roles) {
        for (DestroyPath path : paths) {
            for (bool isSack : classes) {
                ++combinations;
                if (ForceNoRespawnOnDestroy(role, path, isSack)) ++forced;
            }
        }
    }
    std::printf("INFO: %d of %d combinations force\n", forced, combinations);
    ok &= Check(combinations == 12 && forced == 1,
                "exactly one of the 12 combinations forces (client, remote apply, drone sack)");

    return ok ? 0 : 1;
}
