# Relay adoption decision

`RELAY_DECISION=ADD_CONTRACT_LAYER_KEEP_CURRENT_TRANSPORT`

**Decision.** `[INFERRED]` `[D-001]` Add an in-tree declarative contract layer over Multivoid's current transport, introduced in shadow mode and cut over one family at a time. Keep Relay as a reference and a separately gated future experiment; do not add it to the player package under this decision.

**Scope.** `[INFERRED]` `[D-002]` This is a maintainability and control-surface choice, not a claim that the layer fixes the backlog. The frozen evidence credits Option 2 with only three inferred `HELP` identity rows representing two independent verified outcomes; 45 outcome rows remain causally unresolved and six provenance envelopes are neutral. The 2026-09-04 intake remains outside the frozen universe.

## Why this option

`[SOURCE_PROVEN]` `[D-024]` Multivoid clients write a validated host save to an ephemeral slot, invoke the native save boot path, then signal world-ready; save-created actors are bound/adopted, and the implementation keeps stable logical keys, transient actor expressions, network/EID identity, peer identity and per-player inventory/save ownership distinct. Evidence: `Multivoid@f690c017/src/votv-coop/src/coop/save/save_transfer.cpp:281-371`, `src/votv-coop/src/harness/session_runtime.cpp:359-373`, `src/votv-coop/src/ue_wrap/engine/engine_save.cpp:401-413`, `src/votv-coop/src/coop/session/net_pump.cpp:563-609`, `src/votv-coop/src/coop/props/save_identity_bind.cpp:94-175`, `src/votv-coop/include/coop/element/element.h:86-117`, `src/votv-coop/include/coop/element/registry.h:53-141`, `src/votv-coop/include/coop/net/peer_identity.h:1-54`, `src/votv-coop/include/coop/props/prop_synth_key.h:1-27`, `src/votv-coop/src/coop/items/player_inventory_sync.cpp:64-87,314-334`.

`[INFERRED]` `[D-029]` A viable model must therefore preserve hybrid bind/adopt/materialize behavior and keep those identity and ownership domains independent.

`[INFERRED]` `[D-001]` The wire enum, one lane-selection switch, one feature-dispatch entry and the central lifecycle fan-out occupy distinct source sites, and the LightGroup history independently demonstrates one omitted receiver later supplied by a follow-up. A bounded prototype can test whether an internal declaration makes applicable obligations reviewable without replacing proven transport/save ownership or adding an opaque runtime.

The layer must keep independent declarations for IDENTITY, PRESENCE, AUTHORITY, DURABLE_STATE, EVENTS, TRANSITIONS, MATERIALIZATION, JOIN_SEED, RESYNC and VALIDATION. It must not be a universal reflection serializer. `[INFERRED]` `[D-029]`

`[INFERRED]` `[D-025]` MTA remains a bounded conceptual reference for identity indirection and a server-driven initial data stream, but its server-created root/resource tree is not transplanted into VotV's native-save boot. Evidence: `MTA@cc3b35de/Server/mods/deathmatch/logic/CElement.cpp:36-44`, `Server/mods/deathmatch/logic/CElementIDs.cpp:23-55`, `Server/mods/deathmatch/logic/CMapManager.cpp:51-64`, `Server/mods/deathmatch/logic/CResource.cpp:900-925`, `Server/mods/deathmatch/logic/CGame.cpp:1396-1433,1514-1518`; `Multivoid@f690c017/src/votv-coop/src/coop/save/save_transfer.cpp:281-371`, `src/votv-coop/src/harness/session_runtime.cpp:359-373`.

## Broad-migration decision rule

| Mandatory broad-migration condition | Result |
|---|---|
| Adequate redistribution/dependency licence | **FAIL — `UNKNOWN`.** `[UNKNOWN]` `[D-003]` |
| Inspectable or independently black-box-qualified critical behavior | **FAIL — `UNKNOWN`.** `[UNKNOWN]` `[D-004]` |
| Reproducible package/API/wire contract | **FAIL — `UNKNOWN`.** `[UNKNOWN]` `[D-005]` |
| Implemented non-dual-authority migration path | **FAIL.** `[INFERRED]` `[D-006]` |
| Objective pilot acceptance and rollback gates | **DESIGN EXISTS; NOT RUN.** `[INFERRED]` `[D-007]` |
| More duplicated risk removed than introduced | **FAIL — `UNKNOWN`.** `[UNKNOWN]` `[D-008]` |
| Viable upstream contribution strategy | **FAIL — `UNKNOWN`.** `[UNKNOWN]` `[D-009]` |
| No critical dependency/control unknown | **FAIL.** `[UNKNOWN]` `[D-010]` |

`[INFERRED]` `[D-003]` `[D-004]` `[D-005]` `[D-006]` `[D-008]` `[D-009]` `[D-010]` Because seven mandatory conditions fail or remain unknown, `BROAD_RELAY_MIGRATION_CANDIDATE` is forbidden at this evidence state.

`CRITICAL_RELAY_UNKNOWNS=CU-001_PROJECT_LICENCE;CU-002_CORRESPONDING_SOURCE_AND_PACKAGE_COMPLETENESS;CU-003_REPRODUCIBLE_NATIVE_BUILD;CU-004_SUPPORTED_PLAYER_DISTRIBUTION_AND_AUTHENTICITY;CU-005_API_NET_SCHEMA_STABILITY_AND_COMPATIBILITY;CU-006_MULTI_MOD_DEPENDENCY_COEXISTENCE;CU-007_LAWFUL_MAINTAINED_CONTINUITY;CU-008_NATIVE_CPP_TO_BLUEPRINT_RULE_INTEGRATION_AND_GAMEPLAY_PAK_NEED;CU-009_UPSTREAM_ACCEPTANCE` `[UNKNOWN]` `[D-010]`

## OPTION_1 — CONTINUE_CLASS_BY_CLASS

- `CURRENT_CODE_RETAINED=` All current transport, protocol, session, dispatch, identity, save, lifecycle and feature architecture. `[INFERRED]` `[D-013]`
- `CURRENT_CODE_REPLACED=` Only exact feature code named by a causal diagnosis; the total is unknown because no patch set is supplied. `[INFERRED]` `[D-013]`
- `NEW_DEPENDENCIES=` None by option definition; Relay remains reference material only. `[INFERRED]` `[D-013]`
- `BUG_CLASSES_HELPED=` No demonstrated matrix cell; a concrete tested class patch may help its exact row. `[UNKNOWN]` `[D-014]`
- `BUG_CLASSES_NOT_HELPED=` Six provenance envelopes are neutral and 48 outcome rows have no demonstrated help under this option; this does not prove they are unhelpable. `[UNKNOWN]` `[D-002]` `[D-014]`
- `MIGRATION_SEQUENCE=` Diagnose one outcome, patch the smallest owner, run exact regression and compatibility tests, then ship or revert independently. `[INFERRED]` `[D-013]`
- `TEST_REQUIREMENTS=` Paired-role reproduction, positive/negative control, exact regression, and wire/save/cohort tests whenever those surfaces change. `[INFERRED]` `[D-013]`
- `PLAYER_PACKAGE_IMPACT=` Ordinary Multivoid package update only; no Relay DLL, pak or rule profile. `[INFERRED]` `[D-013]`
- `UPSTREAMABILITY=` Acceptance remains unknown; the contribution shape stays within current in-tree ownership. `[UNKNOWN]` `[D-009]`
- `BACKOUT=` Revert per patch before data/wire cutover; version/data compatibility is required after such a cutover. `[INFERRED]` `[D-013]`
- `KNOWN_RISKS=` Repeated multi-site wiring and accumulating class-specific architecture burden. `[UNKNOWN]` `[D-014]`
- `UNKNOWN_CRITICAL_FACTS=` Which outcome rows are isolated native defects and the aggregate patch cost. `[UNKNOWN]` `[D-014]`
- `FALSIFYING_EVIDENCE=` Predominantly repeated contract diagnoses, or class fixes whose review/test burden repeatedly exceeds a bounded contract declaration, would make this the wrong programme direction. `[UNKNOWN]` `[D-014]`

## OPTION_2 — ADD_DECLARATIVE_CONTRACT_LAYER_OVER_CURRENT_TRANSPORT

- `CURRENT_CODE_RETAINED=` Existing transport, native-save bootstrap, actor bind/adopt/materialize ownership, feature behavior and test corpus. `[INFERRED]` `[D-015]`
- `CURRENT_CODE_REPLACED=` Per-family manual contract wiring/validation only after shadow parity; replaced paths are then removed. `[INFERRED]` `[D-015]`
- `NEW_DEPENDENCIES=` No external runtime dependency; author the abstraction in-tree and copy no Relay/libvotv implementation. `[INFERRED]` `[D-015]`
- `BUG_CLASSES_HELPED=` MATERIALIZATION for `MVF-0001` and TRANSITIONS for `PENDING-BUG-025`/`MVF-0002`, all only at inferred strength and representing two independent observed outcomes. `[INFERRED]` `[D-016]`
- `BUG_CLASSES_NOT_HELPED=` Forty-five causally unresolved outcome rows have no credited contract help; six envelope rows are neutral. `[INFERRED]` `[D-002]`
- `MIGRATION_SEQUENCE=` Define the ten-axis schema; shadow one bounded family; verify parity and omission detection; make one declared path authoritative; delete the replaced path; repeat. `[INFERRED]` `[D-015]`
- `TEST_REQUIREMENTS=` Schema validation, intentional omission negatives, current-wire parity, native-save bind/adopt, identity/ownership, join/rejoin/resync, teardown and per-family regression. `[INFERRED]` `[D-015]`
- `PLAYER_PACKAGE_IMPACT=` Multivoid-only update; no Relay artifacts or cross-peer Relay rules. `[INFERRED]` `[D-015]`
- `UPSTREAMABILITY=` Acceptance remains unknown; in-tree ownership preserves current build reproducibility, but the cross-cutting abstraction needs a concrete design review. `[UNKNOWN]` `[D-009]` `[D-017]`
- `BACKOUT=` Disable the family declaration and restore the old authoritative path while its wire/data format remains compatible; require a converter/backup before any durable-format cutover. `[INFERRED]` `[D-015]`
- `KNOWN_RISKS=` Abstraction mismatch, incomplete axis coverage, temporary dual paths, review breadth and failure to remove migration baggage. `[INFERRED]` `[D-017]`
- `UNKNOWN_CRITICAL_FACTS=` Schema/generator shape, implementation size, per-family cost, performance, omission-detection efficacy and upstream acceptance. `[UNKNOWN]` `[D-017]`
- `FALSIFYING_EVIDENCE=` A bounded prototype that cannot express a current family, misses injected omissions, regresses parity/backout, adds greater wiring burden, or finds no repeated contract burden would make this decision wrong. `[INFERRED]` `[D-001]` `[D-017]`

## OPTION_3 — SELECTIVE_RELAY_PILOT_OR_NEW_SUBSYSTEMS

- `CURRENT_CODE_RETAINED=` All Multivoid architecture outside one explicitly disjoint pilot namespace. `[INFERRED]` `[D-018]`
- `CURRENT_CODE_REPLACED=` Nothing initially; after qualification, exactly one target's owner may be replaced, never co-owned. `[INFERRED]` `[D-018]` `[D-020]`
- `NEW_DEPENDENCIES=` Exact Relay player DLL/pak/libsodium package, rule profile, and an unresolved native-to-Blueprint/rule bridge; services only if fallback transport is selected. `[INFERRED]` `[D-018]`
- `BUG_CLASSES_HELPED=` None demonstrated by the 54-row matrix. `[UNKNOWN]` `[D-019]`
- `BUG_CLASSES_NOT_HELPED=` Six envelope rows are neutral and all 48 outcome rows lack demonstrated Relay-pilot help; this is not proof of no possible help. `[UNKNOWN]` `[D-002]` `[D-019]`
- `MIGRATION_SEQUENCE=` Close legal/package/version/bridge/upstream preflight; baseline; mismatch negatives; disjoint canary; authority/state/join/resync/performance tests; Relay-free rollback; independent review. `[INFERRED]` `[D-020]`
- `TEST_REQUIREMENTS=` Every G01–G17 gate in `PILOT-DESIGN.md`, with exact versions, roles, positive/negative controls and rollback. `[INFERRED]` `[D-020]`
- `PLAYER_PACKAGE_IMPACT=` Mandatory aligned Relay artifacts/rules for every participating peer during the pilot, followed by a verified clean uninstall path. `[INFERRED]` `[D-018]` `[D-020]`
- `UPSTREAMABILITY=` Unknown; the dependency/package boundary and complete-payload reproducibility require explicit upstream review. `[UNKNOWN]` `[D-009]`
- `BACKOUT=` Kill the canary, remove its bridge/rules and restore a Relay-free player package; the pilot may not migrate native saves or per-player ownership. `[INFERRED]` `[D-020]`
- `KNOWN_RISKS=` Opaque native behavior, licence uncertainty, version mismatch, dual-stack operations, ambiguous ownership and dependency conflicts. `[UNKNOWN]` `[D-019]`
- `UNKNOWN_CRITICAL_FACTS=` Every CU-001 through CU-009 item, plus exact runtime safety/performance results. `[UNKNOWN]` `[D-010]` `[D-019]`
- `FALSIFYING_EVIDENCE=` Failure of any required preflight/non-dual/behavior/backout gate makes this the wrong current choice; conversely, all gates passing with greater measured benefit than the in-tree prototype would falsify the decision not to select it now. `[INFERRED]` `[D-020]`

## OPTION_4 — BROAD_RELAY_MIGRATION

- `CURRENT_CODE_RETAINED=` Gameplay UI and native-save consent/naming/loading/travel integration, per-player ownership, game-specific hooks/materialization and any capability Relay does not supply. `[INFERRED]` `[D-021]`
- `CURRENT_CODE_REPLACED=` Broad transport/session/replication, routing, identity, authority, state/event/join/resync ownership where a complete non-dual mapping exists. `[INFERRED]` `[D-021]`
- `NEW_DEPENDENCIES=` Mandatory opaque Relay player DLL/pak/libsodium, aligned rules and versions, native bridge, possible gameplay assets, plus optional services if fallback is required. `[INFERRED]` `[D-021]`
- `BUG_CLASSES_HELPED=` None demonstrated. `[UNKNOWN]` `[D-022]`
- `BUG_CLASSES_NOT_HELPED=` No frozen outcome row has demonstrated broad-migration help; six provenance envelopes remain neutral. `[UNKNOWN]` `[D-002]` `[D-022]`
- `MIGRATION_SEQUENCE=` First close all eight decision-rule conditions; then pilot representative families, transfer ownership one namespace at a time, validate converters/backout, and only then expand. `[INFERRED]` `[D-023]`
- `TEST_REQUIREMENTS=` Full current regression plus package/API/wire mismatch, identity/authority, native-save, inventory ownership, join/rejoin/resync, travel/teardown, load/performance, security and forward/reverse data conversion. `[INFERRED]` `[D-023]`
- `PLAYER_PACKAGE_IMPACT=` Every peer becomes locked to the compatible Relay DLL/pak/rule cohort in addition to retained gameplay integration. `[INFERRED]` `[D-021]`
- `UPSTREAMABILITY=` Unknown; upstream would need to accept a mandatory external binary/pak dependency and a revised own-substrate boundary. `[UNKNOWN]` `[D-009]`
- `BACKOUT=` Simple only before wire/authority/persisted-state cutover; afterward it needs backups, reverse converters and compatible player cohorts that do not yet exist. `[INFERRED]` `[D-023]`
- `KNOWN_RISKS=` Broad ownership transition, opaque critical dependency, split brain, package mismatch, non-reproducibility, save/inventory damage and difficult recovery. `[UNKNOWN]` `[D-021]` `[D-022]`
- `UNKNOWN_CRITICAL_FACTS=` All CU-001 through CU-009 items, net risk reduction, concrete migration size and representative results. `[UNKNOWN]` `[D-008]` `[D-010]`
- `FALSIFYING_EVIDENCE=` Any one unresolved mandatory condition already makes this the wrong current choice; only closure of every condition plus successful representative forward/back migration could reverse that result. `[INFERRED]` `[D-023]`

## What proceeds and what waits

**DO NOT WAIT:** `[INFERRED]` `[D-011]` A small, isolated, confirmed fix may proceed when a diff-scoped review proves it does not conflict with the ten-axis contract direction and its exact regression passes. This decision approves no specific patch.

**WAIT:** `[INFERRED]` `[D-012]` Broad repetitive implementation across the bug backlog should wait for causal classification and the bounded contract-layer prototype. Unknown symptoms are not a mandate for bulk class wiring.

## Independent evidence differences retained

`[INFERRED]` `[D-027]` The overlap analysis finds no currently certified non-dual Relay subsystem; the pilot design supplies a prospective way to certify one. “Not certified now” and “could be qualified later” are both retained.

`[INFERRED]` `[D-026]` The migration analysis retains chat as a bounded candidate integration surface, while the pilot design starts with a disjoint canary and later native-bound qualification. The scopes are not averaged: the canary is first; chat remains later and needs a publication-resolvable wire-test citation.

## Prior rejection closure

The publication-contained correction record restates and accounts for all fourteen determinations and four quantified finding groups, including the independent-review boundary for claim-strength classes, in `PRIOR-REVIEW-CLOSURE.md`.

`[INFERRED]` `[D-028]` The prior result remains rejected and is used only as a correction map.

| Binding finding | Disposition |
|---|---|
| Eleven missing applicability identities | **CLOSED:** all 54 frozen identities appear once; the separate six-envelope 2026-09-04 delta is explicitly open. `[INFERRED]` `[D-028]` |
| Eight over-credited cells (`BUG-001`, `BUG-009`, `BUG-010`, `BUG-013`, `PENDING-20260902-001a`, `...001b`, `...002a`, `...002b`) | **CLOSED:** applicable cells are `UNKNOWN`; no Relay-backed `HELP` is credited. `[INFERRED]` `[D-028]` |
| Invented/non-concrete Drive Box declaration | **CLOSED:** no invented record/class/slot model is used; only verified `MVF-0001` and `MVF-0002` outcomes constrain future tests. `[INFERRED]` `[D-028]` |
| Defective mapping-write citation | **CLOSED:** the clause is not reused; current identity and bind/adopt claims use the corrected pinned-source ranges. `[INFERRED]` `[D-028]` |
| Unsupported per-peer known-set claim | **CLOSED:** the clause is not reused. `[INFERRED]` `[D-028]` |
| Defective rebind citation | **CLOSED:** the clause is not reused; the corrected bind/adopt evidence is used. `[INFERRED]` `[D-028]` |
| Relay manifest failed to enumerate packaged artifacts | **CLOSED:** the exact 75-path inventory and three-component player package are used. `[INFERRED]` `[D-028]` |
| MTA startup-allocation citation did not support its clause | **CLOSED:** the clause is not reused; MTA is retained only at the bounded concept boundary. `[INFERRED]` `[D-025]` `[D-028]` |
| Option document's packaged-file manifest was incomplete | **CLOSED:** package effects are sourced from the exact Relay inventory. `[INFERRED]` `[D-028]` |
| Nine over-strength/coverage claims | **CLOSED:** the eight cell claims are downgraded, Relay behavior is not promoted above documentation/static evidence, and current-ledger completeness remains explicitly open pending the six-envelope addendum. `[INFERRED]` `[D-002]` `[D-028]` |
