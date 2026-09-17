// coop/props/destroy_respawn_policy.h -- whether applying a destroy forces a class's own
// destroy-time respawn off. Engine-free, so the standalone test compiles it on its own.

#pragma once

#include <cstdint>

namespace coop::props {

// The peer applying a destroy. NoSession is a peer with no session role, where every destroy is
// native.
enum class DestroyRole : uint8_t { NoSession, Host, Client };

// How the destroy reached the actor: the engine's own call on the peer where it happened, or this
// peer applying a PropDestroy another peer sent (DestroyResolvedLocalActor_ in
// remote_prop_destroy.cpp).
enum class DestroyPath : uint8_t { NativeDestroy, RemoteApply };

// Whether to mark the dying actor taken, so its class skips its own destroy-time respawn.
//
// prop_dronesack_C's destroy handler removes the prop from the game's registry and then, unless
// takenByDrone is set, shows a hint, spawns a new sack at the drone and plays a teleport cue.
// - A client applying a remote destroy of the sack: force. Its respawn would be a sack no other
//   peer knows about; any replacement that should exist is the one the host's copy spawns, and it
//   reaches the client as a PropSpawn.
// - A host applying a remote destroy: never. Its native respawn is the authoritative one, so a
//   take performed on a client's own copy still becomes a respawn on the host.
// - A native destroy: never. The peer where the destroy happened keeps single-player behaviour.
// - Any other class: never.
inline bool ForceNoRespawnOnDestroy(DestroyRole role, DestroyPath path, bool classIsDroneSack) {
    return role == DestroyRole::Client && path == DestroyPath::RemoteApply && classIsDroneSack;
}

}  // namespace coop::props
