#include "coop/element/registry.h"
#include "coop/net/protocol.h"

#if __has_include("coop/dispatch/prop_spawn_gate.h")
#include "coop/dispatch/prop_spawn_gate.h"
#else
// The pinned boundary policy, retained only so this same test source can prove
// its pre-fix negative before the production policy header exists.
namespace coop::dispatch {
inline bool IsAllowedInboundPropSpawn(const coop::net::PropSpawnPayload& payload,
                                      bool senderIsHost) {
    return senderIsHost
        ? (coop::element::Registry::IsAllowedHostAllocatedEid(payload.elementId) ||
           coop::element::Registry::IsAllowedPeerAllocatedEid(payload.elementId))
        : coop::element::Registry::IsAllowedPeerAllocatedEid(payload.elementId);
}
}  // namespace coop::dispatch
#endif

#include <cstdio>
#include <cstring>

namespace {

coop::net::PropSpawnPayload Payload(uint32_t eid, const char* key) {
    coop::net::PropSpawnPayload payload{};
    payload.elementId = eid;
    if (key != nullptr) {
        const size_t n = std::strlen(key);
        payload.key.len = static_cast<uint8_t>(n);
        std::memcpy(payload.key.data, key, n);
    }
    return payload;
}

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    using coop::dispatch::IsAllowedInboundPropSpawn;
    using coop::element::kHostRangeSize;
    bool ok = true;

    ok &= Check(IsAllowedInboundPropSpawn(Payload(1, nullptr), true),
                "positive control accepts a host-range id from the host");
    ok &= Check(IsAllowedInboundPropSpawn(Payload(kHostRangeSize, nullptr), false),
                "positive control accepts a peer-range id from a client");

    ok &= Check(IsAllowedInboundPropSpawn(Payload(0, "logical-key"), true),
                "keyed zero-id spawn is admitted from the host");
    ok &= Check(IsAllowedInboundPropSpawn(Payload(0, "logical-key"), false),
                "keyed zero-id spawn is admitted from a client");
    ok &= Check(!IsAllowedInboundPropSpawn(Payload(0, nullptr), true),
                "eid-only zero-id spawn is refused");
    ok &= Check(!IsAllowedInboundPropSpawn(Payload(0, "None"), true),
                "NAME_None zero-id spawn is refused");
    ok &= Check(!IsAllowedInboundPropSpawn(Payload(0xFFFFFFFFu, "logical-key"), true),
                "key does not admit the nonzero invalid-id sentinel");
    ok &= Check(!IsAllowedInboundPropSpawn(Payload(1, "logical-key"), false),
                "client cannot use a host-range id even when keyed");
    return ok ? 0 : 1;
}
