# Multivoid current replication model

Audit pin: `VOTV-MP/Multivoid@f690c017e8e55f2996d0e9121956aae66486742e`.

## Implemented shape

**CLAIM `[SOURCE_PROVEN]`.** Multivoid is a host-centred, maximum-four-peer star. `Session` owns GameNetworkingSockets connections and a network thread; slot 0 is host, slots 1–3 are clients, and client traffic is host-routed. Evidence: `Multivoid@f690c017/src/votv-coop/include/coop/net/session.h:1-15,45-50`.

**FALSIFIER.** A source path at the pin establishes a larger registry or direct client-to-client application connection.

**PROBE.** Trace session ownership, connection creation, roles and slot constants.

**GAP.** This is source structure, not a packet-capture observation.

**CLAIM `[SOURCE_PROVEN]`.** Four replication-responsibility classes are present at distinct source sites: the wire enum, one session lane-selection switch, one feature-dispatch entry, and the central install/join/cleanup/tick fan-out declaration. Evidence: `Multivoid@f690c017/src/votv-coop/include/coop/net/protocol.h:1155-1270`, `src/votv-coop/src/coop/net/session_lanes.h:45-55`, `src/votv-coop/src/coop/dispatch/event_dispatch_state.cpp:37`, `src/votv-coop/include/coop/session/subsystems.h:1-13`.

**FALSIFIER.** Any cited range does not contain its named class, or all four observations are the same declaration site.

**PROBE.** Read the four pinned ranges and classify only the declaration or switch visible in each range.

**GAP.** These ranges do not establish payload-validation, refan, engine-wrapper or feature-state-machine sites, do not enumerate a complete registration universe, and cannot support a negative claim about whether one declaration owns all ten axes. Generic helpers may exist for some families.

## Independent contract axes

Any declarative layer must keep these declarations independent:

| Axis | Current concern the declaration must preserve |
|---|---|
| IDENTITY | Logical key, EID and live actor binding must not collapse into one value. |
| PRESENCE | Native, mirrored, adopted, absent and retired expressions differ. |
| AUTHORITY | Host routing, origin validation and feature-specific authorship are separate decisions. |
| DURABLE_STATE | Native save bytes and per-player data are not session event streams. |
| EVENTS | One-shot intents/results are not durable fields. |
| TRANSITIONS | Multi-expression state changes need ordered postconditions. |
| MATERIALIZATION | Actor creation/adoption/destruction occurs on the game/world side, not the network thread. |
| JOIN_SEED | Native save load and ordered per-slot replay form a staged bootstrap. |
| RESYNC | Current convergence is feature-specific and must stay explicit. |
| VALIDATION | Size/version/sender/range/liveness/ownership checks remain per contract. |

**CLAIM `[SOURCE_PROVEN]`.** Current identity has at least four non-substitutable domains: stable authored/save key, session EID, transient UE actor/index, and peer identity/GUID. Per-player inventory/save ownership is a fifth ownership domain, not an object ID. Evidence: `Multivoid@f690c017/src/votv-coop/include/coop/element/element.h:86-117`, `src/votv-coop/include/coop/element/registry.h:53-141`, `src/votv-coop/include/coop/net/peer_identity.h:1-54`, `src/votv-coop/include/coop/props/prop_synth_key.h:1-27`, `src/votv-coop/src/coop/items/player_inventory_sync.cpp:64-87,314-334`.

**FALSIFIER.** The implementation safely uses one canonical identity for all these roles without a separate ownership relation.

**PROBE.** Trace fields, reverse maps, owner/range checks, save keys and player identity.

**GAP.** Runtime uniqueness and every cooked key were not exercised.

## Native-save constraint

**CLAIM `[SOURCE_PROVEN]`.** A client receives and validates host save bytes, writes an ephemeral slot, invokes VotV's native loader, and only then signals world-ready; the host responds to that signal by starting ordered replay. Save-loaded actors are bound/adopted rather than all being created from a server-authored element tree. Evidence: `Multivoid@f690c017/src/votv-coop/src/coop/save/save_transfer.cpp:281-371`, `src/votv-coop/src/harness/session_runtime.cpp:359-373`, `src/votv-coop/src/ue_wrap/engine/engine_save.cpp:401-413`, `src/votv-coop/src/coop/session/net_pump.cpp:563-609`, `src/votv-coop/src/coop/dispatch/event_feed.cpp:221-239`, `src/votv-coop/src/coop/session/subsystems.cpp:254-350`, `src/votv-coop/src/coop/props/save_identity_bind.cpp:94-175`.

**FALSIFIER.** The pinned client world is completely materialized from a server-created replicated tree without native save load/reconciliation.

**PROBE.** Trace capture, streaming, assembly, native load, ready edge and save-actor binding.

**GAP.** The game serializer/loader internals are not readable; not every family necessarily takes the same path.

**CLAIM `[INFERRED]`.** A suitable contract layer is therefore hybrid bind/adopt/materialize. A universal reflection serializer is unsuitable because it would erase explicit identity, ownership, event/state, transition and materialization decisions.

**FALSIFIER.** A concrete alternative preserves every named distinction and the native-save path without independent contracts.

**PROBE.** Apply the ten-axis model to session, save, element, lifecycle and inventory paths.

**GAP.** This constrains architecture; it does not select a schema syntax or code generator.

## Evidence for an internal contract seam

**CLAIM `[SOURCE_PROVEN]`.** The LightGroup feature history added protocol/wrapper/adapter/probe sites in commit `96d142505819b47d65b220e98e293e5ef74829ab` before follow-up `7fed1c1a552b022bc81b24729f8beeba0073d36a` added the missing receiver dispatch, demonstrating a real multi-site wiring omission in the current composition model. Current-code evidence: `Multivoid@f690c017/src/votv-coop/src/coop/dispatch/event_dispatch_state.cpp:46`.

**FALSIFIER.** The first change already contained a functioning receiver or the follow-up addressed an unrelated behavior.

**PROBE.** Compare the two pinned diffs and trace the message from producer to receiver.

**GAP.** One history example does not establish backlog prevalence or prove a proposed contract implementation.

**CLAIM `[INFERRED]`.** This independently evidenced receiver omission, together with the four observed responsibility classes at distinct source sites, supports testing whether a contract registry can make required wiring reviewable while retaining the transport. It does not establish a complete add-path census or support claiming that such a registry fixes every reported multiplayer symptom.

**FALSIFIER.** A prototype adds equal or greater wiring burden, cannot express a current family, or fails to detect an intentionally omitted applicable site.

**PROBE.** Shadow-declare one bounded family, generate/validate registration metadata, and inject a missing-site negative test.

**GAP.** No prototype exists in this evidence set.
