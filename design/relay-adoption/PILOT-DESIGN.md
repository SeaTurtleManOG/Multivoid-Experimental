# Selective Relay pilot design

This is a prospective gate design, not authorization to run or ship Relay. The architecture decision does not select Option 3; these gates define what would have to change before reconsideration.

## Target and containment

**CLAIM `[INFERRED]`.** The first framework qualification target should be a disposable, disjoint synthetic canary with no native-save or per-player inventory state. It should exercise stable canary identity, one host authority, one durable scalar, one event, one transition, late join seed, targeted resync, validation, mismatch rejection, teardown and rollback.

**FALSIFIER.** The canary cannot exercise the required framework/control surface, or a smaller existing subsystem is proven equally disjoint and better tested.

**PROBE.** Review the namespace manifest against every contract axis and non-dual condition before packaging.

**GAP.** No canary, bridge or gameplay asset exists; integration size is `UNKNOWN`.

The canary manifest must independently declare IDENTITY, PRESENCE, AUTHORITY, DURABLE_STATE, EVENTS, TRANSITIONS, MATERIALIZATION, JOIN_SEED, RESYNC and VALIDATION. It must keep stable logical identity, transient actor/expression identity, Relay/Multivoid network identity and per-player ownership distinct. It must not serialize arbitrary reflected properties.

## Gates

| Gate | Unknown/control answered | Pass condition | Stop/rollback condition |
|---|---|---|---|
| G01 Legal-use boundary | CU-001 licence | Counsel-approved use for the exact pilot; no redistribution outside that grant | No grant: do not package, run or share |
| G02 Artifact identity | CU-002 completeness/authenticity | Every required DLL/pak/rule/bridge hash and origin recorded; expected files exact | Missing/extra/hash drift: stop |
| G03 Reproducibility boundary | CU-003 native build | Either corresponding reproducible source/build is supplied, or the pilot is explicitly black-box-only and cannot graduate to broad migration | Any claim exceeds its qualified evidence |
| G04 Player package | CU-004 supported distribution | Clean install/upgrade/uninstall on all peers with post-install hash verification and no native save change | Load failure, residue or unverifiable package: restore baseline |
| G05 Version contract | CU-005 API/net/schema stability | Exact good cohort joins; one-field mismatches for package/API/net/rules fail closed with named diagnostics | Mixed cohort enters gameplay or fails ambiguously |
| G06 Dependency coexistence | CU-006 resolver behavior | Two-mod test with compatible and conflicting requirements has deterministic resolution | Silent version selection or partial load |
| G07 Continuity | CU-007 maintenance/fork | Named maintained source/package continuity and incident owner exist before any production proposal | No lawful continuity route: pilot cannot graduate |
| G08 Native bridge | CU-008 C++→Blueprint/rules | Supported bridge and gameplay-asset/package needs are documented, bounded and reproducible | Hidden second pak/asset owner or unsupported call path |
| G09 Upstream path | CU-009 acceptance | Maintainers review a concrete design/diff before merge claims | No acceptance evidence: upstreamability stays UNKNOWN |
| G10 Non-dual namespace | authority/collision | Exactly one enroller, writer, seed source and materializer for every canary object/state/transition | Any dual or unmapped owner: kill canary |
| G11 Identity/authentication | behavioral qualification | Displayed identity cannot substitute for possession; mapping failures reject | Spoof, collision or fallback identity admitted |
| G12 Authority/lease/rejoin | behavioral qualification | One writer at all times; disconnect/expiry/rejoin converge without split brain | Concurrent accepted writers or orphaned authority |
| G13 State/event/transition | behavioral qualification | Scalar converges; event count and transition postcondition match under reorder/loss/retry tests | Duplicate/lost durable effect or simultaneous forbidden expressions |
| G14 Join/resync/world fence | behavioral qualification | Late join and targeted resync converge only after both native-world and pilot-ready gates | Premature apply, stale seed, or global destructive reset |
| G15 Resource/performance | operational bound | Repeated connect/travel/disconnect cycles remain within predeclared latency, memory, bandwidth and error budgets | Threshold breach, leak trend or unexplained crash |
| G16 Backout | reversibility | One switch restores a Relay-free package; no save/per-player migration; baseline tests pass | Backout requires data repair or leaves runtime residue |
| G17 Independent review | evidence promotion | Exact versions, role logs, controls, raw result hashes and reviewer verdict are complete | Missing role/control/artifact: result is inconclusive |

## Run sequence

1. Pass G01–G09 as preflight; failures are stop conditions, not runtime negatives.
2. Capture a Relay-free Multivoid baseline and prove the observation harness with a positive control.
3. Install exact artifacts and prove G05 mismatch rejection before a success-path run.
4. Run the disjoint canary through G10–G14 with at least host plus one joining client, disconnect/rejoin and travel/teardown cycles.
5. Run G15 budgets and G16 rollback; rerun baseline after removal.
6. Obtain G17 review. Promote only the exact passed behaviors at the exact versions.

## Native-bound follow-up boundary

**CLAIM `[RUNTIME_OBSERVED]`.** The frozen evidence verifies only two relevant Drive Box outcomes: host-inserted contents absent from a connected-client view (`MVF-0001`) and coexistence of the stale closed-box lid with a detached-lid expression (`MVF-0002`/`PENDING-BUG-025`). It does not verify MVF-0004's rejected exact sequence, and MVF-0003 remains reported. Evidence: `BUG-LEDGER.md:62-63,88-91`.

**FALSIFIER.** A later independently reviewed evidence generator promotes or contradicts those bounded statuses.

**PROBE.** Use the publication-contained 54-row ledger and provenance at the digest pinned in `PINNED-SOURCES.json`.

**GAP.** Root causes, interaction behavior, persistence and a Relay remedy are not established.

A native-bound follow-up may be designed only after the synthetic canary passes. It must first discover and validate exact native class/property/key facts, use throwaway save copies, retain Multivoid's native-save bind/adopt path, and assign exactly one materializer. Failure to reproduce a Drive Box outcome says nothing about Relay; it invalidates that fixture.

## Acceptance and graduation

Passing this pilot would justify only `RUN_SELECTIVE_RELAY_PILOT` evidence for the canary or exact later target. It would not justify broad migration. Broad consideration still requires legal adequacy, inspectable or independently qualified critical behavior, reproducible package/API/wire contracts, a complete non-dual migration path, favourable risk removal, viable upstream strategy, and no critical dependency/control unknown.
