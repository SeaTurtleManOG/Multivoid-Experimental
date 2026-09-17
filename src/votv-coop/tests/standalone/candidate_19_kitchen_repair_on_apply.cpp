// candidate_19 -- the kitchen oven's repair on an applied ON (ue_wrap/devices/kitchen_repair.h).
//
// It proves the DECISION only. It does not prove that appliance.cpp reads `fixed`, `widget` and the
// widget's Visibility from the right members, nor that fix() runs; those are read in the source, and
// the behaviour (an oven light, a switch that works) is an in-game observation.

#include <cstdint>
#include <cstdio>

#if __has_include("ue_wrap/devices/kitchen_repair.h")
#include "ue_wrap/devices/kitchen_repair.h"
#else
// The pinned pre-fix policy, retained only so this same source can prove its pre-fix negative:
// before the fix an applied ON wrote Active and ran upd(), and never repaired the oven.
namespace ue_wrap::appliance {
inline constexpr uint8_t kKitchenWidgetCollapsed = 1;
enum class KitchenRepairStep : uint8_t { None, Repair, SkipWidgetOpen };
struct KitchenRepairInputs {
    bool on = false;
    bool ready = false;
    bool fixed = false;
    bool widgetLive = false;
    uint8_t widgetVisibility = kKitchenWidgetCollapsed;
};
inline KitchenRepairStep DecideKitchenRepair(const KitchenRepairInputs&) { return KitchenRepairStep::None; }
}  // namespace ue_wrap::appliance
#endif

namespace {

using ue_wrap::appliance::DecideKitchenRepair;
using ue_wrap::appliance::KitchenRepairInputs;
using ue_wrap::appliance::KitchenRepairStep;

// ESlateVisibility: Visible 0, Collapsed 1, Hidden 2, HitTestInvisible 3, SelfHitTestInvisible 4.
// The oven widget is authored Collapsed, its enter() writes 4 and its quit writes 1.
constexpr uint8_t kCollapsed = 1;
constexpr uint8_t kEnterValue = 4;

KitchenRepairInputs In(bool on, bool ready, bool fixed, bool widgetLive, uint8_t visibility) {
    KitchenRepairInputs in;
    in.on = on;
    in.ready = ready;
    in.fixed = fixed;
    in.widgetLive = widgetLive;
    in.widgetVisibility = visibility;
    return in;
}

bool Check(bool condition, const char* label) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label);
    return condition;
}

}  // namespace

int main() {
    bool ok = true;

    // The defect: an ON applied to an oven this machine never fixed left it heating with its light
    // hidden and its switch opening the repair minigame. FAIL under the pinned pre-fix policy.
    ok &= Check(DecideKitchenRepair(In(true, true, false, true, kCollapsed)) == KitchenRepairStep::Repair,
                "DEFECT: ON + unfixed + widget present but collapsed (every oven after BeginPlay) repairs");
    ok &= Check(DecideKitchenRepair(In(true, true, false, false, kCollapsed)) == KitchenRepairStep::Repair,
                "DEFECT: ON + unfixed + no widget (before BeginPlay) repairs");

    // The widget's presence is not the signal; its Visibility is. A stale byte behind a dead
    // pointer is ignored.
    ok &= Check(DecideKitchenRepair(In(true, true, false, false, kEnterValue)) == KitchenRepairStep::Repair,
                "ON + unfixed + a non-live widget repairs whatever its Visibility byte holds");
    ok &= Check(DecideKitchenRepair(In(true, true, false, true, kEnterValue)) == KitchenRepairStep::SkipWidgetOpen,
                "ON + unfixed + widget open (enter's value 4) skips: fix() would strand the player's input");
    for (int v = 0; v <= 4; ++v) {
        if (v == kCollapsed) continue;
        ok &= Check(DecideKitchenRepair(In(true, true, false, true, static_cast<uint8_t>(v))) ==
                        KitchenRepairStep::SkipWidgetOpen,
                    "ON + unfixed + widget in any non-collapsed Visibility skips");
    }

    // What must not change.
    ok &= Check(DecideKitchenRepair(In(false, true, false, true, kCollapsed)) == KitchenRepairStep::None,
                "OFF on an unfixed oven does nothing more (active=false, fixed=false is native)");
    ok &= Check(DecideKitchenRepair(In(true, true, true, false, kCollapsed)) == KitchenRepairStep::None,
                "ON on a fixed oven does nothing more");
    ok &= Check(DecideKitchenRepair(In(true, false, false, true, kCollapsed)) == KitchenRepairStep::None,
                "an unresolved repair step does nothing (the row applies as before)");

    // The whole table: on x ready x fixed x widgetLive x Visibility 0..4 = 80 combinations. Repair
    // needs on, ready and unfixed: 5 with no live widget plus 1 with a collapsed one; the other 4
    // Visibility values with a live widget skip.
    int repair = 0;
    int skip = 0;
    int combinations = 0;
    for (int on = 0; on <= 1; ++on)
        for (int ready = 0; ready <= 1; ++ready)
            for (int fixed = 0; fixed <= 1; ++fixed)
                for (int live = 0; live <= 1; ++live)
                    for (int v = 0; v <= 4; ++v) {
                        ++combinations;
                        const KitchenRepairStep step = DecideKitchenRepair(
                            In(on != 0, ready != 0, fixed != 0, live != 0, static_cast<uint8_t>(v)));
                        if (step == KitchenRepairStep::Repair) ++repair;
                        if (step == KitchenRepairStep::SkipWidgetOpen) ++skip;
                    }
    std::printf("INFO: %d combinations, %d repair, %d skip\n", combinations, repair, skip);
    ok &= Check(combinations == 80 && repair == 6 && skip == 4,
                "exactly 6 of 80 combinations repair and 4 skip");

    return ok ? 0 : 1;
}
