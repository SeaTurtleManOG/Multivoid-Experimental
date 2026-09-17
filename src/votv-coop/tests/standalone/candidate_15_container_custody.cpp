// candidate_15_container_custody.cpp -- the container custody park, both arms in one source.
//
// WHAT THIS PROVES. coop/props/container_custody.h is a header-only, engine-free reducer: the six
// arm conditions and the thirteen consume checks, with every
// engine-touching operation injected through SlotIo. Driving that reducer against a stub world
// turns each named refusal into an assertion. Thirty-two assertions, numbered
// 1-30 plus 15b and 24b.
//
// WHAT THIS DOES NOT PROVE -- and none of it is a formality:
//   * that the PRODUCTION SlotIo binding calls the shipped IsWorldContainerInventory /
//     GObjStackSlot / EngineAlloc / RederiveManagedState and not something else;
//   * that the production capture call, the production author-slot latch and the production
//     subsystems.cpp Tick registration exist at all -- three omission rows below name NONE for
//     exactly this reason;
//   * that the production OtherWorldContainerIndices binding performs the actor/eid exclusion and
//     the pairs.size() + 1 sizing. What IS proven here is that the interface has somewhere to put
//     them, that the store honours whatever the seam returns (18), that a correctly-excluding seam
//     consumes (25), that an incomplete one refuses (24) and that a complete one consumes (24b);
//   * that prop_box_C walks to prop_container_C (a stated assumption).
// Those are proven only by in-game runs and by reading the code. NO IN-GAME RUN WAS MADE FOR THIS
// FILE. A headless test of a pure policy proves the POLICY, not the production binding.
//
// THE TWO ARMS. run.ps1 builds this source twice: once with the include tree as it ships, and once
// against a copy with coop/props/container_custody.h REMOVED, which selects the pinned pre-fix
// policy below. The pre-fix arm MUST fail. Two assertions -- 9's fan-out half and 22 -- turn on
// behaviour that lives OUTSIDE container_custody.h (the capture's neuterNested=false argument and
// the fan-out sizing, both in container_contents_sync.cpp), so their negatives are driven by the
// MV_CUSTODY_FIXED switch below rather than by the header swap. That is stated rather than
// disguised: for those two, the arm shows the CODEC and the STORE behave differently under the two
// policies, not that production ReadContents passes the flag.
//
// AND WHICH ASSERTIONS THE PRE-FIX ARM DOES NOT DISCRIMINATE. MEASURED, not argued: the numbers
// below were read off a real run of both arms, by taking the distinct assertion labels that carry
// at least one FAIL line in the pre-fix arm. An earlier version of this banner claimed five
// both-arms assertions and twenty-seven discriminating ones; that was wrong, and it was wrong in
// the direction that overstates coverage, so it is corrected here rather than left standing.
//
//   DISCRIMINATING -- red in the pre-fix arm, 19 assertions:
//     2, 5, 6, 7 (via 7d), 9, 12, 13, 14, 15, 15b, 21, 22, 24, 24b, 26, 27, 28, 29, 30
//   GREEN IN BOTH ARMS -- 13 assertions, and therefore carrying NO negative control in this file:
//     1, 3, 4, 8, 10, 11, 16, 17, 18, 19, 20, 23, 25   (and 7's a/b/c quarters; only 7d is red)
//   19 + 13 = 32.
//
//   THREE DIFFERENT QUANTITIES LIVE IN THIS FILE, and every count error it has had came from
//   letting one of them stand in for another. Say which one you mean:
//     (i)   EIGHT ROWS of the omission table at the foot of this file carry no negative control:
//           the rows targeting 16, 17, 18, 19, 20, 25 and 4, plus the ceiling / world-gen /
//           author-left / author-gen sweep row, which is controlled only in its 7d quarter. That
//           is a property of the TABLE.
//     (ii)  THIRTEEN ASSERTIONS are green in both arms and so carry no negative control:
//           1, 3, 4, 8, 10, 11, 16, 17, 18, 19, 20, 23 and 25, plus 7a/7b/7c. That is the
//           green-in-both list directly above -- a property of THIS SUITE'S PRE-FIX ARM, and the
//           authority any other list of ASSERTIONS must reconcile against.
//     (iii) ONE refusal, of the NINE whose deletion has actually been tried, is one whose
//           DELETION FROM THE SHIPPED HEADER this suite would not catch. That is a property of
//           the SHIPPED POLICY UNDER MUTATION, measured by rebuilding the FIXED arm against a
//           copy of the include tree with exactly one refusal removed. See MUTATION-MEASURED,
//           below, for the nine and for which one it is.
//   EIGHT ROWS IS NOT THIRTEEN ASSERTIONS IS NOT ONE UNCAUGHT REFUSAL. The three numbers are 8,
//   13 and 1; none of them may be substituted for another, because they answer different
//   questions. Reading (ii) as if it were (iii) is what made this paragraph wrong before.
//   The reconciliation of (i) with (ii) is exact: seven of the thirteen (4, 16, 17, 18, 19, 20,
//   25) are the target of a row, and those seven rows plus the sweep row are the eight; the
//   remaining six -- 1, 3, 8, 10, 11 and 23, i.e. the happy path, NotParked, the 512-record
//   fan-out, Clear, author-mismatch and the record codec -- are the target of no omission row at
//   all. (iii) reconciles with neither: it counts MUTATIONS RUN, not rows and not assertions.
//
// WHY, and it is one cause and not thirteen. The pinned pre-fix policy is an earlier draft's WHOLE store
// -- ONE composite omission -- not twenty-seven separately deleted checks (that is restated at the
// omission table at the foot of this file). That draft already had the outer slot-alias test, the
// Boundary 1 refusal, the allocation pre-flight, the author-slot check, the record codec and a
// store that honours whatever the enumeration seam returns. Those checks were not among the
// defects found in it, so hiding container_custody.h cannot turn them red.
//
// WHAT THAT COSTS, stated because a later contributor will act on it, and it costs THIRTEEN
// assertions and not eight: all thirteen of the green-in-both list above -- 1, 3, 4, 8, 10, 11,
// 16, 17, 18, 19, 20, 23 and 25, plus 7a/7b/7c -- are proven in the FIXED arm only. An earlier
// version of this paragraph named a subset of eight (11, 16, 17, 18, 19, 20, 23, 25) by no
// stated principle; that under-stated the cost and contradicted the declaration block at the foot
// of this file, which names 4 and 7a/7b/7c as cost rows, so the subset is replaced by the whole
// list rather than left standing. "No negative control" states ONE thing and only that: THE
// PRE-FIX ARM DOES NOT DISCRIMINATE THEM. Hiding container_custody.h leaves them green, so this
// suite's NEGATIVE ARM is not what protects them; in-game runs and reading the code are, exactly as
// they already are for the three production-call-site omission rows below.
//
// MUTATION-MEASURED, and it is NOT the same statement. An earlier version of this paragraph went
// on to say that DELETING one of those refusals from the shipped header would leave BOTH arms of
// this suite green, and that their green lines are no evidence that this file would catch their
// removal. THAT WAS FALSE, and it was falsified by building it: NINE refusals were deleted ONE AT
// A TIME from a COPY of the include tree, and the FIXED arm was rebuilt against the copy each
// time. EIGHT OF THE NINE turn the fixed arm RED -- the refusals targeting 17, 18, 19, 20 and 4,
// and the sweep's 7a, 7b and 7c clauses. The mechanism is plain once it is stated: the PRE-FIX arm
// compiles with the header REMOVED, so a deletion INSIDE that header is invisible to it, while the
// FIXED arm compiles AGAINST the header, so a deletion there usually turns its assertion red.
// GREEN IN BOTH ARMS DOES NOT ENTAIL UNCAUGHT. The two must not be written as one sentence again.
//
// THE ONE REFUSAL WHOSE DELETION THIS SUITE DOES NOT CATCH is Row 8's Index == 0 (assertion 16):
// with Row 8 deleted, the fixed arm still reports zero failing assertions. That is a REDUNDANCY IN
// THE SHIPPED HEADER rather than a hole in this file. Wherever the personal store's index IS 0,
// Row 9's refusal SUBSUMES Row 8's, and case 16 is such a world: it leaves the personal-store
// index at its default of 0, so Row 9's idx == personal already fires and refuses with the same
// slot-aliased reason. Measured as a truth table over the two
// rows: delete Row 8 alone and 16 stays green; delete Row 9 alone and 16 stays green while 17 goes
// red; delete BOTH and 16 goes red. The subsumption runs one way only, which is why deleting Row 9
// by itself IS caught. (Row 8's own comment says GObjStack[0] is the personal store in production
// as well, so the redundancy is probably not an artefact of this test's world -- but that half is
// INFERRED from a comment and is NOT measured here.)
//
// NOT TESTED, and therefore claimed neither way: row 25's self-exclusion omission, which lives in
// an INJECTED seam and cannot be deleted from the shipped header at all. NINE deletions were run,
// not every refusal the header contains. Do not read "eight of the nine" as "all".
//
// AND WHY NO NEGATIVE CONTROL WAS INVENTED FOR THEM (coverage is established by
// declaring rather than by inventing). The pre-fix fallback's own rule is that nothing in it is
// invented -- every departure is a defect found in that draft. Adding a deletion the draft did
// not have would break that rule and make the arm a fiction. The only other route is a second
// omission switch inside the shipped header, i.e. shipping a policy toggle for the test's benefit.
// Both were rejected; these thirteen are DECLARED instead. The one omission this file DOES
// exercise in-arm -- assertion 25's second half, a seam that does not self-exclude -- is possible
// only because that omission lives in an INJECTED seam, which is not true of the others.
//
// The 19 that are red each fail FOR THEIR OWN REASON -- `got=applied` where a refusal was required,
// or a missing counter -- rather than being masked by a cascade.

#if __has_include("coop/props/container_custody.h")
#include "coop/props/container_custody.h"
#define MV_CUSTODY_FIXED 1
#else
#define MV_CUSTODY_FIXED 0
#include "coop/net/blob_chunks.h"
#include "ue_wrap/actors/save_record.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------
// THE PINNED PRE-FIX POLICY -- an earlier draft of the custody store, retained here so this one
// source can prove its own negative. Each departure from the shipped header is a defect found in
// that draft, and is listed below. Nothing here is invented.
//
//   -  the author slot is narrowed to uint8_t, so "no slot was latched" cannot be told
//      from "an out-of-range slot was latched" -- the AuthorUnknown refusal cannot fire
//   -  no author-GENERATION binding, at the arm or in the sweep
//   -  outComplete is ignored
//   -  the buffer is sized with no headroom: want = need, so a full buffer -- the
//      ORDINARY case -- cannot be told from a truncation
// The draft's seam defect (a seam called with no target, so it cannot self-exclude) is deliberately NOT
// carried here: it refuses at the outer slot check and would MASK every assertion downstream of it,
// making this arm's red lines say less than they appear to. Its omission row is exercised inside
// the FIXED arm instead, at assertion 25's second half.
//   -  no nested-slot test at all
//   -  no class binding at the arm
//   -  no arm-once rule
//   -  no enrolled-key re-check at the consume
//   -  no fresh-empty refusal
//   prop_drop_intent.cpp:70-89   the consume drops the map entry only, desyncing map and FIFO,
//               and there is no eviction cap at all
// ---------------------------------------------------------------------------------------------
namespace coop::props::container_custody {

using Rec = ue_wrap::save_record::SaveRecord;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

inline constexpr int kMaxPeers = 4;
inline constexpr int kNoAuthorSlot = -2147483647 - 1;
inline constexpr size_t kMaxCustodyParks = 32;
inline constexpr size_t kMaxRecordsPerContainer = 512;
inline constexpr int kCustodyParkCeilingSec = 900;
inline constexpr size_t MaxFanoutBytes() { return coop::blob_chunks::MaxBlobBytes(); }

struct CustodyRecordSet {
    std::vector<Rec> recs;
    uint64_t hash = 0;
    int32_t  index = -1;
    size_t   fanoutBytes = 0;
};
struct CustodyKeyT { std::wstring key; std::wstring cls; };

enum class Reason {
    Applied, DiscardedFreshNotEmpty, NotParked, AlreadyArmed, AuthorMismatch, AuthorReplaced,
    ClassMismatch, KeyMismatch, KeyRekeyed, WorldChanged, AuthorLeft, Boundary1, SlotUnresolved,
    SlotAliased, NestedSlotAliased, EnumerationUnproven, AllocPreflight, Expired, Evicted,
    AuthorUnknown, SlotOutOfRange, KeyEmpty, CapExceeded, OverBlobCeiling, AllocFailed, Replaced,
    kCount
};
inline const char* ReasonText(Reason r) {
    switch (r) {
        case Reason::Applied: return "applied";
        case Reason::DiscardedFreshNotEmpty: return "fresh-not-empty";
        case Reason::NotParked: return "not-parked";
        case Reason::AlreadyArmed: return "already-armed";
        case Reason::AuthorMismatch: return "author-mismatch";
        case Reason::AuthorReplaced: return "author-replaced";
        case Reason::ClassMismatch: return "class-mismatch";
        case Reason::KeyMismatch: return "key-mismatch";
        case Reason::KeyRekeyed: return "key-rekeyed";
        case Reason::WorldChanged: return "world-changed";
        case Reason::AuthorLeft: return "author-left";
        case Reason::Boundary1: return "boundary1";
        case Reason::SlotUnresolved: return "slot-unresolved";
        case Reason::SlotAliased: return "slot-aliased";
        case Reason::NestedSlotAliased: return "nested-slot-aliased";
        case Reason::EnumerationUnproven: return "enumeration-unproven";
        case Reason::AllocPreflight: return "alloc-preflight";
        case Reason::Expired: return "expired";
        case Reason::Evicted: return "evicted";
        case Reason::AuthorUnknown: return "author-unknown";
        case Reason::SlotOutOfRange: return "slot-out-of-range";
        case Reason::KeyEmpty: return "key-empty";
        case Reason::CapExceeded: return "cap-512";
        case Reason::OverBlobCeiling: return "over-blob-ceiling";
        case Reason::AllocFailed: return "alloc-failed";
        case Reason::Replaced: return "replaced";
        case Reason::kCount: break;
    }
    return "?";
}

struct SlotIo {
    bool (*IsWorldContainer)(void* inv) = nullptr;
    bool (*ReadSlotIndex)(void* inv, int32_t& outIndex) = nullptr;
    int32_t (*ReadSlotCount)(void* inv) = nullptr;
    bool (*AllocPreflight)() = nullptr;
    bool (*WriteRecords)(void* inv, const CustodyRecordSet& set) = nullptr;
    bool (*Rederive)(void* owner, void* inv) = nullptr;
    size_t (*OtherWorldContainerIndices)(void* selfActor, uint32_t selfEid,
                                         int32_t* out, size_t want, bool& outComplete) = nullptr;
    bool (*PersonalStoreIndex)(int32_t& out) = nullptr;
    int32_t (*GObjStackLength)() = nullptr;
    bool (*IsNestedContainerRecord)(const Rec& r) = nullptr;
};

class CustodyParkStore {
public:
    struct Entry {
        CustodyKeyT id;
        CustodyRecordSet set;
        int authorSlot = kNoAuthorSlot;
        uint32_t authorGen = 0;
        uint32_t worldGen = 0;
        TimePoint at{};
        bool armed = false;
        void* actor = nullptr;
        int adoptTries = 0;
    };

    bool Park(CustodyKeyT id, CustodyRecordSet set, int authorSlot, uint32_t authorGen,
              uint32_t worldGen, TimePoint now, Reason& out) {
        // The draft's seam took uint8_t, so every "unknown" value folds into a byte and the
        // AuthorUnknown refusal is unreachable.
        const int narrowed = static_cast<int>(static_cast<uint8_t>(authorSlot));
        if (narrowed < 0 || narrowed >= kMaxPeers) {
            out = Reason::SlotOutOfRange;
            ++parksRefused[static_cast<size_t>(out)];
            return false;
        }
        if (id.key.empty() || id.key == L"None") {
            out = Reason::KeyEmpty;
            ++parksRefused[static_cast<size_t>(out)];
            return false;
        }
        if (set.recs.size() > kMaxRecordsPerContainer) {
            out = Reason::CapExceeded;
            ++parksRefused[static_cast<size_t>(out)];
            return false;
        }
        if (set.fanoutBytes > MaxFanoutBytes()) {
            out = Reason::OverBlobCeiling;
            ++parksRefused[static_cast<size_t>(out)];
            return false;
        }
        if (byKey_.find(id.key) != byKey_.end()) {
            byKey_.erase(id.key);   // the FIFO copy is deliberately left behind (the shipped desync)
            ++parksReplaced;
        }
        // No eviction cap at all: the draft's store grew without bound.
        Entry p;
        p.id = std::move(id);
        p.set = std::move(set);
        p.authorSlot = narrowed;
        p.authorGen = authorGen;
        p.worldGen = worldGen;
        p.at = now;
        const std::wstring key = p.id.key;
        fifo_.push_back(key);
        byKey_.emplace(key, std::move(p));
        ++parksTaken;
        out = Reason::Applied;
        return true;
    }

    bool Arm(const CustodyKeyT& id, int spawnSlot, uint32_t /*spawnGen*/, uint32_t worldGen,
             void* actor, TimePoint /*now*/, Reason& out) {
        auto it = byKey_.find(id.key);
        if (it == byKey_.end()) {
            out = Reason::NotParked;
            ++parksRefusedAtArm[static_cast<size_t>(out)];
            return false;
        }
        Entry& p = it->second;
        if (spawnSlot != p.authorSlot) {
            out = Reason::AuthorMismatch;
            ++parksRefusedAtArm[static_cast<size_t>(out)];
            Retire_(id.key);
            return false;
        }
        // No author-generation binding, no class binding, no arm-once rule.
        if (worldGen != p.worldGen) {
            out = Reason::WorldChanged;
            ++parksRefusedAtArm[static_cast<size_t>(out)];
            Retire_(id.key);
            return false;
        }
        p.armed = true;
        p.actor = actor;
        ++parksArmed;
        out = Reason::Applied;
        return true;
    }

    bool TryConsume(void* actor, uint32_t eid, void* inv,
                    const std::wstring& /*enrolledKey*/, const std::wstring& /*observedCls*/,
                    const SlotIo& io, TimePoint /*now*/, Reason& out) {
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
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            return false;
        }
        // No enrolled-key re-check and no class re-check.
        if (!io.IsWorldContainer || !io.IsWorldContainer(inv)) {
            out = Reason::Boundary1;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        int32_t idx = -1;
        if (!io.ReadSlotIndex || !io.ReadSlotIndex(inv, idx)) {
            out = Reason::SlotUnresolved;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        if (idx == 0) {
            out = Reason::SlotAliased;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        int32_t personal = -1;
        if (io.PersonalStoreIndex && io.PersonalStoreIndex(personal) && idx == personal) {
            out = Reason::SlotAliased;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        // outComplete is ignored and the buffer is sized with NO headroom
        // (want = need), so a full buffer -- the ordinary case -- is indistinguishable from a
        // truncation and is silently treated as a complete enumeration.
        bool ignored = false;
        const size_t need = io.OtherWorldContainerIndices(actor, eid, nullptr, 0, ignored);
        std::vector<int32_t> others(need == 0 ? size_t{1} : need, 0);
        const size_t n = io.OtherWorldContainerIndices(actor, eid, others.data(), need, ignored);
        others.resize(n);
        for (int32_t other : others) {
            if (other == idx) {
                out = Reason::SlotAliased;
                ++parksRefusedAtApply[static_cast<size_t>(out)];
                Retire_(key);
                return false;
            }
        }
        // No nested-slot test, and no fresh-empty refusal either.
        if (!io.AllocPreflight || !io.AllocPreflight()) {
            out = Reason::AllocPreflight;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        if (!io.WriteRecords || !io.WriteRecords(inv, p->set)) {
            out = Reason::AllocFailed;
            ++parksRefusedAtApply[static_cast<size_t>(out)];
            Retire_(key);
            return false;
        }
        ++writes;
        if (io.Rederive && io.Rederive(actor, inv)) ++rederives;
        ++parksApplied;
        byKey_.erase(key);   // the map only: the shipped desync, reproduced
        out = Reason::Applied;
        return true;
    }

    void Sweep(TimePoint now, uint32_t worldGen, const bool* slotConnected,
               const uint32_t* /*slotGen*/) {
        std::vector<std::wstring> doomed;
        std::vector<Reason> why;
        for (const auto& kv : byKey_) {
            const Entry& p = kv.second;
            if (p.worldGen != worldGen) {
                doomed.push_back(kv.first); why.push_back(Reason::WorldChanged); continue;
            }
            const bool slotOk = p.authorSlot >= 0 && p.authorSlot < kMaxPeers;
            if (!slotOk || !slotConnected || !slotConnected[p.authorSlot]) {
                doomed.push_back(kv.first); why.push_back(Reason::AuthorLeft); continue;
            }
            // No author-generation clause.
            if (std::chrono::duration_cast<std::chrono::seconds>(now - p.at).count()
                > kCustodyParkCeilingSec) {
                doomed.push_back(kv.first); why.push_back(Reason::Expired);
            }
        }
        for (size_t i = 0; i < doomed.size(); ++i) {
            Retire_(doomed[i]);
            if (why[i] == Reason::Expired) ++parksExpired;
            else ++parksCleared[static_cast<size_t>(why[i])];
        }
    }

    void Clear() {
        byKey_.clear();
        fifo_.clear();
        parksTaken = parksArmed = parksApplied = parksDiscarded = 0;
        parksExpired = parksEvicted = parksReplaced = nestedSentinelAccepted = 0;
        writes = rederives = 0;
        for (size_t i = 0; i < static_cast<size_t>(Reason::kCount); ++i) {
            parksRefused[i] = parksRefusedAtArm[i] = parksRefusedAtApply[i] = parksCleared[i] = 0;
        }
    }

    size_t Size() const { return byKey_.size(); }
    bool Has(const std::wstring& key) const { return byKey_.find(key) != byKey_.end(); }
    size_t FifoCount(const std::wstring& key) const {
        size_t n = 0;
        for (const auto& k : fifo_) { if (k == key) ++n; }
        return n;
    }
    std::vector<std::wstring> FifoKeys() const { return {fifo_.begin(), fifo_.end()}; }
    const Entry* Find(const std::wstring& key) const {
        auto it = byKey_.find(key);
        return it == byKey_.end() ? nullptr : &it->second;
    }

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
    size_t writes = 0;
    size_t rederives = 0;

private:
    void Retire_(const std::wstring& key) {
        byKey_.erase(key);
        for (auto it = fifo_.begin(); it != fifo_.end();) {
            it = (*it == key) ? fifo_.erase(it) : it + 1;
        }
    }
    std::map<std::wstring, Entry> byKey_;
    std::deque<std::wstring> fifo_;
};

}  // namespace coop::props::container_custody
#endif  // __has_include

#include "coop/items/save_record_wire.h"
#include "ue_wrap/core/log.h"

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

// The one symbol the linked save_record_wire.cpp / signal_wire.cpp pair needs and this harness has
// no use for. A test double, confined to this TU: nothing here asserts on log output.
namespace ue_wrap::log {
void Write(Level, const char*, ...) {}
}  // namespace ue_wrap::log

namespace {

using coop::props::container_custody::CustodyKeyT;
using coop::props::container_custody::CustodyParkStore;
using coop::props::container_custody::CustodyRecordSet;
using coop::props::container_custody::Reason;
using coop::props::container_custody::ReasonText;
using coop::props::container_custody::SlotIo;
using coop::props::container_custody::kCustodyParkCeilingSec;
using coop::props::container_custody::kMaxCustodyParks;
using coop::props::container_custody::kMaxPeers;
using coop::props::container_custody::kNoAuthorSlot;
using Rec = ue_wrap::save_record::SaveRecord;
using Clock = std::chrono::steady_clock;

int g_failures = 0;

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    if (!condition) ++g_failures;
    return condition;
}

bool CheckReason(Reason got, Reason want, const char* label) {
    const bool ok = (got == want);
    std::printf("%s: %s  [got=%s want=%s]\n", ok ? "PASS" : "FAIL", label,
                ReasonText(got), ReasonText(want));
    if (!ok) ++g_failures;
    return ok;
}

// ---- the stub world ---------------------------------------------------------------------------
// SlotIo is a table of raw function pointers, so the world it reads is file-scope. ResetWorld()
// restores the ordinary case before every assertion; each assertion then perturbs exactly one term.

char g_targetStorage[8] = {};
char g_otherStorage[8] = {};
char g_invStorage[8] = {};
void* const kTargetActor = static_cast<void*>(g_targetStorage);
void* const kOtherActor = static_cast<void*>(g_otherStorage);
void* const kInv = static_cast<void*>(g_invStorage);
constexpr uint32_t kTargetEid = 4242;
constexpr uint32_t kOtherEid = 99;

struct WorldEntry {
    void* actor;
    uint32_t eid;
    int32_t index;
};

struct StubWorld {
    bool     isWorldContainer = true;
    bool     slotIndexOk = true;
    int32_t  slotIndex = 5;
    int32_t  freshCount = 0;
    bool     allocPreflight = true;
    bool     writeOk = true;
    bool     personalOk = true;
    int32_t  personalIndex = 0;
    int32_t  stackLen = 64;
    bool     enumComplete = true;
    bool     honourExclusion = true;   // clause 1, performed INSIDE the seam
    std::vector<WorldEntry> world;     // every live world container, the target included
    // observations
    size_t   writeCount = 0;
    size_t   rederiveCount = 0;
    std::vector<Rec> written;
    size_t   enumCalls = 0;
    size_t   lastWant = 0;
};

StubWorld g_w;

void ResetWorld() {
    g_w = StubWorld{};
    // The adopted container appears in its OWN snapshot: at consume time it is live, walks to the
    // container base and passes Boundary 1 (container_contents_sync.cpp:794-798). Every ordinary
    // world therefore contains it, which is why clause 1's self-exclusion is load-bearing.
    g_w.world.push_back(WorldEntry{kTargetActor, kTargetEid, 5});
}

bool StubIsWorldContainer(void*) { return g_w.isWorldContainer; }
bool StubReadSlotIndex(void*, int32_t& out) {
    if (!g_w.slotIndexOk) return false;
    out = g_w.slotIndex;
    return true;
}
int32_t StubReadSlotCount(void*) { return g_w.freshCount; }
bool StubAllocPreflight() { return g_w.allocPreflight; }
bool StubWriteRecords(void*, const CustodyRecordSet& set) {
    if (!g_w.writeOk) return false;
    ++g_w.writeCount;
    g_w.written = set.recs;
    return true;
}
bool StubRederive(void*, void*) {
    ++g_w.rederiveCount;
    return true;
}
size_t StubOtherIndices(void* selfActor, uint32_t selfEid, int32_t* out, size_t want,
                        bool& outComplete) {
    ++g_w.enumCalls;
    g_w.lastWant = want;
    outComplete = g_w.enumComplete;
    size_t n = 0;
    for (const WorldEntry& e : g_w.world) {
        // Clause 1, by BOTH identifiers: an entry that lost one (a recycled actor pointer, a
        // re-minted eid) still self-excludes on the other.
        if (g_w.honourExclusion && (e.actor == selfActor || e.eid == selfEid)) continue;
        if (out) {
            if (n >= want) break;   // the buffer filled: SnapshotWorldContainers' break at :792
            out[n] = e.index;
        }
        ++n;
    }
    return n;
}
bool StubPersonalStoreIndex(int32_t& out) {
    if (!g_w.personalOk) return false;
    out = g_w.personalIndex;
    return true;
}
int32_t StubGObjStackLength() { return g_w.stackLen; }
// The stub's nested-container predicate. RecordIsNestedContainer is FindClass + WalksToBase
// (container_contents_sync.cpp:228-233), i.e. an engine call, so the harness names nested records
// by class name instead.
bool StubIsNestedContainerRecord(const Rec& r) { return r.className == L"prop_container_C"; }

SlotIo MakeIo() {
    SlotIo io;
    io.IsWorldContainer = &StubIsWorldContainer;
    io.ReadSlotIndex = &StubReadSlotIndex;
    io.ReadSlotCount = &StubReadSlotCount;
    io.AllocPreflight = &StubAllocPreflight;
    io.WriteRecords = &StubWriteRecords;
    io.Rederive = &StubRederive;
    io.OtherWorldContainerIndices = &StubOtherIndices;
    io.PersonalStoreIndex = &StubPersonalStoreIndex;
    io.GObjStackLength = &StubGObjStackLength;
    io.IsNestedContainerRecord = &StubIsNestedContainerRecord;
    return io;
}

// ---- record helpers -----------------------------------------------------------------------------

Rec MakeItem(const wchar_t* cls, const wchar_t* key) {
    Rec r;
    r.className = cls;
    r.key = key;
    return r;
}

// A nested container record whose ints[0][0] is `idx`.
Rec MakeNested(int32_t idx) {
    Rec r = MakeItem(L"prop_container_C", L"nested");
    r.ints.push_back(std::vector<int32_t>{idx});
    return r;
}

// A nested container record with NO ints at all. Boundary 2's comment
// (container_contents_sync.cpp:252-257) establishes the engine reads that as index 0, so it is not
// "a record with no index".
Rec MakeNestedNoInts() { return MakeItem(L"prop_container_C", L"nested"); }

Rec MakeNestedEmptyInts() {
    Rec r = MakeItem(L"prop_container_C", L"nested");
    r.ints.push_back(std::vector<int32_t>{});
    return r;
}

// NeuterNestedIndex, restated byte-for-byte from container_contents_sync.cpp:258-262. It is
// private to that TU, so the harness restates it; assertion 22 therefore proves what the CODEC
// does to a neutered record, not that production ReadContents chooses to neuter.
void NeuterNestedIndex(Rec& r) {
    if (r.ints.empty()) r.ints.resize(1);
    if (r.ints[0].empty()) r.ints[0].resize(1);
    r.ints[0][0] = -1;
}

// The slice pack's shape with baseHash zeroed, which is what ContentHash hashes. Written against
// a8437658's private PackContents; on ba3bf74a the same bytes are container_slice_wire::Pack and
// ContentHash (container_slice_wire.cpp), which this harness still restates rather than links. The
// RECORD codec itself is the shipped one, linked, not a copy.
std::vector<uint8_t> PackLike(uint32_t eid, const std::vector<Rec>& recs) {
    std::vector<uint8_t> b;
    b.push_back(0);  // kOpContents
    coop::save_record_wire::AppU32(b, eid);
    for (int i = 0; i < 8; ++i) b.push_back(0);  // baseHash = 0
    b.push_back(static_cast<uint8_t>(recs.size() & 0xFF));
    b.push_back(static_cast<uint8_t>(recs.size() >> 8));
    for (const Rec& r : recs) coop::save_record_wire::SerSave(b, r);
    return b;
}

uint64_t HashLike(uint32_t eid, const std::vector<Rec>& recs) {
    return coop::blob_chunks::Fnv64(PackLike(eid, recs));
}

// The size the FAN-OUT would produce: BroadcastContainer re-packs with nested indices NEUTERED,
// and NeuterNestedIndex resizes an empty ints[] to one element before writing -1, so the wire pack
// can be LARGER than the host-local one (capture condition 9).
size_t FanoutBytes(uint32_t eid, const std::vector<Rec>& recs) {
#if MV_CUSTODY_FIXED
    std::vector<Rec> neutered = recs;
    for (Rec& r : neutered) {
        if (StubIsNestedContainerRecord(r)) NeuterNestedIndex(r);
    }
    return PackLike(eid, neutered).size();
#else
    // The pre-fix quantity: the HOST-LOCAL pack, which is the wrong one.
    return PackLike(eid, recs).size();
#endif
}

CustodyRecordSet MakeSet(std::vector<Rec> recs) {
    CustodyRecordSet s;
    s.hash = HashLike(0, recs);
    s.index = 5;
    s.fanoutBytes = FanoutBytes(0, recs);
    s.recs = std::move(recs);
    return s;
}

// What the CAPTURE hands the store. Under the fix the capture reads with neuterNested=FALSE, so a
// nested container's host slot number survives into the park; an earlier draft neutered here and silently
// emptied every nested container on re-attach.
CustodyRecordSet CaptureSet(std::vector<Rec> recs) {
#if !MV_CUSTODY_FIXED
    for (Rec& r : recs) {
        if (StubIsNestedContainerRecord(r)) NeuterNestedIndex(r);
    }
#endif
    return MakeSet(std::move(recs));
}

// ---- one full park -> arm -> consume, with everything at its ordinary value ----------------------

struct Run {
    Reason park = Reason::NotParked;
    Reason arm = Reason::NotParked;
    Reason consume = Reason::NotParked;
    bool parked = false;
    bool armed = false;
    bool applied = false;
};

Run ParkArmConsume(CustodyParkStore& s, CustodyRecordSet set,
                   const std::wstring& key = L"K", const std::wstring& cls = L"prop_box_C",
                   int authorSlot = 1, uint32_t authorGen = 7, uint32_t worldGen = 3,
                   const std::wstring& enrolled = L"K",
                   const std::wstring& observedCls = L"prop_box_C") {
    const SlotIo io = MakeIo();
    const auto now = Clock::now();
    Run r;
    r.parked = s.Park(CustodyKeyT{key, cls}, std::move(set), authorSlot, authorGen, worldGen,
                      now, r.park);
    if (!r.parked) return r;
    r.armed = s.Arm(CustodyKeyT{key, cls}, authorSlot, authorGen, worldGen, kTargetActor,
                    now, r.arm);
    if (!r.armed) return r;
    r.applied = s.TryConsume(kTargetActor, kTargetEid, kInv, enrolled, observedCls, io, now,
                             r.consume);
    return r;
}

std::vector<Rec> TwoItems() {
    return {MakeItem(L"item_can_C", L"a"), MakeItem(L"item_bottle_C", L"b")};
}

}  // namespace

int main() {
    std::printf("candidate_15 container custody -- MV_CUSTODY_FIXED=%d\n", MV_CUSTODY_FIXED);
    std::printf("MaxFanoutBytes=%zu kMaxCustodyParks=%zu kCeilingSec=%d kMaxPeers=%d\n",
                coop::props::container_custody::MaxFanoutBytes(), kMaxCustodyParks,
                kCustodyParkCeilingSec, kMaxPeers);

    // ===== 1. The core claim ====================================================================
    {
        ResetWorld();
        CustodyParkStore s;
        const std::vector<Rec> input = TwoItems();
        const uint64_t wantHash = HashLike(0, input);
        const Run r = ParkArmConsume(s, MakeSet(input));
        CheckReason(r.consume, Reason::Applied, "1  park -> arm -> consume applies");
        Check(g_w.writeCount == 1, "1  exactly one write");
        Check(g_w.written.size() == 2 && g_w.written[0].key == L"a" && g_w.written[1].key == L"b",
              "1  the identical ordered record set is written");
        Check(HashLike(0, g_w.written) == wantHash, "1  the identical FNV-64 hash");
        Check(s.parksApplied == 1 && s.Size() == 0, "1  the park is retired on apply");
    }

    // ===== 2. Never overwrite newer state ========================================================
    {
        ResetWorld();
        g_w.freshCount = 3;
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::DiscardedFreshNotEmpty,
                    "2  a fresh slot already holding records DISCARDS");
        Check(g_w.writeCount == 0, "2  and writes nothing");
    }

    // ===== 3. Arm for a key never parked =========================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        const bool armed = s.Arm(CustodyKeyT{L"NEVER", L"prop_box_C"}, 1, 7, 3, kTargetActor,
                                 Clock::now(), why);
        Check(!armed, "3  an arm for an unparked key fails");
        CheckReason(why, Reason::NotParked, "3  NotParked");
        Check(g_w.writeCount == 0, "3  nothing is written");
    }

    // ===== 4. Consumed exactly once ==============================================================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::Applied, "4  first consume applies");
        Reason second = Reason::Applied;
        const SlotIo io = MakeIo();
        const bool again = s.TryConsume(kTargetActor, kTargetEid, kInv, L"K", L"prop_box_C", io,
                                        Clock::now(), second);
        Check(!again, "4  a second consume does not apply");
        CheckReason(second, Reason::NotParked, "4  NotParked -- consumed exactly once");
        Check(g_w.writeCount == 1, "4  still exactly one write");
    }

    // ===== 5. Re-park replaces, and the map and the FIFO stay paired ==============================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"K", L"prop_box_C"},
               MakeSet({MakeItem(L"item_A_C", L"A")}), 1, 7, 3, Clock::now(), why);
        s.Park(CustodyKeyT{L"K", L"prop_box_C"},
               MakeSet({MakeItem(L"item_B_C", L"B")}), 1, 7, 3, Clock::now(), why);
        Check(s.Size() == 1, "5  a second park for the same key does not accumulate");
        Check(s.FifoCount(L"K") == 1, "5  the FIFO holds exactly one entry for K");
        const SlotIo io = MakeIo();
        Reason armWhy = Reason::Applied;
        s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 1, 7, 3, kTargetActor, Clock::now(), armWhy);
        Reason consumeWhy = Reason::Applied;
        s.TryConsume(kTargetActor, kTargetEid, kInv, L"K", L"prop_box_C", io, Clock::now(),
                     consumeWhy);
        CheckReason(consumeWhy, Reason::Applied, "5  the survivor consumes");
        Check(g_w.written.size() == 1 && g_w.written[0].key == L"B", "5  latest wins (B, not A)");
        Check(s.FifoCount(L"K") == 0, "5  the consume clears the FIFO copy too");
    }

    // ===== 6. The cap evicts exactly the oldest ==================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        std::vector<std::wstring> keys;
        for (size_t i = 0; i <= kMaxCustodyParks; ++i) {  // kMaxCustodyParks + 1 distinct keys
            wchar_t buf[16];
            std::swprintf(buf, 16, L"K%02zu", i);
            keys.push_back(buf);
            s.Park(CustodyKeyT{keys.back(), L"prop_box_C"}, MakeSet(TwoItems()),
                   1, 7, 3, Clock::now(), why);
        }
        Check(s.Size() == kMaxCustodyParks, "6  the store holds exactly the cap");
        Check(!s.Has(keys.front()), "6  the OLDEST key was evicted");
        Check(s.Has(keys.back()), "6  the newest key survives");
        Check(s.parksEvicted == 1, "6  exactly one eviction");
        const std::vector<std::wstring> fifo = s.FifoKeys();
        bool paired = fifo.size() == s.Size();
        for (size_t i = 0; paired && i < fifo.size(); ++i) {
            if (fifo[i] != keys[i + 1] || !s.Has(fifo[i])) paired = false;
        }
        Check(paired, "6  map and FIFO agree on membership, size and order");
    }

    // ===== 7. Four aging assertions, none of them a join bracket ==================================
    // NoteJoinSnapshotBracket is client-only and g_joinBracketOpen is permanently
    // false on the host, the only role that holds a park. No assertion here references it.
    {
        bool connected[kMaxPeers] = {true, true, true, true};
        uint32_t gens[kMaxPeers] = {1, 7, 7, 7};

        {   // (a) the absolute ceiling
            ResetWorld();
            CustodyParkStore s;
            Reason why = Reason::Applied;
            const auto t0 = Clock::now();
            s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, t0, why);
            s.Sweep(t0 + std::chrono::seconds(kCustodyParkCeilingSec + 1), 3, connected, gens);
            Check(s.Size() == 0 && s.parksExpired == 1, "7a park past the ceiling EXPIRES");
        }
        {   // (b) the world generation
            ResetWorld();
            CustodyParkStore s;
            Reason why = Reason::Applied;
            const auto t0 = Clock::now();
            s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, t0, why);
            s.Sweep(t0, 4, connected, gens);
            Check(s.Size() == 0
                      && s.parksCleared[static_cast<size_t>(Reason::WorldChanged)] == 1,
                  "7b a stale world generation CLEARS world-changed");
        }
        {   // (c) author liveness
            ResetWorld();
            CustodyParkStore s;
            Reason why = Reason::Applied;
            const auto t0 = Clock::now();
            s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, t0, why);
            bool gone[kMaxPeers] = {true, false, true, true};
            s.Sweep(t0, 3, gone, gens);
            Check(s.Size() == 0 && s.parksCleared[static_cast<size_t>(Reason::AuthorLeft)] == 1,
                  "7c a disconnected author CLEARS author-left");
        }
        {   // (d) author IDENTITY, while the slot still reads connected
            ResetWorld();
            CustodyParkStore s;
            Reason why = Reason::Applied;
            const auto t0 = Clock::now();
            s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, t0, why);
            uint32_t replaced[kMaxPeers] = {1, 8, 7, 7};
            s.Sweep(t0, 3, connected, replaced);
            Check(s.Size() == 0
                      && s.parksCleared[static_cast<size_t>(Reason::AuthorReplaced)] == 1,
                  "7d a replaced occupant CLEARS author-replaced while still connected");
        }
    }

    // ===== 8. Order and order-sensitivity =========================================================
    {
        ResetWorld();
        CustodyParkStore s;
        // Minimal records (no class name): 512 of them must fit UNDER the fan-out ceiling, or the
        // set would be refused at Park by capture condition 9 and this assertion would be measuring
        // the ceiling rather than the ordering.
        std::vector<Rec> big;
        big.reserve(512);
        for (int i = 0; i < 512; ++i) {
            wchar_t buf[16];
            std::swprintf(buf, 16, L"i%03d", i);
            Rec rec;
            rec.key = buf;
            big.push_back(rec);
        }
        std::printf("     [8] 512-record fan-out pack = %zu B (ceiling %zu)\n",
                    PackLike(0, big).size(), coop::props::container_custody::MaxFanoutBytes());
        const Run r = ParkArmConsume(s, MakeSet(big));
        CheckReason(r.consume, Reason::Applied, "8  a 512-record set consumes");
        bool inOrder = g_w.written.size() == big.size();
        for (size_t i = 0; inOrder && i < big.size(); ++i) {
            if (g_w.written[i].key != big[i].key) inOrder = false;
        }
        Check(inOrder, "8  512 records round-trip IN ORDER");
        std::vector<Rec> swapped = big;
        std::swap(swapped[0], swapped[1]);
        Check(HashLike(0, swapped) != HashLike(0, big), "8  the hash is order-sensitive");
    }

    // ===== 9. The two capture bounds ==============================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        // Minimal records again, so the 512 case is decided by the CAP and not by the ceiling.
        std::vector<Rec> at512;
        for (int i = 0; i < 512; ++i) {
            Rec rec;
            rec.key = L"x";
            at512.push_back(rec);
        }
        Check(s.Park(CustodyKeyT{L"K512", L"prop_box_C"}, MakeSet(at512), 1, 7, 3,
                     Clock::now(), why),
              "9  a 512-record set is accepted at Park");
        std::vector<Rec> at513 = at512;
        at513.push_back(MakeItem(L"item_can_C", L"x"));
        const bool over = s.Park(CustodyKeyT{L"K513", L"prop_box_C"}, MakeSet(at513), 1, 7, 3,
                                 Clock::now(), why);
        Check(!over, "9  a 513-record set is refused at Park");
        CheckReason(why, Reason::CapExceeded, "9  cap-512");

        // The fan-out half. Every record is a nested container with an EMPTY ints[], so the
        // host-local pack is small and NeuterNestedIndex GROWS each one to a one-element array
        // before writing -1: the wire pack is the larger of the two. The set is built so the
        // host-local pack is UNDER the ceiling while the neutered pack is OVER it, which is the
        // only way condition 9's correction is what turns this red.
        const size_t ceiling = coop::props::container_custody::MaxFanoutBytes();
        std::vector<Rec> fat;
        size_t hostLocal = 0;
        size_t wire = 0;
        // Search for a count of nested-no-ints records that straddles the ceiling: each one costs
        // the SAME extra 8 bytes on the wire (an absent ints[] is grown to a one-element array
        // before -1 is written), so a set exists whose host-local pack fits and whose wire pack does
        // not. Searched rather than hard-coded, so a codec size change reports BROKEN instead of
        // quietly turning this into a different test.
        for (size_t n = 1; n <= 1024; ++n) {
            std::vector<Rec> trial(n, MakeNestedNoInts());
            const size_t local = PackLike(0, trial).size();
            std::vector<Rec> neutered = trial;
            for (Rec& r : neutered) {
                if (StubIsNestedContainerRecord(r)) NeuterNestedIndex(r);
            }
            const size_t w = PackLike(0, neutered).size();
            if (local <= ceiling && w > ceiling) {
                fat = std::move(trial);
                hostLocal = local;
                wire = w;
                break;
            }
        }
        std::printf("     [9] records=%zu hostLocal=%zu wire=%zu ceiling=%zu\n",
                    fat.size(), hostLocal, wire, ceiling);
        Check(wire > hostLocal, "9  the neutered (wire) pack is LARGER than the host-local pack");
        if (!fat.empty() && hostLocal <= ceiling && wire > ceiling) {
            const bool ok = s.Park(CustodyKeyT{L"KFAT", L"prop_box_C"}, MakeSet(fat), 1, 7, 3,
                                   Clock::now(), why);
            Check(!ok, "9  a set whose FAN-OUT pack exceeds the ceiling is refused at Park");
            CheckReason(why, Reason::OverBlobCeiling, "9  over-blob-ceiling");
        } else {
            // Do not let a mis-sized fixture look like coverage.
            Check(false, "9  FIXTURE BROKEN: needs hostLocal <= ceiling < wire");
        }
    }

    // ===== 10. Nothing survives a session =========================================================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        Check(r.applied, "10 (setup) a park was taken and applied");
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"L", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, Clock::now(), why);
        s.Clear();
        Check(s.Size() == 0 && s.FifoKeys().empty(), "10 Clear empties BOTH containers");
        Check(s.parksTaken == 0 && s.parksApplied == 0 && s.writes == 0,
              "10 Clear zeroes the counters");
    }

    // ===== 11. The author-slot binding at the arm ==================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, Clock::now(), why);
        Reason armWhy = Reason::Applied;
        const bool armed = s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 2, 7, 3, kTargetActor,
                                 Clock::now(), armWhy);
        Check(!armed, "11 a different peer slot does not arm");
        CheckReason(armWhy, Reason::AuthorMismatch, "11 author-mismatch");
        Check(g_w.writeCount == 0, "11 nothing is written");
        Check(s.Size() == 0 && s.FifoCount(L"K") == 0, "11 the park is retired");
    }

    // ===== 12. The class binding ===================================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"K", L"prop_crate_C"}, MakeSet(TwoItems()), 1, 7, 3, Clock::now(), why);
        Reason armWhy = Reason::Applied;
        const bool armed = s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 1, 7, 3, kTargetActor,
                                 Clock::now(), armWhy);
        Check(!armed, "12 a different class does not arm");
        CheckReason(armWhy, Reason::ClassMismatch, "12 class-mismatch");
    }

    // ===== 13. Arm once ============================================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, Clock::now(), why);
        Reason a1 = Reason::Applied, a2 = Reason::Applied;
        const bool first = s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 1, 7, 3, kTargetActor,
                                 Clock::now(), a1);
        const bool second = s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 1, 7, 3, kOtherActor,
                                  Clock::now(), a2);
        Check(first, "13 the first matching host spawn arms");
        Check(!second, "13 a second spawn does not arm");
        CheckReason(a2, Reason::AlreadyArmed, "13 already-armed");
        // An already-ARMED park has left PARKED, so the rule "every exit from PARKED retires
        // the park" does not reach it: a duplicated PropDropIntent must not destroy a park that is
        // already waiting for its adoption. That is an interpretation of the state machine, and it
        // is asserted here rather than left implicit.
        const SlotIo io = MakeIo();
        Reason c = Reason::NotParked;
        s.TryConsume(kTargetActor, kTargetEid, kInv, L"K", L"prop_box_C", io, Clock::now(), c);
        CheckReason(c, Reason::Applied, "13 the already-ARMED park still consumes for its own actor");
    }

    // ===== 14. The enrolled-key re-check (the MarkPropElement re-key) ===============================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()), L"K", L"prop_box_C", 1, 7, 3,
                                     /*enrolled=*/L"K-rekeyed");
        CheckReason(r.consume, Reason::KeyRekeyed,
                    "14 an actor re-keyed at adoption refuses key-rekeyed");
        Check(g_w.writeCount == 0, "14 nothing is written");
    }

    // ===== 15. The latch, both halves ===============================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        const bool noLatch = s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()),
                                    kNoAuthorSlot, 7, 3, Clock::now(), why);
        Check(!noLatch, "15 a park with NO latched author slot is refused");
        CheckReason(why, Reason::AuthorUnknown, "15 author-unknown");
        const bool minusOne = s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()),
                                     -1, 7, 3, Clock::now(), why);
        Check(!minusOne, "15 a latched -1 is refused");
        CheckReason(why, Reason::SlotOutOfRange, "15 slot-out-of-range for -1");
        const bool tooBig = s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()),
                                   kMaxPeers, 7, 3, Clock::now(), why);
        Check(!tooBig, "15 a latched slot >= kMaxPeers is refused");
        CheckReason(why, Reason::SlotOutOfRange, "15 slot-out-of-range for kMaxPeers");
        Check(s.Size() == 0, "15 no park was created by any of the three");
    }

    // ===== 15b. Same slot, different occupant =======================================================
    {
        ResetWorld();
        CustodyParkStore s;
        Reason why = Reason::Applied;
        s.Park(CustodyKeyT{L"K", L"prop_box_C"}, MakeSet(TwoItems()), 1, 7, 3, Clock::now(), why);
        Reason armWhy = Reason::Applied;
        const bool armed = s.Arm(CustodyKeyT{L"K", L"prop_box_C"}, 1, 8, 3, kTargetActor,
                                 Clock::now(), armWhy);
        Check(!armed, "15b a new occupant of the SAME slot does not arm");
        CheckReason(armWhy, Reason::AuthorReplaced, "15b author-replaced");
    }

    // ===== 16. Index 0 is the host player's own inventory ============================================
    {
        ResetWorld();
        g_w.slotIndex = 0;
        g_w.world[0].index = 0;
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::SlotAliased, "16 a resolved Index of 0 refuses slot-aliased");
        Check(g_w.writeCount == 0, "16 nothing is written");
    }

    // ===== 17. The personal store's own index ========================================================
    {
        ResetWorld();
        g_w.personalIndex = 5;   // the MEASURED index, not an assumed 0
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::SlotAliased,
                    "17 an Index equal to the personal store's refuses slot-aliased");
        Check(g_w.writeCount == 0, "17 nothing is written");
    }

    // ===== 18. Another live world container's index ==================================================
    {
        ResetWorld();
        // A DIFFERENT actor and a DIFFERENT eid, so the seam's exclusion does not remove it, and
        // the same slot index as the target.
        g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 5});
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::SlotAliased,
                    "18 an Index another live world container owns refuses slot-aliased");
        Check(g_w.writeCount == 0, "18 nothing is written");
    }

    // ===== 19. Boundary 1 ============================================================================
    {
        ResetWorld();
        g_w.isWorldContainer = false;
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::Boundary1,
                    "19 a Player=true (or unreadable) inventory refuses boundary1");
        Check(g_w.writeCount == 0, "19 nothing is written");
    }

    // ===== 20. The allocation pre-flight ==============================================================
    {
        ResetWorld();
        g_w.allocPreflight = false;
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::AllocPreflight, "20 a failed pre-flight refuses");
        Check(g_w.writeCount == 0, "20 nothing is written");
    }

    // ===== 21. Rederive exactly once on Applied, zero times on every refusal ==========================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        Check(r.applied && g_w.rederiveCount == 1, "21 Rederive runs exactly once on Applied");

        struct Perturb { const char* what; void (*apply)(); };
        const Perturb perturbs[] = {
            {"boundary1",   []{ g_w.isWorldContainer = false; }},
            {"slot-unres",  []{ g_w.slotIndexOk = false; }},
            {"index-0",     []{ g_w.slotIndex = 0; }},
            {"personal",    []{ g_w.personalIndex = 5; }},
            {"aliased",     []{ g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 5}); }},
            {"enum-unprov", []{ g_w.enumComplete = false; }},
            {"fresh",       []{ g_w.freshCount = 2; }},
            {"preflight",   []{ g_w.allocPreflight = false; }},
            {"write-fail",  []{ g_w.writeOk = false; }},
        };
        size_t rederives = 0;
        for (const Perturb& p : perturbs) {
            ResetWorld();
            p.apply();
            CustodyParkStore t;
            const Run rr = ParkArmConsume(t, MakeSet(TwoItems()));
            if (rr.applied) {
                std::printf("     [21] perturbation '%s' unexpectedly APPLIED\n", p.what);
                ++g_failures;
            }
            rederives += g_w.rederiveCount;
        }
        Check(rederives == 0, "21 Rederive runs zero times across every refusal above");
    }

    // ===== 22. The codec, and the nested index the HOST-LOCAL path must keep ===========================
    // The shipped record codec is LINKED here (coop/items/save_record_wire.cpp), not restated.
    // What IS restated is NeuterNestedIndex, which is private to container_contents_sync.cpp, so
    // this assertion proves the codec preserves what it is handed under each policy -- NOT that
    // production ReadContents passes neuterNested=false. That binding is left to an in-game run and to reading the code.
    {
        ResetWorld();
        Rec hostLocal = MakeNested(7);
        Rec wire = MakeNested(7);
        NeuterNestedIndex(wire);

        std::vector<uint8_t> b1;
        coop::save_record_wire::SerSave(b1, hostLocal);
        size_t o1 = 0;
        Rec back1;
        const bool ok1 = coop::save_record_wire::DeSave(b1, o1, back1);
        Check(ok1 && !back1.ints.empty() && !back1.ints[0].empty() && back1.ints[0][0] == 7,
              "22 the HOST-LOCAL path PRESERVES a nested ints[0][0]");

        std::vector<uint8_t> b2;
        coop::save_record_wire::SerSave(b2, wire);
        size_t o2 = 0;
        Rec back2;
        const bool ok2 = coop::save_record_wire::DeSave(b2, o2, back2);
        Check(ok2 && !back2.ints.empty() && !back2.ints[0].empty() && back2.ints[0][0] == -1,
              "22 the WIRE path replaces it with the -1 sentinel");

        // And the capture the store is actually fed keeps it: this is the half that goes red when
        // the host-local path neuters.
        const CustodyRecordSet captured = CaptureSet({MakeNested(7)});
        Check(!captured.recs.empty() && !captured.recs[0].ints.empty()
                  && captured.recs[0].ints[0][0] == 7,
              "22 the CAPTURE hands the park a preserved nested index");
    }

    // ===== 23. The hostile-input contract is not weakened ===============================================
    {
        std::vector<uint8_t> b;
        coop::save_record_wire::SerSave(b, MakeNested(7));
        Check(b.size() > 8, "23 (setup) a record serialises to something to truncate");
        std::vector<uint8_t> truncated(b.begin(), b.begin() + static_cast<long>(b.size() / 2));
        size_t o = 0;
        Rec out;
        Check(!coop::save_record_wire::DeSave(truncated, o, out),
              "23 a truncated blob fails cleanly");
        Check(!coop::save_record_wire::Feasible(1000000u, b, 0),
              "23 an oversized declared count fails the Feasible bound");
        size_t o2 = 0;
        Rec ok;
        Check(coop::save_record_wire::DeSave(b, o2, ok), "23 the intact blob still round-trips");
    }

    // ===== 24. A truncated enumeration is not a pass =====================================================
    {
        ResetWorld();
        g_w.enumComplete = false;
        // The resolved index collides with nothing the seam DID return.
        g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 9});
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::EnumerationUnproven,
                    "24 an incomplete enumeration refuses enumeration-unproven");
        Check(g_w.writeCount == 0, "24 nothing is written -- an unproven absence is not an absence");
    }

    // ===== 24b. A COMPLETE enumeration must CONSUME, not refuse ==========================================
    {
        // (a) The target is the only container: need = 0, the store sizes want = 1, n = 0.
        // Shares its world with assertion 25 and claims a different thing: 25 is about the seam's
        // self-exclusion, this is about the store's buffer SIZING. Under a want = need store this
        // world returns n == want == 0 and refuses on a correct build -- the headroom defect.
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::Applied, "24b(a) a lone-container world CONSUMES");
        Check(g_w.writeCount == 1, "24b(a) write counter 1");
        Check(g_w.lastWant == 1, "24b(a) the fill call asked for need + 1 = 1");
    }
    {
        // (b) One other container, at a DIFFERENT index: need = 1, want = 2, n = 1.
        ResetWorld();
        g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 9});
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::Applied,
                    "24b(b) a proven non-collision CONSUMES");
        Check(g_w.writeCount == 1, "24b(b) write counter 1");
        Check(g_w.lastWant == 2, "24b(b) the fill call asked for need + 1 = 2 (HEADROOM)");
    }

    // ===== 25. Self-exclusion, by both identifiers ========================================================
    {
        ResetWorld();
        // The world holds exactly the entry the source guarantees is there at consume time: the
        // adopted container itself, with the target's actor AND eid, at the target's own index.
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::Applied,
                    "25 the target's own snapshot entry is excluded, so the consume APPLIES");
        Check(g_w.writeCount == 1, "25 write counter 1");
        Check(g_w.enumCalls == 2, "25 the seam was called twice: count-only, then fill");
    }
    {
        // The omission row for 25, driven inside the FIXED arm because it lives in the SEAM and not
        // in the store: the same world through a stub that IGNORES selfActor / selfEid returns the
        // target's own index, and the consume then refuses. That is what makes "omit self-exclusion"
        // detectable at all -- without it, a self-including seam turns every consume into a logged
        // no-op and an in-game run fails as an unexplained refusal.
        ResetWorld();
        g_w.honourExclusion = false;
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, MakeSet(TwoItems()));
        CheckReason(r.consume, Reason::SlotAliased,
                    "25 a seam that does NOT self-exclude makes the consume refuse (the omission)");
        Check(g_w.writeCount == 0, "25 nothing is written under the omission");
    }

    // ===== 26. A nested record naming slot 0 ==============================================================
    {
        ResetWorld();
        CustodyParkStore s;
        std::vector<Rec> set = {MakeItem(L"item_can_C", L"a"), MakeNested(0),
                                MakeItem(L"item_can_C", L"c")};
        const Run r = ParkArmConsume(s, CaptureSet(set));
        CheckReason(r.consume, Reason::NestedSlotAliased,
                    "26 a nested ints[0][0] of 0 refuses nested-slot-aliased");
        Check(g_w.writeCount == 0 && g_w.written.empty(),
              "26 the WHOLE set is refused -- the other records are not written either");
    }

    // ===== 27. The four remaining nested aliases ===========================================================
    {
        {   // the personal store's index
            ResetWorld();
            g_w.personalIndex = 3;
            CustodyParkStore s;
            const Run r = ParkArmConsume(s, CaptureSet({MakeNested(3)}));
            CheckReason(r.consume, Reason::NestedSlotAliased, "27 nested == the personal store index");
            Check(g_w.writeCount == 0, "27 nothing written (personal)");
        }
        {   // the outer container's OWN fresh index -- a record cannot address its parent's slot
            ResetWorld();
            CustodyParkStore s;
            const Run r = ParkArmConsume(s, CaptureSet({MakeNested(5)}));
            CheckReason(r.consume, Reason::NestedSlotAliased, "27 nested == the outer's own index");
            Check(g_w.writeCount == 0, "27 nothing written (outer)");
        }
        {   // another live world container's index
            ResetWorld();
            g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 9});
            CustodyParkStore s;
            const Run r = ParkArmConsume(s, CaptureSet({MakeNested(9)}));
            CheckReason(r.consume, Reason::NestedSlotAliased,
                        "27 nested == another live world container's index");
            Check(g_w.writeCount == 0, "27 nothing written (other container)");
        }
        {   // out of range for the current GObjStack -- the same bound GObjStackSlot applies at :209
            ResetWorld();
            CustodyParkStore s;
            const Run r = ParkArmConsume(s, CaptureSet({MakeNested(g_w.stackLen)}));
            CheckReason(r.consume, Reason::NestedSlotAliased, "27 nested >= GObjStackLength()");
            Check(g_w.writeCount == 0, "27 nothing written (out of range)");
        }
    }

    // ===== 28. The zero-fill case: a record that "has no index" IS index 0 ==================================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, CaptureSet({MakeNestedNoInts()}));
        CheckReason(r.consume, Reason::NestedSlotAliased,
                    "28 an EMPTY ints[] reads as index 0 and refuses");
        Check(g_w.writeCount == 0, "28 nothing is written (no ints)");
    }
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, CaptureSet({MakeNestedEmptyInts()}));
        CheckReason(r.consume, Reason::NestedSlotAliased,
                    "28 an empty ints[0] reads as index 0 and refuses");
        Check(g_w.writeCount == 0, "28 nothing is written (empty ints[0])");
    }

    // ===== 29. The accepted sentinel: aliases are refused, nested cases are not ==============================
    {
        ResetWorld();
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, CaptureSet({MakeItem(L"item_can_C", L"a"), MakeNested(-1)}));
        CheckReason(r.consume, Reason::Applied, "29 a nested ints[0][0] of -1 is ACCEPTED");
        Check(g_w.writeCount == 1, "29 the set is written");
        Check(s.nestedSentinelAccepted == 1, "29 nestedSentinelAccepted reads 1");
    }

    // ===== 30. Row 12: an unproven enumeration poisons the nested test too ====================================
    {
        ResetWorld();
        g_w.enumComplete = false;
        // A nested index that would have needed the enumeration to be cleared or condemned.
        g_w.world.push_back(WorldEntry{kOtherActor, kOtherEid, 9});
        CustodyParkStore s;
        const Run r = ParkArmConsume(s, CaptureSet({MakeNested(9)}));
        CheckReason(r.consume, Reason::EnumerationUnproven,
                    "30 an unproven enumeration refuses enumeration-unproven, "
                    "not nested-slot-aliased and not Applied");
        Check(g_w.writeCount == 0, "30 nothing is written");
    }

    // ---- the omission table ---------------------------------------------------------------------------
    // Twenty-seven omissions; twenty-four map to a named assertion above, three are declared
    // UNCOVERABLE by a header-only harness and are stated rather than invented:
    //
    //   delete the capture call (the PRODUCTION call site)   -> NONE here; an in-game capture-hook
    //                                                           [CUSTODY-INIT] line is the detector
    //   delete the author-slot latch (the PRODUCTION line)   -> NONE here; in-game only, by the absence
    //                                                           of a `parked` line
    //   delete the subsystems.cpp Tick registration          -> NONE here; the in-game Tick
    //                                                           [CUSTODY-INIT] clause
    //   delete Park's own author-slot requirement            -> 15
    //   delete the class binding                             -> 12
    //   delete the author-generation binding                 -> 15b / 7d
    //   delete the arm-once rule                             -> 13
    //   delete the enrolled-key re-check                     -> 14
    //   delete the fresh-empty check                         -> 2
    //   delete the FIFO eviction                             -> 6
    //   delete the ceiling / world-gen / author-left / author-gen sweep -> 7a / 7b / 7c / 7d
    //   delete the map/FIFO pairing on consume               -> 5
    //   neuter the nested index on the host-local path       -> 22
    //   park on a Player=true inventory                      -> 19
    //   apply without the allocation pre-flight              -> 20
    //   apply without RederiveManagedState                   -> 21
    //   refuse nothing on Index == 0                         -> 16
    //   refuse nothing on the personal-store index           -> 17
    //   refuse nothing on another container's index          -> 18
    //   treat a truncated enumeration as a pass              -> 24
    //   omit self-exclusion from the enumeration             -> 25
    //   omit the nested-index test entirely                  -> 26, 27
    //   test the stored nested index instead of the EFFECTIVE one -> 28
    //   refuse every nested record instead of only aliasing ones  -> 29
    //   let a nested record pass on a truncated enumeration  -> 30
    //   size the enumeration buffer without headroom         -> 24b
    //   consume twice                                        -> 4
    //
    // The pre-fix arm below does not delete these one at a time: it is an earlier draft's whole store,
    // so it turns many of them red at once. Which ones is visible in this file's own FAIL lines,
    // and it is MEASURED at the head of this file: 19 of the 32 assertions go red, not 27.
    //
    // The omission rows whose assertion is GREEN IN BOTH ARMS are therefore NOT falsified by this
    // suite, and are declared here as well as at the head of the file so the table cannot be read
    // as coverage it does not carry:
    //
    //   refuse nothing on Index == 0                 -> 16   NO negative control here
    //   refuse nothing on the personal-store index   -> 17   NO negative control here
    //   refuse nothing on another container's index  -> 18   NO negative control here
    //   park on a Player=true inventory              -> 19   NO negative control here
    //   apply without the allocation pre-flight      -> 20   NO negative control here
    //   consume twice                                -> 4    NO negative control here
    //   delete the ceiling / world-gen / author-left
    //     quarters of the sweep                      -> 7a / 7b / 7c   NO negative control here.
    //                                                        Only 7d, the author-GEN quarter, is red
    //                                                        in the pre-fix arm; the row as a whole
    //                                                        is therefore NOT falsified by it.
    //   omit self-exclusion from the enumeration     -> 25   exercised IN-ARM (25's second half),
    //                                                        because that omission is in an
    //                                                        INJECTED seam; not by the header swap
    //
    // That is EIGHT rows, not six. EIGHT is a count of TABLE ROWS, and it is NOT the count of
    // assertions with no negative control, which is THIRTEEN; the head of this file reconciles the
    // two exactly. Do not carry either number over to the other quantity. The count was wrong in
    // this block for one version, and the correction is recorded rather than quietly applied
    // because this exact miscount -- a derived list under-enumerating what the banner above it
    // states correctly -- is the defect this file has now had to fix twice. The banner's thirteen
    // green-in-both assertions are the authority; any list derived from it must reconcile against
    // it, not be written out from memory.
    //
    // That draft already performed each of those checks, so hiding the shipped header cannot delete
    // one -- which is the whole of what "no negative control" means here. It is a statement about
    // THIS SUITE'S PRE-FIX ARM, not about the shipped policy.
    //
    // AN EARLIER VERSION WENT FURTHER AND WAS WRONG. It said that deleting any of them from the
    // shipped header today would leave both arms green, and named the retire-on-apply
    // double-consume guard (4) and the ceiling, world-generation and author-left sweep clauses
    // (7a/7b/7c) as further cases of it. Deleting a refusal was then actually tried, one refusal at
    // a time, against a COPY of the include tree with the FIXED arm rebuilt each time: EIGHT OF THE
    // NINE deletions attempted turn the fixed arm RED, INCLUDING 4, 7a, 7b and 7c -- precisely the
    // clauses that sentence had been extended to cover. The PRE-FIX arm cannot see any of those
    // deletions at all, because it compiles with the header REMOVED -- checked on one of the nine,
    // which still produced its usual 48 failing assertions -- and deduced, not measured, for the
    // other eight. The FIXED arm is the one that compiles against the header, and it is not blind.
    //
    // The ONE deletion that is NOT caught is Row 8's Index == 0 (assertion 16), and only because
    // Row 9's personal-store refusal subsumes it in the world that case builds -- a REDUNDANCY IN
    // THE SHIPPED POLICY, recorded at the head of this file. Row 25's omission was not among the
    // nine and is claimed neither way: it lives in an INJECTED seam, not in the header, so there
    // is nothing there to delete.
    //
    // What remains true of 4 and 7a/7b/7c is what put them on this list in the first place: they
    // are the store's BOUNDS, and an expiry or clear
    // during an in-game run invalidates that run.

    std::printf("candidate_15: %d failing assertion(s)\n", g_failures);
    return g_failures == 0 ? 0 : 1;
}
