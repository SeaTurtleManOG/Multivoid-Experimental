#if __has_include("coop/interactables/pending_retry.h")
#include "coop/interactables/pending_retry.h"
#else
// The pinned deadline-first decision, retained so this same test source can
// establish the pre-fix negative before the production policy header exists.
namespace coop::drive_sync {
enum class PendingRetryDisposition { Apply, Keep, Expire };
constexpr PendingRetryDisposition ClassifyPendingRetry(bool resolvable, bool expired) {
    if (expired) return PendingRetryDisposition::Expire;
    return resolvable ? PendingRetryDisposition::Apply : PendingRetryDisposition::Keep;
}
}  // namespace coop::drive_sync
#endif

#include <cstdio>

namespace {

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    using coop::drive_sync::ClassifyPendingRetry;
    using coop::drive_sync::PendingRetryDisposition;
    bool ok = true;

    ok &= Check(ClassifyPendingRetry(false, false) == PendingRetryDisposition::Keep,
                "positive control keeps an unresolved entry before its deadline");
    ok &= Check(ClassifyPendingRetry(true, false) == PendingRetryDisposition::Apply,
                "positive control applies a resolvable entry before its deadline");

    ok &= Check(ClassifyPendingRetry(true, true) == PendingRetryDisposition::Apply,
                "resolution wins when the deadline is reached");
    ok &= Check(ClassifyPendingRetry(false, true) == PendingRetryDisposition::Expire,
                "an unresolved entry is discarded after its deadline");
    return ok ? 0 : 1;
}
