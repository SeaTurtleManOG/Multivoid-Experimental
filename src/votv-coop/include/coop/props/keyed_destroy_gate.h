// coop/props/keyed_destroy_gate.h -- the outbound PropDestroy identity/authority contract.

#pragma once

namespace coop::props {

// Whether the destroy seam should put this dying actor's destroy on the wire.
//
// A destroy names its target twice: by logical key (the receiver's lookup) and by Element id (the
// routing). Zero is the wire sentinel for "sender had no Element row", so a zero-eid destroy is
// KEY-ONLY and the receiver resolves it against whatever live local actor carries that key -- sound
// only while the key names exactly one actor on the sender.
// - keyless (the trash-clump shape): the eid is the whole identity; broadcast iff we have one. An
//   actor with neither is not addressable and is dropped.
// - keyed with an eid: identity is unambiguous. Broadcast.
// - keyed without an eid, on a client: the normal pickup shape. A client mints no Element row for its
//   own save-loaded keyed prop, so the eid is 0 for exactly the props it picks up and shoots
//   (prop_destroy_seam.cpp:189-190). Broadcast.
// - keyed without an eid, on the host: an unenrolled actor, e.g. the in-hand display husk carrying the
//   box's key. The receiver would destroy the mirror of the box the host just PLACED, so broadcast
//   only if no other live local actor still resolves under the key; otherwise the destroy is local.
inline bool ShouldBroadcastKeyedDestroy(bool isHost, bool keyless, bool hasEid,
                                        bool anotherLiveLocalActorHoldsKey) {
    if (keyless) return hasEid;
    if (hasEid) return true;
    return isHost ? !anotherLiveLocalActorHoldsKey : true;
}

// The publication test, layered on top of the identity/authority test above.
//
// The gate above asks "does the key still name someone else here?" -- right for the in-hand husk,
// which dies BEHIND a placed prop that owns the key; wrong for an actor that is the key's ONLY holder
// here and was never published. The host's copy of a client-placed prop is spawned by
// HostSpawnPlacedProp (prop_drop_intent.cpp), which mints no Element and writes no key-index
// entry; peers see it one pump tick later, when host_spawn_watcher::DrainPendingSpawns expresses it.
// Dying in that window, it stamps eid=0, no other local actor holds its key, and no peer has heard of
// it -- yet the receiver resolves the key-only destroy onto ITS OWN actor and deletes the player's item.
// pendingUnadoptedSpawn is exactly "still in host_spawn_watcher's pending-adopt queue": finished
// spawning with no Element (host_spawn_watcher.cpp:234), not yet drained (cleared unconditionally at
// :385). A PropSpawn carries an elementId, so such an actor was never on the wire and refusing its
// destroy orphans no mirror. It is a REFINEMENT: it calls the gate above first and can only turn a
// broadcast into a refusal, never re-admit what that gate refuses; the standalone test asserts that
// implication over every input combination.
inline bool ShouldBroadcastKeyedDestroyOncePublished(bool isHost, bool keyless, bool hasEid,
                                                     bool anotherLiveLocalActorHoldsKey,
                                                     bool pendingUnadoptedSpawn) {
    if (!ShouldBroadcastKeyedDestroy(isHost, keyless, hasEid, anotherLiveLocalActorHoldsKey))
        return false;
    if (isHost && !keyless && !hasEid && pendingUnadoptedSpawn) return false;
    return true;
}

}  // namespace coop::props
