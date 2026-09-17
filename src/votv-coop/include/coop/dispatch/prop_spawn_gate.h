// coop/dispatch/prop_spawn_gate.h -- the PropSpawn element-id/key boundary contract.

#pragma once

#include "coop/element/registry.h"
#include "coop/net/protocol.h"

namespace coop::dispatch {

inline bool HasUsablePropSpawnKey(const coop::net::WireKey& key) {
    if (key.len == 0 || key.len > sizeof(key.data)) return false;
    return !(key.len == 4 && key.data[0] == 'N' && key.data[1] == 'o' &&
             key.data[2] == 'n' && key.data[3] == 'e');
}

// Zero is the wire sentinel for "sender had no Element minted". It is valid only
// when the logical key carries the identity; an eid-only zero remains invalid.
// Every nonzero id still has to belong to a range the sender is allowed to name.
inline bool IsAllowedInboundPropSpawn(const coop::net::PropSpawnPayload& payload,
                                      bool senderIsHost) {
    if (payload.elementId == 0) return HasUsablePropSpawnKey(payload.key);
    return senderIsHost
        ? (coop::element::Registry::IsAllowedHostAllocatedEid(payload.elementId) ||
           coop::element::Registry::IsAllowedPeerAllocatedEid(payload.elementId))
        : coop::element::Registry::IsAllowedPeerAllocatedEid(payload.elementId);
}

}  // namespace coop::dispatch
