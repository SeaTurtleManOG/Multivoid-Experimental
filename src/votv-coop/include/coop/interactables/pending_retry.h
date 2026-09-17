// coop/interactables/pending_retry.h -- deadline ordering for pending drive applies.

#pragma once

namespace coop::drive_sync {

enum class PendingRetryDisposition { Apply, Keep, Expire };

// Resolution is sampled before this decision. A just-resolved entry gets its
// final apply even at/after the deadline; only an unresolved expired entry dies.
constexpr PendingRetryDisposition ClassifyPendingRetry(bool resolvable, bool expired) {
    if (resolvable) return PendingRetryDisposition::Apply;
    return expired ? PendingRetryDisposition::Expire : PendingRetryDisposition::Keep;
}

}  // namespace coop::drive_sync
