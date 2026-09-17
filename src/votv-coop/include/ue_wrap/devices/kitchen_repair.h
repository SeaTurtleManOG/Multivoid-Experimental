// ue_wrap/devices/kitchen_repair.h -- whether an applied ON also repairs a kitchen oven. Engine-free,
// so the standalone test compiles it on its own; appliance.cpp reads the inputs and runs the repair.

#pragma once

#include <cstdint>

namespace ue_wrap::appliance {

// ESlateVisibility::Collapsed: the oven's repair widget is authored Collapsed and its quit writes
// this value, so any other value means the widget is open.
inline constexpr uint8_t kKitchenWidgetCollapsed = 1;

enum class KitchenRepairStep : uint8_t { None, Repair, SkipWidgetOpen };

struct KitchenRepairInputs {
    bool on = false;                 // the value just applied
    bool ready = false;              // fixed, widget, fix() and the widget's Visibility resolved
    bool fixed = false;              // the oven's `fixed`
    bool widgetLive = false;         // the oven's `widget` is non-null and live
    uint8_t widgetVisibility = kKitchenWidgetCollapsed;  // read only when widgetLive
};

// The game switches an oven on only once it is fixed, so an applied ON on an oven this machine never
// repaired is repaired with the oven's own fix(). Not while its repair widget is open: fix() removes
// the widget, and only leaving the screen gives the player back their input: its quit, or another
// interface taking the input, a ragdoll's reset to no interface included. Every oven owns that
// widget from BeginPlay until it is fixed, so its presence says nothing; its Visibility does. A
// null widget cannot be open. An OFF, a fixed oven or an unresolved row: nothing to do.
inline KitchenRepairStep DecideKitchenRepair(const KitchenRepairInputs& in) {
    if (!in.on || !in.ready || in.fixed) return KitchenRepairStep::None;
    if (in.widgetLive && in.widgetVisibility != kKitchenWidgetCollapsed)
        return KitchenRepairStep::SkipWidgetOpen;
    return KitchenRepairStep::Repair;
}

}  // namespace ue_wrap::appliance
