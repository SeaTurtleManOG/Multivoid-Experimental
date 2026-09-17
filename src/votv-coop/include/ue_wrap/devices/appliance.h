// ue_wrap/devices/appliance.h -- standalone engine access for VOTV's simple on/off APPLIANCES
// (faucet / sink / shower / kitchen-oven / serverBox / wallunit-tapes). Principle-7
// engine-wrapper layer (no network/coop state). coop::interactable_sync drives the sync
// through here via ONE Adapter -- this wrapper is the per-class dispatch.
//
// All six are Aactor_save_C descendants carrying a single bool on/off toggle and a no-arg refresh
// verb (upd/updIsOn/updWater), except serverBox which exposes a named setter, visual(active).
// None has a sensor / autoclose -> none auto-reverts -> they sync SYMMETRICALLY (like
// lights/garage), driven by the generic Channel's state poll (the activation verbs are
// BP-internal and bypass our ProcessEvent detour, so we never observe the switch -- we poll
// the resulting bool, exactly as doors/lights/garage do). Identity = the inherited
// Aactor_save_C::Key (save-persistent, cross-peer stable). The per-class bool name, its
// fallback offset and the apply verb live in ONE table, appliance.cpp's g_descs: sink runs
// updIsOn() (water particle and sound) then upd() (the `clean` material parameter), the
// shower runs updWater() and keeps its actor tick off, and serverBox goes through visual(active).

#pragma once

#include <string>

namespace ue_wrap::appliance {

// Resolve the shared Aactor_save_C::Key offset + each leaf class's UClass / bool offset /
// refresh verb, and for the shower the ReceiveBeginPlay observer that keeps its tick off. Lazy +
// best-effort: returns true once the Key offset is known (the family can operate); individual
// classes resolve as they stream in -- a save lacking one class just never indexes it.
// Idempotent. Game thread.
bool EnsureResolved();

// True iff `obj`'s class is (a descendant of) any of the six appliance classes. Cheap
// (pointer compares + one hierarchy walk over the resolved set); false until resolved.
bool IsAppliance(void* obj);

// The appliance's Aactor_save_C::Key as a wide string ("" on failure, L"None" if unkeyed).
std::wstring GetKeyString(void* a);

// Read the appliance's per-class on/off bool into `on`. False if the read could not be made
// (null / class not in the set / not resolved / a shower whose tick-off is not ready); leaves
// `on` untouched on failure.
bool TryReadState(void* a, bool& on);

// Drive the appliance to `on`: serverBox via visual(active); the rest by direct-writing the
// bool then calling the row's no-arg refresh verb (upd/updIsOn/updWater), the verb the class
// runs after its own toggle, and on the shower turning the actor tick off after it. On the
// kitchen oven an ON also runs the oven's own fix() when this machine never fixed it, unless its
// repair widget is open (ue_wrap/devices/kitchen_repair.h). MUST run on the game thread. False on
// null / unresolved; a shower whose tick-off is not ready (the tick call or its BeginPlay
// observer unresolved) is refused before the bool is written.
bool ApplyState(void* a, bool on);

}  // namespace ue_wrap::appliance
