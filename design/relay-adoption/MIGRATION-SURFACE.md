# Migration surface and reversibility

**CLAIM `[UNKNOWN]`.** The counts below were supplied as review universes, not predicted changed lines, but the exact path inventory was not published in this artifact set. Treat the counts as unverified; file groups overlap and must not be added into a total.

**FALSIFIER.** A publication-resolvable path inventory is supplied and re-parsing it yields the stated counts; a different result disproves the retained values.

**PROBE.** Re-enumerate exact file paths and line counts at the Multivoid pin, group them by the declared option units, and retain overlap flags.

**GAP.** Exact membership is unavailable here, and the census would not measure future diff size, engineering time or schedule even if reproduced.

## OPTION_1 — continue class by class

**CLAIM `[INFERRED]`.** Each fix retains the current transport, protocol, dispatch, save, identity and lifecycle architecture and changes only the diagnosed feature plus applicable tests. Aggregate cost is `UNKNOWN` because no fixed set of diagnosed patches exists.

**FALSIFIER.** A supposedly isolated fix requires an architecture-wide wire, identity, save or ownership change.

**PROBE.** Require a diff-scoped diagnosis for each proposed fix and trace its applicable add sequence.

**GAP.** The ledger does not supply 48 causal diagnoses; backlog-wide cost cannot be inferred from symptom count.

Backout is ordinary commit reversion before a persisted/wire change ships; after a protocol or save change it also requires cohort and data compatibility handling.

## OPTION_2 — contract layer over current transport

| Migration unit | Measured review universe | Intended treatment | Reversibility gate |
|---|---:|---|---|
| Contract schema/registry and validators | New code; size unknown | ADD | Feature flag and shadow-only mode |
| Protocol/session/dispatch/config review | 75 files / 25,544 lines | RETAIN, annotate, then selectively generate/validate | Existing manual path remains authoritative until parity |
| Element/lifecycle/save/inventory/world review | 235 files / 56,743 lines | RETAIN; migrate one family at a time | Per-family fallback; no state-format cutover in first wave |
| Tests/harness review | 90 files / 21,673 lines | EXTEND with schema, omission and parity cases | Old test corpus remains required |
| Total non-additive option review scope | 310 files / 73,726 lines | Review universe, not edit estimate | Gate each family independently |

**CLAIM `[INFERRED]`.** This option can be introduced in-tree without a new runtime dependency: first declare a family in shadow mode, compare declared and current routing/validation, make generated validation authoritative for that family, remove the replaced manual path, then repeat. This respects the upstream rule against leaving migration baggage.

**FALSIFIER.** A concrete prototype requires Relay/libvotv runtime material, cannot shadow a current family, or leaves two authoritative paths after cutover.

**PROBE.** Use the migration units MU-007..MU-012 and test an intentionally omitted producer/receiver/replay/cleanup registration.

**GAP.** No schema, generator or diff exists; new-code size, review time and per-family conversion cost are `UNKNOWN`.

Backout is per-family while the old wire/state format remains readable. Any durable-format or authority cutover needs explicit converter/backup and a last-known-good package before it is reversible.

## OPTION_3 — selective Relay pilot/new subsystem

| Migration unit | Measured review universe | Treatment | Reversibility gate |
|---|---:|---|---|
| Candidate chat semantic/UI surface | 12 files / 2,113 lines | Possible later pilot integration | Pilot kill switch restores current chat |
| Routing/dispatch surface touching chat | 4 files / 7,066 lines | Bridge or isolate | Relay and Multivoid must not both author messages |
| Dependency/package integration surface | 6 files / 2,343 lines | ADD exact Relay dependency and checks | Remove dependency/package/rules as one unit |
| New synthetic canary | New code/assets; size unknown | Preferred first qualification target | Delete canary namespace without persisted state |

**CLAIM `[INFERRED]`.** A disjoint synthetic canary is the safer first framework qualification because it can exercise identity, authority, field/event, mismatch, teardown and rollback without taking ownership of native-save actors. Chat is a bounded later integration candidate, but explicit chat wire coverage is `UNKNOWN` because the exact search record was not published in this artifact set.

**FALSIFIER.** The canary cannot exercise critical integration controls, or publication-resolvable evidence establishes complete isolated chat wire tests and a smaller non-dual first target.

**PROBE.** Recompute the chat surface at the Multivoid pin, run a positive-controlled wire-test search, compare the result with the pilot gates, and require positive and negative controls for the chosen target.

**GAP.** The exact migration-unit membership and chat wire-test search record are unavailable here; Relay's bridge, gameplay-pak need, package resolver and actual runtime behavior also remain `UNKNOWN`.

Backout removes the canary/rule profile/bridge and restores a package without the Relay dependency. The pilot may not alter native saves or per-player inventory ownership.

## OPTION_4 — broad Relay migration

| Surface | Measured review universe | Likely disposition | Irreversible edge |
|---|---:|---|---|
| Net/session/config | 75 files / 25,544 lines | Major replacement/bridge | Wire and admission cohort cutover |
| Element/dispatch | 35 files / 7,584 lines | Replace or adapt identities/routing | Namespace and authority cutover |
| World/gameplay replication | 218 files / 53,046 lines | Per-family migration | Persisted/materialized state change |
| Player/save/inventory | 75 files / 17,506 lines | Retain gameplay ownership or bridge | Save/per-player ownership conversion |
| Tests/harness/package | Broad cross-cutting impact | Rewrite and extend | Player package becomes Relay-coupled |

**CLAIM `[INFERRED]`.** Broad migration is not a transport swap alone. Relay documents that gameplay UI, save consent/naming/loading/travel and aligned gameplay rules remain consumer responsibilities; Multivoid's native-save bind/adopt behavior therefore survives as owned integration work.

**FALSIFIER.** A complete design demonstrates a smaller, reproducible replacement that removes those responsibilities without loss.

**PROBE.** Map Relay's documented ownership exclusions to Multivoid's source-owned session, save, element, player and world units.

**GAP.** No buildable migration diff exists, so changed lines, schedule and net risk are `UNKNOWN`.

Backout is straightforward only before wire/authority/persisted-state cutover. After MU-021/MU-022-style state conversion, recovery needs backups, a reverse converter, compatible player packages and a tested cohort rollback; those do not exist.

## Comparative cost conclusion

**CLAIM `[INFERRED]`.** Option 2 has the smallest evidence-supported path to a cross-cutting improvement while keeping build/package ownership and allowing per-family backout. Option 1 is smaller per confirmed fix but offers no measured backlog-wide leverage. Options 3 and 4 add unresolved dependency/package and dual-stack costs; Option 4 also crosses broad non-reversible state boundaries.

**FALSIFIER.** A concrete diff and test packet shows a different option removes more measured duplication/risk at lower migration and rollback cost.

**PROBE.** Compare the four bounded migration units, bug leverage, dependency gates and backout plans.

**GAP.** No option has an implementation estimate or completed prototype; this is ordinal, not a schedule claim.
