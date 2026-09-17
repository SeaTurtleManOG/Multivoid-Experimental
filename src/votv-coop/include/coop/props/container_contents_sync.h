// coop/props/container_contents_sync.h -- a world container's CONTENTS, authored by the peer whose
// verb fired and arbitrated by the host. docs/devices.md carries the model; the invariants a
// client-authored slice must pass are coop/props/container_write_policy, the pen that holds one it
// cannot judge yet is coop/props/container_park, the bytes are coop/props/container_slice_wire.
//
// Two rules restate HERE, because breaking either destroys player data: a container whose
// propInventory_C.Player reads true, OR whose offset will not resolve, is SKIPPED (personal
// inventory shares one global GObjStack array, and a write over another peer's slice wipes that
// player's inventory); and a nested container's own index rides as the sentinel -1, on the way out
// AND on the way in, since a real value there names a slot in the SENDER's array.
//
// The edge is ue_wrap/core/script_gate on addObject and takeObj, marking the OWNING actor's eid
// dirty. Apply raw-writes the receiver's own GObjStack slot, then re-derives the setter-managed
// state through updateVolumesAndMass and recalculateNames; addObject cannot be the apply verb (it
// takes a live AActor* and serialises it itself) and checkObjectsVolume is not called (it ejects).

#pragma once

#include "coop/net/protocol.h"
#include "ue_wrap/actors/save_record.h"  // SaveRecord (POD) -- the custody seams below trade in it

#include <cstdint>
#include <vector>

namespace coop::net { class Session; }

namespace coop::props::container_contents_sync {

void Install(coop::net::Session* session);

// Registers the mutating verbs on first call, then drives the FName resolve and the dirty-set
// drain. Near-free when the dirty set is empty, the steady state.
void Tick();

// Read and clear the takeObj-in-flight latch, armed at the dispatch edge.
// prop_drop_intent::OnClientFinishSpawn consumes it to mark the extracted item's spawn as a
// container extraction. Game thread.
bool TakeObjInFlight();

// ContainerContents chunks: a BlobChunkPayload whose blob is
// `[u8 op=0][u32 eid][u64 baseHash][u16 n]` then n records in the coop/items/save_record_wire
// grammar. Never refanned -- a client accepts slot 0 and nothing else, and the host accepts only a
// non-zero slot, which is a client-authored slice it arbitrates.
void OnContentsChunk(const coop::net::BlobChunkPayload& p, uint8_t senderSlot);

// Host: ship the joiner one contents blob per live world container. The join save-transfer blob is
// a snapshot taken at request time; this is the anchor that corrects it.
void QueueConnectBroadcastForSlot(int peerSlot);

// Park aging is anchored to the client's own join-snapshot bracket: parks do not age while it is
// open, because a contents slice systematically precedes its PropSpawn under backpressure and a
// slow link can exceed any fixed TTL with no loss at all. At Complete every park is re-stamped and
// the TTL runs as a leak guard only. Called from the client-side snapshot dispatch.
void NoteJoinSnapshotBracket(bool open);

void OnDisconnect();

// ---- dev-instrument seams (coop/dev/container_selftest) ------------------------
// These exist so the instrument reuses this module's measured world-versus-personal boundary
// instead of reimplementing it. A probe free to get boundary 1 wrong would be testing a different
// rule than the one that ships.

struct WorldContainer {
    uint32_t eid;
    void* actor;
    void* inv;    // the propInventory_C component
};

// Fill `out` with up to `want` live world containers, boundary 1 applied. Host or client.
size_t SnapshotWorldContainers(WorldContainer* out, size_t want);

// Has the addObject/takeObj watch fired on this peer at least once? The instrument's verdict
// needs it: an absent effect means one thing if the edge is live and another if it never was.
bool VerbWatchEntered();

// The observable digest for one container: how many records its slice holds and the currVol the
// engine reports. Both peers print it; the smoke compares the numbers. False if the eid is not a
// live world container here.
bool ContentsDigest(uint32_t eid, int32_t& outCount, float& outVol);

// ---- custody seams (coop/props/container_custody) --------------------------------------------
// The host-side container custody park reads and writes THIS lane's state: the same Boundary 1, the
// same GObjStack slot resolve, the same record codec, the same apply block, the same content hash.
// Every one of those is in this file's ANONYMOUS namespace (or in container_slice_wire), so reuse
// needs a public wrapper and never a copy -- "ONE implementation"
// (coop/items/save_record_wire.h). Each seam below exists for exactly that reason; the only
// behaviour one of them adds is MarkHostCustodyWrite's bookkeeping, which is the host verb edge's.

// Boundary 1 and the container test, exposed unchanged from this lane's own readers.
bool IsContainer(void* actor);
bool IsWorldContainer(void* inv);
void* InventoryOfContainer(void* containerActor);

// A nested-container record, by the same class walk the wire path uses.
bool IsNestedContainerRecord(const ue_wrap::save_record::SaveRecord& r);

// Read a world container's slice. neuterNested=false is the HOST-LOCAL path: a park keeps the same
// host's GObjStack, so a nested container's ints[0][0] is still valid and neutering it would
// silently empty every nested container on re-attach. The wire path keeps the default.
bool ReadWorldContainerRecords(void* actor, std::vector<ue_wrap::save_record::SaveRecord>& out,
                               bool neuterNested);

// The raw-write block: AllocZeroed, WriteSaveRecord, WriteArrHeader. Does NOT run the allocator
// pre-flight and does NOT re-derive -- both are separate seams so a caller can order them itself.
bool WriteWorldContainerRecords(void* actor, const std::vector<ue_wrap::save_record::SaveRecord>& r);

// updateVolumesAndMass + recalculateNames, the setter-managed state a raw write leaves stale.
bool RederiveContainerManagedState(void* actor);

// The index half of the GObjStack slot resolve, with the same two refusals: a negative index and
// an index past the array. The slot-aliasing rows cannot read the private one.
bool WorldContainerSlotIndex(void* inv, int32_t& out);

// How many records the component's slot currently holds. -1 is not written; false means the slot
// did not resolve at all.
bool WorldContainerRecordCount(void* inv, int32_t& out);

// The current GObjStack length: the same `stack.num` the slot resolve compares against, and the
// range bound the nested-slot test needs.
bool GObjStackLength(int32_t& out);

// The size the FAN-OUT pack (container_slice_wire::Pack) would produce for this record set --
// nested indices NEUTERED, which is what BroadcastContainer refuses on. It can EXCEED the host-local
// pack, because neutering GROWS an absent ints[] to a one-element array.
size_t FanoutPackBytes(const std::vector<ue_wrap::save_record::SaveRecord>& recs);

// The content identity every gate and compare-and-swap on this lane uses
// (container_slice_wire::ContentHash).
uint64_t ContentsHash(uint32_t eid, const std::vector<ue_wrap::save_record::SaveRecord>& recs);

// Host: the custody consume just raw-wrote this container. Recorded exactly as the host's own
// addObject/takeObj verb edge records a mutation: the eid is marked dirty so the next sweep's
// DrainDirty fans the contents out through the shipped lane (and BroadcastContainer's
// NotePublished moves the write policy's baseline), the change is stamped through
// container_write_policy::NoteLocalChange so ANY client slice for this container inside the conflict
// window (not only one racing the write) is refused HostChangeInFlight and answered with the host's
// truth, as after a host verb edge, and the applied hash is dropped.
// Host-only, like the stamp it makes.
void MarkHostCustodyWrite(uint32_t eid);

}  // namespace coop::props::container_contents_sync
