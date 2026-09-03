# Published bug ledger and provenance

## Publication boundary

**CLAIM `[MEASURED]`.** This publication-contained ledger has exactly 54 identity rows: the 53-row architecture input plus `PENDING-20260903-003a`, which was accepted before the architecture universe was frozen. Every identity, short description and status matches the frozen architecture input byte-for-byte. The table below is the complete evidence input used by the applicability matrix and pilot design; no external ledger or provenance file is required to enumerate it. Cross-references are deliberately normalized rather than byte-preserved: identity tokens use Markdown code formatting, the four private finding-file locators are replaced by publication identities, and the column is augmented with parent/child, corroboration and bug/finding identity links, including reciprocal navigation where a relationship is represented on both published rows.

**FALSIFIER.** The table has other than 54 unique identities, an identity used by the matrix is absent, an identity/description/status cell differs from the frozen architecture input, a normalized cross-reference publishes a private locator, or a cross-reference difference falls outside the stated formatting, locator-replacement and relationship-augmentation rule.

**PROBE.** Parse every table body row; compare identity, description and status cells exactly with the frozen architecture input; compare the identity multiset with `BUG-APPLICABILITY-MATRIX.md`; then compare cross-reference cells separately and inspect every difference against the stated formatting, locator-replacement and relationship-augmentation rule.

**GAP.** Publication of the ledger establishes its fixed universe and retained identity, description and status cells. Cross-reference normalization supports navigation but does not independently verify a relationship beyond the frozen evidence fields, verify reported behavior, establish root cause, or classify the excluded intake.

`AUDITED` means the identity is accounted for; it does not mean runtime evidence verifies the report. `REPORTED` means accepted intake contains the report but no controlled measurement verifies it. `PARTIALLY_VERIFIED` and `VERIFIED` retain only the bounded evidence status stated in the row; neither establishes an architecture-level cause or remedy.

## Known and deliberate scope limit

**CLAIM `[MEASURED]`.** The six capture envelopes received on 2026-09-04, and any identities arising from them, are outside this fixed 54-row universe. An addendum is required before the matrix can be called complete for the current intake.

**FALSIFIER.** Any excluded identity appears in the table, or the matrix is described as complete for the 2026-09-04 intake without an addendum.

**PROBE.** Treat the 54 identities below as the complete fixed universe and compare them exactly with the matrix identity column.

**GAP.** The excluded intake remains unclassified.

## Counting rule

**CLAIM `[INFERRED]`.** Count one row per distinct identity. Envelope identities (`PENDING-YYYYMMDD-NNN`) count separately from child identities (`...NNNa`, `...NNNb`) because an envelope is a provenance record with its own disposition. MVF rows count because they are published findings to which an architecture option may or may not apply.

**FALSIFIER.** Applying this rule to the table yields other than 54 identities or duplicates an identity.

**PROBE.** Parse and group the identity column using exact, case-sensitive values.

**GAP.** An identity count is not an incident count; linked rows can describe the same observed outcome.

## Identity rows

| Identity | Short description | Status | Evidence boundary | Cross-references |
|---|---|---|---|---|
| BUG-001 | Garage door state differs between peers. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-002 | All-terrain vehicle steering, lights, and horn are not represented on the client. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-003 | Drone-bag weight and items are not synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | `PENDING-20260903-002d` |
| BUG-004 | Processing-unit playback fields are only partly synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-005 | A processor-slot drive can become invisible. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-006 | Shower state and cold-water tiredness effects differ by role. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-007 | Paper text is not synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-008 | The base elevator is not synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-009 | A client flashlight can flicker in the host view. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-010 | The client player model can show broken textures after a chair action. | AUDITED | Later intake adds a ragdoll trigger report; no mechanism is established. | `PENDING-20260903-001c` |
| BUG-011 | Hook state and hooked-item state are not synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-012 | A client cannot open satellite doors. | AUDITED | Later intake contains corroboration; no status promotion is made here. | `PENDING-20260903-003a` |
| BUG-013 | Trash bags can remain unsettled and produce repeated interaction sounds. | AUDITED | Later intake adds a sweeping-physics report; no mechanism is established. | `PENDING-20260903-002c` |
| BUG-014 | Base doors can fail to close automatically. | AUDITED | Reviewed intake identity; no status promotion is made here. | `PENDING-20260903-003a` |
| BUG-015 | Clients can receive phantom trash balls. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-016 | A client-broken supply box can fail to produce host-side items. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-017 | A client inventory handoff can leave a Drive Box or Tape Box empty for the host. | PARTIALLY_VERIFIED | A shorter direct-inventory route was measured; the exact reported sequence was rejected by binding review. | `MVF-0004`; `MVF-0001` is a related content-gap finding. |
| BUG-018 | Floppy discs can disappear or duplicate on networked computers. | AUDITED | Later intake adds an ejection-trigger report; no mechanism is established. | `PENDING-20260903-001b` |
| BUG-019 | Server-room terminal commands are not synchronized. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| BUG-020 | A drone bag can duplicate when removed. | AUDITED | Reviewed intake identity; no status promotion is made here. | — |
| PENDING-BUG-021 | Sleeping-bag deployment is not represented for another player. | REPORTED | Corroborated report; no controlled runtime measurement exists. | `PENDING-20260902-003a` |
| PENDING-BUG-022 | A client-spawned or client-placed barrel was absent for the host. | REPORTED | The verb and runtime class are unresolved; it is not merged with `BUG-017`. | `BUG-017` |
| PENDING-BUG-023 | Repeated drone-bag duplication preceded a reported cross-player inventory loss. | REPORTED | One high-risk report exists; cross-player loss is not measured. | `BUG-003`; `BUG-020` |
| PENDING-BUG-024 | Food progress diverges and stale client placement can restore the older host display. | REPORTED | The display sequence is repeated in the accepted evidence, but no cited artifact records a food-progress value; exact field and persistence are unknown. | `MVF-0003` |
| PENDING-BUG-025 | A detached Drive Box lid can coexist with the stale closed-box mesh lid on a client. | VERIFIED | Paired views and role records confirm the visible result; client interaction and persistence are untested. | `MVF-0002` |
| WORLD-RELOAD-STACK-OVERFLOW-OBSERVATION | The official release host terminated with a stack overflow during a world-change and re-seed episode. | PARTIALLY_VERIFIED | One event is measured; the triggering function and repeatability are unknown. | — |
| PENDING-20260902-001 | Provenance envelope for the first multi-report capture. | AUDITED | Parent envelope retained separately from its split child identities. | `PENDING-20260902-001a`; `PENDING-20260902-001b` |
| PENDING-20260902-001a | Host rules reportedly do not become effective for a client. | REPORTED | Two reports can describe one candidate defect; they are not merged and no controlled measurement exists. | `PENDING-20260902-004a` |
| PENDING-20260902-001b | A key card reportedly works only after the host uses it first. | REPORTED | The ordering condition is reported; the captured message tail is incomplete. | — |
| PENDING-20260902-002 | Provenance envelope for the low-specificity multi-report capture. | AUDITED | Parent envelope retained separately from its split child identities. | `PENDING-20260902-002a`; `PENDING-20260902-002b`; `PENDING-20260902-002c`; `PENDING-20260902-002d` |
| PENDING-20260902-002a | Networked performance and frame-rate degradation. | REPORTED | The initial report contains no build, hardware, interval, or repeatable measurement. | — |
| PENDING-20260902-002b | A remote player model disappears outside the current view. | REPORTED | Camera conditions and role details remain unspecified. | — |
| PENDING-20260902-002c | Remote-player collision weight can fling props. | REPORTED | Actor roles and prop class remain unspecified. | — |
| PENDING-20260902-002d | Joined-player reach or hitboxes are incorrect. | REPORTED | The report does not separate reach from hitbox behavior. | — |
| PENDING-20260902-003 | Provenance envelope for sleeping-bag corroboration and the mattress datum. | AUDITED | Parent envelope retained separately from its child identity. | `PENDING-20260902-003a`; `PENDING-BUG-021` |
| PENDING-20260902-003a | A removable mattress reportedly has the same remote-expression problem as sleeping bags. | REPORTED | Corroborating variant, not a separate promotion. | `PENDING-BUG-021` |
| PENDING-20260902-004 | Provenance envelope for the second world-rules report. | AUDITED | Parent envelope retained separately; its child is reconciled with `PENDING-20260902-001a` without merging. | `PENDING-20260902-004a`; `PENDING-20260902-001a` |
| PENDING-20260902-004a | Host-authored world settings reportedly do not become effective for joining clients. | REPORTED | Corroborates `PENDING-20260902-001a`; no controlled effective-rule comparison exists. | `PENDING-20260902-001a` |
| PENDING-20260903-001 | Provenance envelope for the current multi-report capture. | AUDITED | Parent envelope retained separately from corroborating and new child candidates. | `PENDING-20260903-001a`; `PENDING-20260903-001b`; `PENDING-20260903-001c` |
| PENDING-20260903-001a | Entering a starter suitcase reportedly destroys its inventory. | REPORTED | Trigger and container differ from `BUG-017`; no permanent loss is established. | `BUG-017` |
| PENDING-20260903-001b | Daily tapes and satellite floppies reportedly disappear when a player is ejected. | REPORTED | Cross-referenced, not merged. | `BUG-018` |
| PENDING-20260903-001c | Repeated ragdolling reportedly corrupts player-model textures. | REPORTED | New trigger report, not a separate mechanism claim. | `BUG-010` |
| PENDING-20260903-002 | Provenance envelope for the current multi-report issue capture. | AUDITED | Parent envelope retained separately from existing and new child candidates. | `PENDING-20260903-002a`; `PENDING-20260903-002b`; `PENDING-20260903-002c`; `PENDING-20260903-002d`; `PENDING-20260903-002e`; `PENDING-20260903-002f` |
| PENDING-20260903-002a | A player reportedly spawned at the gate on one rejoin. | REPORTED | One occurrence is reported; no repeat exists. | — |
| PENDING-20260903-002b | Picking up an airborne item reportedly makes it disappear. | REPORTED | Kept distinct from `BUG-017`. | `BUG-017` |
| PENDING-20260903-002c | Sweeping reportedly does not update trash-pile physics. | REPORTED | New trigger report, cross-referenced and not merged. | `BUG-013` |
| PENDING-20260903-002d | The drone user interface reportedly fails until reload. | REPORTED | Related to `BUG-003`, but the user-interface symptom remains separate. | `BUG-003` |
| PENDING-20260903-002e | A client inventory reportedly becomes empty after rejoin. | REPORTED | High-risk report; permanent data loss is unknown. | — |
| PENDING-20260903-002f | Upgrade modules reportedly duplicate. | REPORTED | New object-class report in a known duplication family; not merged. | — |
| MVF-0001 | Host-inserted Drive Box contents are absent from the connected-client view. | VERIFIED | Verification is limited to the host-to-client content view. | `BUG-017` is a related inventory-handoff identity. |
| MVF-0002 | Drive Box lid state diverges and produces two visible client lids. | VERIFIED | Verification is limited to the paired visible state and loose-lid actor. | `PENDING-BUG-025` |
| MVF-0003 | Food progress remains stale and a stale client drop restores the older host display. | REPORTED | The display sequence remains reported because no cited artifact records a food-progress value. | `PENDING-BUG-024` |
| MVF-0004 | BUG-017 client inventory placement can produce an empty host-side Drive Box. | PARTIALLY_VERIFIED | The rejected exact-reproduction status and its promotion condition are preserved. | `BUG-017`; `MVF-0001` is a related content-gap finding. |
| PENDING-20260903-003a | Base and satellite doors reportedly do not close on the client side. | REPORTED | Accepted as a separate identity and cross-referenced rather than merged. | `BUG-012`; `BUG-014` |
