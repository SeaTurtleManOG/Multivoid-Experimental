// coop/props/container_custody.h -- a world container's CONTENTS survive the actor gap between a
// client's keyed PropDestroy (the take) and that same client's PropDropIntent (the place).
//
// Contents are eid-addressed on container_contents_sync's lane; custody is key-addressed on the prop
// lifecycle. A client pickup destroys the host's actor, the host's only addressor for that GObjStack
// slice, and the re-placed container is a FRESH actor with a FRESH eid and a FRESH, EMPTY slice. The
// repair is host-local: PARK the record set at the destroy, ARM it when the SAME peer in the SAME slot
// occupancy re-places the SAME key under the SAME class, and APPLY it on the next tick, after
// host_spawn_watcher::DrainPendingSpawns mints the new eid. No new ReliableKind, blob op or protocol bump.
// SlotIo INJECTS every engine operation, so the decidable policy (bindings, six arm conditions,
// thirteen consume checks) is an engine-free reducer a standalone test drives; a green run proves the
// POLICY, never that the production SlotIo binding, capture call, author-slot latch or Tick exist.
// FAIL-CLOSED: every failure leaves the unfixed build's empty box. Nothing writes a partial set, into a
// slot it could not verify and prove exclusively owned (outer or nested), or into an inventory whose
// Player flag it could not read. Game thread only: the store is unsynchronised, touched by the net pump.

#pragma once

#include "coop/net/blob_chunks.h"        // MaxBlobBytes() -- the fan-out ceiling, reused not restated
#include "ue_wrap/actors/save_record.h"  // SaveRecord: engine-agnostic POD, no engine call from here

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>

namespace coop::net { class Session; }

namespace coop::props::container_custody {

using Rec = ue_wrap::save_record::SaveRecord;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// The peer-slot bound. Spelled here rather than pulled from coop/net/session.h so this header stays
// engine-free and standalone-compilable; container_custody.cpp static_asserts it against
// net::kMaxPeers. The same idiom, for the same reason, as coop/session/join_seed.h:45.
inline constexpr int kMaxPeers = 4;

// "No sender slot was latched for this destroy." Distinct from an out-of-range latched value: the
// latch's own unknown value is -1 (Session::ReliableMessage::senderPeerSlot, session.h:95), and
// a uint8_t seam would fold -1 into slot 255 and make the AuthorUnknown refusal
// unobservable. Park() therefore separates "nothing was latched" from "something out of range was".
inline constexpr int kNoAuthorSlot = -2147483647 - 1;  // INT_MIN, without <climits>

// A park set is bounded on three axes: how many parks, how many records in one park, and how many
// bytes the FAN-OUT of one park would produce.
inline constexpr size_t kMaxCustodyParks = 32;
// Mirrors container_slice_wire.h's kMaxRecords (512); container_contents_sync.cpp's ReadContents
// already refuses above it, so this is the store restating a bound it must not exceed, not a new one.
inline constexpr size_t kMaxRecordsPerContainer = 512;
// A leak guard, NOT a safety bound: the safety property is causal (author slot, author generation,
// class, first-spawn-only, world generation), and the memory is bounded harder by the park cap and
// by the fan-out ceiling. There is deliberately no wall-clock consume window.
inline constexpr int kCustodyParkCeilingSec = 900;
// How many custody Ticks an ARMED park may wait for its eid before it is refused eid-unresolved.
inline constexpr int kMaxAdoptTries = 8;

// The size the FAN-OUT pack would produce must fit the transport, or a park could be restored and
// never published -- a host/client divergence rather than a fix (capture condition 9).
inline constexpr size_t MaxFanoutBytes() { return coop::blob_chunks::MaxBlobBytes(); }

// ---- the record set ---------------------------------------------------------------------------

// What the capture read off the dying container. `recs` are ordered and their nested indices are
// PRESERVED (the capture calls ReadContents with neuterNested=false): a host-local park keeps the
// same host's GObjStack, so a nested container's ints[0][0] is still valid and neutering it would
// silently empty every nested container on re-attach. That is the sharpest trap in the design.
struct CustodyRecordSet {
    std::vector<Rec> recs;
    uint64_t hash = 0;         // ContentHash's shape: FNV-64 over the canonical pack
    int32_t  index = -1;       // the host's own slot number at capture; DIAGNOSTICS ONLY, never sent
    size_t   fanoutBytes = 0;  // the size the NEUTERED (wire) pack would produce -- see condition 9
};

// Custody identity is a tuple, never a bare key: the same key has been measured on five live
// actors. The key is stored WIRE-NORMALISED (<= 31 chars), the same normalisation both senders
// already apply, so a comparison against a wire key means something.
struct CustodyKeyT {
    std::wstring key;
    std::wstring cls;
};

// Every outcome this store can produce. The strings the host log prints for each are listed beside
// the enumerator; log checks grep those, so they are part of the contract.
enum class Reason {
    Applied,                 // (no refusal line; "reattached")
    DiscardedFreshNotEmpty,  // fresh-not-empty  -> DISCARDED
    NotParked,               // no park for this key / this actor
    AlreadyArmed,            // already-armed
    AuthorMismatch,          // author-mismatch
    AuthorReplaced,          // author-replaced
    ClassMismatch,           // class-mismatch
    KeyMismatch,             // key-mismatch
    KeyRekeyed,              // key-rekeyed
    WorldChanged,            // world-changed
    AuthorLeft,              // author-left
    Boundary1,               // boundary1
    SlotUnresolved,          // slot-unresolved
    SlotAliased,             // slot-aliased
    NestedSlotAliased,       // nested-slot-aliased
    EnumerationUnproven,     // enumeration-unproven
    AllocPreflight,          // alloc-preflight
    Expired,                 // EXPIRED
    Evicted,                 // EVICTED
    AuthorUnknown,           // author-unknown
    SlotOutOfRange,          // slot-out-of-range
    // --- Each one below is required by a numbered capture condition or consume row.
    // --- Named here so each is visible rather than silently folded into a
    // --- neighbouring reason.
    KeyEmpty,                // key-empty        -- capture condition 3
    CapExceeded,             // cap-512          -- capture condition 8
    OverBlobCeiling,         // over-blob-ceiling-- capture condition 9
    AllocFailed,             // alloc-failed     -- consume row 15
    Replaced,                // REPLACED         -- a second park for a key that already has one
    ActorDead,               // actor-dead       -- consume row 1  (the CALLER's engine read)
    EidUnresolved,           // eid-unresolved   -- consume row 2  (the CALLER's engine read)
    NotContainer,            // not-container    -- consume row 5  (the CALLER's engine read)
    Session,                 // session          -- one of the CLEARED reasons;
                             //                     it is the reason Clear() retires under, i.e. the
                             //                     host tore the session down while parks were held
    kCount
};

const char* ReasonText(Reason r);  // defined inline below; the exact strings the log rows carry

// ---- the injected engine seam ------------------------------------------------------------------

// Every engine-touching operation the consume needs, injected. The production binding lives in
// container_custody.cpp over container_contents_sync's public wrappers -- one implementation of the
// record codec and of Boundary 1, never a second (save_record_wire.h:3-5).
struct SlotIo {
    // Boundary 1: propInventory_C.Player == 0, fail-closed on an unresolvable flag.
    bool (*IsWorldContainer)(void* inv) = nullptr;
    // GObjStackSlot's index half (container_contents_sync.cpp:200-211), exposed as an index.
    bool (*ReadSlotIndex)(void* inv, int32_t& outIndex) = nullptr;
    // How many records the fresh slot already holds; < 0 = unresolvable, which refuses.
    int32_t (*ReadSlotCount)(void* inv) = nullptr;
    // The EngineAlloc probe of container_contents_sync.cpp:494-500. Its shipped comment: without it
    // "an empty array would silently replace real contents".
    bool (*AllocPreflight)() = nullptr;
    bool (*WriteRecords)(void* inv, const CustodyRecordSet& set) = nullptr;
    // updateVolumesAndMass + recalculateNames (container_contents_sync.cpp:453-457).
    bool (*Rederive)(void* owner, void* inv) = nullptr;
    // Takes the TARGET, because the actor/eid self-exclusion of consume row 10 clause 1
    // is performed INSIDE this function, where SnapshotWorldContainers supplies both identifiers;
    // the store receives a list that is already other-only. Returns the number of OTHER containers'
    // indices, writes min(that, want) of them to `out`, and sets outComplete for THAT enumeration.
    //
    // Called with (out = nullptr, want = 0) it is a COUNT-ONLY call: enumerate, exclude, write
    // nothing, return the count. That is how the store sizes its own buffer WITH HEADROOM
    // (need + 1). The want == 0 convention belongs to THIS injected seam and not to the shipped
    // SnapshotWorldContainers, which refuses want == 0 outright at container_contents_sync.cpp:785.
    size_t (*OtherWorldContainerIndices)(void* selfActor, uint32_t selfEid,
                                         int32_t* out, size_t want, bool& outComplete) = nullptr;
    // The live personal store's GObjStack index, read rather than assumed to be 0. False =
    // unreadable, which refuses: an unprovable non-alias is not a non-alias.
    bool (*PersonalStoreIndex)(int32_t& out) = nullptr;
    // The nested-slot test's range bound: the `stack.num` GObjStackSlot compares against at
    // container_contents_sync.cpp:209. <= 0 refuses every non-sentinel nested index.
    int32_t (*GObjStackLength)() = nullptr;
    // Rows 11-12 cannot be written without it:
    // RecordIsNestedContainer (container_contents_sync.cpp:228-233) is FindClass + WalksToBase,
    // i.e. an engine call, so it must be injected like every other one.
    bool (*IsNestedContainerRecord)(const Rec& r) = nullptr;
};

// ---- the store ---------------------------------------------------------------------------------

// key -> park, plus a FIFO holding EXACTLY the same keys. The pairing is the invariant, not an
// implementation detail: the shipped place-park set (prop_drop_intent.cpp:70-89) records what a
// desynced pair costs -- "a consume that dropped only the set desynced them, a re-parked same-key
// prop then had two FIFO copies, and after enough cycles the evict popped a stale copy and erased
// the live entry, so the place stopped syncing". Every mutation here touches both.
class CustodyParkStore {
public:
    struct Entry {
        CustodyKeyT id;
        CustodyRecordSet set;
        int      authorSlot = kNoAuthorSlot;
        uint32_t authorGen = 0;
        uint32_t worldGen = 0;
        TimePoint at{};
        bool     armed = false;
        void*    actor = nullptr;   // set at Arm; the consume finds its park by this
        int      adoptTries = 0;    // custody Ticks this ARMED park has waited for its eid
    };

    // Capture. False on every refusal; `out` always names the outcome.
    bool Park(CustodyKeyT id, CustodyRecordSet set, int authorSlot, uint32_t authorGen,
               uint32_t worldGen, TimePoint now, Reason& out);

    // PARKED -> ARMED. The six arm conditions, in that order.
    //
    // Arm condition 4 (the intent key equals park.key) is structurally satisfied here: the store is
    // keyed BY that key, so a mismatched key surfaces as NotParked, not KeyMismatch. There is
    // therefore no arm-KeyMismatch assertion in the harness and no omission row asks for one --
    // stated rather than left to look like coverage.
    bool Arm(const CustodyKeyT& id, int spawnSlot, uint32_t spawnGen, uint32_t worldGen,
             void* actor, TimePoint now, Reason& out);

    // ARMED -> APPLIED. `enrolledKey` is the adopted actor's key AFTER MarkPropElement may have
    // re-keyed it, and `observedCls` is R::ClassNameOf(actor); both are read by the caller because
    // both are engine reads. Consume rows 1, 2 and 5 (actor-dead, eid-unresolved,
    // not-container) are likewise the caller's, and are refused before this is reached.
    bool TryConsume(void* actor, uint32_t eid, void* inv,
                    const std::wstring& enrolledKey, const std::wstring& observedCls,
                    const SlotIo& io, TimePoint now, Reason& out);

    // Every-Tick sweep. `slotConnected` and `slotGen` are kMaxPeers-long snapshots of
    // Session::IsSlotConnected and Session::peerGenerationForSlot.
    void Sweep(TimePoint now, uint32_t worldGen, const bool* slotConnected, const uint32_t* slotGen);

    // Session teardown. Empties both containers AND zeroes every counter: nothing survives a
    // session, including the numbers.
    void Clear();

    // ---- observability sink ----
    // Null in the harness; the production module points it at the host log. It exists because the
    // EXPIRED / EVICTED / CLEARED lines must name the KEY and both generations, and only the store
    // knows which entry left and why. A sweep that retires silently is a store whose bounds nobody
    // can measure -- a parksExpired count with no matching line is exactly the destroy-for-good
    // residual that has to stay observable.
    struct Event {
        const wchar_t* key = nullptr;
        Reason reason = Reason::Applied;
        uint32_t was = 0;   // the park's own stamp (world or author generation), where one applies
        uint32_t now = 0;   // what the sweep observed instead
        size_t   cap = 0;   // the park cap, on an eviction
    };
    void (*onRetire)(const Event&) = nullptr;

    // ---- the ARMED set, for the Tick that waits on the adoption ----
    std::vector<void*> ArmedActors() const;
    const Entry* FindByActor(void* actor) const;
    // Count one Tick in which this armed park's eid did not resolve. True once the park has waited
    // longer than kMaxAdoptTries, which is the caller's cue to refuse eid-unresolved.
    bool NoteAdoptMiss(void* actor);
    // Retire an ARMED park the caller refused on its own. Consume rows 1, 2 and 5 are engine
    // reads and belong to the caller, so their refusals are counted here rather than inside
    // TryConsume.
    void RetireArmed(void* actor, Reason why);

    // --- read-only inspection (the harness needs the map/FIFO pairing to be observable) ---
    size_t Size() const { return byKey_.size(); }
    bool Has(const std::wstring& key) const { return byKey_.find(key) != byKey_.end(); }
    size_t FifoCount(const std::wstring& key) const;
    std::vector<std::wstring> FifoKeys() const { return {fifo_.begin(), fifo_.end()}; }
    const Entry* Find(const std::wstring& key) const;

    // --- counters ---
    size_t parksTaken = 0;
    size_t parksArmed = 0;
    size_t parksApplied = 0;
    size_t parksDiscarded = 0;
    size_t parksExpired = 0;
    size_t parksEvicted = 0;
    size_t parksReplaced = 0;
    size_t nestedSentinelAccepted = 0;
    size_t parksRefused[static_cast<size_t>(Reason::kCount)] = {};
    size_t parksRefusedAtArm[static_cast<size_t>(Reason::kCount)] = {};
    size_t parksRefusedAtApply[static_cast<size_t>(Reason::kCount)] = {};
    size_t parksCleared[static_cast<size_t>(Reason::kCount)] = {};

    // --- the apply-refused log row's `nestedIndex=%d` and `others=%zu` diagnostic fields ---
    // Only TryConsume computes them. They live here rather than in its signature so that a caller's
    // own refusals (rows 1, 2 and 5, which never reach TryConsume) print the honest -1 / 0, never a
    // value borrowed from an earlier consume. Both are RESET at the top of every TryConsume and by
    // Clear(); read them IMMEDIATELY after the call -- a scratchpad, not durable state.
    //   lastNestedIndex      : the effective index E that aliased, on a NestedSlotAliased refusal.
    //                          -1 on every other outcome, including the OUTER SlotAliased refusal,
    //                          where the aliasing index is the container's own and not a nested one.
    //   lastEnumerationSize  : n, the number of OTHER world-container indices this consume was
    //                          tested against; 0 until the enumeration is taken, so an
    //                          enumeration-unproven refusal correctly reports 0.
    int32_t lastNestedIndex = -1;
    size_t  lastEnumerationSize = 0;

    // The write the consume performed, for the harness's "write counter 0" assertions. The
    // production binding's own write counter is SlotIo::WriteRecords; this is the store's record of
    // having called it.
    size_t writes = 0;
    size_t rederives = 0;

private:
    void Retire_(const std::wstring& key);
    static size_t Ix_(Reason r) { return static_cast<size_t>(r); }

    std::map<std::wstring, Entry> byKey_;
    std::deque<std::wstring> fifo_;   // oldest first; holds exactly byKey_'s keys
};

// ---- inline definitions -------------------------------------------------------------------------

inline const char* ReasonText(Reason r) {
    switch (r) {
        case Reason::Applied:                return "applied";
        case Reason::DiscardedFreshNotEmpty: return "fresh-not-empty";
        case Reason::NotParked:              return "not-parked";
        case Reason::AlreadyArmed:           return "already-armed";
        case Reason::AuthorMismatch:         return "author-mismatch";
        case Reason::AuthorReplaced:         return "author-replaced";
        case Reason::ClassMismatch:          return "class-mismatch";
        case Reason::KeyMismatch:            return "key-mismatch";
        case Reason::KeyRekeyed:             return "key-rekeyed";
        case Reason::WorldChanged:           return "world-changed";
        case Reason::AuthorLeft:             return "author-left";
        case Reason::Boundary1:              return "boundary1";
        case Reason::SlotUnresolved:         return "slot-unresolved";
        case Reason::SlotAliased:            return "slot-aliased";
        case Reason::NestedSlotAliased:      return "nested-slot-aliased";
        case Reason::EnumerationUnproven:    return "enumeration-unproven";
        case Reason::AllocPreflight:         return "alloc-preflight";
        case Reason::Expired:                return "expired";
        case Reason::Evicted:                return "evicted";
        case Reason::AuthorUnknown:          return "author-unknown";
        case Reason::SlotOutOfRange:         return "slot-out-of-range";
        case Reason::KeyEmpty:               return "key-empty";
        case Reason::CapExceeded:            return "cap-512";
        case Reason::OverBlobCeiling:        return "over-blob-ceiling";
        case Reason::AllocFailed:            return "alloc-failed";
        case Reason::Replaced:               return "replaced";
        case Reason::ActorDead:              return "actor-dead";
        case Reason::EidUnresolved:          return "eid-unresolved";
        case Reason::NotContainer:           return "not-container";
        case Reason::Session:                return "session";
        case Reason::kCount:                 break;
    }
    return "?";
}

inline size_t CustodyParkStore::FifoCount(const std::wstring& key) const {
    size_t n = 0;
    for (const auto& k : fifo_) {
        if (k == key) ++n;
    }
    return n;
}

inline const CustodyParkStore::Entry* CustodyParkStore::Find(const std::wstring& key) const {
    auto it = byKey_.find(key);
    return it == byKey_.end() ? nullptr : &it->second;
}

// The ONE retire. Both containers, always, in one operation.
inline void CustodyParkStore::Retire_(const std::wstring& key) {
    byKey_.erase(key);
    for (auto it = fifo_.begin(); it != fifo_.end();) {
        it = (*it == key) ? fifo_.erase(it) : it + 1;
    }
}

inline std::vector<void*> CustodyParkStore::ArmedActors() const {
    std::vector<void*> out;
    for (const auto& kv : byKey_) {
        if (kv.second.armed && kv.second.actor) out.push_back(kv.second.actor);
    }
    return out;
}

inline const CustodyParkStore::Entry* CustodyParkStore::FindByActor(void* actor) const {
    if (!actor) return nullptr;
    for (const auto& kv : byKey_) {
        if (kv.second.armed && kv.second.actor == actor) return &kv.second;
    }
    return nullptr;
}

inline bool CustodyParkStore::NoteAdoptMiss(void* actor) {
    if (!actor) return false;
    for (auto& kv : byKey_) {
        if (kv.second.armed && kv.second.actor == actor) {
            ++kv.second.adoptTries;
            return kv.second.adoptTries > kMaxAdoptTries;
        }
    }
    return false;
}

inline void CustodyParkStore::RetireArmed(void* actor, Reason why) {
    const Entry* e = FindByActor(actor);
    if (!e) return;
    const std::wstring key = e->id.key;
    ++parksRefusedAtApply[Ix_(why)];
    if (onRetire) {
        Event ev;
        ev.key = key.c_str();
        ev.reason = why;
        onRetire(ev);
    }
    Retire_(key);
}

inline bool CustodyParkStore::Park(CustodyKeyT id, CustodyRecordSet set, int authorSlot,
                                    uint32_t authorGen, uint32_t worldGen, TimePoint now,
                                    Reason& out) {
    // Capture condition 2, both halves. "Nothing was latched" and "an out-of-range value was
    // latched" are DIFFERENT observations and are refused under different names: folding them is
    // exactly the defect of a gate that cannot fire.
    if (authorSlot == kNoAuthorSlot) {
        out = Reason::AuthorUnknown;
        ++parksRefused[Ix_(out)];
        return false;
    }
    if (authorSlot < 0 || authorSlot >= kMaxPeers) {
        out = Reason::SlotOutOfRange;
        ++parksRefused[Ix_(out)];
        return false;
    }
    // Capture condition 3. A keyless park could never be matched by an arm, and "None" is the
    // engine's empty FName spelling.
    if (id.key.empty() || id.key == L"None") {
        out = Reason::KeyEmpty;
        ++parksRefused[Ix_(out)];
        return false;
    }
    // Capture condition 8.
    if (set.recs.size() > kMaxRecordsPerContainer) {
        out = Reason::CapExceeded;
        ++parksRefused[Ix_(out)];
        return false;
    }
    // Capture condition 9, and it measures the FAN-OUT pack, not the host-local one: BroadcastContainer
    // re-packs with nested indices NEUTERED, and NeuterNestedIndex resizes an empty ints[] to one
    // element before writing -1, so the wire pack can be LARGER than the host-local one. A set the
    // host could park but never publish would be restored into a container no peer would ever see.
    if (set.fanoutBytes > MaxFanoutBytes()) {
        out = Reason::OverBlobCeiling;
        ++parksRefused[Ix_(out)];
        return false;
    }

    // One row per key: latest wins, logged, never accumulating. The replaced set is dropped, which
    // is today's outcome for both containers, so the replacement is not a regression.
    if (byKey_.find(id.key) != byKey_.end()) {
        if (onRetire) {
            Event ev;
            ev.key = id.key.c_str();
            ev.reason = Reason::Replaced;
            onRetire(ev);
        }
        Retire_(id.key);
        ++parksReplaced;
    } else if (byKey_.size() >= kMaxCustodyParks) {
        // Overflow evicts the OLDEST. Both containers, so the pair cannot desync.
        const std::wstring victim = fifo_.front();
        if (onRetire) {
            Event ev;
            ev.key = victim.c_str();
            ev.reason = Reason::Evicted;
            ev.cap = kMaxCustodyParks;
            onRetire(ev);
        }
        Retire_(victim);
        ++parksEvicted;
    }

    Entry p;
    p.id = std::move(id);
    p.set = std::move(set);
    p.authorSlot = authorSlot;
    p.authorGen = authorGen;
    p.worldGen = worldGen;
    p.at = now;
    p.armed = false;
    const std::wstring key = p.id.key;
    fifo_.push_back(key);
    byKey_.emplace(key, std::move(p));
    ++parksTaken;
    out = Reason::Applied;
    return true;
}

inline bool CustodyParkStore::Arm(const CustodyKeyT& id, int spawnSlot, uint32_t spawnGen,
                                  uint32_t worldGen, void* actor, TimePoint /*now*/, Reason& out) {
    auto it = byKey_.find(id.key);
    if (it == byKey_.end()) {
        out = Reason::NotParked;
        ++parksRefusedAtArm[Ix_(out)];
        return false;
    }
    Entry& p = it->second;

    // Condition 5 first among the failures that must NOT retire. An already-ARMED park is no longer
    // in PARKED, so the rule "every exit from PARKED retires the park" does not reach it: a
    // duplicated PropDropIntent must not destroy a park that is already waiting for its adoption.
    // Every OTHER arm refusal is an exit from PARKED and does retire.
    if (p.armed) {
        out = Reason::AlreadyArmed;
        ++parksRefusedAtArm[Ix_(out)];
        return false;
    }
    if (spawnSlot != p.authorSlot) {
        out = Reason::AuthorMismatch;
        ++parksRefusedAtArm[Ix_(out)];
        Retire_(id.key);
        return false;
    }
    if (spawnGen != p.authorGen) {
        out = Reason::AuthorReplaced;
        ++parksRefusedAtArm[Ix_(out)];
        Retire_(id.key);
        return false;
    }
    if (id.cls != p.id.cls) {
        out = Reason::ClassMismatch;
        ++parksRefusedAtArm[Ix_(out)];
        Retire_(id.key);
        return false;
    }
    if (worldGen != p.worldGen) {
        out = Reason::WorldChanged;
        ++parksRefusedAtArm[Ix_(out)];
        Retire_(id.key);
        return false;
    }

    p.armed = true;
    p.actor = actor;
    p.adoptTries = 0;
    ++parksArmed;
    out = Reason::Applied;
    return true;
}

inline bool CustodyParkStore::TryConsume(void* actor, uint32_t eid, void* inv,
                                         const std::wstring& enrolledKey,
                                         const std::wstring& observedCls,
                                         const SlotIo& io, TimePoint /*now*/, Reason& out) {
    // The two diagnostic fields, reset FIRST so that no refusal below can report a
    // pair left behind by an earlier consume. Every `return false` between here and the enumeration
    // therefore reports -1 / 0, which is the truth for it: it refused before either was computed.
    lastNestedIndex = -1;
    lastEnumerationSize = 0;
    // The consume finds its park by the ADOPTED ACTOR, which is what Arm recorded; the key cannot be
    // the lookup here, because row 3 exists precisely for the case where the key changed.
    std::wstring key;
    Entry* p = nullptr;
    for (auto& kv : byKey_) {
        if (kv.second.armed && kv.second.actor == actor && actor != nullptr) {
            key = kv.first;
            p = &kv.second;
            break;
        }
    }
    if (!p) {
        out = Reason::NotParked;
        ++parksRefusedAtApply[Ix_(out)];
        return false;
    }

    // Row 3. MarkPropElement re-keys only when another LIVE actor already carried the key
    // (prop_element_tracker.cpp:253-267) -- i.e. exactly the measured same-key collision family.
    // Writing the parked contents into this container would be provably guessing.
    if (enrolledKey != p->id.key) {
        out = Reason::KeyRekeyed;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 4.
    if (observedCls != p->id.cls) {
        out = Reason::ClassMismatch;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 6, Boundary 1. A Player == true (or unresolvable) inventory is refused everywhere,
    // always: a violation destroys a player's own inventory.
    if (!io.IsWorldContainer || !io.IsWorldContainer(inv)) {
        out = Reason::Boundary1;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 7.
    int32_t idx = -1;
    if (!io.ReadSlotIndex || !io.ReadSlotIndex(inv, idx)) {
        out = Reason::SlotUnresolved;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 8. GObjStack[0] IS the host player's own inventory by construction (inventory.h:63-72),
    // and GObjStackSlot accepts index 0, so aliasing is possible and must be refused explicitly.
    if (idx == 0) {
        out = Reason::SlotAliased;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 9. The personal store's index is READ, not assumed to be 0; an unreadable one refuses,
    // because an unprovable non-alias is not a non-alias.
    int32_t personal = -1;
    const bool personalOk = io.PersonalStoreIndex && io.PersonalStoreIndex(personal);
    if (!personalOk || idx == personal) {
        out = Reason::SlotAliased;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }

    // Row 10, in its two calls. The count-only call sizes the buffer; the fill call answers the
    // question. Clause 3 is scoped to the FILL call: with the store's own headroom, want on the
    // count-only call is 0 and n == want would read 0 == 0 in every ordinary world.
    if (!io.OtherWorldContainerIndices) {
        out = Reason::EnumerationUnproven;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    bool completeCount = false;
    const size_t need = io.OtherWorldContainerIndices(actor, eid, nullptr, 0, completeCount);
    if (!completeCount) {
        out = Reason::EnumerationUnproven;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    const size_t want = need + 1;   // HEADROOM. With want == need, a world in which every
                                    // enrolled prop is a world container -- an ordinary game world --
                                    // returns n == want and refuses on a correct build.
    std::vector<int32_t> others(want, 0);
    bool completeFill = false;
    const size_t n = io.OtherWorldContainerIndices(actor, eid, others.data(), want, completeFill);
    if (!completeFill || n == want) {
        // n == want on the FILL call means the second walk qualified MORE entries than the sizing
        // snapshot contained at all, i.e. the registry grew between the two calls. That is the only
        // way this enumeration can genuinely truncate, and an unproven absence is not an absence.
        out = Reason::EnumerationUnproven;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    others.resize(n <= want ? n : want);
    // The log row's `others=%zu`: the enumeration this consume is actually being tested against, recorded
    // the moment it is known and therefore correct on the outer alias refusal below as well.
    lastEnumerationSize = others.size();
    for (int32_t other : others) {
        if (other == idx) {
            out = Reason::SlotAliased;
            ++parksRefusedAtApply[Ix_(out)];
            Retire_(key);
            return false;
        }
    }

    // Rows 11 and 12, BEFORE any write. Row 12 needs no separate test: the enumeration verdict was
    // taken above and refuses there, so the nested loop is never reached on an unproven enumeration.
    // That is what "row 12 is row 10's enumeration verdict, reused" means in code.
    const int32_t stackLen = io.GObjStackLength ? io.GObjStackLength() : 0;
    size_t sentinels = 0;
    size_t nested = 0;
    for (const Rec& r : p->set.recs) {
        if (!io.IsNestedContainerRecord || !io.IsNestedContainerRecord(r)) continue;
        // The EFFECTIVE index the engine would read, not the one the record literally stores:
        // prop_container::loadData reads ints[0][0] unguarded and Array_Get zero-fills an
        // out-of-range read, so an absent or empty ints[] IS index 0 (Boundary 2's own comment,
        // container_contents_sync.cpp:252-257).
        const int32_t E = (r.ints.empty() || r.ints[0].empty()) ? 0 : r.ints[0][0];
        if (E < 0) {
            // The CDO sentinel propInventory::init's `index >= 0` guard is written against: the
            // nested container restores EMPTY, which is exactly what the unfixed build gives, and is
            // not a wrong write. Counted so the frequency is observable rather than invisible.
            ++sentinels;
            continue;
        }
        bool aliased = (E == 0) || (E == personal) || (E == idx) || (E >= stackLen);
        if (!aliased) {
            for (int32_t other : others) {
                if (other == E) { aliased = true; break; }
            }
        }
        if (aliased) {
            // The WHOLE set is refused. Never a partial set, never a repaired set, never a silently
            // index-rewritten one: the outer box then stays empty, exactly as the unfixed build
            // leaves it.
            //
            // The log row's `nestedIndex=%d`: E is the EFFECTIVE index -- what prop_container::loadData
            // would actually have read -- not the one the record literally stores, so the number in
            // the log is the number that would have done the damage.
            lastNestedIndex = E;
            out = Reason::NestedSlotAliased;
            ++parksRefusedAtApply[Ix_(out)];
            Retire_(key);
            return false;
        }
        ++nested;
    }

    // Row 13. Never overwrite newer state: if something already restored contents, discard with a
    // log line and no write.
    const int32_t freshCount = io.ReadSlotCount ? io.ReadSlotCount(inv) : -1;
    if (freshCount != 0) {
        out = Reason::DiscardedFreshNotEmpty;
        ++parksDiscarded;
        Retire_(key);
        return false;
    }
    // Row 14.
    if (!io.AllocPreflight || !io.AllocPreflight()) {
        out = Reason::AllocPreflight;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    // Row 15.
    if (!io.WriteRecords || !io.WriteRecords(inv, p->set)) {
        out = Reason::AllocFailed;
        ++parksRefusedAtApply[Ix_(out)];
        Retire_(key);
        return false;
    }
    ++writes;
    if (io.Rederive && io.Rederive(actor, inv)) ++rederives;
    nestedSentinelAccepted += sentinels;
    (void)nested;
    ++parksApplied;
    Retire_(key);
    out = Reason::Applied;
    return true;
}

inline void CustodyParkStore::Sweep(TimePoint now, uint32_t worldGen, const bool* slotConnected,
                                    const uint32_t* slotGen) {
    std::vector<Event> doomed;
    std::vector<std::wstring> keys;
    for (const auto& kv : byKey_) {
        const Entry& p = kv.second;
        Event ev;
        ev.was = p.worldGen;
        ev.now = worldGen;
        if (p.worldGen != worldGen) {
            // Generation() bumps on ANY observed CurrentWorld change, a transient null included, so
            // this is fail-closed and correct AND a coverage loss. It is counted, and it names both
            // generations, which is what makes that loss measurable rather than invisible.
            ev.reason = Reason::WorldChanged;
            keys.push_back(kv.first);
            doomed.push_back(ev);
            continue;
        }
        const bool slotOk = p.authorSlot >= 0 && p.authorSlot < kMaxPeers;
        if (!slotOk || !slotConnected || !slotConnected[p.authorSlot]) {
            ev.reason = Reason::AuthorLeft;
            ev.was = p.authorGen;
            ev.now = 0;
            keys.push_back(kv.first);
            doomed.push_back(ev);
            continue;
        }
        // The disconnect-and-replacement race a liveness sweep alone loses: same slot, different
        // occupant. Closed by construction rather than by sweep timing.
        if (!slotGen || slotGen[p.authorSlot] != p.authorGen) {
            ev.reason = Reason::AuthorReplaced;
            ev.was = p.authorGen;
            ev.now = slotGen ? slotGen[p.authorSlot] : 0;
            keys.push_back(kv.first);
            doomed.push_back(ev);
            continue;
        }
        const auto age = std::chrono::duration_cast<std::chrono::seconds>(now - p.at).count();
        if (age > kCustodyParkCeilingSec) {
            ev.reason = Reason::Expired;
            ev.was = static_cast<uint32_t>(age);
            ev.now = static_cast<uint32_t>(kCustodyParkCeilingSec);
            keys.push_back(kv.first);
            doomed.push_back(ev);
        }
    }
    for (size_t i = 0; i < doomed.size(); ++i) {
        if (onRetire) {
            Event ev = doomed[i];
            ev.key = keys[i].c_str();
            onRetire(ev);
        }
        Retire_(keys[i]);
        if (doomed[i].reason == Reason::Expired) {
            ++parksExpired;
        } else {
            ++parksCleared[Ix_(doomed[i].reason)];
        }
    }
}

inline void CustodyParkStore::Clear() {
    // `session` is one of the CLEARED reasons, and a store that empties itself
    // silently is a store whose held parks nobody can count from the log. Every entry that was
    // still held at teardown gets its own CLEARED line, ahead of the erase, for the same reason the
    // sweep's EXPIRED / EVICTED / REPLACED lines exist: only the store knows which entry left.
    //
    // The counters are zeroed below in this same call, so the LINE is the record of a session
    // clear, not `parksCleared[Session]`. The caller logs the counters BEFORE calling this
    // (container_custody.cpp's teardown path) precisely because this call destroys them.
    if (onRetire) {
        for (const auto& kv : byKey_) {
            Event ev;
            ev.key = kv.first.c_str();
            ev.reason = Reason::Session;
            ev.was = kv.second.worldGen;
            ev.now = kv.second.authorGen;
            onRetire(ev);
        }
    }
    byKey_.clear();
    fifo_.clear();
    parksTaken = parksArmed = parksApplied = parksDiscarded = 0;
    parksExpired = parksEvicted = parksReplaced = nestedSentinelAccepted = 0;
    writes = rederives = 0;
    lastNestedIndex = -1;
    lastEnumerationSize = 0;
    for (size_t i = 0; i < static_cast<size_t>(Reason::kCount); ++i) {
        parksRefused[i] = 0;
        parksRefusedAtArm[i] = 0;
        parksRefusedAtApply[i] = 0;
        parksCleared[i] = 0;
    }
}

// ---- the production module (container_custody.cpp) ----------------------------------------------
// Declarations only, so everything above stays header-only and standalone-compilable. The module
// holds its own session pointer and SELF-GATES on role and connection, which is why each of the
// three call sites below is ONE unconditional line: remote_prop_destroy.cpp has no IsHost() and no
// role() accessor at all (MEASURED -- `git grep 'IsHost|role()|Role::'` over that file returns
// exit 1), so an `if (IsHost()) ...` three-liner cannot be written there.

void Install(coop::net::Session* session);

// Latch the peer slot whose PropDestroy is being dispatched. PropDestroyPayload carries no sender
// field (protocol.h:1088-1093) and remote_prop::OnDestroy takes none, so this is the only scope
// that has one. Takes `int` and RANGE-CHECKS it against [0, kMaxPeers): Session::ReliableMessage
// ::senderPeerSlot is `int senderPeerSlot = -1` (session.h:95), and narrowing to uint8_t would
// latch -1 as slot 255, making the author-unknown refusal impossible to observe -- a gate that
// cannot fire passes forever (docs/coop-sync-doctrine.md:125-126).
void NoteInboundDestroySlot(int senderPeerSlot);
void ClearInboundDestroySlot();

// The latch is SCOPE-BOUNDED. OnDestroyImpl_ returns before the capture on three paths
// (remote_prop_destroy.cpp:154-161, :182-188, :196-228), so a bare latch outlives its message on
// all three. Harmless on today's call graph -- the next PropDestroy overwrites it before any
// capture can read it -- but a guard makes author-unknown an invariant rather than a coincidence.
struct ScopedInboundDestroySlot {
    explicit ScopedInboundDestroySlot(int senderPeerSlot) {
        NoteInboundDestroySlot(senderPeerSlot);
    }
    ~ScopedInboundDestroySlot() { ClearInboundDestroySlot(); }
    ScopedInboundDestroySlot(const ScopedInboundDestroySlot&) = delete;
    ScopedInboundDestroySlot& operator=(const ScopedInboundDestroySlot&) = delete;
};

// The capture, called unconditionally from DestroyResolvedLocalActor_ AFTER
// native_pile_mirror::Unpin and BEFORE the K2_DestroyActor call: the actor is dereferenced live
// through that whole window.
void CaptureForDyingContainer(void* actor, const std::wstring& payloadKey);

// The arm, called from OnPropDropIntent's `if (actor)` block once HostSpawnPlacedProp has returned a
// live actor; no element id exists at that point yet.
void NoteHostSpawnForIntent(void* actor, const std::wstring& key, const std::wstring& cls,
                            int senderSlot);

// The sweep and the consume, registered immediately AFTER host_spawn_watcher::DrainPendingSpawns
// (which is where the eid is minted) and before prop_drop_intent::Tick.
void Tick(coop::net::Session* session);

// The personal-inventory check's predicate, under the already-shipped container_selftest=1 gate: would this
// inventory be parked, and if not, why. Read-only; reaches no writer.
bool WouldParkForInventory(void* inv, Reason& out);

void OnDisconnect();

}  // namespace coop::props::container_custody
