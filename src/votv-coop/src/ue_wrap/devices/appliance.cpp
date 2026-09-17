// ue_wrap/devices/appliance.cpp -- see ue_wrap/devices/appliance.h. Per-class engine access for the
// six simple on/off appliances. Offsets/verbs resolved from the live classes via reflection
// (version-portable); the Alpha 0.9.0-n values are logged fallbacks.

#include "ue_wrap/devices/appliance.h"

#include "ue_wrap/core/call.h"
#include "ue_wrap/core/game_thread.h"
#include "ue_wrap/core/log.h"
#include "ue_wrap/core/reflection.h"
#include "ue_wrap/core/sdk_profile_names.h"
#include "ue_wrap/devices/kitchen_repair.h"
#include "ue_wrap/engine/engine.h"

#include <atomic>
#include <cstdint>
#include <cwchar>

namespace ue_wrap::appliance {
namespace {

namespace R = reflection;
namespace E = engine;
namespace GT = game_thread;
namespace P = profile;

// One descriptor per appliance class. A row that names `applyParam` applies through a named setter
// taking the bool as that parameter (serverBox's visual(active), which writes `active` and calls
// check()); the others direct-write the bool then call a no-arg refresh verb.
struct Desc {
    const wchar_t* className;
    const wchar_t* boolName;
    int32_t        boolOffFallback;  // used when reflection misses boolName; -1 = none, the row refuses
    const wchar_t* applyFn;
    const wchar_t* applyFn2;       // optional 2nd refresh verb (sink: upd() AFTER updIsOn()); nullptr if none
    // resolved lazily (game-thread serial -- no lock):
    void*   cls;
    int32_t boolOff;
    void*   fn;
    void*   fn2;
    // The named setter's bool parameter, spelled as that UFunction's own FProperty (nullptr for a
    // refresh-verb row). It and the tick-off members have defaults; a row that uses none omits them.
    const wchar_t* applyParam = nullptr;
    // True on the shower row, whose tick can raise this machine's sleep meter: the apply turns the
    // tick off after the verb, and OnTickOffBeginPlayPost turns it off again at BeginPlay.
    bool           tickOff = false;
    // True on the kitchen row: an applied ON on an oven this machine never fixed also runs the oven's
    // own fix() (ApplyKitchenRepair).
    bool           repairOnOn = false;
    // Resolved with the row: SetActorTickEnabled resolved and that BeginPlay observer registered. A
    // tickOff row without both is not synced: TryReadState and ApplyState refuse it outright.
    bool           tickOffReady = false;
    // Resolved with a repairOnOn row, all by name: the oven's `fixed` and `widget` members and its
    // parameterless fix(). Without all of them and the widget's Visibility, repairReady stays false
    // and the row applies exactly as a row without the repair step.
    int32_t        fixedOff = -1;
    int32_t        widgetOff = -1;
    void*          fixFn = nullptr;
    bool           repairReady = false;
};

// faucet_C's tap is `active`: actionOptionIndex with action 5 negates it and calls upd(), which
// switches the water particle, the water sound and the actor tick from it (or an attached hose's
// effect, which reads it too), and the save data carries it. Its other bool, `turnOn`, is
// assigned by lookAt on every call (whether the look trace hit the `faucet` box component) and
// chooses whether Toggle (action 5) or Take (action 7) is offered. The row has no numeric
// fallback: no offset of `active` has been measured, so a reflection miss leaves the faucet
// unsynced rather than writing a guessed byte.
//
// sink_C carries a second refresh verb. In the 0.9.0n bytecode updIsOn() sets the water particle
// and sound from isOn, and upd() re-clamps `clean` and writes it to the Cube mesh's `clean`
// material parameter. The tap flips only in actionOptionIndex, and only when its action is 5:
// isOn = !isOn, then updIsOn(); upd() is not on that path.
//
// prop_shower_C draws running_cold only in updWater(), which sets the water particle, the water
// sound and the actor tick from it; its upd() only re-applies `clean`. Its row sets tickOff.
Desc g_descs[] = {
    { L"faucet_C",         L"active",       -1,     L"upd",       nullptr, nullptr, -1, nullptr, nullptr },
    { L"sink_C",           L"isOn",         0x0278, L"updIsOn",   L"upd",  nullptr, -1, nullptr, nullptr },
    { L"prop_shower_C",    L"running_cold", 0x0298, L"updWater",  nullptr, nullptr, -1, nullptr, nullptr, nullptr, true },
    { L"kitchen_C",        L"Active",       0x02E1, L"upd",       nullptr, nullptr, -1, nullptr, nullptr, nullptr, false, true },
    { L"serverBox_C",      L"Active",       0x03D5, L"visual",    nullptr, nullptr, -1, nullptr, nullptr, L"active" },
    { L"wallunit_tapes_C", L"Active",       0x0290, L"upd",       nullptr, nullptr, -1, nullptr, nullptr },
};
constexpr int kNumDescs = sizeof(g_descs) / sizeof(g_descs[0]);

std::atomic<bool> g_keyResolved{false};
int32_t g_keyOff = -1;                 // Aactor_save_C::Key (Alpha 0.9.0-n: 0x0230)

void* g_bases[kNumDescs] = {};         // resolved class pointers, for the IsAppliance fast filter
int   g_nBases = 0;

// Find the descriptor whose class matches `obj` -- exact-class pointer compare first (the
// common case: an appliance instance IS its class), then a hierarchy walk for any subclass.
Desc* DescFor(void* obj) {
    if (!obj) return nullptr;
    void* cls = R::ClassOf(obj);
    if (!cls) return nullptr;
    for (auto& d : g_descs)
        if (d.cls && cls == d.cls) return &d;          // exact match (fast path)
    for (auto& d : g_descs) {
        if (!d.cls) continue;
        void* base[1] = { d.cls };
        if (R::IsDescendantOfAny(cls, base, 1)) return &d;  // subclass fallback
    }
    return nullptr;
}

// POST observer on a tickOff row's ReceiveBeginPlay. prop_shower_C's tick traces from the shower
// head and, on a hit of class mainPlayer_C (a remote player's puppet is one), adds to this
// machine's own sleep meter, so a machine applying another player's switch keeps that tick off.
// ApplyState turns it off after updWater(), but an apply can land before the shower begins play,
// and BeginPlay runs updWater() itself, switching the tick back on. running_cold is written in the
// pak only by the shower's own toggle, and no map, class default or save carries it, so at
// BeginPlay it is true only after an apply: the tick goes off again. False, this does nothing.
void OnTickOffBeginPlayPost(void* self, void* /*function*/, void* /*params*/) {
    Desc* d = DescFor(self);
    if (!d || !d->tickOff || d->boolOff < 0) return;
    if (*reinterpret_cast<const bool*>(reinterpret_cast<const char*>(self) + d->boolOff))
        E::SetActorTickEnabled(self, false);
}

int32_t g_widgetVisibilityOff = -1;    // Widget's Visibility byte, resolved with the kitchen row

// Visibility is a native member of the Widget class, so every widget holds it at the same offset.
// Its size is checked: a wider field reads as unresolved rather than as the wrong byte.
int32_t ResolveWidgetVisibilityOffset() {
    if (g_widgetVisibilityOff >= 0) return g_widgetVisibilityOff;
    void* widgetCls = R::FindClass(P::name::WidgetClass);
    if (!widgetCls) return -1;
    for (const R::StructFieldInfo& f : R::EnumerateStructFields(widgetCls)) {
        if (::_wcsicmp(f.name.c_str(), L"Visibility") == 0 && f.size == 1) {
            g_widgetVisibilityOff = f.offset;
            break;
        }
    }
    return g_widgetVisibilityOff;
}

// The kitchen row's step after its Active write and upd(). The decision is DecideKitchenRepair's.
// fix() sets fixed, runs upd() again, and removes and clears the repair widget: the verb the oven's
// own loadData runs for a saved fixed oven. It does not write Active, so the channel's next poll
// reads the applied value and sends nothing back.
void ApplyKitchenRepair(void* a, const Desc& d, bool on) {
    if (!on || !d.repairReady) return;
    const char* base = reinterpret_cast<const char*>(a);
    KitchenRepairInputs in;
    in.on = on;
    in.ready = true;
    in.fixed = *reinterpret_cast<const uint8_t*>(base + d.fixedOff) != 0;
    void* const widget = *reinterpret_cast<void* const*>(base + d.widgetOff);
    in.widgetLive = widget && R::IsLive(widget);
    if (in.widgetLive) {
        in.widgetVisibility = *reinterpret_cast<const uint8_t*>(
            reinterpret_cast<const char*>(widget) + g_widgetVisibilityOff);
    }
    switch (DecideKitchenRepair(in)) {
    case KitchenRepairStep::None:
        return;
    case KitchenRepairStep::SkipWidgetOpen:
        UE_LOGI("appliance: %ls repair skipped on apply key='%ls' -- its repair widget is open "
                "(Visibility=%u); the fix is left to the player using it",
                d.className, GetKeyString(a).c_str(), static_cast<unsigned>(in.widgetVisibility));
        return;
    case KitchenRepairStep::Repair: {
        ParamFrame f(d.fixFn);
        const bool ok = f.valid() && Call(a, f);
        const bool fixedNow = *reinterpret_cast<const uint8_t*>(base + d.fixedOff) != 0;
        UE_LOGI("appliance: %ls repaired on apply key='%ls' fix=%d fixed=%d widget=%s",
                d.className, GetKeyString(a).c_str(), ok ? 1 : 0, fixedNow ? 1 : 0,
                in.widgetLive ? "closed" : "none");
        return;
    }
    }
}

}  // namespace

bool EnsureResolved() {
    // The shared Key is declared on the Aactor_save_C base; resolve it against actor_save_C, the
    // declaring class (the same pattern the door follows).
    if (!g_keyResolved.load(std::memory_order_acquire)) {
        void* saveCls = R::FindClass(L"actor_save_C");
        if (!saveCls) return false;  // base not loaded yet
        int32_t k = R::FindPropertyOffset(saveCls, L"Key");
        if (k < 0) {
            UE_LOGW("appliance: reflected Key offset not found -- using fallback 0x0230");
            k = 0x0230;
        }
        g_keyOff = k;
        g_keyResolved.store(true, std::memory_order_release);
        UE_LOGI("appliance: Key@0x%04X (actor_save_C)", k);
    }
    // Lazily resolve each leaf class (best-effort -- cheap hash lookups, skipped once cached).
    bool newlyResolved = false;
    for (auto& d : g_descs) {
        if (d.cls) continue;
        void* cls = R::FindClass(d.className);
        if (!cls) continue;
        int32_t off = R::FindPropertyOffset(cls, d.boolName);
        if (off < 0) {
            if (d.boolOffFallback < 0) {
                UE_LOGW("appliance: %ls.%ls offset not found, no fallback -- row not synced",
                        d.className, d.boolName);
            } else {
                UE_LOGW("appliance: %ls.%ls offset not found -- fallback 0x%04X",
                        d.className, d.boolName, d.boolOffFallback);
            }
            off = d.boolOffFallback;
        }
        void* fn = R::FindFunction(cls, d.applyFn);
        if (!fn)
            UE_LOGW("appliance: %ls.%ls() apply verb not found -- field write only",
                    d.className, d.applyFn);
        void* fn2 = nullptr;
        if (d.applyFn2) {
            fn2 = R::FindFunction(cls, d.applyFn2);
            if (!fn2)
                UE_LOGW("appliance: %ls.%ls() 2nd refresh verb not found", d.className, d.applyFn2);
        }
        // A tickOff row records whether it is ready before `cls` is published; TryReadState and
        // ApplyState refuse it unless the tick call and the BeginPlay observer are both known.
        bool tickOffReady = false;
        if (d.tickOff) {
            const bool tickFn = E::SetActorTickEnabledResolved();
            void* beginPlay = R::FindFunction(cls, L"ReceiveBeginPlay");
            tickOffReady = tickFn && beginPlay &&
                           GT::RegisterPostObserver(beginPlay, &OnTickOffBeginPlayPost);
            if (tickOffReady) {
                UE_LOGI("appliance: %ls tick-off ready (ReceiveBeginPlay observer on %p)",
                        d.className, beginPlay);
            } else {
                UE_LOGW("appliance: %ls tick-off unavailable (SetActorTickEnabled=%d "
                        "ReceiveBeginPlay=%p, observer not registered) -- row not synced",
                        d.className, tickFn ? 1 : 0, beginPlay);
            }
        }
        d.tickOffReady = tickOffReady;
        // A repairOnOn row resolves its repair inputs before `cls` is published, like tickOff. A
        // miss is logged here, once, and leaves the row applying without the repair step.
        bool repairReady = false;
        if (d.repairOnOn) {
            const int32_t fixedOff = R::FindPropertyOffset(cls, L"fixed");
            const int32_t widgetOff = R::FindPropertyOffset(cls, L"widget");
            void* fixFn = R::FindFunction(cls, L"fix");
            const int fixParams = fixFn ? static_cast<int>(R::FunctionParams(fixFn).size()) : -1;
            const int32_t visOff = ResolveWidgetVisibilityOffset();
            repairReady = fixedOff >= 0 && widgetOff >= 0 && fixParams == 0 && visOff >= 0;
            if (repairReady) {
                d.fixedOff = fixedOff;
                d.widgetOff = widgetOff;
                d.fixFn = fixFn;
                UE_LOGI("appliance: %ls repair-on-apply ready (fixed@0x%04X widget@0x%04X fix=%p "
                        "Visibility@0x%04X)", d.className, fixedOff, widgetOff, fixFn, visOff);
            } else {
                UE_LOGW("appliance: %ls repair-on-apply unavailable (fixed=%d widget=%d fix=%p "
                        "params=%d Visibility=%d) -- an applied ON will not fix an unfixed oven",
                        d.className, fixedOff, widgetOff, fixFn, fixParams, visOff);
            }
        }
        d.repairReady = repairReady;
        d.cls = cls;
        d.boolOff = off;
        d.fn = fn;
        d.fn2 = fn2;
        newlyResolved = true;
        if (off >= 0) {
            UE_LOGI("appliance: resolved %ls bool@0x%04X fn=%p fn2=%p", d.className, off, fn, fn2);
        }
    }
    if (newlyResolved) {
        g_nBases = 0;
        for (auto& d : g_descs)
            if (d.cls) g_bases[g_nBases++] = d.cls;
    }
    return g_keyResolved.load(std::memory_order_acquire);
}

bool IsAppliance(void* obj) {
    if (!obj || g_nBases == 0) return false;
    void* cls = R::ClassOf(obj);
    if (!cls) return false;
    return R::IsDescendantOfAny(cls, g_bases, g_nBases);
}

std::wstring GetKeyString(void* a) {
    if (!a || g_keyOff < 0) return std::wstring();
    const R::FName& key = *reinterpret_cast<const R::FName*>(
        reinterpret_cast<const char*>(a) + g_keyOff);
    return R::ToString(key);
}

bool TryReadState(void* a, bool& on) {
    Desc* d = DescFor(a);
    if (!d || d->boolOff < 0 || (d->tickOff && !d->tickOffReady)) return false;
    on = *reinterpret_cast<const bool*>(reinterpret_cast<const char*>(a) + d->boolOff);
    return true;
}

bool ApplyState(void* a, bool on) {
    Desc* d = DescFor(a);
    if (!d) return false;
    if (d->applyParam) {
        // serverBox: visual(active) writes `active`, the field TryReadState polls, then calls
        // check() to re-derive the body material, server audio and glow. The apply must move that
        // field or the receiver's next poll sends the old value straight back; the box's setActive
        // switches only its two audio components and never writes `active`.
        if (!d->fn) {
            if (d->boolOff >= 0) *reinterpret_cast<bool*>(reinterpret_cast<char*>(a) + d->boolOff) = on;
            return false;
        }
        ParamFrame f(d->fn);
        f.Set<bool>(d->applyParam, on);
        // A failed named write leaves the frame not ready; refuse it here (Call() refuses it too).
        if (!f.ready()) return false;
        return Call(a, f);
    }
    // The rest: direct-write the bool, then call the row's no-arg refresh verb (upd, updIsOn or
    // updWater), the verb the class runs after its own toggle. The toggle itself lives in the BP; we
    // set the authoritative state and the refresh renders it -- the same rule the lights follow,
    // drive the visual through the verb rather than a bare field write. The channel only applies
    // when cur != want (idempotent guard), so a refresh verb with toggle semantics would also
    // converge. A tickOff row that is not ready is refused before the write: a bool written with
    // no way to turn the tick off would be switched on by a not-yet-begun shower's own BeginPlay.
    if (d->boolOff < 0 || (d->tickOff && !d->tickOffReady)) return false;
    *reinterpret_cast<bool*>(reinterpret_cast<char*>(a) + d->boolOff) = on;
    bool ok = true;
    if (d->fn) {
        ParamFrame f(d->fn);
        if (f.valid()) ok = Call(a, f);
    }
    // The verb set the actor tick from the bool; a tickOff row turns it off (OnTickOffBeginPlayPost).
    if (d->tickOff) ok = E::SetActorTickEnabled(a, false) && ok;
    // sink: upd() after updIsOn(). upd()'s only state is `clean`, which this channel does not
    // carry: it re-clamps the local value and re-applies it to the Cube material, so it is not
    // part of the tap. nullptr/no-op for the single-verb appliances.
    if (d->fn2) {
        ParamFrame f2(d->fn2);
        if (f2.valid()) Call(a, f2);
    }
    // kitchen: an ON on an oven this machine never fixed also runs its fix(), unless its repair
    // widget is open. The step's result is its own log line; `ok` stays the apply's.
    if (d->repairOnOn) ApplyKitchenRepair(a, *d, on);
    return ok;
}

}  // namespace ue_wrap::appliance
