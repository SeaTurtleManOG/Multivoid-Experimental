// candidate_12 -- the client drain's drive-payload authorship note.
//
// One source, both policies. With coop/props/drive_place_authorship.h on the include path this
// builds the SHIPPED predicate; with that header hidden it builds the PRE-FIX one below, so the
// negative arm is measured rather than asserted.

#if __has_include("coop/props/drive_place_authorship.h")
#include "coop/props/drive_place_authorship.h"
#else
// The pinned pre-fix decision: the note lived inside `if (freshBirth)`, so only an UNPARKED
// whitelisted birth -- the rack take -- ever noted a drive. A parked pickup-then-place and a
// container extract both reach the same point with freshBirth false and were silently skipped.
namespace coop::prop_drop_intent {
constexpr bool NotesDrivePayloadAuthorship(bool isDriveClass, bool freshBirth) {
    return isDriveClass && freshBirth;
}
}  // namespace coop::prop_drop_intent
#endif

#include <cstdio>

namespace {

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    using coop::prop_drop_intent::NotesDrivePayloadAuthorship;
    bool ok = true;

    // Positive controls. These hold under BOTH policies, so they show the predicate can say yes
    // for the right case and no for the right reason -- a negative arm that failed on these would
    // be failing for a reason unrelated to the fix.
    ok &= Check(NotesDrivePayloadAuthorship(/*isDriveClass*/true, /*freshBirth*/true) == true,
                "positive control: the shipped unparked rack-take birth of a drive notes");
    ok &= Check(NotesDrivePayloadAuthorship(/*isDriveClass*/false, /*freshBirth*/true) == false,
                "positive control: a non-drive fresh birth never notes");
    ok &= Check(NotesDrivePayloadAuthorship(/*isDriveClass*/false, /*freshBirth*/false) == false,
                "positive control: a non-drive parked place never notes");

    // THE DEFECT CASE. A drive-class place admitted by the drain's authoring gate with
    // freshBirth false -- the PARKED pickup-then-place, and the container extract -- must note its
    // authorship, or drive_sync's birth branch has no way to emit the recorded row for the eid the
    // host is about to mint. This is the assertion that is red on the pre-fix policy.
    ok &= Check(NotesDrivePayloadAuthorship(/*isDriveClass*/true, /*freshBirth*/false) == true,
                "a parked place or container extract of a drive notes its payload authorship");
    return ok ? 0 : 1;
}
