// coop/props/container_custody.cpp -- see coop/props/container_custody.h.
//
// The ENGINE BINDING of that header's pure policy: the session holder and the Enabled() self-gate,
// the production SlotIo over container_contents_sync's public wrappers, the three seam entries, the
// Tick that sweeps and consumes, and every host log row. The decidable logic lives in the header and
// is driven headless by tests/standalone/candidate_15_container_custody.cpp; nothing here re-decides it.
//
// What only this file can get wrong, and no headless harness can see: whether the SlotIo table below
// really points at the shipped IsWorldContainerInventory / GObjStackSlot / EngineAlloc /
// RederiveManagedState, and whether OtherWorldContainerIndices really performs the actor/eid
// self-exclusion and the pairs.size() + 1 sizing. Review the enumerator first.
//
// Game thread throughout: every entry point is reached from the net pump or a dispatch it drives.

#include "coop/props/container_custody.h"

#include "coop/element/registry.h"
#include "coop/net/session.h"
#include "coop/props/container_contents_sync.h"
#include "ue_wrap/actors/inventory.h"     // ReadLivePersonalStore -- row 9 reads the index
#include "ue_wrap/actors/prop.h"          // GetInteractableKeyString / WalksToBase
#include "ue_wrap/core/log.h"
#include "ue_wrap/core/reflection.h"      // ClassNameOf / IsLiveByIndex / InternalIndexOf / EngineAlloc
#include "ue_wrap/engine/world_identity.h"

#include <atomic>
#include <string>
#include <vector>

namespace coop::props::container_custody {
namespace {

namespace R  = ue_wrap::reflection;
namespace CS = coop::props::container_contents_sync;

// The header spells the peer bound itself so it stays standalone-compilable. This is the coupling
// that keeps that spelling honest, exactly as coop/session/join_seed.h:45 promises for its own.
static_assert(kMaxPeers == static_cast<int>(coop::net::kMaxPeers),
              "container_custody.h's kMaxPeers must equal net::kMaxPeers");

std::atomic<coop::net::Session*> g_session{nullptr};

CustodyParkStore g_store;

// The author-slot latch. kNoAuthorSlot is "nothing was latched", which is a DIFFERENT observation
// from "an out-of-range value was latched" and is refused under a different name.
int g_latchedSlot = kNoAuthorSlot;

// The lane-alive lines, one-shot, each emitted as the FIRST statement of its function -- ahead of
// every filter and ahead of Enabled() -- so each proves its entry point was REACHED. Two prefixes
// deliberately: a fixed-string grep for "[CUSTODY]" cannot match "[CUSTODY-INIT]", because the
// character after "[CUSTODY" is '-' and not ']'. That is what lets a log check assert a clean operational
// zero WITHOUT asserting the lane is dead.
bool g_saidCaptureAlive = false;
bool g_saidTickAlive = false;

const char* RoleText(coop::net::Session* s) {
    if (!s) return "none";
    return s->role() == coop::net::Role::Host ? "Host" : "Client";
}

coop::net::Session* LoadSession() { return g_session.load(std::memory_order_acquire); }

// Capture condition 1. Silent: the module is simply not installed for this role, and a refusal line
// per client-side destroy would be noise on every peer that can never hold a park.
bool Enabled(coop::net::Session** out = nullptr) {
    coop::net::Session* s = LoadSession();
    if (!s || !s->connected() || s->role() != coop::net::Role::Host) return false;
    if (out) *out = s;
    return true;
}

// The key is stored WIRE-NORMALISED, the same truncation both senders already apply
// (prop_destroy_seam.cpp's 31-char wire key loop; FillWireStr's N - 1 cap at prop_drop_intent.cpp:93-96). An
// implementer who stores the untruncated actor key silently breaks every comparison against a wire
// key. Normalise, then compare.
constexpr size_t kWireKeyMax = 31;
std::wstring WireNormalise(const std::wstring& k) {
    return k.size() > kWireKeyMax ? k.substr(0, kWireKeyMax) : k;
}

// ---- the production SlotIo -----------------------------------------------------------------------
// Every entry delegates to container_contents_sync's public wrapper over that lane's own private
// reader or writer. Nothing here re-implements a boundary, a codec or an apply block.

bool IoIsWorldContainer(void* inv) { return CS::IsWorldContainer(inv); }

bool IoReadSlotIndex(void* inv, int32_t& out) { return CS::WorldContainerSlotIndex(inv, out); }

int32_t IoReadSlotCount(void* inv) {
    int32_t n = -1;
    if (!CS::WorldContainerRecordCount(inv, n)) return -1;   // unresolvable refuses, fail-closed
    return n;
}

// The same two calls ApplyContents makes at container_contents_sync.cpp:495-500, whose comment
// states that without them "an empty array would silently replace real contents".
bool IoAllocPreflight() {
    if (void* probe = R::EngineAlloc(16)) {
        R::EngineFree(probe);
        return true;
    }
    return false;
}

// container_contents_sync's write wrapper addresses the container by ACTOR, because the Boundary 1
// re-check belongs with the write and the actor is what carries the class. SlotIo hands the store's
// callback the COMPONENT, so the consume publishes the actor here first. Set immediately before
// TryConsume and cleared immediately after: both happen inside one Tick, on the game thread, with
// no dispatch between them.
void* g_writeActor = nullptr;

bool IoWriteRecords(void* /*inv*/, const CustodyRecordSet& set) {
    if (!g_writeActor) return false;
    return CS::WriteWorldContainerRecords(g_writeActor, set.recs);
}

bool IoRederive(void* owner, void* /*inv*/) { return CS::RederiveContainerManagedState(owner); }

bool IoPersonalStoreIndex(int32_t& out) {
    // Row 9 READS the personal store's index rather than assuming 0. inventory.h:63-72 records that
    // the class template serialises index=0, but "measured to be 0 by construction" is a
    // measurement of one build, not a licence to hard-code it -- and this reader is fail-closed on
    // Player == 0 at inventory.cpp:155-165, which is the assertion that makes the index meaningful.
    //
    // COST, stated: ReadLivePersonalStore also copies the store's whole record list (:178-183),
    // which this caller throws away. Once per consume -- and a consume happens at most once per
    // drop intent, not per frame -- so it is paid rather than optimised, because the alternative is
    // a second reader of the same walk, and the walk has one implementation.
    ue_wrap::inventory::LivePersonalStore store;
    if (!ue_wrap::inventory::ReadLivePersonalStore(store)) return false;
    out = store.slotIndex;
    return true;
}

int32_t IoGObjStackLength() {
    int32_t n = 0;
    if (!CS::GObjStackLength(n)) return 0;   // 0 refuses every non-sentinel nested index
    return n;
}

bool IoIsNestedContainerRecord(const Rec& r) { return CS::IsNestedContainerRecord(r); }

// ---- the enumeration seam (consume row 10, clauses 1 and 2) ------------------------------------
// Three things must hold here, and the standalone harness can prove none of them:
//   1. SELF-EXCLUSION, BY BOTH IDENTIFIERS, HERE. At consume time the adopted container is live,
//      walks to the container base and passes Boundary 1, so it APPEARS IN ITS OWN SNAPSHOT
//      (container_contents_sync.cpp:794-798); unexcluded, every consume refuses slot-aliased, a
//      logged no-op that passes every standalone assertion. Matching actor pointer AND eid survives
//      losing either; excluding by INDEX would make row 10 vacuously true and re-open fail-open.
//   2. HEADROOM. SnapshotWorldContainers writes at most one entry per pair of its own snapshot and
//      stops at `if (n >= want) break;` (:792), so a buffer of exactly pairs.size() fills whenever
//      every enrolled Prop element is a world container. With one slot spare, a full buffer can only
//      mean the registry grew between the two walks -- the only genuine truncation.
//   3. outComplete FROM THE RAW RETURN, before exclusion: excluding never masks a full buffer.
// The want == 0 count-only convention belongs to THIS seam. SnapshotWorldContainers refuses want == 0
// outright at :785, so the scratch below is always non-null and non-empty and only the return to the
// store is suppressed.
size_t IoOtherWorldContainerIndices(void* selfActor, uint32_t selfEid, int32_t* out, size_t want,
                                    bool& outComplete) {
    outComplete = false;

    std::vector<coop::element::Registry::ActorIdPair> pairs;
    coop::element::Registry::Get().SnapshotActorsByType(coop::element::ElementType::Prop, pairs);
    const size_t scratchWant = pairs.size() + 1;   // clause 2's headroom
    std::vector<CS::WorldContainer> scratch(scratchWant);
    const size_t raw = CS::SnapshotWorldContainers(scratch.data(), scratchWant);

    // Clause 3, on the RAW return: a full scratch means the registry grew between the two walks.
    outComplete = (raw < scratchWant);
    if (!outComplete) return 0;

    size_t n = 0;
    for (size_t i = 0; i < raw; ++i) {
        const CS::WorldContainer& wc = scratch[i];
        if (wc.actor == selfActor || wc.eid == selfEid) continue;   // clause 1, BOTH identifiers
        int32_t idx = 0;
        if (!CS::WorldContainerSlotIndex(wc.inv, idx)) {
            // SKIPPED, not refused, and the distinction is load-bearing. WorldContainerSlotIndex
            // fails on exactly the two conditions GObjStackSlot fails on -- a negative Index (never
            // initialised) and an Index past the array -- so such a container ADDRESSES NO SLOT and
            // cannot alias the one we are about to write. Refusing the whole enumeration on it
            // would refuse every consume in any world holding one uninitialised container, which is
            // the headroom defect again: a gate that fails the ordinary case on a correct build.
            continue;
        }
        if (out) {
            if (n >= want) break;   // the store's own buffer filled; it refuses on n == want
            out[n] = idx;
        }
        ++n;
    }
    return n;
}

SlotIo MakeProductionIo() {
    SlotIo io;
    io.IsWorldContainer = &IoIsWorldContainer;
    io.ReadSlotIndex = &IoReadSlotIndex;
    io.ReadSlotCount = &IoReadSlotCount;
    io.AllocPreflight = &IoAllocPreflight;
    io.WriteRecords = &IoWriteRecords;
    io.Rederive = &IoRederive;
    io.OtherWorldContainerIndices = &IoOtherWorldContainerIndices;
    io.PersonalStoreIndex = &IoPersonalStoreIndex;
    io.GObjStackLength = &IoGObjStackLength;
    io.IsNestedContainerRecord = &IoIsNestedContainerRecord;
    return io;
}

// ---- the observability sink ----------------------------------------------------------------------

void OnRetire(const CustodyParkStore::Event& ev) {
    const wchar_t* key = ev.key ? ev.key : L"";
    switch (ev.reason) {
        case Reason::Expired:
            UE_LOGI("[CUSTODY] HOST park EXPIRED key='%ls' after %us", key, ev.was);
            break;
        case Reason::Evicted:
            UE_LOGI("[CUSTODY] HOST park EVICTED key='%ls' (cap %zu)", key, ev.cap);
            break;
        case Reason::Replaced:
            UE_LOGI("[CUSTODY] HOST park REPLACED key='%ls' (latest wins)", key);
            break;
        default:
            UE_LOGI("[CUSTODY] HOST park CLEARED key='%ls' reason=%s was=%u now=%u",
                    key, ReasonText(ev.reason), ev.was, ev.now);
            break;
    }
}

bool g_sinkBound = false;
void BindSink() {
    if (g_sinkBound) return;
    g_store.onRetire = &OnRetire;
    g_sinkBound = true;
}

// The counters are readable from the log at teardown: parksTaken,
// parksRefused (by reason), parksApplied, parksDiscarded, parksExpired, parksEvicted,
// nestedSentinelAccepted. The store carried those counters from the first commit, but nothing in
// this module ever READ one, and Clear() zeroes them all -- so at the one moment they are meant to
// be read they were being destroyed unread. This is the read.
//
// It must run BEFORE Clear(), and every teardown path goes through TeardownParks_ below so that
// ordering cannot be got wrong at a call site.
//
// Refusal reasons are printed one line per reason that actually fired: a fixed table of ~30 zeroes
// per teardown would bury the two or three numbers that matter. A reason absent from the log fired
// zero times -- that is the reading, and it is stated here so the absence is not read as a gap.
void LogCounters(const char* at) {
    UE_LOGI("[CUSTODY] counters at=%s held=%zu taken=%zu armed=%zu applied=%zu discarded=%zu "
            "expired=%zu evicted=%zu replaced=%zu nestedSentinelAccepted=%zu writes=%zu",
            at, g_store.Size(), g_store.parksTaken, g_store.parksArmed, g_store.parksApplied,
            g_store.parksDiscarded, g_store.parksExpired, g_store.parksEvicted,
            g_store.parksReplaced, g_store.nestedSentinelAccepted, g_store.writes);
    for (size_t i = 0; i < static_cast<size_t>(Reason::kCount); ++i) {
        const size_t atArm = g_store.parksRefusedAtArm[i];
        const size_t atApply = g_store.parksRefusedAtApply[i];
        const size_t cleared = g_store.parksCleared[i];
        const size_t any = g_store.parksRefused[i];
        if (!atArm && !atApply && !cleared && !any) continue;
        UE_LOGI("[CUSTODY] counters at=%s refused reason=%s park=%zu arm=%zu apply=%zu swept=%zu",
                at, ReasonText(static_cast<Reason>(i)), any, atArm, atApply, cleared);
    }
}

// The ONE teardown path. Logs the counters, then Clear() -- which emits a CLEARED reason=session
// line per still-held park and only then destroys both the parks and the numbers.
void TeardownParks_(const char* at) {
    BindSink();   // OnDisconnect can be the first thing this module is asked to do
    LogCounters(at);
    g_store.Clear();
}

}  // namespace

// ---- the seams ------------------------------------------------------------------------------------

void Install(coop::net::Session* session) {
    g_session.store(session, std::memory_order_release);
    BindSink();
}

void NoteInboundDestroySlot(int senderPeerSlot) {
    if (senderPeerSlot < 0 || senderPeerSlot >= kMaxPeers) {
        g_latchedSlot = kNoAuthorSlot;
        UE_LOGW("[CUSTODY] HOST latch REFUSED reason=slot-out-of-range slot=%d", senderPeerSlot);
        return;
    }
    g_latchedSlot = senderPeerSlot;
}

void ClearInboundDestroySlot() { g_latchedSlot = kNoAuthorSlot; }

void CaptureForDyingContainer(void* actor, const std::wstring& payloadKey) {
    // FIRST STATEMENT, ahead of every filter and ahead of the self-gate: this line proves the
    // capture seam was REACHED. It is event-driven -- the hook is entered only when the host
    // RECEIVES a PropDestroy that resolved a local actor -- so it cannot fire on a wholly
    // host-authored chain, and it is scored by an in-game take-and-place, not by the Tick line.
    if (!g_saidCaptureAlive) {
        g_saidCaptureAlive = true;
        UE_LOGI("[CUSTODY-INIT] capture hook ENTERED for the first time on this peer (role=%s)",
                RoleText(LoadSession()));
    }
    coop::net::Session* s = nullptr;
    if (!Enabled(&s)) return;                       // condition 1, silent
    if (!actor) return;

    BindSink();

    const std::wstring cls = R::ClassNameOf(actor);
    const std::wstring ownKey = WireNormalise(ue_wrap::prop::GetInteractableKeyString(actor));
    const std::wstring wireKey = WireNormalise(payloadKey);

    auto refuse = [&](const char* reason) {
        UE_LOGW("[CUSTODY] HOST park REFUSED key='%ls' actor=%p cls='%ls' reason=%s",
                ownKey.c_str(), actor, cls.c_str(), reason);
    };

    // Condition 2. A capture with no latched slot is refused: fail-closed, and defence in depth
    // against a FUTURE caller. It is NOT what excludes the deferred quiescence path -- that path
    // does not run on a host at all (npc_mirror.cpp:543 returns on Host), and a contributor who
    // deletes the latch discipline as "an extra seam" must be able to see where the real protection
    // lies.
    if (g_latchedSlot == kNoAuthorSlot) { refuse("author-unknown"); return; }
    if (g_latchedSlot < 0 || g_latchedSlot >= kMaxPeers) { refuse("slot-out-of-range"); return; }
    // Condition 3. Also closes the kerfur_convert_client.cpp:311-313 synthetic destroy, whose
    // dp.key.len = 0; that entry is host-unreachable anyway (its caller returns unless
    // role() == Client at kerfur_convert_client.cpp:269), so this is defence in depth, not a live
    // hole.
    if (ownKey.empty() || ownKey == L"None") { refuse("key-empty"); return; }
    // Condition 4.
    if (ownKey != wireKey) { refuse("key-mismatch"); return; }
    // Condition 5.
    if (!CS::IsContainer(actor)) { refuse("not-container"); return; }
    // Condition 6, Boundary 1.
    void* inv = CS::InventoryOfContainer(actor);
    if (!inv || !CS::IsWorldContainer(inv)) { refuse("boundary1"); return; }
    // Condition 7.
    int32_t index = -1;
    if (!CS::WorldContainerSlotIndex(inv, index)) { refuse("slot-unresolved"); return; }

    // Condition 8 is enforced inside ReadContents, which refuses above the 512 cap at
    // container_contents_sync.cpp:275-279 and returns false; the store restates the same bound.
    CustodyRecordSet set;
    if (!CS::ReadWorldContainerRecords(actor, set.recs, /*neuterNested=*/false)) {
        refuse("cap-512");
        return;
    }
    set.index = index;
    set.hash = CS::ContentsHash(0, set.recs);
    // Condition 9 measures the FAN-OUT pack, not the host-local one: a set the host could park but
    // never publish would be restored into a container no peer would ever see.
    set.fanoutBytes = CS::FanoutPackBytes(set.recs);

    const uint32_t authorGen = s->peerGenerationForSlot(g_latchedSlot);
    const uint32_t worldGen = ue_wrap::world_identity::Generation();

    Reason why = Reason::Applied;
    const int records = static_cast<int>(set.recs.size());
    const size_t fanout = set.fanoutBytes;
    const uint64_t hash = set.hash;
    if (!g_store.Park(CustodyKeyT{ownKey, cls}, std::move(set), g_latchedSlot, authorGen, worldGen,
                      Clock::now(), why)) {
        refuse(ReasonText(why));
        return;
    }
    UE_LOGI("[CUSTODY] HOST parked key='%ls' cls='%ls' records=%d fanoutBytes=%zu hash=%llu "
            "index=%d slot=%d gen=%u worldgen=%u",
            ownKey.c_str(), cls.c_str(), records, fanout,
            static_cast<unsigned long long>(hash), index, g_latchedSlot, authorGen, worldGen);
}

void NoteHostSpawnForIntent(void* actor, const std::wstring& key, const std::wstring& cls,
                            int senderSlot) {
    if (!Enabled()) return;
    if (!actor) return;
    BindSink();

    const std::wstring k = WireNormalise(key);
    coop::net::Session* s = LoadSession();
    const uint32_t spawnGen = s ? s->peerGenerationForSlot(senderSlot) : 0;
    const uint32_t worldGen = ue_wrap::world_identity::Generation();

    const CustodyParkStore::Entry* before = g_store.Find(k);
    const int parkSlot = before ? before->authorSlot : -1;
    const uint32_t parkGen = before ? before->authorGen : 0u;

    Reason why = Reason::Applied;
    if (!g_store.Arm(CustodyKeyT{k, cls}, senderSlot, spawnGen, worldGen, actor, Clock::now(),
                     why)) {
        // NotParked is the ordinary case -- most placed props were never taken from a host
        // container -- so it is not a refusal line. Every other arm outcome IS one: without it, a game log
        // cannot tell an arm refusal from an arm hook that never ran, the silent-gate defect one
        // phase over.
        if (why != Reason::NotParked) {
            UE_LOGW("[CUSTODY] HOST park ARM REFUSED key='%ls' reason=%s spawnslot=%d parkslot=%d "
                    "spawngen=%u parkgen=%u",
                    k.c_str(), ReasonText(why), senderSlot, parkSlot, spawnGen, parkGen);
        }
        return;
    }
    UE_LOGI("[CUSTODY] HOST park ARMED key='%ls' actor=%p slot=%d gen=%u (awaiting adoption)",
            k.c_str(), actor, senderSlot, spawnGen);
}

void Tick(coop::net::Session* session) {
    // FIRST STATEMENT again. subsystems.cpp's per-tick block is unconditional and runs on both
    // roles, so this line fires in every session on a fixed build -- which is what makes it the
    // detector for a deleted Tick registration. What it proves is that the Tick is REGISTERED and
    // REACHED, NOT that the module is ENABLED; a take-and-place on the same build proves enablement.
    if (!g_saidTickAlive) {
        g_saidTickAlive = true;
        UE_LOGI("[CUSTODY-INIT] Tick ENTERED for the first time on this peer (role=%s)",
                RoleText(session));
    }
    // Belt and braces for the session clear: the module holds the Session* it was installed with
    // (g_session, stored by Install) and clears every park when that session is down at Tick entry,
    // so the clear does not depend on the DisconnectAll fan-out (coop/session/subsystems.cpp:425),
    // the only route to OnDisconnect below. Without it a missed fan-out left parks held, mitigated only
    // by authorGen never being reused (such a park clears author-replaced on the NEXT session's first
    // sweep). It must run BEFORE the connection filter below, which would otherwise swallow the case.
    // The INSTALLED pointer is never dereferenced unless it is `session`: a mismatch alone counts as
    // "not connected" and clears (more conservative than needed), so connected() is only called on the
    // pointer the caller holds live. Short-circuit order is load-bearing. Today both are one object --
    // net_pump::Tick(coop::net::Session&) [net_pump.cpp:251] passes one reference to subsystems::Install
    // [:514] and subsystems::TickGameplay [:449], on the ONE Session the process owns
    // [harness/session_runtime.cpp:78, :261, :300] -- and the guard is for when that stops being true.
    // Only PARKS are cleared; the author-slot latch and the one-shot INIT flags are OnDisconnect's.
    // Control then falls through to the ordinary connection filter, which returns as it always did.
    coop::net::Session* const installed = LoadSession();
    if (installed && (installed != session || !installed->connected()) && g_store.Size() != 0) {
        UE_LOGW("[CUSTODY] HOST session is down at Tick entry -- clearing %zu held park(s) "
                "(2.8(c) belt; the DisconnectAll fan-out did not reach this module)",
                g_store.Size());
        TeardownParks_("tick-session-down");
    }

    if (!session || !session->connected() || session->role() != coop::net::Role::Host) return;
    if (g_store.Size() == 0) return;   // the steady state: nothing walked, nothing allocated
    BindSink();

    // The sweep runs on EVERY custody Tick -- the same cadence as DrainPendingSpawns -- because it
    // is a walk of at most 32 entries doing integer and boolean comparisons, with no engine call
    // and no allocation.
    bool connected[kMaxPeers] = {};
    uint32_t gens[kMaxPeers] = {};
    for (int i = 0; i < kMaxPeers; ++i) {
        connected[i] = session->IsSlotConnected(i);
        gens[i] = session->peerGenerationForSlot(i);
    }
    const uint32_t worldGen = ue_wrap::world_identity::Generation();
    g_store.Sweep(Clock::now(), worldGen, connected, gens);

    // The consume. This runs immediately after host_spawn_watcher::DrainPendingSpawns, which is
    // where the eid is minted (-> prop_lifecycle::ExpressSpawnedProp -> MarkPropElement), so the
    // adoption this park is waiting for has just happened if it is going to.
    const SlotIo io = MakeProductionIo();
    const std::vector<void*> armed = g_store.ArmedActors();
    for (void* actor : armed) {
        const CustodyParkStore::Entry* e = g_store.FindByActor(actor);  // nulled before TryConsume
        if (!e) continue;
        const std::wstring parkKey = e->id.key;

        // Row 1. IsLiveByIndex is the guard DrainPendingSpawns itself uses at
        // host_spawn_watcher.cpp:360: the registry snapshot does not protect an actor pointer.
        //
        // Rows 1, 2 and 5 refuse BEFORE TryConsume runs, so no index has been resolved, no
        // enumeration has been taken and no nested record has been examined. Their literal
        // -1 / -1 / 0 is therefore the measurement, not a placeholder: printing the store's
        // lastNestedIndex here would report whatever some earlier consume found.
        const int32_t internalIdx = R::InternalIndexOf(actor);
        if (internalIdx < 0 || !R::IsLiveByIndex(actor, internalIdx)) {
            UE_LOGW("[CUSTODY] HOST reattach REFUSED key='%ls' reason=actor-dead index=%d "
                    "nestedIndex=%d others=%zu", parkKey.c_str(), -1, -1, size_t{0});
            g_store.RetireArmed(actor, Reason::ActorDead);
            continue;
        }
        // Row 2. The eid is allowed to be late: kMaxAdoptTries pump ticks, then a named refusal.
        const uint32_t eid =
            static_cast<uint32_t>(coop::element::Registry::Get().EidForActor(actor));
        if (eid == static_cast<uint32_t>(coop::element::kInvalidId)) {
            if (g_store.NoteAdoptMiss(actor)) {
                UE_LOGW("[CUSTODY] HOST reattach REFUSED key='%ls' reason=eid-unresolved index=%d "
                        "nestedIndex=%d others=%zu", parkKey.c_str(), -1, -1, size_t{0});
                g_store.RetireArmed(actor, Reason::EidUnresolved);
            }
            continue;
        }
        // Row 5.
        if (!CS::IsContainer(actor)) {
            UE_LOGW("[CUSTODY] HOST reattach REFUSED key='%ls' reason=not-container index=%d "
                    "nestedIndex=%d others=%zu", parkKey.c_str(), -1, -1, size_t{0});
            g_store.RetireArmed(actor, Reason::NotContainer);
            continue;
        }
        void* inv = CS::InventoryOfContainer(actor);

        // Rows 3 and 4 are the store's, and it needs both engine reads. The enrolled key is re-read
        // AFTER MarkPropElement may have re-keyed this container: a key-rekeyed refusal in a host
        // log is a measurement that a collision family is live in that world.
        const std::wstring enrolled = WireNormalise(ue_wrap::prop::GetInteractableKeyString(actor));
        const std::wstring observedCls = R::ClassNameOf(actor);

        // EVERY read off the park happens HERE. TryConsume retires the entry on every outcome, so
        // `e` is dangling the moment it returns; reading e->set after the call would be a
        // use-after-free of a std::map node.
        const int records = static_cast<int>(e->set.recs.size());
        const uint64_t hash = e->set.hash;
        int nested = 0;
        for (const Rec& r : e->set.recs) {
            if (CS::IsNestedContainerRecord(r)) ++nested;
        }
        e = nullptr;

        g_writeActor = actor;
        Reason why = Reason::Applied;
        const bool applied = g_store.TryConsume(actor, eid, inv, enrolled, observedCls, io,
                                                Clock::now(), why);
        g_writeActor = nullptr;

        int32_t resolvedIndex = -1;
        CS::WorldContainerSlotIndex(inv, resolvedIndex);

        if (!applied) {
            if (why == Reason::DiscardedFreshNotEmpty) {
                int32_t held = -1;
                CS::WorldContainerRecordCount(inv, held);
                UE_LOGI("[CUSTODY] HOST park DISCARDED key='%ls' -- fresh slot already holds %d "
                        "records", parkKey.c_str(), held);
            } else {
                // The two diagnostic fields, read from the consume that just
                // produced this refusal: nestedIndex names the offending EFFECTIVE index on a
                // nested-slot-aliased refusal (-1 when no nested record was the cause), and others
                // is the enumeration size this consume was tested against (0 when the refusal came
                // before the enumeration was taken). Both are reset at the top of every
                // TryConsume, so neither can be a value left over from an earlier one. This is
                // what makes an in-game refusal diagnosable without a rebuild.
                UE_LOGW("[CUSTODY] HOST reattach REFUSED key='%ls' reason=%s index=%d "
                        "nestedIndex=%d others=%zu",
                        parkKey.c_str(), ReasonText(why), resolvedIndex,
                        g_store.lastNestedIndex, g_store.lastEnumerationSize);
            }
            continue;
        }

        UE_LOGI("[CUSTODY] HOST reattached key='%ls' eid=%u records=%d hash=%llu index=%d nested=%d",
                parkKey.c_str(), eid, records, static_cast<unsigned long long>(hash),
                resolvedIndex, nested);

        // The postcondition. Record the write the way the host's own verb edge records a mutation
        // -- the new eid marked dirty, the write policy's NoteLocalChange stamp, the applied hash
        // dropped -- and let the shipped DrainDirty ship it, so every peer converges through the
        // ContainerContents lane rather than a new one, and any client slice for this container inside
        // container_write_policy's conflict window is refused and answered with host truth, as after a
        // host verb edge (a racing one included). The fan-out ships
        // on the NEXT contents sweep -- container_contents_sync::Tick already ran earlier in this
        // same pass, behind a 250 ms gate -- so a log check that greps for the fan-out line in the same
        // log second as the reattach line would score a false negative.
        CS::MarkHostCustodyWrite(eid);
        UE_LOGI("[CUSTODY] HOST fanout QUEUED key='%ls' eid=%u records=%d",
                parkKey.c_str(), eid, records);
    }
}

// The personal-inventory check. Its point is to show that the LIVE PERSONAL INVENTORY -- the one
// component this whole mechanism must never touch -- is refused by the very first boundary, on the
// running build, rather than by an argument in a document. Read-only: it resolves nothing it could
// write through, and the predicate below reaches no writer.
bool WouldParkForInventory(void* inv, Reason& out) {
    out = Reason::Applied;
    // Boundary 1, the same refusal the capture makes at condition 6. For a personal store this is
    // where the answer must come from: Player == 1.
    if (!inv || !CS::IsWorldContainer(inv)) { out = Reason::Boundary1; return false; }
    int32_t index = -1;
    if (!CS::WorldContainerSlotIndex(inv, index)) { out = Reason::SlotUnresolved; return false; }
    // Row 8: GObjStack[0] is the host player's own inventory by construction.
    if (index == 0) { out = Reason::SlotAliased; return false; }
    // Row 9: and the MEASURED personal index, whatever it turns out to be on this build.
    ue_wrap::inventory::LivePersonalStore store;
    if (!ue_wrap::inventory::ReadLivePersonalStore(store)) { out = Reason::SlotAliased; return false; }
    if (index == store.slotIndex) { out = Reason::SlotAliased; return false; }
    return true;
}

void OnDisconnect() {
    // The counters are readable from the log at teardown -- the read happens HERE,
    // inside TeardownParks_, ahead of the Clear() that zeroes them. Clear() then emits one
    // `CLEARED key=... reason=session` line per park that was still held, which is the `session`
    // CLEARED reason.
    TeardownParks_("disconnect");
    g_latchedSlot = kNoAuthorSlot;
    g_saidCaptureAlive = false;
    g_saidTickAlive = false;
    g_writeActor = nullptr;
}

}  // namespace coop::props::container_custody
