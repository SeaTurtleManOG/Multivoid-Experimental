# Bug applicability matrix

## Fixed universe

**CLAIM `[INFERRED]`.** This matrix covers each of the 54 identities in the publication-contained ledger at SHA-256 `A2A1671C3A39F9C61F16F29725300C758E1AD096776F2F3254743C5A1E05A32F` exactly once. It deliberately excludes the six capture envelopes received on 2026-09-04; an addendum is required before this can be called complete for the current intake. Evidence: `BUG-LEDGER.md:3-33,37-92`.

**FALSIFIER.** A digest mismatch, duplicate/missing/extra identity, or inclusion of an excluded intake identity.

**PROBE.** Compare this matrix's identity multiset with the identity multiset in `BUG-LEDGER.md`; validate 54 unique rows and the cell enums.

**GAP.** Identity coverage does not validate causal classification. The excluded intake is unclassified.

## Cell key

- `U1` `[UNKNOWN]`: no evaluated class-specific fix.
- `U2` `[UNKNOWN]`: the row does not establish a named contract failure.
- `U3` `[UNKNOWN]`: no row-specific, implementation-qualified Relay pilot evidence.
- `U4` `[UNKNOWN]`: no row-specific, implementation-qualified broad-migration evidence.
- `N` `[INFERRED]`: administrative provenance envelope, not a runtime outcome.
- `H-M` `[INFERRED]`: Option 2 can declare and validate the verified host-to-client content-view materialization obligation.
- `H-T` `[INFERRED]`: Option 2 can declare the verified presentation transition and forbid simultaneous old/new expressions.

**CLAIM `[INFERRED]`.** Three identities support a named contract-gap classification: `MVF-0001` as MATERIALIZATION and `PENDING-BUG-025` plus `MVF-0002` as TRANSITIONS. The two transition identities refer to the same observed lid-divergence finding, so they are two ledger rows but not two independent incidents. Of the remaining 51 identity rows, 45 runtime-outcome identities are causally `UNKNOWN` and six administrative provenance-envelope identities are `NEUTRAL`; no row is proven a class-specific native defect.

**FALSIFIER.** Controlled causal evidence assigns a different contract/class, or establishes that the two lid rows are independent outcomes.

**PROBE.** Apply the ten-axis taxonomy only to verified bounded outcomes in `BUG-LEDGER.md`; do not promote reports by symptom wording.

**GAP.** Even the three contract labels are inferred counterfactuals, not source-proven root causes.

## Matrix

| Identity | Class / contract | OPTION_1 | OPTION_2 | OPTION_3 | OPTION_4 |
|---|---|---|---|---|---|
| BUG-001 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-002 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-003 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-004 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-005 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-006 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-007 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-008 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-009 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-010 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-011 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-012 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-013 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-014 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-015 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-016 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-017 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-018 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-019 | UNKNOWN | U1 | U2 | U3 | U4 |
| BUG-020 | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-BUG-021 | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-BUG-022 | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-BUG-023 | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-BUG-024 | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-BUG-025 | CONTRACT_GAP / TRANSITIONS | U1 | H-T | U3 | U4 |
| WORLD-RELOAD-STACK-OVERFLOW-OBSERVATION | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-001 | provenance envelope | N | N | N | N |
| PENDING-20260902-001a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-001b | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-002 | provenance envelope | N | N | N | N |
| PENDING-20260902-002a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-002b | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-002c | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-002d | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-003 | provenance envelope | N | N | N | N |
| PENDING-20260902-003a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260902-004 | provenance envelope | N | N | N | N |
| PENDING-20260902-004a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-001 | provenance envelope | N | N | N | N |
| PENDING-20260903-001a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-001b | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-001c | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002 | provenance envelope | N | N | N | N |
| PENDING-20260903-002a | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002b | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002c | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002d | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002e | UNKNOWN | U1 | U2 | U3 | U4 |
| PENDING-20260903-002f | UNKNOWN | U1 | U2 | U3 | U4 |
| MVF-0001 | CONTRACT_GAP / MATERIALIZATION | U1 | H-M | U3 | U4 |
| MVF-0002 | CONTRACT_GAP / TRANSITIONS | U1 | H-T | U3 | U4 |
| MVF-0003 | UNKNOWN; status REPORTED | U1 | U2 | U3 | U4 |
| MVF-0004 | UNKNOWN; exact reproduction rejected/not reproduced | U1 | U2 | U3 | U4 |
| PENDING-20260903-003a | UNKNOWN | U1 | U2 | U3 | U4 |

## Leverage result

| Option | HELP | HURT | NEUTRAL | UNKNOWN |
|---|---:|---:|---:|---:|
| OPTION_1 | 0 | 0 | 6 | 48 |
| OPTION_2 | 3 inferred identity rows | 0 | 6 | 45 |
| OPTION_3 | 0 | 0 | 6 | 48 |
| OPTION_4 | 0 | 0 | 6 | 48 |

**CLAIM `[INFERRED]`.** The validated 216 cells aggregate to 3 `HELP`, 24 `NEUTRAL`, and 189 `UNKNOWN`. Relay-backed options receive no `HELP` because their relevant mechanisms are only `README_REPORTED` or statically named and no row-specific Relay run exists.

**FALSIFIER.** Re-parsing all four verdict/strength cells on all 54 rows in the Markdown matrix table above yields different totals, or row-specific implementation/runtime evidence satisfies the promotion rule.

**PROBE.** Group all four cells on all 54 source rows by verdict and strength.

**GAP.** `UNKNOWN` does not mean an option cannot help. These counts are not scores and do not measure incident independence or decision value.

## Corrected claim-strength boundary

The prior rejected matrix omitted 11 frozen identities and over-credited eight cells. This matrix closes those findings by using all 54 frozen identities and retaining the corrected `UNKNOWN` verdicts for `BUG-001`, `BUG-009`, `BUG-010`, `BUG-013`, `PENDING-20260902-001a`, `PENDING-20260902-001b`, `PENDING-20260902-002a`, and `PENDING-20260902-002b` where applicable. It also preserves `MVF-0003` as reported and `MVF-0004` as unreproduced at the rejected exact sequence.
