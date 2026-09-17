// coop/net/session_watermark_reset.h -- the SESSION-boundary contract for a receive watermark.
//
// Gameplay/network layer. Every unreliable scalar stream Session receives is newest-wins against a
// retained per-stream watermark: the store keeps `has` and `lastSeq` and drops a packet whose
// sequence has not advanced past `lastSeq` (session_streams.cpp:201-202 and its siblings in that
// file, session_npc.cpp and session_worldactor.cpp). The sender's `Session::sendSeq_` (session.h)
// is one per-process atomic, starting at 0 and assigned nowhere else: only a PROCESS restart resets it.
//
// A Session OBJECT outlives its sessions (the harness calls Start on its file-scope g_session again
// after Stop), and Session::Stop never reaches the per-connection close edge where
// Session::ResetPeerRemoteState clears per-slot watermarks. Stop's ResetPoseBatches clears the NPC
// and WorldActor batches; the per-slot desk cursor and the four host-originated streams had no
// clearer at all. A retained watermark then drops every packet of the NEXT session from a peer whose
// sendSeq_ restarted at 0. This header makes the session-seam clear one named decision, shared with
// tests/standalone. Clearing only WIDENS what the gate accepts, so it cannot narrow a working path.

#pragma once

#include <cstdint>

namespace coop::net {

// Called for every retained receive watermark when a Session (re)opens, from
// Session::ResetRemoteStreamStateForNewSession. Safe by placement: it runs before EnsureGnsInit,
// before any socket exists and before the net thread is spawned, so every value it clears belongs
// to a session that has already stopped.
inline void ResetWatermarkForNewSession(bool& has, uint32_t& lastSeq) {
    has = false;
    lastSeq = 0;
}

// The same seam, one step further out: a retained receive QUEUE rather than a watermark. A
// watermark is a number the next session can out-run; a queue holds the dead session's PAYLOAD, and
// Session::ReliableMessage (session.h) names its origin by a bare peer-slot index -- no epoch, no
// generation, no connection handle, unlike relayEligible_ which is hConn-stamped "so a recycled slot
// never inherits it". Session::Stop frees every slot and Session::FindFreePeerSlotForClient hands
// out the lowest one again, so a message that crosses the seam is dispatched by the NEXT session
// with authority derived from a slot the dead one minted -- and on a client-then-host cycle in one
// process, senderPeerSlot 0 stops meaning "the host I was talking to" and starts meaning "me, the
// authority". Retiring the queue at the session boundary is the receive-side counterpart of Stop's
// `backlog_.FreeSlot(i);  // queued state dies with the session` (session_start.cpp:423).
//
// Called from Session::ResetRemoteStreamStateForNewSession, under the queue's OWN mutex; see the
// lock-order note at that call site.
template <class Queue>
inline void RetireReceiveQueueForNewSession(Queue& q) {
    q.clear();
}

}  // namespace coop::net
