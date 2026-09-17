// coop/props/place_queue_admission.h -- admission policy for the client pending-place queue.
//
// `cap` is a runaway backstop, not a budget. The policy picks WHICH entry loses when the queue is
// full. prop_drop_intent's drain runs five filters that never read the extraction latch (still live,
// not tracked to an eid, no late incoming-echo mark, not on the hand axis, key restored within
// kMaxKeyTries), then authors if the entry is PARKED, a whitelisted FRESH BIRTH (reel/module/drive)
// or a CONTAINER EXTRACTION. Only extraction is knowable at enqueue (parking needs the key, which the
// finish-spawn hook usually runs too early to read), so a latched arrival's authoring condition is a
// strict superset of a latch-less occupant's. That is DOMINANCE, not certainty: a latched entry whose
// key never restores authors nothing, and an evicted occupant may have been a genuine place, so
// eviction is a last resort taken only at a full queue. Against a reserve (latch-less arrivals admitted
// only below cap-R, bound fixed at 32), eviction refuses no arrival a reserve admits and refuses an
// extraction only when every occupant is latched, with no R to pick; but it can RETAIN fewer latch-less
// entries (cap 4, R=1, arrivals L,L,L,E,E: eviction keeps 2, a reserve 3), concentrating that loss on
// streams that actually carry extraction arrivals.

#pragma once

#include <cstddef>

namespace coop::prop_drop_intent {

enum class PlaceQueueAdmission {
    Admit,            // a slot is free: seat the arrival, evict nothing
    AdmitByEviction,  // queue full: drop the oldest latch-less occupant, then seat the arrival
    Refuse,           // queue full and nothing may be traded away: the arrival is lost
};

// PRECONDITION: `queueSize <= cap`. That is the CALLER's invariant, not something this function
// enforces -- as a pure function it answers for any input, and asked with queueSize > cap it can
// still return AdmitByEviction, which erase-then-push would leave above the bound. The invariant
// holds in prop_drop_intent because the queue grows in exactly one place -- the push this decision
// guards, which adds one entry after erasing one on an eviction -- while the drain only shrinks it
// (it swaps in `keep`, built with at most one push per surviving entry) and the teardown paths
// clear it. So the queue enters every call at or below the bound. The latch is a CONSUMING read:
// sample it once per arrival, before this call, and carry it into the enqueued entry (sampling and
// not enqueuing destroys the only evidence); this function consumes nothing, safe to re-ask on one sample.
//
// `queueSize`               entries currently pending (<= cap, see above)
// `cap`                     the absolute bound (kMaxPending); under the precondition the result
//                           never takes the queue past it
// `arrivalEligible`         the arrival carries the container-extraction latch (sampled once)
// `oldestIneligiblePresent` at least one occupant does NOT carry the latch
constexpr PlaceQueueAdmission ClassifyPlaceQueueArrival(size_t queueSize, size_t cap,
                                                        bool arrivalEligible,
                                                        bool oldestIneligiblePresent) {
    if (queueSize < cap) return PlaceQueueAdmission::Admit;
    if (arrivalEligible && oldestIneligiblePresent) return PlaceQueueAdmission::AdmitByEviction;
    return PlaceQueueAdmission::Refuse;
}

}  // namespace coop::prop_drop_intent
