// candidate_08_place_queue_admission -- the client pending-place queue admission policy.
//
// Single translation unit, no game, no engine. Build both arms from THIS source:
//   pre-fix   cl /std:c++20 /EHsc /W4 /WX candidate_08_place_queue_admission.cpp
//             (no -I: the __has_include fallback below supplies the pinned policy)
//   post-fix  cl /std:c++20 /EHsc /W4 /WX /I <repo>/src/votv-coop/include
//             candidate_08_place_queue_admission.cpp
//
// "Eligible" is the container-extraction latch that prop_drop_intent samples once per arrival from
// a consuming read; at the drain it settles the authoring gate on its own, while an occupant that
// does not carry it must ALSO be found parked or fresh-birth whitelisted there. Both must first
// survive the same five filters that never read the latch (liveness, tracked-eid, echo, hand-axis,
// key-restore), so the relation the policy encodes is dominance, not certainty.
//
// The policy admits by EVICTION, which is an explicit deviation from the reserve the fix was
// first specified as; the header carries the deviation and the comparison behind it. What that
// means for these assertions: they check that the delivered policy behaves as specified, not that
// eviction is the better of the two shapes. A reserve would fail the killing case here by
// construction.
//
// SCOPE, so the PASS count is not over-read: this TU compiles the production POLICY header and
// nothing else. The queue below is a hand-written model of g_pending -- a vector<bool> plus a copy
// of OnClientFinishSpawn's enqueue discipline (find the oldest latch-less entry, ask the policy,
// erase-then-push on an eviction). The production enqueue block is never compiled here, so the
// sequence cases are an assertion about the policy plus a re-implementation of its caller: a
// regression in prop_drop_intent.cpp's own call -- a wrong argument order, the eviction erasing the
// wrong entry, the latch sampled on a refusing path -- would not be caught by any assertion in this
// file. Those are read from the source and compiled by the production TU build, not tested here.

#if __has_include("coop/props/place_queue_admission.h")
#include "coop/props/place_queue_admission.h"
#else
// The pinned pre-fix policy, retained so this same test source can establish its negative before
// the production policy header exists: the capacity test runs alone, ahead of any eligibility
// sample, so eligibility is not a term in the decision at all.
#include <cstddef>
namespace coop::prop_drop_intent {
enum class PlaceQueueAdmission { Admit, AdmitByEviction, Refuse };
constexpr PlaceQueueAdmission ClassifyPlaceQueueArrival(size_t queueSize, size_t cap,
                                                        bool /*arrivalEligible*/,
                                                        bool /*oldestIneligiblePresent*/) {
    return queueSize < cap ? PlaceQueueAdmission::Admit : PlaceQueueAdmission::Refuse;
}
}  // namespace coop::prop_drop_intent
#endif

#include <cstddef>
#include <cstdio>
#include <vector>

namespace {

using coop::prop_drop_intent::ClassifyPlaceQueueArrival;
using coop::prop_drop_intent::PlaceQueueAdmission;

constexpr size_t kCap = 32;  // kMaxPending

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

// The admission decision omitting the eligibility term -- the shape the defect has. Kept here so
// the killing assertion below is shown to be non-vacuous in BOTH arms.
constexpr PlaceQueueAdmission ClassifyWithEligibilityOmitted(size_t queueSize, size_t cap) {
    return queueSize < cap ? PlaceQueueAdmission::Admit : PlaceQueueAdmission::Refuse;
}

// A model of g_pending carrying only the flag the policy reads. Arrival handling mirrors
// OnClientFinishSpawn: find the oldest entry without the latch, ask the policy, erase-then-push on
// an eviction, push on an admission, drop on a refusal.
struct Queue {
    std::vector<bool> eligible;  // enqueue order, front = oldest
    size_t refusals  = 0;
    size_t evictions = 0;

    size_t OldestIneligible() const {
        for (size_t i = 0; i < eligible.size(); ++i)
            if (!eligible[i]) return i;
        return eligible.size();
    }

    PlaceQueueAdmission Arrive(bool arrivalEligible) {
        const size_t oldest = OldestIneligible();
        const PlaceQueueAdmission d = ClassifyPlaceQueueArrival(
            eligible.size(), kCap, arrivalEligible, oldest != eligible.size());
        switch (d) {
            case PlaceQueueAdmission::Refuse:
                ++refusals;
                break;
            case PlaceQueueAdmission::AdmitByEviction:
                ++evictions;
                eligible.erase(eligible.begin() + static_cast<std::ptrdiff_t>(oldest));
                eligible.push_back(arrivalEligible);
                break;
            case PlaceQueueAdmission::Admit:
                eligible.push_back(arrivalEligible);
                break;
        }
        return d;
    }

    size_t EligibleCount() const {
        size_t n = 0;
        for (bool e : eligible)
            if (e) ++n;
        return n;
    }
};

}  // namespace

int main() {
    bool ok = true;

    // Positive controls: with a slot free the policy admits either kind, so a FALSE below is a
    // decision the policy actually made and not a policy that refuses everything.
    ok &= Check(ClassifyPlaceQueueArrival(0, kCap, true, false) == PlaceQueueAdmission::Admit,
                "positive control admits an eligible arrival into an empty queue");
    ok &= Check(ClassifyPlaceQueueArrival(0, kCap, false, false) == PlaceQueueAdmission::Admit,
                "positive control admits an ineligible arrival into an empty queue");
    ok &= Check(ClassifyPlaceQueueArrival(kCap - 1, kCap, false, true) == PlaceQueueAdmission::Admit,
                "positive control admits into the last free slot without evicting");

    // The killing case, stated on the policy directly.
    ok &= Check(ClassifyPlaceQueueArrival(kCap, kCap, true, true) ==
                    PlaceQueueAdmission::AdmitByEviction,
                "full queue with an ineligible occupant admits an eligible arrival by eviction");

    // The killing case, driven as the sequence the log signature describes: 32 arrivals whose
    // eligibility is not established, then one container-extraction arrival.
    {
        Queue q;
        for (int i = 0; i < 32; ++i) q.Arrive(false);
        ok &= Check(q.eligible.size() == kCap && q.EligibleCount() == 0 && q.refusals == 0,
                    "setup: 32 ineligible arrivals fill the queue with no refusal");
        const PlaceQueueAdmission d = q.Arrive(true);
        ok &= Check(d != PlaceQueueAdmission::Refuse,
                    "the eligible container-extraction arrival is not refused at a full queue");
        ok &= Check(q.EligibleCount() == 1,
                    "the eligible container-extraction arrival is seated");
        ok &= Check(q.eligible.size() == kCap,
                    "bound: the queue holds exactly 32 after the eviction, never 33");
        ok &= Check(q.evictions == 1 && q.eligible.front() == false,
                    "exactly one occupant was evicted and it was the oldest ineligible one");
    }

    // The inverted-order control: the same 33 arrivals with the eligible one FIRST. Nothing in the
    // fix may change this case -- the eligible entry is already seated, the 33rd arrival is
    // ineligible and is refused by the unchanged bound.
    {
        Queue q;
        q.Arrive(true);
        for (int i = 0; i < 32; ++i) q.Arrive(false);
        ok &= Check(q.eligible.size() == kCap && q.EligibleCount() == 1,
                    "inverted order: the queue holds 32 entries, the eligible one among them");
        ok &= Check(q.refusals == 1 && q.evictions == 0,
                    "inverted order: one ineligible arrival refused, nothing evicted");
        ok &= Check(q.eligible.front() == true,
                    "inverted order: the first-seated eligible entry is untouched");
    }

    // A full queue with nothing tradeable keeps the genuine refusal.
    ok &= Check(ClassifyPlaceQueueArrival(kCap, kCap, true, false) == PlaceQueueAdmission::Refuse,
                "full queue of eligible occupants still refuses an eligible arrival");
    ok &= Check(ClassifyPlaceQueueArrival(kCap, kCap, false, true) == PlaceQueueAdmission::Refuse,
                "full queue refuses an ineligible arrival rather than evicting for it");

    // The bound, asserted over a long mixed stream rather than one case.
    {
        Queue q;
        size_t high = 0;
        unsigned state = 12345u;
        for (int i = 0; i < 500; ++i) {
            state = state * 1103515245u + 12345u;
            q.Arrive(((state >> 16) & 7u) == 0u);
            if (q.eligible.size() > high) high = q.eligible.size();
        }
        ok &= Check(high == kCap, "bound: 500 mixed arrivals never take the queue past 32");
    }

    // The latch is a consuming read, so the caller samples it once and reuses the value. That is
    // sound only because the policy is pure: repeated evaluation on one sample is identical, and
    // no evaluation consumes anything.
    {
        const bool sampledOnce = true;  // stands for one TakeObjInFlight() read
        const PlaceQueueAdmission first =
            ClassifyPlaceQueueArrival(kCap, kCap, sampledOnce, true);
        const PlaceQueueAdmission second =
            ClassifyPlaceQueueArrival(kCap, kCap, sampledOnce, true);
        ok &= Check(first == second,
                    "latch sampled once: the policy is pure, repeated evaluation is identical");
    }
    // And the cost of NOT carrying the sample: an arrival whose consumed latch value was dropped
    // on the way to the decision is indistinguishable from an ineligible one, and is refused.
    ok &= Check(ClassifyPlaceQueueArrival(kCap, kCap, false, true) == PlaceQueueAdmission::Refuse,
                "latch value not carried: the same arrival is refused as if ineligible");

    // The omission case: with the eligibility term omitted from the decision, the killing case
    // refuses in both arms -- the assertion above is not satisfied by any cap-only policy.
    ok &= Check(ClassifyWithEligibilityOmitted(kCap, kCap) == PlaceQueueAdmission::Refuse,
                "omission case: a cap-only decision refuses the eligible arrival");
    ok &= Check(ClassifyWithEligibilityOmitted(0, kCap) == PlaceQueueAdmission::Admit,
                "omission case control: the cap-only decision still admits below the bound");

    return ok ? 0 : 1;
}
