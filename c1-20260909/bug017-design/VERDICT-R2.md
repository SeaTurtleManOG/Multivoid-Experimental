# BUG-017 contract design, REVISION 2 — reconciled binding verdict

Subject: `C:\AgenticStaging\VOTV\Coordination\C1\LANES\bug017-design\BUG017-CONTRACT-DESIGN-R2.md`
Prior verdict on draft 1: `VERDICT.md` (this directory), RB-1 … RB-7.
Inputs reconciled: two independent reviews of revision 2 — lens `r2-source-correctness`
(`DESIGN_CLEAN`) and lens `r2-data-loss-regression` (`DESIGN_NOT_CLEAN`). Both appended verbatim
as JSON in the appendix.

All file:line citations below are on `3af5ddae` (new upstream main) unless stated otherwise. b150
(`ba6d8c39`) is code-identical in the mechanism files (CONTEXT.md §0.6 basis), so every source fact
here applies to the private b150 line too. Every disputed fact was re-opened by the reconciler in
the repository; the reconciler's own reads are marked `RECONCILER-VERIFIED`.

## DECISION

**`DESIGN_NOT_CLEAN`. Implementation is NOT authorised. Revision 3 is required.**

Six of the seven prior blocking findings are closed on the merits, in the source and not merely in
prose. **RB-5 is not closed**: revision 2's restated acceptance arm A2 reproduces, in a different
section, the exact failure shape RB-5 exists to prevent. Three further blocking findings are
carried into revision 3. **All four are closable in the document.** No mechanism is rejected: the
host-local custody park, the three-phase PARKED→ARMED→APPLIED machine, the causal (clock-free)
consume predicate, the `SlotIo` injection and the whole seam set survive review unchanged. The
reviewed direction is honoured — no transport rewrite, no Relay, no universal serializer, no new
kind/op/format, `kProtocolVersion` untouched on the b150 line.

The persistence result stays bounded everywhere in revision 2 and in this verdict:
`ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE; UNKNOWN_BEYOND_SCOPE`. Nothing here restates it as
unrestricted permanent data loss.

## Where the two lenses disagreed, and how the reconciler decided

The two reviews agree on every *fact*. They differ on the *severity* of four of them. Each was
decided by re-opening the cited lines.

| # | Disagreement | Decision | Basis (reconciler's own read) |
|---|---|---|---|
| **D-A** | Nested containers' preserved `ints[0][0]` re-installed with no ownership proof. Source-correctness files it as an optional sharpening of §11; data-loss makes it blocking (`BF-R2-1`). | **BLOCKING** — carried as `BF-R3-1`. | `RECONCILER-VERIFIED` `container_contents_sync.cpp:246-262`: Boundary 2's own shipped comment states the failure mode in the engine's terms — *"a nested container's ints[0][0] is its own GObjStack index, a slot number in the sender's array"*, and a wrong one means *"the container reuses GObjStack[0], a slot owned by someone else."* §2.10 and §6 invariant 6 forbid writing a slot not proved exclusively owned; the design proves the OUTER slot and installs the NESTED slot addresses unproven. `STRENGTH=SOURCE_PROVEN` for the mechanism, `UNKNOWN` for whether inner slots are reused (Q-4), which is precisely why fail-closed applies. Source-correctness's mitigation ("the native host-authored `loadData` path re-installs the same index") is not equivalent: on the native path every index comes from one consistent save image; here one stale index is re-installed into a world that has kept allocating for up to `kCustodyParkCeilingSec` = 900 s. Revision 2 itself writes *"A reviewer may reasonably make this an eighth blocker."* |
| **D-B** | A2's pass condition, "zero `[CUSTODY]` lines of any kind". Source-correctness files it as a scoring-hygiene improvement; data-loss makes it blocking (`BF-R2-2`). | **BLOCKING** — carried as `BF-R3-2`; **RB-5 is NOT closed.** | `RECONCILER-VERIFIED` `container_contents_sync.cpp:630-639`: the lane-alive line is emitted as *"The first statement, ahead of every filter"*, precisely so its absence proves the lane dead. §2.11's "first entry" row adopts that convention verbatim and extends it — *"and the same for the Tick"*. `RECONCILER-VERIFIED` `subsystems.cpp:515/:547/:548`: the per-tick block is unconditional (each module self-gates internally), so §5.2's custody `Tick` registration between `DrainPendingSpawns` (`:547`) and `prop_drop_intent::Tick` (`:548`) runs on the host in every session of a FIXED build. A correct implementation therefore emits at least one `[CUSTODY]` line during A2, and A2 as written fails it. This is RB-5's own definition — *"A2 asserts an observable a correct implementation cannot produce"* — reintroduced by a different section. Both lenses independently derived the same second trigger (an unrelated client-authored keyed destroy produces a legitimate `park REFUSED … reason=not-container` line, which §3 already declares expected). |
| **D-C** | §2.5 row 10's enumerator. Source-correctness calls the missing self-exclusion "minor" inside a closed RB-3; data-loss makes the enumeration contract blocking (`BF-R2-3`). | **BLOCKING** — carried as `BF-R3-3` (both halves). | `RECONCILER-VERIFIED` `container_contents_sync.cpp:784-801` and `container_contents_sync.h:52-63`: `SnapshotWorldContainers` is declared under *"dev-instrument seams"*, fills *"up to `want`"*, breaks at `if (n >= want) break;`, returns only the count written and signals nothing about entries it never reached; `WorldContainer` carries `{eid, actor, inv}` and no index. The fail-open direction (a world holding more live containers than the caller's buffer) silently converts row 10 — a fail-closed wrong-write gate — into a pass, and no specified arm detects it. The fail-broken direction (no self-exclusion; the adopted container is live, is a container and passes Boundary 1 by consume time, so it appears in its own snapshot) is caught by A1, but only as a mystery refusal. One clause fixes both. |
| **D-D** | §0.3 fact 3's `MEASURED` tag and the account of why the deferred quiescence path cannot fill a park. Both lenses filed it under claims-overstated; neither made it blocking. | **BLOCKING** — carried as `BF-R3-4` (reconciler escalation). | `RECONCILER-VERIFIED` trace: `ApplyPendingDestroys` (`quiescence_drain.cpp:242`) is called only by `RunReconcile` (`:439`), which is reached only from `join_membership_sweep.cpp:496` (`TickClientReconcile`) and `quiescence_drain.cpp:463` (`OnTick`, itself called only from `join_membership_sweep.cpp:460`); `TickClientReconcile`'s only caller is `npc_mirror::TickClientNpcs` (`npc_mirror.cpp:541`), which returns on Host at `:543` — *"client-only (the host streams, it doesn't drive mirrors)"*. So a host arms deferred destroys and never applies them, and a deferred apply would echo-suppress rather than broadcast. §0.3 fact 3's causal half is therefore the reporter's reading, contradicted by the source, while carrying a `MEASURED` tag on the section revision 2 titles *"load-bearing"*. Escalated on the verdict's own RB-4 precedent, where a load-bearing mis-tag (§1.3) was blocking. The same finding absorbs the `NoteInboundDestroySlot(uint8_t)` signature defect: `RECONCILER-VERIFIED` `session.h:91-95` — `int senderPeerSlot = -1` — so the specified `uint8_t` parameter latches −1 as slot 255 and the `author-unknown` refusal §5.4 names for this very case can never fire (doctrine `coop-sync-doctrine.md:125-126`, *"a gate that cannot fire passes forever"*, the same rule that made RB-6 blocking). |
| **D-E** | Q-7's `SOURCE_PROVEN` closure. Data-loss endorses it; source-correctness says the write-site argument does not by itself prove a fresh eid has no map entry. | **Both correct; NOT blocking.** Optional tightening. | `RECONCILER-VERIFIED` `registry.cpp:152-174`: freed ids ARE returned to a free list, with a documented FIFO front-push so *"a freed id is only re-issued after the entire never-allocated pool above it is exhausted"*. The conclusion (a freshly minted eid has no stale entry) holds, but its support is the free-list discipline, not the write sites. Retag the support, note the pool-wrap case. |

## RB closure table (RB-1 … RB-7 from `VERDICT.md`)

| RB | Closed in revision 2? | Reconciled note |
|---|---|---|
| **RB-1** — key-only consume predicate on a line with measured key collisions | **YES** | All four required items delivered, and delivered in the source. `RECONCILER-VERIFIED` `remote_prop_destroy.cpp:73-99`: `DestroyResolvedLocalActor_` takes `keyW` as a parameter (`:73-74`) and the actor is live from `:79` through `Unpin` at `:97`, with `K2_DestroyActor` at `:98` — capture conditions 3 and 4 are writable exactly where §5.2 specifies. Item 3's short clock is replaced by a causal predicate (same author slot, same class, same key, first spawn only, same world generation, author still connected). Both lenses agree; the reconciler concurs that this is stronger on four axes and weaker on one (the 60 s window becomes a 900 s ceiling) — see the wording item in "claims to correct". |
| **RB-2** — no element id at the apply point | **YES** | `RECONCILER-VERIFIED` `subsystems.cpp:547/:548` (insertion point) and `prop_drop_intent.cpp:158/:394/:427` (`git grep` returns the definition, exactly ONE call, and a call to `OnPropDropIntent` at `:427`) — revision 2's correction of the verdict's "two call sites" is right. The eid is minted inside `DrainPendingSpawns`, so the precondition genuinely holds one statement later. The second consequence closes at `container_contents_sync.cpp:364` (`g_publishedHash[eid]` on any publication, fan-out included). |
| **RB-3** — the apply writes into a slot it has not proved is exclusively owned | **YES for the three required rows** | `RECONCILER-VERIFIED` `container_contents_sync.cpp:200-211`: `GObjStackSlot` refuses only `idx < 0` (`:207`) and `idx >= stack.num` (`:209`) — index 0 IS accepted, so row 8 is a real gate; Boundary 1's own comment (`:183-190`) supplies rows 8 and 9's rationale. The RB itself is closed. **Two new blocking findings attach to this same area** and must not be read as re-opening it: `BF-R3-1` (the nested indices the apply installs with none of this proof) and `BF-R3-3` (row 10's enumeration contract). |
| **RB-4** — the apply may refuse silently, no refusal observable | **YES** | §1.3's clause boxed and re-tagged `INFERRED`; §2.11's apply-refusal row carries the five required reasons verbatim plus revision 2's additions, with `index=%d`; A1 scores them; the fallback is named. Residual, not blocking: the **ARM** phase this revision introduces has no refusal observable at all, and §6 invariant 8 was narrowed from *"Every refusal is logged with a named reason"* to *"at both the capture and the apply"* — which exactly exempts the new phase. Highest-value optional item; see below. |
| **RB-5** — A2 asserts an observable a correct implementation cannot produce | **NO** | The required change was delivered literally and the reachability proof beneath it is sound and independently re-verified (a correct fix emits no operational `[CUSTODY]` line on the CONTROL chain: `HostSpawnPlacedProp` has one host-gated call site reached only `CLIENT→HOST`; `DestroySeamBody` only **sends**, `prop_destroy_seam.cpp:149`). But the restated pass condition — "zero `[CUSTODY]` lines **of any kind**" — is unsatisfiable against §2.11's own one-shot lane-alive line for the Tick, and against §3's own expected `not-container` refusal. `BF-R3-2`. One clause fixes it. |
| **RB-6** — the join-bracket guard is dead on the host; the coverage bound undeclared | **YES** | Bracket clause deleted with a correct host-side proof (`container_contents_sync.cpp:666`; two call sites in `event_feed.cpp`, both host-early-returning; the shipped sweep itself host-excluded at `:726`, `RECONCILER-VERIFIED`). The deliberate substitution is justified and correct: `world_load_episode`'s raise paths are client-only, so the verdict's own suggestion would have repeated RB-6, while `ue_wrap::world_identity::Generation()` is role-agnostic. Caveat (optional): `Generation()` bumps on any observed `CurrentWorld` change including a transient null, so a momentarily unresolvable player controller clears every park — a fail-closed coverage loss worth counting and logging. |
| **RB-7** — omission cases that cannot turn any assertion red | **YES** | The `SlotIo` injection makes the three wrong-write gates assertable (19 Boundary 1, 20 the `EngineAlloc` pre-flight whose shipped comment at `:494` says an empty array would otherwise *"silently replace real contents"*, 21 rederive-called-once), and 16–18 name the slot refusals; §4.2 closes by declaring what the harness does **not** prove. Residual, not blocking: two omission rows (`delete the capture call` → 1, `delete the author-slot latch` → 15) exercise `CustodyParkStore` directly and stay green if the production call site is deleted, and the `subsystems.cpp` Tick registration has no omission row at all. Both lenses found this; fold those three into §4.2's honest declaration or map them to A1's lines. |

## Blocking findings for revision 3

### BF-R3-1 — the apply installs nested `GObjStack` slot addresses with none of the ownership proof it applies to the outer slot

*Provenance: `r2-data-loss-regression` BF-R2-1, upheld; `r2-source-correctness` reached the same
mechanism as an optional sharpening. Reconciler decides BLOCKING (D-A).*

- **Requirement violated.** §2.10 (*"No path may … write into a slot it could not prove exclusively
  owned"*), §6 invariant 6, and Boundary 2's shipped rationale.
- **Evidence.** §2.3 requires the host-local capture to call `ReadContents` with
  `neuterNested=false`, so a nested container's `ints[0][0]` survives into the park.
  `RECONCILER-VERIFIED` `container_contents_sync.cpp:246-262`: `RecordIsNestedContainer` at
  `:228-233`, `NeuterNestedIndex` at `:258-262`, called at `:285`, with the comment stating that
  `ints[0][0]` *"is its own GObjStack index, a slot number in the sender's array"* and that a wrong
  one makes *"the container reuse GObjStack[0], a slot owned by someone else."* §2.5's rows 8, 9
  and 10 validate ONLY the fresh **outer** `Index`; §6 invariant 9 states the nested preservation as
  a positive rule with no validation; §5.4's not-covered table has no row for it. §11 declines the
  check on the ground that it *"needs the nested-slot liveness reader that does not exist yet, and
  … would refuse every nested case"* — **both halves are wrong**: the three readers such a check
  needs (`WorldContainerSlotIndex`, `SnapshotWorldContainers`, `ReadLivePersonalStore`) are already
  in §5.1's file set for rows 9 and 10, and the test is per-record, so it refuses only records that
  actually alias.
- **Consequence.** Park lifetime is bounded only by 900 s, author liveness and world generation, and
  Q-4 leaves *"reuse a free one"* open for `propInventory_C::init`. Concrete input: host container O
  holds nested container N whose record carries `ints[0][0] = 7`; a client takes O (the park keeps
  the raw 7); during the carry another container's fresh component is allocated slot 7 — a slot
  nothing addresses while N is merely a record; the client re-places O, every arm and consume check
  passes, and the record naming slot 7 is written through `SR::WriteSaveRecord` / `WriteArrHeader`
  (`:502-517`). Two live addressors now share `GObjStack[7]`, and the next `addObject`/`takeObj`
  through either orphans the other's buffer (`WriteArrHeader`, `:516`). The loss falls on the
  **third** container, so §11's *"stale, not destroyed, and no worse than the empty box the unfixed
  build gives"* understates it. Today no live component addresses that record at all, because the
  record set is never restored — the path is created by the fix.
- **Fix (document only).** At the consume, before the write, apply the same three tests to every
  preserved `ints[0][0]` of a record for which `RecordIsNestedContainer` is true: refuse when it is
  0, when it equals `ReadLivePersonalStore().slotIndex`, or when it equals the `Index` of any live
  world container. Refuse the **whole** apply with a named reason (`nested-slot-aliased`), never a
  partial or repaired set. Cost: one enumeration and at most 512 integer comparisons per consume —
  the same order as row 10. Add: the row to §2.5, the reason to §2.11's apply-refusal line, the
  invariant to §6 beside invariant 9, an assertion to §4.2 beside 22, the case to §5.4's table, and
  rewrite §11's "still open" paragraph to say what is now closed and what genuinely remains
  (`UNKNOWN`: whether inner slots are reused at all — Q-4).

### BF-R3-2 — A2's pass condition is unsatisfiable by a correct implementation (RB-5, reintroduced)

*Provenance: `r2-data-loss-regression` BF-R2-2, upheld; `r2-source-correctness` reached the same
conclusion for the second trigger only, as an optional scoring fix. Reconciler decides BLOCKING and
marks RB-5 NOT closed (D-B).*

- **Requirement violated.** §4.1 arm A2 (*"PASS = ZERO `[CUSTODY]` lines of any kind in the host log,
  AND the drive still present at C5"*) together with *"A1 without A2 is not a result"*, and doctrine
  Step 8 (`coop-sync-doctrine.md:120-128`). This is the property RB-5 exists to secure.
- **Evidence.** §2.11's "first entry" row specifies a one-shot
  `[CUSTODY] capture hook ENTERED for the first time on this peer (role=%s)` **"and the same for the
  Tick"**, explicitly modelled on `container_contents_sync.cpp:630-639` — `RECONCILER-VERIFIED`:
  that line is emitted as *"The first statement, ahead of every filter"*, so that its absence proves
  the lane dead. §5.1/§5.2 register `container_custody::Tick(&session)` in `subsystems.cpp`'s
  unconditional per-tick block (`RECONCILER-VERIFIED` `:515`, `:547`, `:548`), and the host is the
  only role that owns a park, so on a FIXED build the host emits that line in every session —
  including the A2 CONTROL arm, which is scored by grepping the host log for `[CUSTODY]`.
  Second, independent trigger: any client-authored keyed destroy that reaches the host during A2
  legitimately produces `[CUSTODY] HOST park REFUSED … reason=not-container`, which §3 already
  declares expected for A1.
- **Consequence.** The arm the design makes mandatory cannot be passed by a correct implementation.
  The two available outcomes are exactly RB-5's two: fail a correct fix, or reinterpret the pass
  condition at scoring time — which destroys the arm's evidentiary value, and historically leads to
  making the seams fire on the host-authored chain, the single change most likely to endanger the
  working `CONTROL_PASS` path.
- **Fix (document only).** State A2's grep exactly: **zero `[CUSTODY]` lines other than the one-shot
  lane-alive lines** — or give those a distinct prefix (`[CUSTODY-INIT]`) so the A2 grep and the
  planted-line probe both target the operational prefix only. Require the planted line to be shown
  matching **that exact grep** before the zero is scored, so probe-before-claim item (b) means what
  it says. And either declare the client idle for A2's duration or scope the grep to the subject
  key, so an unrelated client destroy cannot fail the control.

### BF-R3-3 — §2.5 row 10 names a dev-instrument enumerator with no completeness or self-exclusion contract

*Provenance: `r2-data-loss-regression` BF-R2-3, upheld in both halves; `r2-source-correctness`
found the self-exclusion half independently and filed it as minor. Reconciler decides BLOCKING
(D-C).*

- **Requirement violated.** §2.5 row 10 (*"the resolved `Index` != any **other** live world
  container's `Index`"*), stated as fail-closed, plus §2.10 and §6 invariant 6.
- **Evidence.** `RECONCILER-VERIFIED` `container_contents_sync.cpp:784-801` and
  `container_contents_sync.h:52-63`: `SnapshotWorldContainers` sits under *"dev-instrument seams"*,
  fills *"up to `want`"*, breaks at `if (n >= want) break;`, returns only the count written, and
  gives the caller no way to distinguish a complete enumeration from a truncated one except the
  ambiguous `n == want`; `WorldContainer` is `{eid, actor, inv}` — no index, hence §5.1's new
  `WorldContainerSlotIndex` is required per entry. Row 10 states no completeness requirement and no
  self-exclusion rule, although by consume time the adopted container has been through
  `MarkPropElement`, is live, is a container and passes Boundary 1, so it appears in its own
  snapshot. §9 item 2 asks a reviewer to check that row 10 *"really uses `SnapshotWorldContainers`"*
  but not that the enumeration is complete or self-excluding.
- **Consequence.** Two failures in opposite directions from one unstated contract. **Fail-open:** in
  a world holding more live containers than the caller's buffer, an aliasing `Index` is never
  compared, row 10 passes, and the parked records are written over another live world container's
  slot through `:494-517` — destroying contents that survive today, the exact outcome rows 8–10
  exist to prevent, and no specified arm detects it. **Fail-broken:** an implementation that does
  not exclude the target matches the target against itself, every consume refuses `slot-aliased`,
  and the patch is a logged no-op that still passes all 23 standalone assertions (A1 fails, but as
  an unexplained refusal).
- **Fix (document only).** In row 10: exclude the target by `actor` pointer or `eid`; size the
  buffer from the registry's prop count; and treat a full buffer (`n == want`) as an **unproven**
  enumeration that refuses with its own named reason rather than passing. Add an assertion in §4.2
  driving `OtherWorldContainerIndices` as a stub that reports truncation, so both halves turn an
  assertion red.

### BF-R3-4 — the deferred-quiescence exclusion is justified by a fact the source contradicts, and the refusal it names cannot fire as specified

*Provenance: both lenses filed the strength-tag half under claims-overstated; `r2-source-correctness`
filed the signature half as optional. Reconciler escalates to BLOCKING on the verdict's own RB-4
precedent (D-D).*

- **Requirement violated.** The programme's strength-tag rule (CONTEXT.md: a `MEASURED` claim is
  what a command or a log line says, nothing more) as already applied to this document by RB-4, and
  doctrine `coop-sync-doctrine.md:125-126` (*"a gate that cannot fire passes forever"*), the rule
  RB-6 was written to install.
- **Evidence.** (a) §0.3 fact 3 is tagged `MEASURED` in a section revision 2 titles *"load-bearing"*,
  but only the quoted log lines are measured; the causal half — *"the host's deferred destroy path
  … applied it at 21:51:16 against a different actor"* — is the reporter's narrative and the source
  contradicts it. `RECONCILER-VERIFIED` chain: `ApplyPendingDestroys` (`quiescence_drain.cpp:242`)
  ← `RunReconcile` (`:439`) ← `TickClientReconcile` (`join_membership_sweep.cpp:496`) and `OnTick`
  (`quiescence_drain.cpp:463` ← `join_membership_sweep.cpp:460`) ← `npc_mirror::TickClientNpcs`
  (`npc_mirror.cpp:541`), which returns on Host at `:543` (*"client-only (the host streams, it
  doesn't drive mirrors)"*). A host arms deferred destroys and never applies them; a deferred apply
  would also echo-suppress (`remote_prop_destroy.cpp:93`) rather than broadcast. The claim is reused
  in §2.2, §2.4, §5.4's not-covered table and §11. (b) §5.2/§5.1 specify
  `NoteInboundDestroySlot(uint8_t)`, but `RECONCILER-VERIFIED` `session.h:91-95`:
  `int senderPeerSlot = -1`. Narrowing latches −1 as slot 255, so "no sender slot was latched" —
  capture condition 1, and the `author-unknown` refusal §5.4 names for exactly this case — can never
  be observed; the park is instead created and later starved at the arm. `/W4 /WX` will force the
  decision anyway.
- **Consequence.** A normative not-covered table justifies a refusal with a false causal statement,
  and names a refusal that cannot fire. The mechanism's own exclusion is real but rests on a
  different fact, which the document does not state; a later contributor removing the (harmless)
  latch discipline would have no way to see what actually protects the path.
- **Fix (document only).** Re-tag §0.3 fact 3: `MEASURED` for the log lines and the same-second
  key collision; `INFERRED, CONTRADICTED BY SOURCE` for the causal attribution, with the trace
  above. Restate §2.2's *"deliberate, valuable side effect"* — the deferred path cannot fill a host
  park because **it does not run on a host at all**; keep `author-unknown` as defence in depth.
  Restate §5.4's deferred-quiescence row's "why it is out" column with the real reason. Change the
  seam signature to `NoteInboundDestroySlot(int)` with an explicit out-of-range refusal (the sibling
  sites range-check: `event_dispatch_entity.cpp:82`, `event_dispatch_intent.cpp:267`). The
  clock-removal decision itself **survives** on its independent and sound coverage argument
  (unmeasured carry duration, §0.1 and Q-9) and is not re-opened.

## Minimal change list for revision 3

Document-only. No mechanism is re-designed; no seam moves; no arm is added.

1. **§2.5 / §2.11 / §6 / §4.2 / §5.4 / §11** — nested-index ownership test, refusal reason
   `nested-slot-aliased`, invariant, assertion, not-covered row, and a rewritten §11 paragraph
   (`BF-R3-1`).
2. **§4.1 A2 (+ §2.11's first-entry row, + §4.1 probe-before-claim item (b))** — exclude the
   lane-alive lines from A2's grep, by exception or by a distinct prefix; scope the grep to the
   subject key or declare the client idle; require the planted line to match the exact A2 grep
   (`BF-R3-2`). RB-5 is re-scored on this change alone.
3. **§2.5 row 10 (+ one §4.2 assertion)** — self-exclusion by actor/eid, buffer sizing, and
   `n == want` treated as unproven with its own refusal (`BF-R3-3`).
4. **§0.3 fact 3, §2.2, §5.4's deferred-quiescence row, §5.1/§5.2's seam signature** — retag,
   restate with the real (client-driven) reason, and take the latch as `int` with a range refusal
   (`BF-R3-4`).

Strongly recommended in the same pass (not blocking, but each is one row):

5. **§2.11 + §6 invariant 8** — add an ARM-phase refusal line
   (`author-mismatch|class-mismatch|already-armed|key-mismatch|world-changed`) and restore
   invariant 8 to *every* phase. Without it, A1 cannot distinguish an arm refusal from an arm hook
   that never ran — RB-4's defect, one phase over.
6. **§5.4** — add the destroy-for-good row (a hammer break, a coin-gun sale and a trash disposal
   ride the same `DestroySeamBody`; with no wall clock such a park stays consumable to the 900 s
   ceiling). Duplication into a container that is empty today, not loss — but it is the residue the
   removed clock used to bound, and §5.4 currently reads as complete.
7. **§4.2 omission table** — relabel `delete the capture call` → 1 and `delete the author-slot
   latch` → 15, and add the missing `subsystems.cpp` Tick registration row, or fold all three into
   §4.2's existing honest declaration. A header-only harness that calls `Park` directly cannot
   detect a missing production call site; A1 covers those.

## What revision 3 will authorise if these four close

Stated now so the scope is not renegotiated later. **Not authorised by this verdict.**

- **Files:** the two new module files (`include/coop/props/container_custody.h`,
  `src/coop/props/container_custody.cpp`); one-or-two-line call sites in
  `event_dispatch_entity.cpp`, `remote_prop_destroy.cpp`, `prop_drop_intent.cpp`,
  `subsystems.cpp`; the wrapper/`neuterNested` block in `container_contents_sync.{h,cpp}`; the
  read-only `ue_wrap` accessor; the `container_selftest` A5 hook; three docs files; the test tree.
- **Tests:** `src/votv-coop/tests/custody_park/**` plus `candidate_17_container_custody.cpp` — the
  23 assertions and 17 omissions of §4.2, plus the new nested-index and truncation assertions,
  landed red first.
- **Arms, in §5.3's order:** A0 (cited, not re-run) → A2 → A1 → A4b → A3 → A5. A4 stays
  `BLOCKED_ON_OBS_R3Q_C01`.
- **Remaining `UNKNOWN` at that point:** Q-4 (`propInventory_C::init`'s allocation policy, hence
  whether inner-slot reuse can occur at all), Q-2 (orphaned-slot fate), Q-3 (whether a wire custody
  transfer is ever needed), Q-5's residual (whether `ClearAnyDriveFor` /
  `ReleaseMainPlayerGrabIfHolding` can mutate the inventory before the capture reads it), Q-9
  (real carry duration), and whether the production `SlotIo` binding calls the shipped functions —
  proven only by A1, A5 and code review, as §4.2 already declares.

## Merged optional improvements (deduplicated across both lenses)

1. Make the author-slot latch scope-bounded — clear it on every exit of the `PropDestroy` dispatch
   with a scope guard, not only inside the capture. `OnDestroyImpl_` returns before the capture at
   `:115-122`, `:143-149` and `:157-189` (including the defer at `:183`), so a latch set at
   `event_dispatch_entity.cpp:254` outlives its message. Harmless on today's call graph; a guard
   makes author-unknown an invariant rather than a coincidence.
2. Count and log world-changed clears, and consider pairing the generation stamp with
   `WorldKind::Gameplay`: `Generation()` bumps on any observed `CurrentWorld` change including a
   transient null.
3. Tighten Q-7's support to the free-list discipline (`registry.cpp:152-174`) and note the
   pool-wrap case, where a stale `g_sentHash` entry could skip the fan-out at `:344-347`.
4. Score A2 positively as well as negatively — "zero park-taken / ARMED / reattached / fanout /
   DISCARDED lines under the subject key" — rather than only as an absence.
5. Carry the actor pointer and class on the park-refused line, so §3's expected `not-container`
   refusal (the husk) can be told apart from the subject box itself failing `IsContainerActor`
   (`:247-250`). State explicitly, as an assumption with A1 as its observable, that the R3Q subject
   class `prop_box_C` walks to `prop_container_C` — the reachability precondition of the whole
   mechanism on the measured chain, nowhere established in the document.
6. Note in A4b/A5 that `container_selftest=1` also dispatches `prop_container_C::extract(0)` on a
   world container at +10 s (host) and +25 s (client) (`container_selftest.h:9-15`); the arm must
   ensure that container is not the subject box, or the digest comparison and A1's `records=N` are
   perturbed.
7. State in §5.4 that the apply copies the parked set into the fresh slot while the orphaned old
   slot keeps its copy, so the host's save carries the record set twice — consistent with the
   shipped orphan-the-old-buffer rule (`:511-515`), bounded, but bounded only by Q-2.
8. Bind a connection generation or peer identity alongside the raw author slot (a disconnect
   followed by a different player occupying slot S before the sweep observes
   `IsSlotConnected(S) == false` leaves the park armable by the new occupant), and state the custody
   sweep's cadence in §2.3 — the document says *"the next sweep"* without saying how often.
9. Capture condition 9 measures the wrong pack: the bound exists so anything parked can be fanned
   out, but `BroadcastContainer` re-packs with nested indices neutered and `NeuterNestedIndex`
   resizes an empty `ints[]` to one element, so the wire pack can exceed the host-local one. Measure
   the size the fan-out would produce.
10. §2.1's "latest wins" note covers the dropped set but not the surviving one: a client that takes
    box B (key K) then box A (key K) and places B first gives B the contents of A. No loss relative
    to today; say so.

## Claims to correct in revision 3 (merged, both lenses)

- §0.3 fact 3's `MEASURED` tag and its downstream uses (`BF-R3-4`).
- §2.2's *"deliberate, valuable side effect"* (`BF-R3-4`), and §2.2's *"The class binding and the
  author binding are what restore it"* — they narrow AUTHORITY substantially but do not restore it:
  a client that destroys a container and then places a same-class container under a colliding key
  still determines which container the host's contents land in.
- §2.4's *"strictly stronger than the single generation / same authoring peer / short window the
  verdict asked for"* — stronger on four axes, weaker on one (60 s → 900 s). State the trade.
- §5.4's *"The limits above are causal, not temporal, and each is observable in the host log"* —
  incomplete until the destroy-for-good row and the nested-index row are added.
- §11's reason for leaving the nested-slot question open (`BF-R3-1`): the readers exist and the test
  is per-record.
- §6 invariant 8's narrowing to "at both the capture and the apply", which exactly exempts the ARM
  phase this revision introduced.
- §4.2's *"Seventeen omissions, seventeen named assertions"* — fifteen hold; two name assertions a
  header-only harness cannot turn red, and the Tick registration has no row.
- Q-7's `SOURCE_PROVEN` support (D-E).

## Citations re-opened by the reconciler on `3af5ddae`

| Citation | Holds | Note |
|---|---|---|
| `container_contents_sync.cpp:183-195` Boundary 1, fail-closed on `Player != 0` or an unresolvable offset | yes | Comment carries the `GObjStack[0]` / `ReadLivePersonalStore` symmetry as quoted by both lenses. |
| `container_contents_sync.cpp:200-211` `GObjStackSlot` refuses only `idx < 0` (`:207`) and `idx >= stack.num` (`:209`) | yes | Index 0 is accepted — the load-bearing fact under rows 8–10. |
| `container_contents_sync.cpp:246-262` Boundary 2, `RecordIsNestedContainer`, `NeuterNestedIndex` | yes | The comment states the wrong-owner slot-reuse failure in the engine's own terms. Basis of `BF-R3-1`. |
| `container_contents_sync.cpp:271-289` `ReadContents`, 512 cap, neuter at `:285`; one caller at `:334` | yes | The defaulted `neuterNested` changes no wire behaviour. |
| `container_contents_sync.cpp:344-364` sent/published hashes | yes | `g_publishedHash[eid]` at `:364` closes RB-2's second consequence. |
| `container_contents_sync.cpp:494-521` pre-flight, `AllocZeroed`, `WriteArrHeader`, `Rederive` | yes | Pre-flight comment as quoted. |
| `container_contents_sync.cpp:630-639` the lane-alive convention, *"first statement, ahead of every filter"* | yes | Basis of `BF-R3-2`. |
| `container_contents_sync.cpp:726` `if (!IsHost()) SweepParked();` | yes | Supports RB-6's closure. |
| `container_contents_sync.cpp:784-801` / `.h:52-63` `SnapshotWorldContainers` | yes, **with the gap** | Dev-instrument seam; breaks at `want`; no truncation signal; `{eid, actor, inv}` only. Basis of `BF-R3-3`. |
| `subsystems.cpp:515 / :547 / :548` | yes | Per-tick block is unconditional; insertion point and ≤250 ms fan-out latency both follow. |
| `prop_drop_intent.cpp:158 / :394 / :427`; `git grep HostSpawnPlacedProp` | yes | Exactly ONE call site. Revision 2's correction of `VERDICT.md` is right. |
| `remote_prop_destroy.cpp:73-99` capture point (`keyW` a parameter; `Unpin` `:97`; `K2_DestroyActor` `:98`) | yes | The specified insertion is writable with both operands in scope. |
| `event_dispatch_entity.cpp:245-254`, `session.h:91-95` | yes | Insertion point is the statement immediately before `remote_prop::OnDestroy`; `int senderPeerSlot = -1`. Basis of `BF-R3-4`(b). |
| `npc_mirror.cpp:541-543` → `join_membership_sweep.cpp:456-496` → `quiescence_drain.cpp:439 / :463` | **the design's causal claim does not hold** | The deferred apply is unreachable on a host. Basis of `BF-R3-4`(a). The data-loss lens cited the host gate as `:542`; it is `:543`. |
| `registry.cpp:152-174` `FreeId` FIFO front-push | yes | Q-7's real support; the conclusion holds, the stated support does not (D-E). |
| `container_contents_sync.h:44-47` `NoteJoinSnapshotBracket` is client-side by its own comment | yes | Supports RB-6's closure. |

---

# Appendix — the two raw reviews, verbatim

```json
[
 {
  "lens": "r2-source-correctness",
  "review": {
   "verdict": "DESIGN_CLEAN",
   "blocking_findings": [],
   "rb_closure": [
    {
     "rb": "RB-1",
     "closed": true,
     "note": "Closed. (1) Capture conditions 3-4 park under the dying actor's own wire-normalised key and refuse on empty or on mismatch with payload.key; implementable exactly where specified, because DestroyResolvedLocalActor_ takes keyW as a parameter (remote_prop_destroy.cpp:73-74) and the actor is still live from :79 through :97 (K2_DestroyActor is :98). The kerfur third entry is additionally shown unreachable on a host (kerfur_convert_client.cpp:269). (2) CustodyPark.cls + arm condition 2 + consume row 4 + assertion 12; R::ClassNameOf exists (reflection.h:264) and is the same domain as the wire className (protocol.h:1100), so the comparison is well-formed. (3) The peer binding is implementable: msg.senderPeerSlot is in scope at event_dispatch_entity.cpp:254 and is the ONLY scope that has it (PropDestroyPayload has no sender field, protocol.h:1088-1093); OnPropDropIntent carries senderSlot (prop_drop_intent.cpp:378) and the router forces slot >= 1 (event_dispatch_intent.cpp:267). Arm-once + same class + same key + same world generation (world_identity.h:55-58) replace the clock, with the substitution reasoned at the point of change. (4) The MarkPropElement re-key rule is source-grounded: prop_lifecycle.cpp:211-214 and prop_element_tracker.cpp:252-267 re-key only against a live incumbent. Residue not named in 5.4: the same peer destroying a container for a non-pickup reason and later placing a colliding-keyed same-class container with no live incumbent can still consume, now up to 900 s later rather than 60 s."
    },
    {
     "rb": "RB-2",
     "closed": true,
     "note": "Closed. HostSpawnPlacedProp ends at return actor (prop_drop_intent.cpp:231) with no element minted, and git grep confirms one call site (:394; :427 is a call to OnPropDropIntent). The eid is minted in the next-tick adoption: host_spawn_watcher.cpp:237 enqueues, :352-383 drains, :374 ExpressSpawnedProp, :375-379 the eid exists. The custody Tick is placed between subsystems.cpp:547 (DrainPendingSpawns) and :548, so the eid is available on the same pass; kMaxAdoptTries plus a named eid-unresolved refusal bound the wait. MarkDirty feeds the shipped edge set (container_contents_sync.cpp:654) drained by DrainDirty (:400-421). The second consequence is genuinely closed: the fan-out's success path writes g_publishedHash[eid] at :364, so the first client write is judged against a real base by HostAcceptsClientWrite (:528-558). Latency statement is correct: container_contents_sync::Tick runs earlier in the same pass (subsystems.cpp:515) behind kSweepMs=250 (:51,:717-719). File set, call sites, TRANSITIONS row and the G16 claim are rebuilt and the one-line-revert claim retracted."
    },
    {
     "rb": "RB-3",
     "closed": true,
     "note": "Closed. GObjStackSlot refuses only idx<0 (:207) and idx>=num (:209) - index 0 is accepted, exactly as claimed - and inventory.h:63-74 plus inventory.cpp:155-169 support both the index-0 rationale and the personal-store comparison via ReadLivePersonalStore().slotIndex (declared inventory.h:87, fail-closed on Player==0 at inventory.cpp:155-165). Rows 8/9/10 are all cheap and fail-closed; SnapshotWorldContainers (container_contents_sync.h:56-63, definition :784-801) applies Boundary 1 itself at :797, and because it returns only {eid, actor, inv} the design correctly adds a public WorldContainerSlotIndex to 5.1. index=%d is printed on park, reattach and apply-refusal lines. Section 9 item 4 is retracted and Q-4 is moved to the blocking column with the degradation path (slot-unresolved, logged) named. Minor: row 10 must exclude the consuming container's own entry, since it now has an eid and appears in its own enumeration."
    },
    {
     "rb": "RB-4",
     "closed": true,
     "note": "Closed. The 1.3 clause is boxed and re-tagged INFERRED with the load-bearing note. The 2.11 apply-refusal row carries the verdict's five required reasons verbatim (not-container|boundary1|slot-unresolved|slot-aliased|alloc-preflight) plus this revision's added ones, with index=%d, and A1 scores them. The silent-no-op mode the finding named is real in the source (GObjStackSlot returns null on Index<0, :206-207) and now has an observable. The named fallback is the next-tick consume that RB-2 installs."
    },
    {
     "rb": "RB-5",
     "closed": true,
     "note": "Closed, and the reachability proof re-verified independently. Apply seam: HostSpawnPlacedProp has exactly one call site, prop_drop_intent.cpp:394 inside OnPropDropIntent, host-gated at :380 and entered only from event_dispatch_intent.cpp:258-280 (CLIENT->HOST, dropped on a client at :263-266); the reel door reaches it only through :427. Capture seam: DestroyResolvedLocalActor_ is anonymous-namespace, called only at :190 from OnDestroyImpl_, entered from OnDestroy (:196-198, event_dispatch_entity.cpp:254, a received PropDestroy), TryApplyDestroy (:204-207) and kerfur (client-only, :269); a host's own pickup runs DestroySeamBody, which only sends (prop_destroy_seam.cpp:149). So a correct fix emits no [CUSTODY] line on the CONTROL chain and A2's new polarity is right. The fresh-empty rule is re-justified against three reachable cases, test 2 is relabelled pure logic, and the DISCARDED branch is declared unit-tested only."
    },
    {
     "rb": "RB-6",
     "closed": true,
     "note": "Closed. The bracket clause is deleted with the host-side proof re-verified: NoteJoinSnapshotBracket's comment at container_contents_sync.cpp:666, exactly two call sites (event_feed.cpp:361, :424), both handlers returning early on the host (:341, :413), and the shipped sweep itself host-excluded (if (!IsHost()) SweepParked(), :726). Test 7 is retargeted to three host-valid aging assertions. The deliberate substitution is justified: world_load_episode is client-only on every raise path (header:1 and :50-53, net_pump.cpp:234 with RaiseReconcileForReload at :243, session_runtime.cpp:315-317 'the sole, client-only arm site'), so the verdict's suggested clear would have repeated RB-6; world_identity::Generation() (world_identity.h:55-58, bump site world_identity.cpp:178-184) is role-agnostic. Ceiling 900 s, author-liveness clear and OnDisconnect bound the store, and 5.4 states the residual coverage causally. Caveat: Generation() bumps on any observed world-pointer change including a transient null, so a momentary unresolvable player controller clears parks (fail-closed coverage loss, worth counting)."
    },
    {
     "rb": "RB-7",
     "closed": true,
     "note": "Closed by the first branch the verdict allowed. The SlotIo struct injects the nine engine operations, so Boundary 1 refusal (19), the EngineAlloc pre-flight refusal (20, shipped comment at container_contents_sync.cpp:494) and rederive-called-once (21) become named assertions, and 16-18 name the slot refusals; the omission table maps 17 omissions to assertions. A5 gets a concrete instrument under the already-shipped container_selftest=1 gate (container_selftest.h:1-15, :23-31) driven by a new read-only ue_wrap accessor, both listed in 5.1, and 4.2 closes by declaring what the harness still does not prove (that the production SlotIo binding calls the shipped functions). One row is mislabelled: 'delete the capture call -> assertion 1' cannot be turned red by a harness that calls Park directly; that omission is covered by A1."
    }
   ],
   "optional_improvements": [
    "Re-tag section 0.3 fact 3. Its log lines are MEASURED, but the causal half ('the host's deferred destroy path ... applied it at 21:51:16') is the reporter's reading and the source contradicts it: ApplyPendingDestroys (quiescence_drain.cpp:439, inside RunReconcile) is reached only from join_membership_sweep::TickClientReconcile (:456-496), whose sole caller is npc_mirror::TickClientNpcs, which returns on Host at npc_mirror.cpp:543; and a deferred apply would echo-suppress (remote_prop_destroy.cpp:93) rather than produce the host's own destroy-seam broadcast quoted at report :48. The TTL decision survives on its other grounds (unmeasured carry duration; the same-second collision at report :47-49).",
    "Restate section 2.2's 'valuable side effect'. The deferred quiescence path cannot fill a host park because it does not run on a host at all (above), not because it lacks a latch. Keep author-unknown as defence-in-depth.",
    "Make the author-slot latch scope-bounded: clear it on every exit of the PropDestroy dispatch, not only inside the capture. OnDestroyImpl_ returns before the capture at :115-122 (trash proxy), :143-149 (arbiter echo) and :157-189 (no actor, including the defer at :183), so a latch set at event_dispatch_entity.cpp:254 outlives its message. Harmless on today's call graph (every host capture is immediately preceded by a fresh latch write), but a scope guard makes author-unknown an invariant rather than a coincidence.",
    "NoteInboundDestroySlot should take int and refuse out-of-range: msg.senderPeerSlot is int with a -1 default (session.h:95) and is range-checked at the sibling sites (event_dispatch_entity.cpp:82, event_dispatch_intent.cpp:267). Narrowing to uint8_t would latch -1 as slot 255, and /W4 /WX will force the decision anyway.",
    "Add the removed clock's residue to 5.4: the same peer destroying a container for a reason other than a pickup, then placing a same-class container carrying a colliding key with no live incumbent at adoption (so key-rekeyed does not fire), consumes the park at any point up to kCustodyParkCeilingSec = 900 s. Consider a moderate arm ceiling (minutes) as a bound on that window, distinct from the leak guard.",
    "Tighten Q-7's support. The four maps are never erased per eid (only g_appliedHash at :661, and all of them at OnDisconnect :824-827), so 'a freshly minted eid has no entry' rests on eid recycling, not on write sites: the real support is registry.cpp:160-168 (a freed id is pushed to the front and re-issued only after the never-allocated pool of 32768 host ids is exhausted). Note the wrap case, where a stale g_sentHash entry could skip the fan-out at :344-347.",
    "Count and log world-changed clears, and consider pairing the generation stamp with WorldKind::Gameplay: Generation() bumps whenever the observed CurrentWorld pointer changes (world_identity.cpp:167-184), including a transient null from an unresolvable player controller, which would silently clear every park.",
    "Section 2.5 row 10: exclude the consuming container's own entry from the SnapshotWorldContainers enumeration (it has an eid by consume time), and read each entry's index through the new WorldContainerSlotIndex, since the enumerator returns only {eid, actor, inv} (container_contents_sync.h:56-63).",
    "Score A2 as 'zero park-taken / ARMED / reattached / fanout / DISCARDED lines, and no [CUSTODY] line under the subject key', rather than zero lines of any kind: any unrelated client-authored keyed destroy reaching the host during the CONTROL arm produces a park REFUSED reason=not-container line and would fail a correct fix.",
    "Carry the actor pointer and class on the park-refused line. Section 3 declares an expected not-container refusal under the subject key (the husk), but the line prints only key and reason, so it cannot be told apart from the subject box itself failing IsContainerActor (:247-250). Relatedly, state explicitly as an assumption that the R3Q subject class prop_box_C walks to prop_container_C - the reachability precondition of the whole mechanism on the measured chain, nowhere established in the document - with A1 as its observable.",
    "Note in A4b/A5 that container_selftest=1 also dispatches prop_container_C::extract(0) on a world container at +10 s (host) and +25 s (client) (container_selftest.h:9-15); the arm must ensure that container is not the subject box, or the digest comparison and A1's records=N can be perturbed.",
    "State in 5.4 that the apply copies the parked set into the fresh slot while the orphaned old slot keeps its copy, so the host's save carries the record set twice. Consistent with the shipped orphan-the-old-buffer rule (:511-515) and bounded, but the duplication's only real bound is Q-2.",
    "Sharpen the 'still open' nested-slot item: if a freed inner GObjStack slot is ever reused, a preserved ints[0][0] aliases another container's contents rather than merely being stale. The exposure is not novel (the native host-authored path re-installs the same index, per :252-257), so say that, and make the closing assertion 'the preserved index resolves to a live world container whose identity matches' rather than merely 'resolves'.",
    "Relabel the omission row 'delete the capture call -> assertion 1': a header-only harness that calls Park directly cannot detect a missing production call site; A1 covers it, consistent with 4.2's own closing declaration."
   ],
   "citations_checked": [
    {
     "citation": "remote_prop_destroy.cpp:73-99 - the capture point, with keyW as a parameter and the actor live from :79 to :97",
     "holds": true,
     "note": "Signature at :73-74 is (void* actor, const std::wstring& keyW, const PropDestroyPayload&, void* localPlayer); Unpin at :97, K2_DestroyActor at :98. The specified insertion is writable as one line with both operands in scope."
    },
    {
     "citation": "remote_prop_destroy.cpp:123 keyW from the wire payload; :133-135 eid resolve first; :150 key fallback; :190 the only call to DestroyResolvedLocalActor_",
     "holds": true
    },
    {
     "citation": "remote_prop_destroy.cpp:196-198 OnDestroy, :204-207 TryApplyDestroy; no IsHost/role()/Role:: anywhere in the file",
     "holds": true,
     "note": "grep returns nothing, so the session_holder self-gate is required as the design says."
    },
    {
     "citation": "event_dispatch_entity.cpp:254 the OnDestroy dispatch, with msg.senderPeerSlot in scope",
     "holds": true,
     "note": "senderPeerSlot is int, default -1 (session.h:95); other cases range-check it (:82, :267)."
    },
    {
     "citation": "prop_drop_intent.cpp:158-232 HostSpawnPlacedProp ends at return actor with no eid minted",
     "holds": true
    },
    {
     "citation": "prop_drop_intent.cpp:377-400 OnPropDropIntent - senderSlot :378, host gate :380, dup guard :390-393, the one HostSpawnPlacedProp call :394, if (actor) block :395-399",
     "holds": true,
     "note": "The arm point is writable with actor, key, cls and senderSlot all in scope."
    },
    {
     "citation": "git grep HostSpawnPlacedProp 3af5ddae -- src/votv-coop: definition :158, exactly one call :394, :427 is a call to OnPropDropIntent",
     "holds": true,
     "note": "Revision 2's correction of the verdict's 'two call sites' is right."
    },
    {
     "citation": "host_spawn_watcher.h:29-32 enqueue-only; cpp:219-238 push_back; :352-383 DrainPendingSpawns with :360 IsLiveByIndex, :363 EidForActor, :374 ExpressSpawnedProp, :375-379 the eid exists",
     "holds": true
    },
    {
     "citation": "prop_lifecycle.cpp:211-214 the re-key note and MarkPropElement; :371-373 ExpressSpawnedProp",
     "holds": true
    },
    {
     "citation": "prop_element_tracker.cpp:243-275 - re-key only against a different LIVE incumbent (:252-254), MintFreshKeyForDuplicate :261, enrol under the fresh key :267",
     "holds": true,
     "note": "The :268-272 re-key-FAILED branch enrols under the duplicate, so key-rekeyed does not fire there."
    },
    {
     "citation": "subsystems.cpp:515 container_contents_sync::Tick, :547 DrainPendingSpawns, :548 prop_drop_intent::Tick",
     "holds": true,
     "note": "Confirms both the insertion point and the next-sweep fan-out latency."
    },
    {
     "citation": "container_contents_sync.cpp:183-195 Boundary 1; :200-211 GObjStackSlot refuses only idx<0 (:207) and idx>=num (:209), index 0 accepted",
     "holds": true
    },
    {
     "citation": "container_contents_sync.cpp:252-262 Boundary 2 with the loadData/init reading at :252-257; :271-289 ReadContents, cap at :275-279, the neuter call at :285",
     "holds": true
    },
    {
     "citation": "git grep ReadContents: definition :271 and exactly one caller, BroadcastContainer :334 (Q-8)",
     "holds": true
    },
    {
     "citation": "container_contents_sync.cpp:344-347 sentHash gate, :348-352 blob ceiling, :359 g_sentHash, :364 g_publishedHash, :369-373 the shipped ship line",
     "holds": true,
     "note": "RB-2's second consequence is genuinely closed by :364."
    },
    {
     "citation": "container_contents_sync.cpp:400-421 DrainDirty edge-driven, :415 forward resolve, :418 Boundary 1; :654 the only g_dirty write",
     "holds": true
    },
    {
     "citation": "container_contents_sync.cpp:471-524 ApplyContents - :486-490 Boundary 1, :491-492 slot resolve, :494-500 EngineAlloc pre-flight, :502-517 raw write, :521 Rederive",
     "holds": true
    },
    {
     "citation": "container_contents_sync.cpp:528-558 HostAcceptsClientWrite; :563 unknown-op refusal; :573 the sixth Boundary 1 site",
     "holds": true
    },
    {
     "citation": "container_contents_sync.cpp:666 NoteJoinSnapshotBracket's client-side comment; grep gives exactly two call sites (event_feed.cpp:361, :424) with host early-outs at :341 and :413; :726 if (!IsHost()) SweepParked()",
     "holds": true,
     "note": "RB-6's dead-guard evidence fully reproduced."
    },
    {
     "citation": "container_contents_sync.h:56-63 SnapshotWorldContainers; definition :784-801 applies Boundary 1 at :797",
     "holds": true,
     "note": "Returns {eid, actor, inv} only - no index - so row 10 needs the new accessor the design lists."
    },
    {
     "citation": "world_identity.h:55-58 Generation(); bump site world_identity.cpp:167-184",
     "holds": true,
     "note": "Bumps on any observed CurrentWorld change, including a transient null."
    },
    {
     "citation": "inventory.h:63-74 and :84-87; inventory.cpp:138-178 with the Player==0 refusal at :155-165 and the index at :167-169",
     "holds": true
    },
    {
     "citation": "kerfur_convert_client.cpp:269 client-only gate, :310-313 the synthesised empty-key PropDestroy",
     "holds": true
    },
    {
     "citation": "quiescence_drain.cpp:242-256 / :439 ApplyPendingDestroys and the claim that the deferred path can never fill a host park",
     "holds": true,
     "note": "True, but for a stronger reason than the design gives: RunReconcile/OnTick are reached only from join_membership_sweep::TickClientReconcile (:456-496), whose only caller npc_mirror::TickClientNpcs returns on Host (npc_mirror.cpp:543). This also falsifies the causal half of section 0.3 fact 3."
    },
    {
     "citation": "prop_destroy_seam.cpp:84-88 client-only load-episode suppression, :119-121 the 31-char truncation, :131 the eid-0 sentinel, :149 send, :154-156 NoteClientKeyedDestroy (Client-gated)",
     "holds": true
    },
    {
     "citation": "protocol.h:1088-1094 PropDestroyPayload (40 B, no sender field); :1099-1112 PropDropIntentPayload (172 B, one datagram); blob_chunks.h:45-47 MaxBlobBytes",
     "holds": true
    },
    {
     "citation": "element/registry.h:48-51 FreeId and registry.cpp:152-174 - eids ARE returned to a free list",
     "holds": false,
     "note": "Not a cited line in the design, but it bears on Q-7's SOURCE_PROVEN reasoning: the write-site argument alone does not prove a fresh eid has no map entry. The FIFO front-push (:160-168) is what does, and only until the 32768-id pool wraps."
    },
    {
     "citation": "coop-sync-doctrine.md:98-99 the actor-gap sentence and :99-101 'keys are load-bearing'; :125-126 'a gate that cannot fire passes forever'",
     "holds": true,
     "note": "Draft 1's :96-97 drift is correctly fixed."
    },
    {
     "citation": "f3d6b59d:design/relay-adoption/MULTIVOID-CURRENT-MODEL.md:27-38 the ten-axis table and :40 the SOURCE_PROVEN identity-domains claim",
     "holds": true,
     "note": "Draft 1's :44 drift is correctly fixed."
    },
    {
     "citation": "docs/devices.md:171-183 'Who owns what' and :185-196 'Wire messages' carry no container-contents / ContainerContents row",
     "holds": true
    },
    {
     "citation": "prop_drop_intent.cpp:46-47 session_holder, :67-86 the park set/FIFO invariant and UnparkKey, :88-93 FillWireStr's N-1 cap; container_selftest.h:1-15,:23-31",
     "holds": true,
     "note": "container_selftest also dispatches prop_container_C::extract(0) on a world container at +10 s / +25 s, which the A4b/A5 arms should account for."
    },
    {
     "citation": "world_load_episode.h:1 and :50-53; net_pump.cpp:234 host early return with RaiseReconcileForReload at :243; session_runtime.cpp:315-317 'the sole, client-only arm site'",
     "holds": true,
     "note": "Confirms the deliberate substitution of world generation for the verdict's suggested clear."
    },
    {
     "citation": "BugDropbox Report 3 bug_03 ... :13,:38-39,:47-49,:57-58,:60 - the key-collision evidence",
     "holds": true,
     "note": "The log lines and the key non-uniqueness hold; only the causal attribution of the 21:51:16 destroy to a host-side deferred apply does not survive the source trace."
    }
   ],
   "claims_overstated": [
    "Section 0.3 fact 3 is tagged MEASURED, but only the log lines are measured; 'the host's deferred destroy path ... applied it at 21:51:16 against a different actor' is the reporter's causal reading, and on 3af5ddae that apply path is unreachable on a host (npc_mirror.cpp:543 -> join_membership_sweep.cpp:456-496 -> quiescence_drain.cpp:439) and would echo-suppress rather than broadcast (remote_prop_destroy.cpp:93).",
    "Section 2.2's 'deliberate, valuable side effect: the deferred destroy-by-key path ... can never fill a park' credits the latch for an exclusion the call graph already provides; the latch's real contribution is the author binding.",
    "Q-7's 'CLOSED, STRENGTH=SOURCE_PROVEN' rests on where the four maps are written, which does not by itself establish that a freshly minted eid has no entry; the maps are never erased per eid (only :661 and OnDisconnect :824-827), so the claim depends on the free-list discipline at registry.cpp:160-168.",
    "Section 5.4's 'The limits above are causal, not temporal, and each is observable in the host log' reads as complete; it omits the same-peer, same-class, colliding-key consume that the removed clock used to bound, now open to the 900 s ceiling.",
    "Section 2.4's 'strictly stronger than the single generation / same authoring peer / short window the verdict asked for' is stronger on four axes and weaker on one (60 s -> 900 s); state the trade rather than a strict ordering.",
    "Section 11's still-open nested-slot note - 'stale, not destroyed, and no worse than the empty box the unfixed build gives' - understates the variant where a reused inner slot makes a preserved ints[0][0] alias another container's contents (the same exposure the native host-authored loadData path already has, which is the honest bound to state).",
    "Section 4.2's omission row 'delete the capture call -> assertion 1' claims harness coverage the header-only harness cannot provide; A1 is what covers it."
   ],
   "implementation_go": true,
   "summary": "Revision 2 closes all seven blocking findings from the reconciled verdict, and it closes them in the source, not only in prose. I re-opened every load-bearing citation on 3af5ddae (about 33 checks): the capture point (remote_prop_destroy.cpp:73-99, with keyW already a parameter and the actor live through :97), the sole apply-seam call site (prop_drop_intent.cpp:394, confirming revision 2's correction of the verdict's 'two call sites'), the arm point's scope (senderSlot at :378, the if (actor) block at :395-399), the eid's real birthplace (host_spawn_watcher.cpp:352-383 -> prop_lifecycle.cpp:371-373 -> MarkPropElement), the Tick insertion point (subsystems.cpp:547/:548) and the resulting <=250 ms fan-out latency (:515, kSweepMs at :51), the index-0 acceptance in GObjStackSlot (:207-210) that RB-3's new refusals close, the fan-out's g_publishedHash write at :364 that closes RB-2's second consequence, and the client-only nature of both the join bracket (event_feed.cpp:341/:413, sweep host-excluded at :726) and world_load_episode (net_pump.cpp:234, session_runtime.cpp:315-317) that justifies the deliberate substitution of world generation. Reachability is as required: the capture fires on the measured PRIMARY chain (a received PropDestroy at event_dispatch_entity.cpp:254) and on neither seam of the host-authored CONTROL chain, so A2's restated zero-line polarity is correct. The direction is honoured - no transport rewrite, no Relay, no universal serializer, no new kind/op/format, kProtocolVersion untouched on the b150 line - and the persistence result stays bounded as ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE. I found no new blocking defect. One candidate (the author-slot latch surviving OnDestroyImpl_'s early returns and being read by a deferred apply) dissolves on trace: the deferred apply is unreachable on a host, so every host capture is immediately preceded by a fresh latch write; I list the scope guard as an optional hardening. The material corrections needed are strength tags and scoring hygiene, not mechanism: section 0.3 fact 3's causal half is the reporter's inference and the source contradicts it, Q-7's SOURCE_PROVEN rests on the eid free-list rather than the write sites, and 5.4 should name the residue the removed clock leaves. Implementation may proceed with those noted."
  }
 },
 {
  "lens": "r2-data-loss-regression",
  "review": {
   "verdict": "DESIGN_NOT_CLEAN",
   "implementation_go": false,
   "summary": "Revision 2 is a large and mostly accurate advance on draft 1. Under the data-loss/regression lens I re-opened roughly twenty of its citations on 3af5ddae and found none wrong; two corrections revision 2 makes to the verdict itself (HostSpawnPlacedProp has ONE call site at prop_drop_intent.cpp:394, and world_load_episode's raise paths are client-only) are correct, and its RB-6 substitution of ue_wrap::world_identity::Generation() is sound - that token is role-agnostic and its refresh is driven from CurrentWorld/CurrentWorldKind on the game thread, so it can genuinely change on a host, unlike the join bracket. Six of the seven RBs are closed on the merits. The hazards the lens asked for are mostly closed and I could not construct an input that breaks CONTROL_PASS (DestroySeamBody at prop_destroy_seam.cpp only SENDS, and DrainPendingSpawns rather than HostSpawnPlacedProp expresses a host place, so a host-authored chain reaches neither seam), that empties a container the host itself re-places, that lets one client's park be consumed by another (AuthorMismatch), that survives a peer disconnect or a MarkPropElement re-key (author-left / key-rekeyed), that defeats the Player==true refusal (Boundary 1 at both new entry points plus rows 8 and 9), or that changes a wire byte on b150 (no new kind, no new op, neuterNested defaults true and ReadContents has exactly one caller at :334). Three blocking defects remain, all closable in the document and none requiring the host-local approach to be abandoned. (1) The parked record set preserves nested containers' ints[0][0] - raw host GObjStack slot numbers - and re-installs them into live state with none of the ownership proof rows 8-10 apply to the outer slot; with a park lifetime of up to 900 s and Q-4 leaving slot reuse open, a restored outer container can end up sharing an inner slot with a live world container, and the next write through either empties the other. Revision 2 declines this in section 11 on the ground that it needs a reader that does not exist, but the readers required are the ones section 5.1 already adds. (2) A2's restated pass condition, zero [CUSTODY] lines of any kind in the host log, is unsatisfiable on a correct implementation, because section 2.11's own observability table specifies a one-shot [CUSTODY] lane-alive line \"for the Tick\" and section 5.2 registers that Tick unconditionally on the host - the RB-5 failure shape, reintroduced by a different section. (3) Section 2.5 row 10 names SnapshotWorldContainers as the aliasing enumerator without stating a completeness or self-exclusion contract; the shipped function breaks at `want` and reports no truncation, and the freshly adopted container appears in its own snapshot. The persistence wording stays correctly bounded throughout (ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE, twice forbidden from being restated as unrestricted permanent data loss); nothing here restates it otherwise.",
   "blocking_findings": [
    {
     "id": "BF-R2-1",
     "requirement": "Section 2.10 error behaviour ('No path may write a partial record set, write into a slot it could not verify, write into a slot it could not prove exclusively owned...'), section 6 invariant 6 ('never writes into a slot it has not proved is neither index 0, nor the local personal store's, nor another live world container's'), and Boundary 2's shipped rationale.",
     "evidence": "Section 2.3 requires the host-local capture to call ReadContents with neuterNested=false so that a nested container's ints[0][0] survives: verified on 3af5ddae that NeuterNestedIndex is at container_contents_sync.cpp:258-262, called at :285 under RecordIsNestedContainer (:228-233), and that ints[0][0] is 'its own GObjStack index, a slot number in the sender's array' (comment :252-257). Section 2.5's rows 8, 9 and 10 validate ONLY the fresh outer Index; no consume-side row inspects the preserved nested indices, and section 6 invariant 9 states the preservation as a positive rule with no accompanying validation. Section 11 acknowledges the gap ('no rule in this revision validates that a preserved ints[0][0] still names the same inner contents at consume time') and declines it because it 'needs the nested-slot liveness reader that does not exist yet' - but the three readers a check needs are the ones section 5.1 already adds for rows 9 and 10: WorldContainerSlotIndex, SnapshotWorldContainers (container_contents_sync.h:62-63, verified to apply Boundary 1 at :797) and ReadLivePersonalStore (inventory.cpp:138-178, slotIndex verified). Section 5.4's not-covered table has no row for it.",
     "consequence": "The park's lifetime is bounded only by kCustodyParkCeilingSec = 900 s, author-liveness and world generation, and section 7 Q-4 leaves 'reuse a free one' open as a live possibility for propInventory_C::init. Input that breaks it: host container O holds nested container N whose record carries ints[0][0] = 7; client S takes O (host parks O's records, including that raw 7); during the carry another world container is materialised on the host and its fresh component is allocated slot 7 (the slot is addressed by nothing while N is nested, so it is exactly the kind of slot a reuse policy would hand out); S re-places O, every arm and consume check passes, and the record naming slot 7 is written into O's fresh slot by SR::WriteSaveRecord/WriteArrHeader (container_contents_sync.cpp:502-517, verified). Two live addressors now share GObjStack[7]; the next addObject/takeObj through either replaces the other's array (WriteArrHeader at :516 orphans the previous buffer), so a container that today keeps its contents is emptied. Today no live component addresses that record at all, because the record set is never restored - the path is created by the fix. The design's own characterisation in section 11, 'stale, not destroyed, and no worse than the empty box the unfixed build gives', understates it: the loss falls on the third container, not on the restored one.",
     "fix": "At the consume, before the write, apply the same three tests to every preserved ints[0][0] of a record for which RecordIsNestedContainer is true: refuse when it is 0, when it equals ReadLivePersonalStore().slotIndex, or when it equals the Index of any live world container. Refuse the whole apply with a named reason (nested-slot-aliased) rather than writing a partial or repaired set. Cost is one enumeration and at most 512 integer comparisons, once per consume - the same order as row 10. Add the invariant to section 6 alongside invariant 9, a named assertion to section 4.2 next to assertion 22, and the case to section 5.4's not-covered table."
    },
    {
     "id": "BF-R2-2",
     "requirement": "Section 4.1 arm A2 ('PASS = ZERO [CUSTODY] lines of any kind in the host log, AND the drive still present at C5') together with section 4.1's 'A1 without A2 is not a result', and doctrine Step 8 (coop-sync-doctrine.md:120-128, a gate that cannot fire passes forever). This is the property RB-5 exists to secure.",
     "evidence": "Section 2.11's 'first entry' row specifies a one-shot line '[CUSTODY] capture hook ENTERED for the first time on this peer (role=%s)' and, verbatim, 'and the same for the Tick', explicitly modelled on container_contents_sync.cpp:630-639 - which I re-read on 3af5ddae: that line is emitted as 'the first statement, ahead of every filter', precisely so that its absence proves the lane is dead. Section 5.1 and section 5.2 register coop::props::container_custody::Tick(&session) unconditionally in subsystems.cpp's per-tick block between DrainPendingSpawns (verified at :547) and prop_drop_intent::Tick (:548), and the host is the only role that owns a park, so the custody Tick runs and emits its one-shot [CUSTODY] line on the host in every session of a FIXED build - including the A2 CONTROL arm, which is scored by grepping the host log for [CUSTODY].",
     "consequence": "The arm the design makes mandatory cannot be passed by a correct implementation. The two available outcomes are exactly the two RB-5 named: fail a correct fix, or reinterpret the pass condition at scoring time, which destroys the arm's evidentiary value - and the second historically leads to making the seams fire on the host-authored chain, the single change most likely to endanger the working CONTROL_PASS path. A second, independent trigger of the same failure: any client-authored keyed destroy that reaches the host during A2 legitimately produces '[CUSTODY] HOST park REFUSED ... reason=not-container' (section 3 already says so for A1), which also breaks 'zero lines of any kind'.",
     "fix": "State A2's grep exactly: zero [CUSTODY] lines other than the one-shot lane-alive lines - or give those a distinct prefix such as [CUSTODY-INIT] so the A2 grep and the planted-line probe both target the operational prefix only. Require the planted line to be shown matching that exact grep before the zero is scored (the probe-before-claim item (b) then means what it says). Also either declare the client idle for the duration of A2 or scope the grep to the subject key, so an unrelated client destroy cannot fail the control."
    },
    {
     "id": "BF-R2-3",
     "requirement": "Section 2.5 row 10 ('the resolved Index != any other live world container's Index'), stated as fail-closed, plus section 2.10 and section 6 invariant 6.",
     "evidence": "SnapshotWorldContainers, re-read on 3af5ddae at container_contents_sync.cpp:784-800, fills at most `want` entries ('if (n >= want) break;'), returns only the count written, and signals nothing about entries it never reached; the caller cannot distinguish a complete enumeration from a truncated one except by the ambiguous n == want. Section 2.5 row 10 names it as the enumerator, correctly notes that it applies Boundary 1 itself, and states no completeness requirement. It also states no self-exclusion rule, although at consume time the newly adopted container has been through MarkPropElement, is live, passes Boundary 1, and therefore appears in its own snapshot (WorldContainer carries eid and actor, container_contents_sync.h:56-60, so exclusion is possible but must be specified). Section 9 item 2 tells the reviewer to check that row 10 'really uses SnapshotWorldContainers', but not that the enumeration is complete or self-excluding.",
     "consequence": "Two failures in opposite directions, from one unstated contract. Fail-open: in a world holding more live containers than the caller's buffer, an aliasing Index is never compared, row 10 passes, and the parked records are written over another live world container's slot through the EngineAlloc pre-flight / AllocZeroed / WriteArrHeader path (container_contents_sync.cpp:494-517) - destroying contents that survive today, the exact outcome rows 8-10 exist to prevent, reached without tripping any of them. Fail-broken: an implementation that does not exclude the target matches the target against itself, every consume refuses with slot-aliased, and the patch is a logged no-op that still passes all 23 standalone assertions and shows a plausible-looking refusal line in A1.",
     "fix": "State the enumeration contract in section 2.5 row 10: exclude the target by actor pointer or eid from the returned WorldContainer; size the buffer from the registry's prop count; and treat a full buffer (n == want) as an unproven enumeration, refusing with its own named reason rather than passing. Add an assertion in section 4.2 driving OtherWorldContainerIndices as a stub that reports truncation, so both halves turn an assertion red."
    }
   ],
   "rb_closure": [
    {
     "rb": "RB-1",
     "closed": true,
     "note": "Items 1, 2 and 4 delivered literally: capture conditions 3 and 4 park under the dying actor's own wire-normalised key and refuse a mismatch with payload.key (remote_prop_destroy.cpp:123 verified as the wire-payload source, :133-135 eid-first, :150 key fallback); CustodyPark.cls with arm condition 2 and consume row 4; and the MarkPropElement re-key rule with the key-rekeyed refusal (prop_lifecycle.cpp:211-214 and prop_element_tracker.cpp:243-275 re-verified). Item 3's short clock is replaced by a causal predicate - same author slot (latched at event_dispatch_entity.cpp:254, where msg.senderPeerSlot is verified in scope and nowhere downstream), same class, same key, first spawn only, same world generation, author still connected - which is stronger than the verdict's ask on every axis except the window. The kerfur third entry is closed twice over (condition 3, and kerfur_convert_client.cpp:269 verified client-only). Residual, not blocking: the capture cannot distinguish a pickup from a destroy-for-good, so with a 900 s ceiling a hammer break or coin-gun sale can still be resurrected into a colliding-key same-class container placed by the same peer - see optional improvements."
    },
    {
     "rb": "RB-2",
     "closed": true,
     "note": "The three-phase machine puts the consume in the custody module's own Tick, registered between DrainPendingSpawns and prop_drop_intent::Tick - I verified subsystems.cpp:547/:548 and that the eid is minted inside DrainPendingSpawns via ExpressSpawnedProp (host_spawn_watcher.cpp:352-383, host-gated, EidForActor skip at :363), so the precondition genuinely holds at that point. Bounded retry with a named eid-unresolved refusal, a fanout QUEUED observable, and the 250 ms sweep latency stated honestly (container_contents_sync::Tick at subsystems.cpp:515 runs earlier in the same pass; kSweepMs = 250 at :51). The second consequence is closed correctly: g_publishedHash[eid] is set by any publication including a fan-out (verified at container_contents_sync.cpp:364), so the first client write is judged against a real base, which also closes Q-7 at SOURCE_PROVEN. Section 2.0's TRANSITIONS row, the G16 claim, section 5.1's file set and section 5.2's call sites are all rebuilt to the real seam count."
    },
    {
     "rb": "RB-3",
     "closed": true,
     "note": "Rows 8, 9 and 10 added with sources I re-verified: GObjStackSlot refuses only idx < 0 (:207) and idx >= stack.num (:209), so index 0 is accepted; inventory.h's personal-store chain and 'GObjStack[0] IS the player's inventory' hold; ReadLivePersonalStore().slotIndex exists (inventory.cpp:138-178). The resolved index is printed on the park, reattach and apply-refusal lines. Section 2.3's third justification is explicitly retracted, section 9 item 4 is replaced, and Q-4 is moved into the blocking column with a degradation path. Two new defects sit in this area rather than reopening the RB: row 10's enumeration contract (BF-R2-3) and the nested indices, which are slot addresses the apply installs with none of this proof (BF-R2-1)."
    },
    {
     "rb": "RB-4",
     "closed": true,
     "note": "Section 1.3's propInventory_C::init clause is boxed and re-tagged INFERRED with the load-bearing note; section 2.11's apply-refusal row carries the verdict's five required reasons verbatim plus this revision's additions; A1's scoring reads them and section 3 tells the scorer that a not-container refusal under the subject key is expected. The fallback is named and is the same next-tick consume RB-2 installs. Residual, not blocking: the ARM phase this revision introduces has no refusal observable at all, and section 6 invariant 8 was narrowed to 'at both the capture and the apply' - see optional improvements."
    },
    {
     "rb": "RB-5",
     "closed": false,
     "note": "The required change was delivered exactly as written - A2 restated as FIXED build, R3Q CONTROL verbatim, pass = zero [CUSTODY] lines plus drive present at C5, with the planted-line probe - and I independently re-verified the reachability proof beneath it (HostSpawnPlacedProp has ONE call site, prop_drop_intent.cpp:394 inside the host-gated OnPropDropIntent; :427 calls OnPropDropIntent, which is this revision's own correction to the verdict and is right; DestroySeamBody only sends, at prop_destroy_seam.cpp:149). Section 2.3's withdrawn 'both authorship directions' justification and section 4.2 test 2's relabelling are both correct. Marked not-closed only because the restated pass condition is unsatisfiable against section 2.11's own one-shot [CUSTODY] Tick line (BF-R2-2), which reproduces the failure shape RB-5 exists to prevent. One clause fixes it."
    },
    {
     "rb": "RB-6",
     "closed": true,
     "note": "The bracket clause is deleted with a correct host-side proof, and the substitution is sound: I re-read world_identity.h/.cpp and Generation() is role-agnostic, incremented inside RefreshOnGameThread_ whenever the observed CurrentWorld changes, driven from CurrentWorld()/CurrentWorldKind() on the game thread - unlike world_load_episode::InEpisode and InReconcileWindow, whose raise paths I confirmed are client-only, so the verdict's own suggestion would indeed have repeated RB-6. Test 7 is retargeted to three host-valid aging assertions with no bracket reference. The wall clock is removed from the consume predicate and replaced by world generation, author-slot connection (Session::IsSlotConnected exists, session.h:308) and a 900 s leak ceiling; section 5.4 states ten causal limits. The coverage argument for dropping the clock (unmeasured carry duration, section 0.1 and Q-9) is sound on its own; the other justification offered for it is over-tagged - see claims overstated."
    },
    {
     "rb": "RB-7",
     "closed": true,
     "note": "The SlotIo injection genuinely makes the three wrong-write gates assertable - assertion 19 for the Player==true refusal, 20 for the EngineAlloc pre-flight (whose shipped comment at container_contents_sync.cpp:494 I re-read: without it 'an empty array would silently replace real contents'), 21 for rederive-called-once - and 16-18 for the slot refusals. A5 gets a concrete instrument listed in section 5.1, under the already-shipped container_selftest gate, with a read-only accessor and the read-only constraint stated. Section 4.2 closes by declaring what the harness does not prove. Residual, not blocking: two omission rows ('delete the capture call' -> 1, 'delete the author-slot latch' -> 15) still exercise CustodyParkStore directly, so deleting the production call sites leaves the harness green, and the subsystems.cpp Tick registration has no omission row at all - see optional improvements."
    }
   ],
   "optional_improvements": [
    "Arm-phase refusals are silent. AuthorMismatch, ClassMismatch, AlreadyArmed and the arm-time world-generation test all fire in OnPropDropIntent, and section 2.11 has no arm-refusal row; section 6 invariant 8 was narrowed from draft 1's 'Every refusal is logged with a named reason' to 'at both the capture and the apply', which exactly exempts the phase this revision introduced. Add '[CUSTODY] HOST park ARM REFUSED key=... reason=<author-mismatch|class-mismatch|already-armed|key-mismatch|world-changed>' to section 2.11 and to A1's scoring, and restore invariant 8 to every phase. Without it, A1 cannot distinguish an arm refusal from an arm hook that never ran - RB-4's defect, one phase over.",
    "The author-slot latch is specified as 'read-and-cleared by the capture' (section 5.2), but OnDestroyImpl_ returns without reaching DestroyResolvedLocalActor_ on the trash-proxy early-out, on the arbiter-echo short-circuit, and on every unresolved destroy - the last of which also calls quiescence_drain::ArmPendingDestroy (remote_prop_destroy.cpp:183, verified). Clear the latch unconditionally at the end of the PropDestroy dispatch case with a scope guard. This is currently harmless on the host only because the deferred apply is client-driven - quiescence_drain::OnTick's sole driver is join_membership_sweep::TickClientReconcile, called from npc_mirror::TickClientNpcs, which returns early on the host (npc_mirror.cpp:542), and HasLoadTailQuiesced() is set only on that client path - a property section 2.2 neither names nor depends on.",
    "Section 5.4's not-covered table omits the destroy-for-good case. The capture fires on every keyed destroy the host receives, and the send-side seam is general: a hammer break, a coin-gun sale (prop_destroy_seam.cpp:146-147 rides the same DestroySeamBody) and a trash disposal reach it identically to a pickup. With no wall clock, such a park stays consumable for up to 900 s, so the same peer placing a same-class container under a colliding key (section 0.3 measures at least four live actors on one key) resurrects the destroyed container's contents into an unrelated container. Contents are duplicated into a container that is empty today rather than lost, so this is not blocking - but the row belongs in section 5.4, and a cheap 'this key was parked twice with no intervening consume -> refuse both' collision bit would close most of it.",
    "Park replacement can apply the wrong container's contents. Section 2.1's 'latest wins' note - 'the replaced set is dropped, which is today's outcome for both containers, so the replacement is not a regression' - is true of the dropped set but does not cover the surviving set: a client that takes box B (key K) then box A (key K) and places B first gives B the contents of A. No loss relative to today, but the note should say so.",
    "Author binding is by raw peer slot. A disconnect followed by a different player occupying slot S before the sweep observes IsSlotConnected(S) == false leaves the park armable by the new occupant. Bind a connection generation or peer identity alongside the slot, and state the custody sweep's cadence in section 2.3 (the design says 'the next sweep' without saying how often that is).",
    "Three production seams remain harness-unprovable and two of them are claimed as covered. 'Delete the capture call' -> assertion 1 and 'delete the author-slot latch' -> assertion 15 both exercise CustodyParkStore directly, so deleting the call sites in remote_prop_destroy.cpp and event_dispatch_entity.cpp leaves all 23 assertions green; the subsystems.cpp Tick registration has no omission row. Either map these three to A1's parked / ARMED / reattached lines, or fold them into the honest declaration section 4.2 already makes about the production SlotIo binding.",
    "Capture condition 9 measures the wrong pack. The bound exists so that anything parked can be fanned out, but BroadcastContainer re-packs with nested indices neutered, and NeuterNestedIndex resizes an empty ints[] to one element (container_contents_sync.cpp:258-262), so the wire pack can be larger than the host-local one. Measure the size the fan-out would produce, not the parked set's."
   ],
   "citations_checked": [
    {
     "citation": "container_contents_sync.cpp:191-195 IsWorldContainerInventory is Boundary 1, fail-closed on Player != 0 or an unresolvable offset",
     "holds": true,
     "note": "Definition at :191; refuses on an unresolvable Player offset and on Player != 0. Comment at :183-190 carries the GObjStack[0] and ReadLivePersonalStore symmetry as quoted."
    },
    {
     "citation": "container_contents_sync.cpp:200-211 GObjStackSlot refuses only idx < 0 (:207) and idx >= stack.num (:209); index 0 is accepted",
     "holds": true,
     "note": "Verified verbatim. This is the load-bearing fact under section 2.5 rows 8-10."
    },
    {
     "citation": "container_contents_sync.cpp:252-262 Boundary 2 comment and NeuterNestedIndex; ints[0][0] is a slot number in the sender's array",
     "holds": true,
     "note": "Comment and function both present; the loadData / Array_Get zero-fill / propInventory::init reading is quoted accurately."
    },
    {
     "citation": "container_contents_sync.cpp:271-289 ReadContents, the 512 cap at :275-279, the neuter call at :285; exactly one caller",
     "holds": true,
     "note": "git grep ReadContents on 3af5ddae returns the definition at :271 and one call at :334 (BroadcastContainer). The defaulted neuterNested parameter therefore changes no wire behaviour, as claimed."
    },
    {
     "citation": "container_contents_sync.cpp:344-352 the force/g_sentHash gate and the MaxBlobBytes ceiling refusal",
     "holds": true,
     "note": "Both present; the ceiling branch logs and returns true."
    },
    {
     "citation": "container_contents_sync.cpp:359 g_sentHash and :364 g_publishedHash - the Q-7 closure",
     "holds": true,
     "note": "g_publishedHash[eid] = h is set on any publication when IsHost(), fan-out included, so the fan-out does establish the base a later client write is judged against. Q-7's SOURCE_PROVEN closure is correct."
    },
    {
     "citation": "container_contents_sync.cpp:494-500 EngineAlloc pre-flight, :503 AllocZeroed, :516 WriteArrHeader, :518/:520 hashes, :521 RederiveManagedState",
     "holds": true,
     "note": "All present at those offsets; the pre-flight comment reads 'without it an empty array would silently replace real contents' as quoted."
    },
    {
     "citation": "container_contents_sync.cpp:528-558 HostAcceptsClientWrite, the stale-base CAS",
     "holds": true,
     "note": "Definition at :528; an author that never received anything sends 0 and is refused."
    },
    {
     "citation": "container_contents_sync.cpp:630-639 the lane-alive convention",
     "holds": true,
     "note": "Verified, and it is the first statement ahead of every filter. This is what makes A2's 'zero lines of any kind' unsatisfiable once the same convention is applied to the custody Tick (BF-R2-2)."
    },
    {
     "citation": "container_contents_sync.cpp:726 'if (!IsHost()) SweepParked();' - the shipped park sweep is host-excluded",
     "holds": true,
     "note": "Supports the RB-6 argument that importing the bracket reasoning was a gate that cannot fire."
    },
    {
     "citation": "container_contents_sync.cpp:784-800 / .h:62-63 SnapshotWorldContainers applies Boundary 1 itself",
     "holds": true,
     "note": "True, and additionally it breaks at `want` and returns only the count written, with no truncation signal - a property section 2.5 row 10 does not account for (BF-R2-3). WorldContainer carries eid, actor and inv, so self-exclusion is possible but unspecified."
    },
    {
     "citation": "prop_drop_intent.cpp:377-400 OnPropDropIntent: :378 senderSlot, :380 host gate, :383-386 key/class validation, :390 duplicate guard, :394 the ONE HostSpawnPlacedProp call; :231 return actor with no eid minted",
     "holds": true,
     "note": "All verified. Revision 2's correction of the verdict - that :427 calls OnPropDropIntent and names HostSpawnPlacedProp only in a trailing comment - is right; git grep returns the definition at :158 and one call at :394."
    },
    {
     "citation": "remote_prop_destroy.cpp:73-99 DestroyResolvedLocalActor_ with Unpin at :97 and K2_DestroyActor at :98; :123 keyW from the wire payload; :133-135 eid first, :150 key fallback; :190 the only call; :196-198 OnDestroy; :204-207 TryApplyDestroy; no IsHost/role in the file",
     "holds": true,
     "note": "All verified, including the absence of any role accessor, which justifies the session_holder self-gate."
    },
    {
     "citation": "event_dispatch_entity.cpp:254 remote_prop::OnDestroy, with msg.senderPeerSlot in scope there and nowhere downstream",
     "holds": true,
     "note": "Verified: the PropDestroy case runs :215-256 and msg.senderPeerSlot is a field on the message used elsewhere in the same file; PropDestroyPayload carries no sender."
    },
    {
     "citation": "subsystems.cpp:515 container_contents_sync::Tick, :547 DrainPendingSpawns, :548 prop_drop_intent::Tick - the custody Tick insertion point and the resulting one-sweep fan-out latency",
     "holds": true,
     "note": "Ordering verified. The stated consequence - the fan-out ships on the next contents sweep, up to 250 ms later - follows."
    },
    {
     "citation": "host_spawn_watcher.cpp:352-383 DrainPendingSpawns mints the eid via ExpressSpawnedProp; :355 host gate, :360 IsLiveByIndex, :363 EidForActor",
     "holds": true,
     "note": "Verified, including that entries already carrying an eid are skipped, so the mint happens inside ExpressSpawnedProp."
    },
    {
     "citation": "kerfur_convert_client.cpp:269 client-only gate and :310-313 the synthesised PropDestroyPayload with dp.key.len = 0",
     "holds": true,
     "note": "Verified. The third capture entry is unreachable on the host, so capture condition 3 is defence-in-depth as revision 2 states."
    },
    {
     "citation": "world_identity.h:55-58 Generation() is 'bumped every time CurrentWorld is observed to change' and is a host-valid aging token",
     "holds": true,
     "note": "Verified in the implementation: g_generation is incremented inside RefreshOnGameThread_ on a pointer change, driven from CurrentWorld()/CurrentWorldKind() on the game thread with no role term anywhere. The RB-6 substitution is sound, and the rejection of world_load_episode is correct - its raise paths are client-only."
    },
    {
     "citation": "inventory.h / inventory.cpp:138-178 ReadLivePersonalStore, slotIndex 'measured to be 0 by construction', the fail-closed Player==0 refusal",
     "holds": true,
     "note": "Verified, including the header's READ-ONLY BY CONSTRUCTION rule that section 5.1's new accessor is written to preserve."
    },
    {
     "citation": "prop_destroy_seam.cpp: DestroySeamBody only SENDS (SendPropDestroy), the 31-char WireKey truncation, NoteClientKeyedDestroy is Role::Client gated, the coin-gun sale rides the same body",
     "holds": true,
     "note": "All verified. This is what makes the host-authored CONTROL chain unreachable by the capture seam, and also what makes a hammer break or sale reach it identically to a pickup on the client-authored direction."
    },
    {
     "citation": "Section 0.3 fact 3: 'the host's deferred destroy path armed a destroy-by-key at 21:51:10 and applied it at 21:51:16 against a different actor', tagged MEASURED",
     "holds": false,
     "note": "The arming half holds - the report quotes the host log arming a deferred destroy. The 'applied ... against a different actor' half is the report's narrative inference: no '[DESTROY-DEFER] ... applied deferred destroy' host line is quoted, and on 3af5ddae the only drivers of ApplyPendingDestroys are client-gated (npc_mirror.cpp:542), with HasLoadTailQuiesced() set only on that client path, so a host arms deferred destroys and does not apply them. See claims overstated; the decision it supports survives on the independent coverage argument."
    }
   ],
   "claims_overstated": [
    "Section 0.3 fact 3 and its downstream use in section 2.3, section 2.4 and section 11: the six-second interval is presented at STRENGTH=MEASURED and made the deciding fact for removing the wall clock ('a short clock is neither necessary nor sufficient for safety here'). The 'applied at 21:51:16 against a different actor' half is the report's own narrative, not a quoted log line, and on 3af5ddae the deferred apply's only drivers are client-gated. The decision to drop the clock still stands on the independent and sound coverage argument (the reported chain is store-then-re-place with an unmeasured carry duration, section 0.1 and Q-9), so this is a strength-tag defect rather than a wrong conclusion - but it is the programme's own named standing defect class, in the one fact the document leans hardest on.",
    "Section 2.2: 'the very path section 0.3 measures firing against the wrong actor can never fill a park' is true on 3af5ddae, but not for the reason given. The stated reason - the deferred path carries no sender slot, so the capture refuses with author-unknown - depends on the latch being cleared, and the specified discipline ('read-and-cleared by the capture') leaves it set whenever OnDestroyImpl_ returns before DestroyResolvedLocalActor_, which is exactly what the deferring branch does. What actually excludes the path is that the drain is client-driven, which the design does not state.",
    "Section 2.2: 'The class binding and the author binding are what restore it' (the AUTHORITY property draft 1 broke). They narrow it substantially but do not restore it: a client that destroys a container and then places a same-class container under a colliding key still determines which container the host's contents land in, and where.",
    "Section 11's reason for leaving the nested-slot question open - 'it needs the nested-slot liveness reader that does not exist yet, and refusing on it without that reader would refuse every nested case' - does not hold. The readers a refusal needs are WorldContainerSlotIndex, SnapshotWorldContainers and ReadLivePersonalStore, all three already in section 5.1's file set for rows 9 and 10, and the test is per-record rather than all-or-nothing, so it refuses only the aliasing cases (BF-R2-1).",
    "Section 6 invariant 8, 'Every refusal is logged with a named reason, at both the capture and the apply', is narrower than draft 1's 'Every refusal is logged with a named reason', and the narrowing exactly covers the ARM phase this revision introduced, which has no observable in section 2.11. An invariant should not be trimmed to fit an omission.",
    "Section 4.2's 'Seventeen omissions, seventeen named assertions' - fifteen hold; two ('delete the capture call', 'delete the author-slot latch') name assertions that exercise CustodyParkStore directly and would stay green if the production call site were removed. The closing declaration of what the harness does not prove covers the SlotIo binding but not these call sites or the Tick registration."
   ]
  }
 }
]
```
