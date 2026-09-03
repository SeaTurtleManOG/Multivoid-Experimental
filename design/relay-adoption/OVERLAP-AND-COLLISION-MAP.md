# Relay–Multivoid overlap and collision map

## Ownership map

| Concern | Multivoid now | Relay public/static surface | Collision if both active | Required boundary |
|---|---|---|---|---|
| Hook/bootstrap | Own native loader/hook integration | Native DLL plus Blueprint/pak adapter surface | Double observation or mutation | Exactly one hook owner per verb/path |
| Transport/session | GNS host-centred session, lanes and refan | Documented session/transport framework | Two connections, routing or admission models | One transport owner per pilot traffic class |
| Peer identity | Durable key/GUID plus session slot | Documented NetKey/player identity | Ambiguous peer mapping | Total, fail-closed mapping; no guessed fallback |
| Object identity | Stable keys, EIDs and actor binding | Documented stable network identity | Two namespaces or duplicate registration | One enroller and one mapping owner |
| Presence/materialization | Native adoption plus feature mirrors | Documented presence modes/spawn/item replacement | Duplicate actors or stale expressions | One materializer for each actor/state/transition set |
| Authority | Host routing plus feature gates | Documented authority and leases | Split brain or conflicting writes | Single writer/arbiter at every instant |
| Fields/events/transitions | Protocol and feature state machines | Documented declarative rules | Duplicate/out-of-order effects | Disjoint message/state domains or one bridge |
| Join/resync | Native save then staged replay | Documented join/resync/world fences | Conflicting seed or premature apply | Conjoined readiness; one seed authority |
| Inventory/save | Native save and per-player ownership | Static inventory/save/item assets | Duplication, loss or ownership crossover | Non-destructive fixture; ownership stays explicit |
| Diagnostics/config | Existing logs/config/version gates | Separate rules/config/diagnostics | Ambiguous operator state | Pinned versions, distinct keys/ports, correlation IDs |

**CLAIM `[INFERRED]`.** Both projects cover transport, identity, presence, authority, world readiness, materialization and inventory-adjacent concerns. Co-loading is therefore not evidence of safe composition; active overlap needs a non-dual-authority manifest.

**FALSIFIER.** A complete integration inventory shows the Relay side is inert or disjoint for every active Multivoid concern.

**PROBE.** Map Multivoid source owners against Relay's documented/static surface and enumerate each place both could enroll, authorize, materialize or replay the same state.

**GAP.** Relay internals were not executed, so actual collision occurrence is `UNKNOWN`.

## Non-dual-authority invariant

For a pilot namespace `P`, all of the following must be true before start:

1. `Actors(P)`, `State(P)` and `Transitions(P)` are enumerated.
2. Exactly one system enrolls each logical object.
3. Exactly one system authorizes each mutation.
4. Exactly one system materializes each actor/expression.
5. The other system is disabled or provably disjoint for those sets.
6. Peer and object mappings fail closed on absence, ambiguity or mismatch.
7. Readiness is the conjunction of native world validity and pilot readiness.
8. Join seed, live changes and resync all name the same authority.
9. Teardown retires pilot objects without touching native-save or per-player ownership state.

**CLAIM `[INFERRED]`.** The current evidence does not show this invariant implemented for any Relay-backed Multivoid subsystem.

**FALSIFIER.** A reviewed manifest, controls and trace demonstrate the nine conditions on a bounded namespace.

**PROBE.** Compare the integration boundary and non-dual invariant in this document against the package and pilot designs.

**GAP.** This is absence of qualifying evidence, not proof that integration is impossible.

## Evidence findings that must not be averaged

**CLAIM `[INFERRED]`.** The overlap analysis concludes that no Relay-backed subsystem is presently certified non-dual. The pilot design defines prospective gates that could certify a disjoint new-subsystem canary and, later, a native-bound fixture. These are different temporal conclusions: “not certified now” and “testable later”; both are retained.

**FALSIFIER.** A passed runtime certificate already exists, or the pilot design lacks a non-dual gate.

**PROBE.** Compare `OVERLAP-AND-COLLISION-MAP.md:26-46` with `PILOT-DESIGN.md:17-45`.

**GAP.** No gate was run in this workstream.

**CLAIM `[INFERRED]`.** The migration analysis retains chat as a bounded semantic/UI pilot surface, while the pilot design prefers a disjoint new canary before a native-bound fixture. These scopes answer different questions and are not interchangeable. This synthesis selects a disjoint synthetic canary for framework qualification; chat may be a later integration candidate only after publication-resolvable wire-test evidence is supplied.

**FALSIFIER.** Publication-resolvable evidence demonstrates that chat is already disjoint, fully tested and a stronger first framework probe, or that a synthetic canary cannot exercise the required controls.

**PROBE.** Compare `MIGRATION-SURFACE.md:43-60` with `PILOT-DESIGN.md:5-58`.

**GAP.** The selected sequence is a design decision, not an observed performance result.
