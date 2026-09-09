# BUG-017 contract design draft — RECONCILED BINDING VERDICT

```
SUBJECT        C:\AgenticStaging\VOTV\Coordination\C1\LANES\bug017-design\BUG017-CONTRACT-DESIGN-DRAFT.md
VERDICT        DESIGN_NOT_CLEAN
IMPLEMENTATION NO — do not start implementation against this revision
INPUTS         two independent reviews: lens=source-correctness, lens=data-loss-and-regression
CODE PIN       3af5ddae (every file:line below re-opened on this ref in this reconciliation)
COMPARE PIN    ba6d8c39 (v0.9.0n-b150-dev)
RECONCILED     2026-09-09, lane C1/bug017-design-review
METHOD         every fact the two reviews disagreed on was re-opened at the cited source line by
               this reconciliation and decided here. Nothing below is carried on a reviewer's word.
```

## 1. What the two reviews agreed on, and what this reconciliation adds

Both reviews return `DESIGN_NOT_CLEAN` and `implementation_go=false`. Both credit the draft with the
same things, and this reconciliation upholds all of them:

- **The reviewed direction is honoured.** No transport rewrite, no Relay artifact, no universal
  serializer, and under the primary mechanism no new `ReliableKind`, no new blob op and no protocol
  bump. §2.7's b150 rule (host-local only, `kProtocolVersion` untouched at 150, `kOpCustody`
  forbidden on that line) is correct and is backed by the shipped pre-join refusals.
- **Code identity across the three pins holds.** Both reviews independently re-measured §0.5 and
  reproduced it.
- **The persistence wording is correct and disciplined.** `ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE` is
  quoted verbatim and twice forbidden from being restated as unrestricted permanent data loss.
- **The sharpest hazard in the design is identified and right.** §2.3's nested-index neuter
  asymmetry (`container_contents_sync.cpp:252-262` vs `:285`) is a real trap and the draft catches
  it. Confirmed here: `ReadContents` neuters only when `RecordIsNestedContainer(r)`, so a host-local
  reuse of the unchanged reader would replace a valid host index with `-1` and orphan every nested
  container's contents on re-attach.
- **The core idea is sound and correctly targeted.** A bounded host-local park, keyed on the logical
  key, captured before `K2_DestroyActor` and consumed at the keyed host spawn, is the right shape
  for the measured chain. None of the findings below require abandoning it or reopening the
  architecture direction.

The two reviews between them raise seven distinct blocking defects. They overlap on three, split on
one severity call, and each found defects the other missed. All seven survive re-verification.

## 2. Disagreements between the two reviews, adjudicated

| # | Point of difference | Decision |
|---|---|---|
| D-1 | The source-correctness lens states in its summary that the mechanism is reachable on the measured R3Q primary chain and does not fail the reachability test; the data-loss lens shows the capture seam is reached for **every** remote keyed container destroy, not only a pickup. | **Both hold; not a conflict.** Re-verified: the seam is reachable on the primary chain (the fix can work) *and* over-reachable (it fires on destroys that are not pickups). The second is the blocking defect — see RB-1. |
| D-2 | Whether the client-only join bracket is blocking. Source-correctness makes it BF-04 (blocking, with an added TTL coverage-bound argument); data-loss files the same fact as an optional improvement. | **BLOCKING (RB-6).** The fact itself is agreed and re-verified. It is blocking because §6 invariant 7 and §4.2 test 7 are normative text that would ship asserting a property the mechanism cannot have on the role that owns the store, and because the residual coverage bound the guard conceals is nowhere declared in §5.4. Closable entirely in the document plus one TTL decision. |
| D-3 | Q-4's status. Source-correctness makes it a detectability problem (a silent no-op is indistinguishable from a dead hook); data-loss makes it a wrong-write problem (an `Index` of 0 aliases `GObjStack[0]`). | **Both, and they are kept separate (RB-3, RB-4).** They share a root — Q-4 is unmeasured and load-bearing — but the failure modes and the required changes differ. §9 item 4's "the design does not depend on it being right" is retracted on both counts. |
| D-4 | §0.5's "Not on the BUG-017 chain" for `prop_destroy_seam.cpp`. Data-loss reads it as contradicting §1.4 step 1; source-correctness does not flag it. | **Not blocking; wording only.** Re-read: the clause's nearest antecedent is the removed dead `SyncDestroyedTrackedProp` function, not the file. The sentence is genuinely ambiguous and §1.4 step 1 plus §8 do cite the file as chain step 1. Filed as an optional clarity fix. |
| D-5 | Citation line drifts. Source-correctness reports three (`:573` omitted from §1.1; doctrine at `:98-99` not `:96-97`; `MULTIVOID-CURRENT-MODEL.md` at `:40` not `:44`); data-loss reports none of them. | **Source-correctness is right on all three; re-verified here.** All are non-load-bearing. Filed as optional. |
| D-6 | Both reviews state the capture seam is entered **only** from a received `PropDestroy`. | **Neither is exactly right.** Re-verified: there is a third entry, `kerfur_convert_client.cpp:313`, a locally synthesised `PropDestroy` with `dp.key.len = 0` and an eid-only teardown. It does not change the A2 conclusion (empty key, client-side prop-mirror teardown), but the draft's §2.10 entry conditions do not list "key non-empty", so it becomes a required refusal — folded into RB-1's fix. |

## 3. Blocking findings — the binding set

Seven. All re-verified at the cited source lines in this reconciliation.

---

### RB-1 — The consume predicate is key-only, on a line where keys are measured non-unique

**Requirement violated.** §2.1 IDENTITY ("one row per actor"), §2.2 AUTHORITY ("a client may cause a
transition; it may never assert container contents"), §6 invariant 5.
*(source: data-loss BF-1; missed by the source-correctness lens.)*

**Evidence, re-verified.** The consume predicate in §2.3 / §2.4 / §2.10 is exactly
{same logical key, actor is a container, Boundary 1 passes, fresh slot reads empty}. There is no
class binding, no generation token and no causal binding to the destroy that filled the park. On the
source:

- `remote_prop_destroy.cpp:123` — `const std::wstring keyW = KeyToWString(payload.key);`
  `:133-135` resolves **eid first**, `:150` is the key fallback. §5.2's insertion parks under the
  **wire payload's** `keyW`, so a resolve-by-eid whose payload key diverges files the records under
  a key that is not the container's.
- `prop_destroy_seam.cpp:154-156` — `NoteClientKeyedDestroy` is gated on `role() == Role::Client`
  and fires for **any** keyed destroy this seam broadcasts. `DestroySeamBody` is the general destroy
  observer (the coin-gun sale rides the same body at `:146-147`), so a hammer break, a sale or a
  trash disposal reaches the host's capture point identically to a pickup.
- `prop_drop_intent.cpp:377-400` — `OnPropDropIntent` validates only non-empty key/class (`:383`)
  and the live-key duplicate guard (`:390`). **No class whitelist.** Only `OnReelEjectIntent`
  whitelists (`:409-421`).
- `prop_lifecycle.cpp:212-214` — *"Mark may re-key a duplicate (the host is the key authority)"*,
  and that re-key runs in the next-tick express drain, **after** the draft's apply point.
- Measured non-uniqueness on this line:
  `C:\AgenticStaging\BugDropbox\Doctaaaa's reports\Report 3\bug_03_floppy_dissolves_deferred_destroy_key_collision.md:14,57,58`
  — key `0zlKei78xf4Ek_bPrYWV0A` carried by ≥4 distinct live client actors plus one host actor, and
  *"All destroy traffic in this incident carries `eid=0`"*. Same pattern recorded on two further
  keys.
- Third capture entry, found in this reconciliation: `kerfur_convert_client.cpp:310-313` synthesises
  a `PropDestroyPayload` with `dp.key.len = 0` and calls `remote_prop::OnDestroy` locally.

**Consequence.** Two corruption paths that do not exist today. (1) **Resurrection.** A container
destroyed for good parks its contents; any keyed host container spawn under a colliding or reused
key within the 60 s TTL receives them — today that container is simply empty, and the fresh-empty
check waves the write through precisely because the new container *is* empty. (2) **Client-directed
relocation.** A client broadcasts `DESTROY(K, eid=0)`, the host parks container A's contents by
key-fallback resolution, and the client then authors a drop intent under `K` with a container class
and transform of its own choosing. That is the client asserting where a host container's contents
land, which §2.2 forbids in as many words.

**Required change.**
1. Bind the park to the **dying actor**, not the wire payload: read
   `ue_wrap::prop::GetInteractableKeyString(actor)` at the capture point and refuse to park on an
   empty key or on any mismatch with `payload.key`. Add "key non-empty" to §2.10's entry conditions
   (this also closes the `kerfur_convert_client.cpp:313` entry).
2. Record the dying actor's class in `CustodyPark` and refuse the consume unless the spawning class
   equals it.
3. Gate the consume on a single-generation token: consumable only by the first keyed host container
   spawn following that destroy, authored by the same peer slot that authored the destroy, within a
   window materially shorter than 60 s.
4. State in §2.4 what happens when `MarkPropElement` re-keys the container on the next tick after a
   consume, and add the corresponding refusal or reconciliation rule.

---

### RB-2 — There is no element id at the specified apply point, so the mandatory fan-out cannot fire

**Requirement violated.** §2.0's TRANSITIONS row and §2.4's own stated postcondition: *"APPLIED must
be followed, on the same tick or the next contents sweep, by the host's ordinary `ContainerContents`
fan-out for the new eid"*, implemented per §5.1/§5.2 by `MarkDirty(uint32_t eid)`.
*(both lenses: source-correctness BF-02, data-loss BF-3. Identical finding.)*

**Evidence, re-verified.** `HostSpawnPlacedProp` read in full at `prop_drop_intent.cpp:158-232`: the
body ends at `return actor` (`:231`) with no element minted. `:155-157` states the design intent —
*"does not mark an incoming spawn, so the host's own finish-spawn watcher catches it"*.
`host_spawn_watcher.h:29-32` — the `FinishSpawningActor` callback *"only ENQUEUES, and
DrainPendingSpawns adopts on the next tick"*. `prop_drop_intent.cpp:223-225` — *"the next-tick
express drain re-reads the live actor"*. The eid is minted inside that drain, at
`prop_lifecycle.cpp:371-373` → the Init-POST funnel → `MarkPropElement` (`:214`). `DrainDirty`
(`container_contents_sync.cpp:400-421`) is edge-driven from `g_dirty`, written only at
`OnVerbEntry:654`.

**Consequence.** `MarkDirty` has no valid eid to insert; no fan-out occurs. The host's box is
restored while every peer — including the placing client, whose own local box was also re-created
empty — keeps showing an empty box until an unrelated `addObject`/`takeObj` edge or a rejoin. That
is a host/client divergence **the unfixed build does not have** (today both sides agree the box is
empty), and the design's own arms cannot see it: A1–A3 are host-scored per §3 and A4 is
`BLOCKED_ON_OBS_R3Q_C01`. Additionally `g_publishedHash` gains no entry for the new eid, so the
first client-authored write against that container is refused as a stale base by
`HostAcceptsClientWrite` (`:528-558`) and host truth is re-published over that player's action.
§2.4's reviewer check ("confirm the three hash maps carry no stale entry for a freshly minted eid")
passes but is the wrong question.

**Required change.** Move the consume — or at minimum the `MarkDirty` — to the point where the eid
exists: the next-tick adoption (`host_spawn_watcher::DrainPendingSpawns` /
`prop_lifecycle::ExpressSpawnedProp`), keyed by the actor ref or the logical key, with a bounded
retry and a named refusal log if the eid never resolves. Then update §5.1's file set, §5.2's call
sites, §2.0's TRANSITIONS row and the §2.0 G16 one-switch-backout claim to include that third seam
(or the module `Tick` registration in `subsystems.cpp`), and add a host-side observable naming the
fan-out (key, new eid, record count) so the postcondition is scorable without a client arm.

---

### RB-3 — The apply writes into a slot it has not proved is exclusively owned

**Requirement violated.** §6 invariants 2 and 4, and §2.5's fail-closed validation table.
*(source: data-loss BF-2.)*

**Evidence, re-verified.** §2.3 recommends writing *"into the slot the host just resolved for its own
fresh component"*. `IsWorldContainerInventory` (`container_contents_sync.cpp:191-195`) reads the
`Player` byte off the **component**; it says nothing about which slot that component's `Index`
names. `GObjStackSlot` (`:200-211`) refuses only `idx < 0` (`:207`) and `idx >= stack.num` (`:209`)
— **index 0 is accepted.** Boundary 1's own comment (`:183-186`) states *"GObjStack[0] is the local
player's inventory by construction"*, and Boundary 2's (`:252-257`) states the exact failure mode:
*"an empty array yields index 0, which propInventory::init's `index >= 0` guard passes, and the
container reuses GObjStack[0], a slot owned by someone else"*. §1.4 step 5 (the fresh component
allocates its own slot) is `INFERRED` from that same in-tree bytecode-derived comment and is not
re-measured; §7 Q-4 then routes the question away from the patch and §9 item 4 argues the dependency
away.

**Consequence.** If `propInventory_C::init` leaves `Index` at 0, or at a slot another live component
owns, the parked record set is written over that slot through
`SR::AllocZeroed`/`WriteSaveRecord`/`WriteArrHeader` (`:502-517`). When the aliased slot is
`GObjStack[0]` and the host player happens to be carrying nothing, the fresh-empty check passes and
the host player's own personal inventory is silently replaced — the exact outcome Boundary 1 exists
to prevent, reached without ever tripping Boundary 1. When it aliases another empty world container,
two containers share one array and each write clobbers the other. Neither failure exists today.

**Required change.** Add slot-ownership rows to §2.5, all cheap: refuse the apply when the resolved
`Index` is 0; refuse when the resolved slot pointer equals the local personal-store slot
(`ue_wrap::inventory::ReadLivePersonalStore` carries the address assertion in the other direction);
refuse when it equals the `GObjStackSlot` of any other live world container
(`SnapshotWorldContainers`, `container_contents_sync.h:56-68`, is the shipped enumerator — note it
applies Boundary 1, so it enumerates world containers only). Print the resolved `Index` on the
`[CUSTODY] ... reattached` line. Retract §9 item 4 and move Q-4 into the blocking column for the
apply strategy.

---

### RB-4 — The apply's own precondition may refuse silently, and there is no refusal observable

**Requirement violated.** §6 invariant 8 (*"Every refusal is logged with a named reason. A silent
refusal is a defect"*) and the programme's probe-before-claim rule.
*(source: source-correctness BF-03. Same root as RB-3, different failure mode and different fix —
kept separate so neither change is dropped.)*

**Evidence, re-verified.** The apply's first precondition is that the fresh container's slot
resolves. `GObjStackSlot` returns null whenever `Index < 0` (`:206-207`, comment *"-1 = never
initialised; nothing to ship or apply"*). Whether a freshly finished `prop_container_C` has an
allocated `Index` at `prop_drop_intent.cpp:231` is exactly Q-4, which §7 tags `UNKNOWN` and routes
to *"the strength tag on §1.4 step 5; not the patch"*. §1.3 nevertheless closes with *"the fresh
`propInventory_C::init` runs inside the finish spawn with the CDO's default index"* under a
`STRENGTH=SOURCE_PROVEN` heading. §2.11's observability table has rows for park taken / park refused
/ apply / discard / expire / evict / first entry — **no row for an apply-side refusal.**

**Consequence.** If `init` defers slot allocation until the first `addObject`, `GObjStackSlot`
returns null at the apply point, every apply refuses, and the patch is a silent no-op that still
passes every §4.2 standalone test (they stub the engine). With no apply-refusal log row, arm A1
cannot distinguish "no slot yet" from "the apply hook never ran" — the probe-narrower-than-the-claim
defect class the programme has already recorded against itself.

**Required change.** Re-tag §1.3's closing clause `INFERRED`. Add
`[CUSTODY] HOST reattach REFUSED key='%ls' reason=<not-container|boundary1|slot-unresolved|slot-aliased|alloc-preflight>`
to §2.11 and to A1's scoring. Name the fallback — which is the same next-tick consume RB-2 already
requires, where `propInventory_C::init` has demonstrably run because the express drain reads the
actor successfully.

---

### RB-5 — Acceptance arm A2 asserts an observable a correct implementation cannot produce

**Requirement violated.** §4.1's own rule *"A1 without A2 is not a result"*, plus doctrine Step 8
(`coop-sync-doctrine.md:120-128`, *"Every detector and gate is shown red before it is trusted,
because a gate that cannot fire passes forever"*).
*(both lenses: source-correctness BF-01, data-loss BF-4. Identical finding.)*

**Evidence, re-verified.** Neither seam is reachable on a wholly host-authored grab/inventory/place
chain.

- **Apply seam.** `git grep HostSpawnPlacedProp 3af5ddae -- src/votv-coop` returns the definition at
  `prop_drop_intent.cpp:158` and exactly two call sites: `:394` inside `OnPropDropIntent` (role-gated
  to Host at `:380`, entered only from `event_dispatch_intent.cpp:258-280`, which is explicitly
  `CLIENT->HOST` and drops on a client at `:263-266`) and `:427` via `OnReelEjectIntent`
  (`event_dispatch_intent.cpp:281-302`, same shape). A host's own place is expressed by the
  `FinishSpawningActor` seam → `DrainPendingSpawns` → `ExpressSpawnedProp`
  (`prop_lifecycle.cpp:371-373`), never by `HostSpawnPlacedProp`.
- **Capture seam.** `DestroyResolvedLocalActor_` (`remote_prop_destroy.cpp:73-99`) is
  anonymous-namespace and called only from `OnDestroyImpl_:190`, entered from
  `remote_prop::OnDestroy` (`:196-198`, called at `event_dispatch_entity.cpp:254` on a **received**
  `PropDestroy`), from `TryApplyDestroy` (`:204-207`, from `quiescence_drain.cpp:249`, also a
  received destroy), and — found in this reconciliation — from `kerfur_convert_client.cpp:313` with
  an **empty key**. A host's own pickup runs `DestroySeamBody`, which only sends
  (`prop_destroy_seam.cpp:149`); the host never receives its own destroy.

**Consequence.** On a fixed build the R3Q CONTROL chain emits **zero** `[CUSTODY]` lines. A2's stated
pass condition — *"discard line present (the native path still wins)"* — is unsatisfiable, so the arm
the design makes mandatory either fails a correct fix or is reinterpreted at scoring time, destroying
its evidentiary value; the second is worse, because making the seams fire on host-authored chains is
the single change most likely to endanger the working `CONTROL_PASS` path. The same error propagates
into §2.3's justification for the fresh-empty refusal (*"Without this rule the fix would double-apply
on the host-authored chain and could revert a newer host state"* and *"This rule is what makes the
mechanism safe to run on both authorship directions with one code path"*): the mechanism never runs
on the host-authored direction, so neither sentence is true. §4.2 test 2's gloss ("the assertion A2
exists to confirm in the world") is likewise unbacked.

**Required change.** Restate A2 as a true must-fire negative control: FIXED build, R3Q CONTROL chain
verbatim, pass = **zero** `[CUSTODY]` lines of any kind in the host log **and** the drive still
present at C5 — with the grep shown matching a planted `[CUSTODY]` line first, per §4.1's own
probe-before-claim rule. Keep the fresh-slot-non-empty refusal as defence-in-depth and re-justify it
against cases that can actually reach it (a duplicated or re-delivered intent; a future native
restore inside `FinishDeferredSpawn`). Keep §4.2 test 2 as the pure-logic assertion it already is,
and either name a real chain that exercises the DISCARDED branch in the world or declare that branch
unit-tested only, in §4.1.

---

### RB-6 — The TTL's join-bracket guard is dead on the host, and the real coverage bound is undeclared

**Requirement violated.** §6 invariant 7 (*"a 60 s TTL that does not run inside a join bracket"*) and
§4.2 test 7 (*"does not expire while the bracket is open"*), justified in §2.3 as reusing
*"NoteJoinSnapshotBracket's already-shipped reasoning"*.
*(source-correctness BF-04, blocking; data-loss filed the same fact as optional. Adjudicated
BLOCKING — see D-2.)*

**Evidence, re-verified.** `NoteJoinSnapshotBracket` is client-only. Its own definition comment
(`container_contents_sync.cpp:666`) reads *"From event_feed's client-side SnapshotBegin and
SnapshotComplete dispatch"*; `git grep` returns exactly two call sites, `event_feed.cpp:361` and
`:424`, and the `SnapshotBegin` handler returns early on the host at `:341`
(`if (session.role() == net::Role::Host) break;  // self-echo guard (host doesn't load-screen)`).
`g_joinBracketOpen` is therefore always false on the host — the only role that owns a custody park,
since §2.3's capture is host-role-gated.

**Consequence.** The aging guard the design imports is dead on the role that holds the store, so
invariant 7's bracket clause is vacuous and §4.2 test 7's second half asserts a state the shipped
mechanism cannot enter — a gate that cannot fire, and a test that mirrors the implementation rather
than controlling it. Worse, the guard's presence conceals the real coverage bound: with a plain 60 s
wall-clock TTL the fix restores contents only when the client re-places within one minute. The
reported chain is store-then-re-place (BUG-3 / R3Q P2 is *"the box goes into the client's personal
store"*), where an arbitrary carry duration is ordinary play. The design silently covers only a
short-carry subset of the very defect it targets, and §5.4's non-coverage list does not say so.

**Required change.** Delete the bracket clause (or replace it with a host-side condition that can
actually be true) and retarget §4.2 test 7. Then either raise and justify the TTL against a measured
carry duration, or anchor expiry to an event rather than wall time (e.g. hold the park until the peer
that authored the destroy disconnects, bounded by the park cap) — noting that RB-1 pulls in the
opposite direction and the two must be resolved together. State the residual coverage bound
explicitly in §5.4 alongside the other named non-coverages. Additionally add an absolute age ceiling
independent of any bracket and clear parks on a host world-load episode / reconcile window
(`coop::world_load_episode::InEpisode` / `InReconcileWindow`, the gates already used at
`prop_destroy_seam.cpp:84-107`); session teardown is already covered by
`container_contents_sync::OnDisconnect`, but a mid-session world reload is not.

---

### RB-7 — Four of the ten omission cases cannot turn any named assertion red

**Requirement violated.** §4.2's closing discipline: *"Each must turn at least one named assertion
red."*
*(source: data-loss BF-5.)*

**Evidence, re-verified.** The proposed harness is, per §4.2 and §5.1, a **header-only, engine-free**
`CustodyParkStore` plus `save_record_wire` codec controls; §5.1 says of `container_custody.h`
"No engine types, so it compiles in the test harness", and puts the two engine-touching seams in
`container_custody.cpp`. Assertions 1–12 are entirely about the store's FIFO/map/TTL/hash behaviour
and the `SerSave`/`DeSave` neuter flag. Four omission cases — *delete the capture call*, *park on a
`Player=true` inventory*, *apply without the allocation pre-flight*, *apply without
`RederiveManagedState`* — live only in `container_custody.cpp` and the
`container_contents_sync.h` wrappers, and no assertion 1–12 names them. A5 nominally covers the
Player refusal but is written as *"or plant a `Player=true` component in the dev instrument"* while
§5.1's complete file list adds no dev-instrument change; the shipped enumerator
`SnapshotWorldContainers` (`container_contents_sync.h:62-63`) applies Boundary 1 itself, so it cannot
produce one.

**Consequence.** Two of the four unproven gates are the ones that prevent a wrong write: the
`Player == true` refusal at the **new** entry point (whose violation destroys a player's inventory,
per `container_contents_sync.cpp:183-195`) and the `EngineAlloc` pre-flight, whose shipped comment
at `:495-500` states that without it an empty array would silently replace real contents. The draft
would ship them in the state its own A5 rationale condemns — *"a boundary that has never been seen
refusing is a boundary nobody has tested"* — while claiming the omission harness covers them.

**Required change.** Give the two engine-touching seams a thin injectable slot-reader / slot-writer
pair the harness can stub, so Player-refusal, pre-flight-refusal and rederive-called become named
assertions in `candidate_17`; and give A5 a concrete instrument, listed in §5.1. If neither is
affordable, delete those four rows from the omission list and state explicitly, in §4.2, that they
are unproven.

---

## 4. The minimal change list before implementation

Grouped by where the change lands. Nothing here requires abandoning the host-local approach.

**Changes to the mechanism (RB-1, RB-2, RB-3):**

1. Park under the **dying actor's own key**, refuse on an empty key or on a mismatch with
   `payload.key`; add "key non-empty" to §2.10's entry conditions.
2. Record the dying actor's **class** in `CustodyPark`; refuse the consume on a class mismatch.
3. Add a **single-generation / same-authoring-peer / short-window** binding to the consume; state
   the `MarkPropElement` re-key interaction.
4. Move the consume (or at minimum the `MarkDirty`) to the **next-tick adoption** where the eid
   exists; add a bounded retry and a named refusal if it never resolves.
5. Add **slot-ownership** rows to §2.5: refuse `Index == 0`; refuse a slot equal to the local
   personal-store slot; refuse a slot equal to any other live world container's slot.

**Changes to the acceptance set (RB-5, RB-6, RB-7):**

6. Restate **A2** as zero `[CUSTODY]` lines plus drive-present, with the detector shown TRUE on a
   planted line first.
7. Delete or replace the **join-bracket clause**; retarget §4.2 test 7; decide the TTL against a
   measured carry duration or an event anchor; add an absolute age ceiling and a world-load-episode
   clear.
8. Either make the four engine-touching **omission cases** assertable (injectable slot reader/writer
   + a dev instrument listed in §5.1) or delete them from the omission list and declare them
   unproven.

**Changes to the document's claims and observability (RB-4, plus the overstatements in §5):**

9. Add an **apply-refusal log row** to §2.11 with named reasons, and to A1's scoring; print the
   resolved `Index` on both the park and the reattach lines.
10. Re-tag §1.3's `propInventory_C::init` clause `INFERRED`; move **Q-4** into the blocking column
    for the apply strategy; retract §9 item 4.
11. Correct §2.3's two sentences about running "on both authorship directions with one code path"
    and about double-applying on the host-authored chain, and §4.2 test 2's gloss.
12. Update §2.0's TRANSITIONS row, §2.0's G16 backout claim, §5.1's file set and §5.2's call sites
    to the real seam count; reword §2.6/§5.4's "no wire change at all" / "zero bytes" to "no new
    kind, no new op, no format or size change" (a restored container is marked dirty and fanned out
    where today it is silent — wire-format compatible with b150, but not byte-identical traffic).
13. State the residual coverage bound (short-carry only, or whatever the TTL decision yields) in
    §5.4.

Re-review after these. Implementation is a reasonable next step once the re-review passes.

## 5. Optional improvements (not blocking)

Carried from both reviews, de-duplicated; the ones this reconciliation re-verified are marked.

- **[verified]** §1.1's Boundary 1 site list omits `container_contents_sync.cpp:573` (the
  refuse-and-re-publish branch inside `ParseAndApply`). Six non-definition sites, not five:
  `:418, :486, :573, :776, :797, :809`. The list reads as exhaustive.
- **[verified]** `remote_prop_destroy.cpp` has **no** `IsHost()` / `role()` accessor at all (grep
  returns nothing), so §5.2's `if (IsHost()) …CaptureForDyingContainer(…)` cannot be written there
  as three self-contained lines. Have the custody module hold its own session pointer (the shipped
  `session_holder` pattern, `prop_drop_intent.cpp:46-47`) and self-gate, so the call site is one
  unconditional call.
- **[verified]** §2.6's `kOpCustody` sketch has no size bound. `BroadcastContainer` refuses an
  over-ceiling blob at `container_contents_sync.cpp:348-352` with a named warning; a custody blob
  needs the same check against `blob_chunks::MaxBlobBytes()` (56,100 B) before `ChunkAndSend`, since
  512 variable-length records can exceed it.
- **[verified]** Citation drifts, none load-bearing: the doctrine actor-gap sentence is at
  `coop-sync-doctrine.md:98-99`, not `:96-97`; `MULTIVOID-CURRENT-MODEL.md`'s `SOURCE_PROVEN`
  identity-domains claim is at `:40`, not `:44`.
- §5.2's `file:line` anchors are 3af5ddae-only and drift materially on the b150 line that §2.7 says
  the same patch text applies to (reported: `DestroyResolvedLocalActor_` at `remote_prop_destroy.cpp:80`,
  `IsWorldContainerInventory` at `:234`, `NeuterNestedIndex` at `:311`, `ReadContents` at `:324`,
  `ApplyContents` at `:549`, `OnVerbEntry` at `:722`). Give §5.3 step 5 **symbol/statement anchors**
  rather than line anchors — §5.2 already does this in prose.
- Say explicitly that the park key must be the **wire-normalised** key on both sides (both senders
  already truncate to 31 chars: `prop_destroy_seam.cpp:119-121` and `FillWireStr`'s N-1 cap). §2.3
  describes the field only as "the logical key (WireKey / std::wstring)", which invites an
  implementer to read the untruncated key off the dying actor and silently break the match. Note this
  interacts with RB-1's fix: read the actor's key, then normalise it the same way before comparing.
- Have the park line carry the orphaned slot's index and record count (`parkedIndex` is already in
  the struct) and the apply line carry the new slot index, so the bounded `GObjStack` probe of
  Q-2/Q-4 can be scored from the same host log without building a second instrument. This is also the
  cheapest detector for RB-3's aliasing case.
- State what happens to a **nested** container's own slot: destroying the outer container does not
  destroy the inner container's `GObjStack` slot, and re-attaching the outer record set re-points at
  inner slots whose contents may have been rewritten in the interval. Add the invariant, and an
  assertion that a parked outer set whose preserved `ints[0][0]` no longer resolves is refused rather
  than written.
- Add a host-scored substitute for the C01-blocked A4: with the dev instrument on, print
  `ContentsDigest(eid)` on both roles (`container_contents_sync.cpp:803-815`, already named in
  §2.11) and compare counts, so client convergence gets an observable that does not depend on the
  client's mirror surviving OBS-R3Q-C01.
- §0.5's *"Not on the BUG-017 chain"* clause reads ambiguously against §1.4 step 1 and §8, which
  cite `prop_destroy_seam.cpp:128-149` and `:154-156` as chain step 1. Reword so the antecedent (the
  removed dead `SyncDestroyedTrackedProp` function) is unambiguous. The identity conclusion it
  supports is correct.

## 6. Claims in the draft that are overstated, consolidated

1. §1.3's *"the fresh `propInventory_C::init` runs inside the finish spawn with the CDO's default
   index"* under a `SOURCE_PROVEN` heading — the same Blueprint-bytecode inference §1.4 step 5 tags
   `INFERRED` and §7 Q-4 tags `UNKNOWN`. Load-bearing, not cosmetic (RB-3, RB-4), and precisely the
   defect class §9.4 names as the programme's standing one.
2. §2.3's *"safe to run on both authorship directions with one code path"* and *"Without this rule
   the fix would double-apply on the host-authored chain"* — the mechanism never runs on the
   host-authored direction (RB-5).
3. §2.0's TRANSITIONS conformance row — the stated seam cannot obtain the eid its postcondition
   names (RB-2).
4. §2.0 (G16) and §5.1's *"Two production files change by three lines each"* / *"the whole mechanism
   is two three-line call sites"* — satisfying RB-2, RB-3 and RB-4 requires a third seam or a module
   `Tick` registration, so the file-set table and the one-switch-backout claim are both understated.
5. §4.1 A2's named observable and pass condition — assert an outcome a correct fix cannot produce
   (RB-5); §4.2 test 2's gloss inherits it.
6. §6 invariant 7's bracket clause and §4.2 test 7's second half — vacuous on the host (RB-6).
7. §7 Q-4's *"Blocks: the strength tag on §1.4 step 5; not the patch"* and §9 item 4's *"The design
   does not depend on it being right"* — writing into whatever slot the fresh component resolved is
   exactly what makes the answer load-bearing (RB-3, RB-4).
8. §2.6's *"no wire change at all"* and §2.7's *"It adds zero bytes"* — format, kinds and ops are
   unchanged and b150 interop is preserved, but a restored container is marked dirty and fanned out
   as traffic that is not sent today. State it as format compatibility.
9. §4.2's *"Each must turn at least one named assertion red"* for the ten omission cases — four
   cannot, in the engine-free harness the same section specifies (RB-7).
10. §1.1's Boundary 1 site list reads as exhaustive but omits `:573`.

---

## 7. Raw reviews, verbatim

The two input reviews, unmodified, as received.

```json
[
 {
  "lens": "source-correctness",
  "review": {
   "verdict": "DESIGN_NOT_CLEAN",
   "blocking_findings": [
    {
     "id": "BF-01",
     "requirement": "§4.1's own rule \"A1 without A2 is not a result\", plus doctrine Step 8 (coop-sync-doctrine.md:120-128, \"Every detector and gate is shown red before it is trusted, because a gate that cannot fire passes forever\"). A2 is declared the mandatory control-regression arm.",
     "evidence": "Neither proposed seam is reachable on a host-authored grab/inventory/place chain. Apply seam: HostSpawnPlacedProp has exactly two call paths on 3af5ddae — prop_drop_intent.cpp:394 inside OnPropDropIntent (role-gated to Host at :380, entered only from event_dispatch_intent.cpp:258-280 on a CLIENT message) and :427 via OnReelEjectIntent (event_dispatch_intent.cpp:281-302, also CLIENT->HOST). A host's own place is expressed by the FinishSpawningActor seam -> host_spawn_watcher::DrainPendingSpawns -> prop_lifecycle::ExpressSpawnedProp (prop_lifecycle.cpp:371-373), never by HostSpawnPlacedProp. Capture seam: DestroyResolvedLocalActor_ (remote_prop_destroy.cpp:73-99) is anonymous-namespace and called only from OnDestroyImpl_:190, entered only from remote_prop::OnDestroy (:196-198, called at event_dispatch_entity.cpp:254 on a RECEIVED PropDestroy) and TryApplyDestroy (:204-207, from quiescence_drain.cpp:249, also a received destroy). A host's own pickup runs DestroySeamBody (prop_destroy_seam.cpp:36-157), which only SENDS; the host never receives its own destroy.",
     "consequence": "On a fixed build the R3Q CONTROL chain emits ZERO [CUSTODY] lines. A2's stated pass condition — \"[CUSTODY] HOST park DISCARDED ... discard line present (the native path still wins)\" — is unsatisfiable, so the arm the design makes mandatory either fails a correct fix or is reinterpreted at scoring time, destroying its evidentiary value. The same error propagates into §2.3's justification for the fresh-empty refusal (\"Without this rule the fix would double-apply on the host-authored chain and could revert a newer host state\") and into \"This rule is what makes the mechanism safe to run on both authorship directions with one code path\": the mechanism never runs on the host-authored direction, so neither statement is true, and §4.2 test 2's claim to be \"the assertion A2 exists to confirm in the world\" is unbacked.",
     "fix": "Restate A2 as a true negative control: FIXED build, R3Q CONTROL chain verbatim, pass = zero [CUSTODY] lines of any kind on the host AND the drive still present at C5. Keep the fresh-slot-non-empty refusal as defence-in-depth and re-justify it against the cases that can actually reach it (a duplicated/re-delivered intent, a future native restore inside FinishDeferredSpawn), and keep §4.2 test 2 as the pure-logic assertion it already is."
    },
    {
     "id": "BF-02",
     "requirement": "§2.0's TRANSITIONS conformance row (MULTIVOID-CURRENT-MODEL.md:34, \"Multi-expression state changes need ordered postconditions\") and §2.4's own stated postcondition: \"APPLIED must be followed, on the same tick or the next contents sweep, by the host's ordinary ContainerContents fan-out for the new eid\", implemented per §5.1/§5.2 by a MarkDirty(uint32_t eid) wrapper called at the apply point.",
     "evidence": "At the stated apply point (prop_drop_intent.cpp, after :230 and before `return actor` at :231) the freshly spawned actor has no element id yet. host_spawn_watcher.h:29-32 states the FinishSpawningActor callback \"only ENQUEUES, and DrainPendingSpawns adopts on the next tick\"; subsystems.cpp:547 repeats \"one tick after Finish\"; prop_drop_intent.cpp:223-225 says \"the next-tick express drain re-reads the live actor\"; the eid is minted only inside that express body (prop_lifecycle.cpp:371-373 -> the Init-POST funnel at :178). DrainDirty (container_contents_sync.cpp:400-421) is purely edge-driven from g_dirty, written only at OnVerbEntry:654, and resolves forward from the eid at :415.",
     "consequence": "MarkDirty has no valid eid to insert, so no ContainerContents fan-out occurs. The host's box is restored while every peer — including the placing client, whose own local box was also re-created with a fresh empty slot — keeps showing an empty box until an unrelated addObject/takeObj edge or a rejoin. That is a host/client divergence the UNFIXED build does not have (today both sides agree the box is empty), and the design's own arms cannot see it: A1-A3 are scored on the HOST only per §3, and A4 is BLOCKED_ON_OBS_R3Q_C01. Consequently §2.0's TRANSITIONS row and §5.2's \"the whole mechanism is two three-line call sites\" (also the §2.0 G16 one-switch-backout claim) are both unmet as written.",
     "fix": "Move the fan-out trigger — and preferably the whole consume — to the point where the eid exists: the next-tick adoption (host_spawn_watcher::DrainPendingSpawns / prop_lifecycle::ExpressSpawnedProp), keyed by the actor ref or the logical key, e.g. arm at :231 and consume in the custody module's own Tick after EidForActor resolves. Update §5.1's file set, §5.2's call sites and the backout claim to include that third seam (or the module Tick registration in subsystems.cpp), and add a host-side observable naming the fan-out (key, new eid, record count) so the postcondition is scorable without a client arm."
    },
    {
     "id": "BF-03",
     "requirement": "§2.5's fail-closed validation table plus §6 invariant 8 (\"Every refusal is logged with a named reason. A silent refusal is a defect\"), and the programme's probe-before-claim rule that a detector must be shown TRUE on a known-true input before its FALSE is believed.",
     "evidence": "The apply's first precondition is that the fresh container's slot resolves. GObjStackSlot returns null whenever Index < 0 (container_contents_sync.cpp:205-207, comment \"-1 = never initialised; nothing to ship or apply\"). Whether a freshly finished prop_container_C has an allocated Index at prop_drop_intent.cpp:231 is exactly Q-4, which §7 tags UNKNOWN and routes to \"the strength tag on §1.4 step 5; not the patch\"; its only support is the in-tree bytecode-derived comment at container_contents_sync.cpp:252-257. §1.3 nevertheless closes with \"the fresh propInventory_C::init runs inside the finish spawn with the CDO's default index\" under a STRENGTH=SOURCE_PROVEN heading. §2.11's observability table has rows for park taken / park refused / apply / discard / expire / evict / first entry, but no row for an apply-side refusal.",
     "consequence": "If init defers slot allocation until the first addObject, GObjStackSlot returns null at :231, every apply refuses, and the patch is a silent no-op that still passes every §4.2 standalone test (they stub the engine). With no apply-refusal log row, arm A1 cannot distinguish \"no slot yet at :231\" from \"the apply hook never ran\" — the probe-narrower-than-the-claim defect class the programme has already recorded against itself.",
     "fix": "Re-tag §1.3's closing clause INFERRED; move Q-4 into the blocking column for the apply strategy (not merely a strength tag); add `[CUSTODY] HOST reattach REFUSED key='%ls' reason=<not-container|boundary1|slot-unresolved|alloc-preflight>` to §2.11 and to the A1 scoring; and name the fallback — which is the same next-tick consume BF-02 already requires, where propInventory_C::init has demonstrably run because the express drain reads the actor successfully."
    },
    {
     "id": "BF-04",
     "requirement": "§6 invariant 7 (\"a 60 s TTL that does not run inside a join bracket\") and §4.2 test 7 (\"does not expire while the bracket is open\"), justified in §2.3 as reusing \"NoteJoinSnapshotBracket's already-shipped reasoning (container_contents_sync.cpp:667-677)\".",
     "evidence": "NoteJoinSnapshotBracket is a CLIENT-only mechanism. Its own definition comment says \"From event_feed's client-side SnapshotBegin and SnapshotComplete dispatch\" (container_contents_sync.cpp:666), and its only two call sites are event_feed.cpp:361 and :424, both inside handlers that return early on the host: event_feed.cpp:341 is `if (session.role() == net::Role::Host) break;` before :361. g_joinBracketOpen is therefore always false on the host, which is the only role that owns a custody park (§2.3 capture is host-role-gated).",
     "consequence": "The aging guard the design imports is dead on the role that holds the store, so invariant 7's bracket clause is vacuous and §4.2 test 7's second half asserts a state the shipped mechanism cannot enter in production — a gate that cannot fire. Worse, the guard's presence conceals the real coverage bound: with a plain 60 s wall-clock TTL the fix restores contents only when the client re-places within one minute. The reported chain is store-then-re-place (BUG-3 / R3Q P2 is \"the box goes into the client's personal store\"), where an arbitrary carry duration is ordinary play, so the design silently covers only a short-carry subset of the very defect it targets and nowhere states that as its scope.",
     "fix": "Delete the bracket clause (or replace it with a host-side condition that can actually be true) and retarget test 7 accordingly; then either raise/justify the TTL against a measured carry duration, or anchor expiry to an event rather than wall time (e.g. keep the park until the peer that authored the destroy disconnects, bounded by the 32-park cap); and state the residual coverage bound explicitly in §5.4 alongside the other named non-coverages."
    }
   ],
   "optional_improvements": [
    "§1.1 enumerates Boundary 1's application sites as :418, :486-490, :776, :797, :809; there is a sixth at container_contents_sync.cpp:573 (the refuse-and-re-publish path inside ParseAndApply). Add it, so a later reader does not treat the list as exhaustive.",
    "§5.2's file:line anchors are 3af5ddae-only and drift materially on the b150 line that §2.7 says the same patch text applies to: on ba6d8c39, DestroyResolvedLocalActor_ is at remote_prop_destroy.cpp:80 (not :73), IsWorldContainerInventory at :234 (not :191), NeuterNestedIndex at :311 (not :258), ReadContents at :324 (not :271), ApplyContents at :549 (not :471), OnVerbEntry at :722 (not :629). HostSpawnPlacedProp happens to be :158 on both. Give §5.3 step 5 symbol anchors rather than line anchors.",
    "Say explicitly that the park key must be the WIRE-normalised key on both sides. The capture's keyW comes from KeyToWString(payload.key) (remote_prop_destroy.cpp:123) and the apply's key from WireToWide of the intent (prop_drop_intent.cpp:382), both already truncated to 31 chars by the senders (prop_destroy_seam.cpp:119-121 and FillWireStr's N-1 cap). The draft's choice is correct, but §2.3 describes the field only as \"the logical key (WireKey / std::wstring)\", which invites an implementer to read the untruncated key off the dying actor and silently break the match.",
    "remote_prop_destroy.cpp has no IsHost()/role() accessor at all (grep returns nothing), so §5.2's `if (IsHost()) coop::props::container_custody::CaptureForDyingContainer(...)` cannot be written there as three self-contained lines. Have the custody module hold its own session pointer (the shipped session_holder pattern used by prop_drop_intent.cpp:46-47) and self-gate, so the call site is one unconditional call.",
    "Have the park line carry the orphaned slot's index and record count (parkedIndex is already in the CustodyPark struct) and have the apply line carry the new slot index, so the bounded GObjStack probe of Q-2/Q-4 can be scored from the same host log without building a second instrument.",
    "§2.6's kOpCustody sketch has no size bound. BroadcastContainer refuses an over-ceiling blob at container_contents_sync.cpp:348-352 with a named warning; the custody blob would need the same check against blob_chunks::MaxBlobBytes() (56,100 B) before ChunkAndSend, since 512 variable-length records can exceed it.",
    "Add a host-scored substitute for the C01-blocked A4: with the dev instrument on, print ContentsDigest(eid) on both roles (container_contents_sync.cpp:803-815 is already the shipped seam and §2.11 already names it) and compare counts, so client convergence gets an observable that does not depend on the client's mirror surviving OBS-R3Q-C01."
   ],
   "citations_checked": [
    {
     "citation": "protocol.h:1099-1112 — PropDropIntentPayload is exactly 172 bytes, complete field list, no contents field, one-datagram assert at :1112 (256-20-8 = 228 B ceiling)",
     "holds": true,
     "note": "Verbatim on 3af5ddae. Fields are className[64], key[32], propName[32], 3+3+3 floats, physFlags + 3 pad, savedScalar. On ba6d8c39 the struct body is byte-identical; only the trailing comment and the static_assert message text differ."
    },
    {
     "citation": "prop_drop_intent.cpp:158-232 — HostSpawnPlacedProp's complete body: FindClass :160, BeginDeferredSpawn :168, setKey :181-197, WriteSpParityIdentity :201-212, FinishDeferredSpawn :214, SetActorScale3D :220-222, ApplySavedScalarForClass :226-230, return :231; no loadData, no propInventory.Index write, no GObjStack touch",
     "holds": true,
     "note": "Read in full. The absence claim is exact. Signature matches §5.2 verbatim."
    },
    {
     "citation": "container_contents_sync.cpp — the ContainerContents lane is eid-addressed end to end: PackContents :307-316 ([u8 op][u32 eid][u64 baseHash][u16 n][n x SerSave]), ApplyContents :471-524 resolving LivePropActor(eid) first, OnVerbEntry :629-662 resolving identity at the verb edge and inserting into g_dirty at :654",
     "holds": true,
     "note": "All three verified. protocol.h's 'eid-addressed' wording is at :585-586; the enum value ContainerContents = 118 is at :589, which the draft cites for both."
    },
    {
     "citation": "container_contents_sync.cpp:271-289 — ReadContents is anonymous-namespace with exactly one call site (:334, BroadcastContainer), and calls NeuterNestedIndex at :285",
     "holds": true,
     "note": "Anonymous namespace spans :32-664, so ReadContents is inside it; `git grep ReadContents` on the file returns the definition at :271 and one call at :334. §7 Q-8's CLOSED finding is correct, including that the custody module needs a declared seam rather than a copy."
    },
    {
     "citation": "container_contents_sync.cpp:252-262 — NeuterNestedIndex and the loadData/init reading; the neuter is wire-destined, so a host-local park must not apply it",
     "holds": true,
     "note": "The comment at :252-257 states the argument the draft reproduces. The trap is real: :285 neuters only when RecordIsNestedContainer(r) (:228-233), so on a host-local path the nested container's own valid host index would be replaced by -1 and its contents orphaned on re-attach. This is the draft's sharpest and most correct catch."
    },
    {
     "citation": "container_contents_sync.cpp:191-195 — IsWorldContainerInventory fail-closed, refusing both Player != 0 and an unresolvable Player offset",
     "holds": true,
     "note": "Exact."
    },
    {
     "citation": "§1.1's enumeration of Boundary 1's application sites (:418, :486-490, :776, :797, :809)",
     "holds": false,
     "note": "There are six non-definition call sites, not five: :418, :486, :573, :776, :797, :809. The omitted :573 is the refuse-and-re-publish branch of ParseAndApply. Not load-bearing for the design's rule, but the list reads as exhaustive."
    },
    {
     "citation": "remote_prop_destroy.cpp:73-99 — DestroyResolvedLocalActor_ as the capture point; actor live :79-:97, K2_DestroyActor at :98; insert after :97 (native_pile_mirror::Unpin)",
     "holds": true,
     "note": "Exact, including the signature (void*, const std::wstring& keyW, const PropDestroyPayload&, void*). Also reached from TryApplyDestroy (:204-207), so the deferred destroy-before-load route funnels through the same point."
    },
    {
     "citation": "remote_prop_destroy.cpp:108-192 — OnDestroyImpl_ resolves by eid (:133-135) then by key (:150), terminating in DestroyResolvedLocalActor_ at :190",
     "holds": true,
     "note": "Exact. The :143-149 arbiter-echo short-circuit is produced only by coingun_arbiter.cpp:338, so it cannot pre-empt the pickup chain."
    },
    {
     "citation": "session.cpp:262-267 — the protocol-mismatch refusal on an unadmitted peer (\"protocol mismatch: peer=v%u, ours=v%u\"), plus peer_admission_internal.h:57-58 binding kProtocolVersion into the admission preimage and session_manager.cpp:592 refusing a browser join on advertised proto",
     "holds": true,
     "note": "All three exact. §2.7's conclusion that there is no mixed-version lobby to break is sound."
    },
    {
     "citation": "blob-chunk lane limits — BlobChunkPayload 228 B with 220 data bytes (protocol.h:1367-1374), MaxBlobBytes() = 220 x 255 = 56,100 (blob_chunks.h:45-47), kMaxRecordsPerContainer = 512 (container_contents_sync.cpp:55)",
     "holds": true,
     "note": "All exact."
    },
    {
     "citation": "container_contents_sync.cpp:563 — the shipped parser refuses an unknown op (`if (!W::RdU8(blob, o, op) || op != kOpContents) return Ingest::Handled;`)",
     "holds": true,
     "note": "Quoted verbatim. §2.6's degrade-not-compatibility reading is correct, as is its insistence on the doctrine Step 7 bump anyway."
    },
    {
     "citation": "container_contents_sync.cpp:528-558 HostAcceptsClientWrite (the CAS), :569-578 the re-publish to the author, :735-742 the sender-slot acceptance matrix, :761-780 the connect seed, :803-815 ContentsDigest, :817-831 OnDisconnect",
     "holds": true,
     "note": "All read and exact."
    },
    {
     "citation": "container_contents_sync.cpp:495-500 EngineAlloc pre-flight, :502-517 the raw-write block (AllocZeroed / WriteSaveRecord / WriteArrHeader), :453-457 RederiveManagedState, applied at :521",
     "holds": true,
     "note": "Exact; the reuse targets §5.2 names are real."
    },
    {
     "citation": "§0.5 — the BUG-017 mechanism is code-identical across ba6d8c39, f690c017 and 3af5ddae",
     "holds": true,
     "note": "Independently reproduced. git diff --numstat f690c017 ba6d8c39 is empty for every listed file. git diff -w --numstat ba6d8c39 3af5ddae reproduces the draft's numbers exactly (185/292, 31/87, 103/106, 28/41, 0/0, 58/59, 89/104, 69/161). A comment-stripped diff of the three mechanism .cpp files shows prop_drop_intent.cpp and remote_prop_destroy.cpp with ZERO executable change and container_contents_sync.cpp with only the warn-once latch (:698-705). Protocol kinds 90/104/118 identical on all three pins; kProtocolVersion 150/151/152."
    },
    {
     "citation": "§0.1 — the R3Q primary chain P0-P5, the control's EID 4129 -> 5975, primary EID 5968, reload EID 4202, world hash 2CBFFC12... vs seed CCCB6FCE..., and the GAP quote about the arms varying the acting peer",
     "holds": true,
     "note": "Verified against R3Q/LANES/ws-runtime-review-codex/REVIEW.md:129, :136, :143, :144, :159 and VERDICT.txt. The GAP is quoted verbatim; the bounded persistence wording is faithfully carried, including the refusal to restate it as unrestricted permanent data loss."
    },
    {
     "citation": "f3d6b59d:design/relay-adoption/DECISION.md:3 (RELAY_DECISION), :17 (the ten axes; \"It must not be a universal reflection serializer\"), :104 (the small-isolated-confirmed-fix clause)",
     "holds": true,
     "note": "All three exact, including the [INFERRED] tag the draft preserves."
    },
    {
     "citation": "f3d6b59d:BUG-APPLICABILITY-MATRIX.md:51/:84/:87 rows and the U2/H-M cell key",
     "holds": true,
     "note": "Rows exact. The cell key is at :15-21 (draft cites :16-22); U2 at :16 and H-M at :20 are verbatim. The draft's refusal to self-promote BUG-017's cell is correct discipline."
    },
    {
     "citation": "f3d6b59d:MULTIVOID-CURRENT-MODEL.md:23-42 (the axis table) and PILOT-DESIGN.md:21-37 (G01-G17, G10 at :30, G16 at :36)",
     "holds": true,
     "note": "Axis concerns quoted verbatim. One drift: the SOURCE_PROVEN four-identity-domains claim the draft cites as :44 is actually at :40; :44 is the PROBE line."
    },
    {
     "citation": "docs/devices.md:41-71 (the container model and the authority rule), :171-183 (\"Who owns what\" has no container-contents row), :185-196 (\"Wire messages\" has no ContainerContents row)",
     "holds": true,
     "note": "All three verified, including the two absences — the tables list ContainerState but never ContainerContents, and no ownership row for contents."
    },
    {
     "citation": "docs/coop-sync-doctrine.md — \"A destroy-and-recreate transition, hold to drop to store to equip, carries the identity across the actor gap\", cited as :96-97",
     "holds": false,
     "note": "The quoted sentence is at :98-99, inside Step 5 (:94-101, which the §8 index cites correctly). Steps 6/7/8 and the forbidden-patterns list are at :103-110, :111-119, :120-128 and :129-138 as cited (individual line offsets drift by one or two)."
    },
    {
     "citation": "prop_drop_intent.h:12-15 — why the client's intent is authored a tick late (the husk-destroy ordering guard), and :8 \"the grab half already works, the hold-R pickup DESTROY crossing the bidirectional destroy-seam\"",
     "holds": true,
     "note": "Verbatim. :8 is the header's own statement that a client pickup crosses as a destroy, which is the load-bearing fact for the capture point's reachability."
    },
    {
     "citation": "prop_lifecycle.cpp:280-283 — \"K2_DestroyActor only marks PendingKill, so reads on the actor are still valid\" (§7 Q-5's fallback)",
     "holds": true,
     "note": "Verbatim. Stronger than the draft needs: the proposed capture is BEFORE the K2_DestroyActor call at remote_prop_destroy.cpp:98, so the component is unambiguously live. Q-5 is closer to closed than the draft claims."
    },
    {
     "citation": "player_inventory_sync.cpp:391-435 — the host persists a client's inventory blob to that player's slot file and applies nothing (`e.blob = std::move(blob); ... FlushSlot(senderPeerSlot);`), and inventory_wire.cpp:36-45 uses the same per-record codec",
     "holds": true,
     "note": "Exact at :429 and :434. §1.3's conclusion that neither the client's carried record nor the host's on-disk copy becomes host world state is correct."
    },
    {
     "citation": "§1.4 step 3 — the word GObjStack does not appear in remote_prop_destroy.cpp, prop_destroy_seam.cpp or event_dispatch_entity.cpp",
     "holds": true,
     "note": "grep -c returns 0 in all three files on 3af5ddae."
    },
    {
     "citation": "§2.3 — park aging \"reuse[s] NoteJoinSnapshotBracket's already-shipped reasoning (container_contents_sync.cpp:667-677)\"",
     "holds": false,
     "note": "See BF-04. The function is client-only (its own comment at :666; call sites event_feed.cpp:361 and :424, both after the host early-return at :341), so g_joinBracketOpen is never true on the host, where the custody park lives."
    },
    {
     "citation": "§2.4 — \"mark the new eid dirty (the same set OnVerbEntry writes at :654) and let DrainDirty (:400-421) ship it\" at the apply point",
     "holds": false,
     "note": "See BF-02. The eid is minted one tick later (host_spawn_watcher.h:29-32; subsystems.cpp:547; prop_drop_intent.cpp:223-225; prop_lifecycle.cpp:371-373), so no eid exists at :231. The rest of Q-7 checks out favourably: g_sentHash/g_publishedHash/g_baseHash/g_appliedHash are all std::map keyed by eid and all cleared at OnDisconnect (:817-831)."
    },
    {
     "citation": "§4.2's templates — 1c393401:src/votv-coop/tests/baseline_delivery/{run.ps1,qualification.cpp,stubs/} and 15589b7f:src/votv-coop/tests/standalone/candidate_02_prop_spawn_gate.cpp",
     "holds": true,
     "note": "Both paths exist at the cited refs."
    }
   ],
   "claims_overstated": [
    "§1.3 closes with \"the fresh propInventory_C::init runs inside the finish spawn with the CDO's default index\" under a STRENGTH=SOURCE_PROVEN heading. That clause is the same Blueprint-bytecode inference §1.4 step 5 tags INFERRED and §7 Q-4 tags UNKNOWN. It is also the apply point's precondition (BF-03), so the overstatement is load-bearing, not cosmetic — and it is precisely the defect class §9.4 names as the programme's standing one.",
    "§2.3: \"This rule is what makes the mechanism safe to run on *both* authorship directions with one code path.\" The mechanism does not run on the host-authored direction at all (BF-01), so the fresh-empty refusal cannot be doing that work. The companion sentence \"Without this rule the fix would double-apply on the host-authored chain and could revert a newer host state\" is false for the same reason.",
    "§2.0's TRANSITIONS conformance row claims §2.4's fan-out postcondition is honoured. The stated seam cannot obtain the eid the postcondition names (BF-02).",
    "§2.0 (G16) and §5.1's summary: \"Two production files change by three lines each\" / \"the whole mechanism is two three-line call sites\". Satisfying BF-02 and BF-03 requires a third seam or a module Tick registration in subsystems.cpp, so the one-switch backout claim and the file-set table are both understated.",
    "§4.1 A2's named observable and pass condition (\"discard line present (the native path still wins)\") assert an outcome a correctly working fix cannot produce (BF-01).",
    "§6 invariant 7's \"a 60 s TTL that does not run inside a join bracket\" is vacuous on the host (BF-04), and §4.2 test 7's bracket-open half tests a property the shipped mechanism does not have on that role.",
    "§1.1's list of Boundary 1 application sites reads as exhaustive but omits container_contents_sync.cpp:573.",
    "Two minor citation drifts that do not change any conclusion: the doctrine actor-gap sentence is at coop-sync-doctrine.md:98-99, not :96-97; MULTIVOID-CURRENT-MODEL.md's SOURCE_PROVEN identity-domains claim is at :40, not :44."
   ],
   "implementation_go": false,
   "summary": "Source-correctness review of BUG017-CONTRACT-DESIGN-DRAFT.md against 3af5ddae and ba6d8c39. I opened 28 cited locations; 24 hold exactly, 4 do not.\\n\\nDIRECTION COMPLIANCE: honoured. The primary mechanism adds no ReliableKind, no payload field, no blob op and no protocol bump; it touches no transport code, introduces no Relay artifact, and reuses the shipped save_record codec rather than adding a serializer (in fact the host-local park serializes nothing). §2.7's b150 rule — host-local mechanism only, kProtocolVersion untouched at 150, kOpCustody forbidden on that line — is correct and is backed by the shipped refusals at session.cpp:262-267, peer_admission_internal.h:57-58 and session_manager.cpp:592.\\n\\nCODE-IDENTITY: independently reproduced. b150 and f690c017 are byte-identical in every mechanism file; a comment-stripped diff b150 -> 3af5ddae shows ZERO executable change in prop_drop_intent.cpp and remote_prop_destroy.cpp and only the warn-once latch in container_contents_sync.cpp. §0.5's conclusion stands.\\n\\nREACHABILITY ON THE MEASURED R3Q PRIMARY CHAIN: the park IS reachable, both ends. Capture: a client's hold-R pickup destroys its local box and broadcasts PropDestroy (prop_destroy_seam.cpp:128-149, and prop_drop_intent.h:8 states the pickup destroy crosses the bidirectional seam); the host routes it at event_dispatch_entity.cpp:254 -> remote_prop::OnDestroy:196-198 -> OnDestroyImpl_:108-192, which resolves by eid then by key at :150 and terminates in DestroyResolvedLocalActor_:73-99 with the actor still live through :97. Nothing in the chain empties the host's container first, so the contents are present at capture. Apply: the client's re-place authors one PropDropIntent (prop_drop_intent.cpp:311-353), routed at event_dispatch_intent.cpp:258-280 to OnPropDropIntent:377-400, whose duplicate guard at :390 provably did NOT fire in the R3Q arm (the host materialized), confirming the host had already destroyed its copy; HostSpawnPlacedProp:158-232 then runs under the SAME logical key, which both sides carry through the same 31-char WireKey truncation. So the design does not fail the reachability test set for it.\\n\\nWHY IT IS STILL NOT CLEAN: four source-grounded defects, none of them \\\"more tests are imaginable\\\". BF-01 — the mandatory control arm A2 is specified against an observable neither seam can emit, because a host-authored chain reaches neither HostSpawnPlacedProp (client-message-only, prop_drop_intent.cpp:394/:427) nor DestroyResolvedLocalActor_ (received-destroy-only); the fresh-empty refusal's stated rationale falls with it. BF-02 — the apply point at :231 has no element id (host_spawn_watcher.h:29-32, subsystems.cpp:547, prop_drop_intent.cpp:223-225: the eid is minted one tick later at prop_lifecycle.cpp:371-373), so §2.4's required fan-out cannot fire, leaving a host-full/client-empty divergence the unfixed build does not have and the host-only arms cannot see. BF-03 — the apply's own precondition (GObjStackSlot resolving on the fresh component, container_contents_sync.cpp:205-207) rests on Q-4, which §7 routes away from the patch, and §2.11 has no apply-refusal log row, so a total no-op is indistinguishable from a dead hook. BF-04 — the TTL's join-bracket guard is client-only (event_feed.cpp:341 returns before :361), making invariant 7 vacuous on the host and concealing the real coverage bound: a 60 s wall-clock TTL covers only short-carry re-places, while the reported defect is store-then-re-place.\\n\\nBF-01, BF-03 and BF-04 are documentation/instrumentation fixes. BF-02 is not trivial: it moves the consume (or at least the dirty-mark) to the next-tick adoption and therefore breaks §5.1's file set, §5.2's two-call-site shape and the G16 one-switch-backout claim. The core idea — a bounded host-local park keyed on the logical key, captured before K2_DestroyActor and consumed at the keyed host spawn — is sound, correctly targeted, and its sharpest hazard (the nested-index neuter asymmetry, §2.3 / test 11) is identified and right. Revise the four points and re-review; do not start implementation against this revision."
  }
 },
 {
  "lens": "data-loss-and-regression",
  "review": {
   "verdict": "DESIGN_NOT_CLEAN",
   "blocking_findings": [
    {
     "id": "BF-1",
     "requirement": "Hazard (b): the design must not apply parked contents to the wrong container across a key generation. Violates the draft's own §2.1 IDENTITY (\"one row per actor\"), §2.2 AUTHORITY (\"a client may cause a transition; it may never assert container contents\"), and §6 invariant 5.",
     "evidence": "The consume predicate in §2.3/§2.4/§2.10 is exactly {same logical key, actor is a container, Boundary 1 passes, fresh slot reads empty}. There is no class binding, no generation token, and no causal binding to the specific destroy that took the park. On the source: (a) keys are measured NOT to be instance-unique on this line -- C:\\AgenticStaging\\BugDropbox\\Doctaaaa's reports\\Report 3\\bug_03_floppy_dissolves_deferred_destroy_key_collision.md records key '0zlKei78xf4Ek_bPrYWV0A' carried by >=4 distinct live client actors plus one host actor, with all destroy traffic at eid=0; (b) the capture seam is reached for EVERY remote keyed destroy of a container, not only a pickup -- prop_destroy_seam.cpp:154-156 -> prop_drop_intent.cpp:365-375 (NoteClientKeyedDestroy) parks the client key for any keyed destroy it broadcasts (hammer break, coin-gun sale, trash disposal all pass through the same DestroySeamBody); (c) the dying actor is resolved eid-first then key-fallback at remote_prop_destroy.cpp:133-151, and the draft's §5.2 insertion parks under the WIRE payload's keyW, not under the dying actor's own key, so a resolve-by-eid whose payload key diverges files the records under a key that is not the container's; (d) the consuming spawn's class and transform are entirely client-chosen -- OnPropDropIntent (prop_drop_intent.cpp:377-400) validates only non-empty key/class and the live-key duplicate guard, with no class whitelist (only OnReelEjectIntent whitelists); (e) prop_lifecycle.cpp:214 records that MarkPropElement \"may re-key a duplicate (the host is the key authority)\", and that re-key happens AFTER the draft's apply point, so the host may change the very key the park was matched on.",
     "consequence": "Two new corruption paths that do not exist today. (1) Resurrection/duplication: a container destroyed for good (client hammer break, sale, disposal) parks its contents, and any keyed host spawn under a colliding or reused key within the 60 s TTL receives them -- today that new container is simply empty. (2) Client-directed relocation: a client can broadcast DESTROY(K, eid=0), have the host park container A's contents by key-fallback resolution, then send a drop intent under K with a container class at a transform of its own choosing and receive A's contents there. That is the client asserting contents placement, which §2.2 forbids.",
     "fix": "Bind the park to the dying actor itself, not to the wire payload: read ue_wrap::prop::GetInteractableKeyString(actor) at the capture point and refuse to park on empty or on mismatch with payload.key; record the dying actor's class and refuse the consume unless the spawning class equals it; and gate the consume on a single-generation token -- consumable only by the first keyed host spawn following that destroy, authored by the same peer slot that authored the destroy, and only within a window far shorter than 60 s."
    },
    {
     "id": "BF-2",
     "requirement": "Hazard (a)/(d): the apply must not write into a GObjStack slot it cannot prove is exclusively owned by the container it just spawned, and the Player==true refusal must actually cover the write target. Violates §6 invariants 2 and 4 and §2.5's fail-closed validation table.",
     "evidence": "§2.3 recommends writing \"into the slot the host just resolved for its own fresh component\" and §2.5 lists only these guards: not-a-container, IsWorldContainerInventory false, GObjStackSlot null, fresh count != 0, EngineAlloc probe, >512 records. Boundary 1 (container_contents_sync.cpp:191-195) reads the Player byte off the COMPONENT; it says nothing about which slot the component's Index names. GObjStackSlot (container_contents_sync.cpp:200-211) refuses only idx < 0 and idx >= stack.num -- index 0 is accepted. The shipped Boundary 2 comment (container_contents_sync.cpp:252-262) states the exact failure mode: \"an empty array yields index 0, which propInventory::init's `index >= 0` guard passes, and the container reuses GObjStack[0], a slot owned by someone else\", and Boundary 1's own comment states \"GObjStack[0] is the local player's inventory by construction\". The draft's §1.4 step 5 (fresh component allocates its own slot) is tagged INFERRED from an in-tree bytecode-derived comment and is NOT re-measured; §7 Q-4 and §9 item 4 then declare it non-blocking on the grounds that \"the recommended strategy writes into whatever slot the fresh component resolved\".",
     "consequence": "If propInventory_C::init leaves Index at 0, or at a slot another live component already owns, the parked record set is written over that slot via SR::WriteArrHeader (container_contents_sync.cpp:502-517). When the aliased slot is GObjStack[0] and the host player happens to be carrying nothing, the fresh-empty check passes and the host player's own personal inventory is silently replaced by the parked records -- exactly the outcome Boundary 1 exists to prevent, reached without ever tripping Boundary 1. When it aliases another empty world container, two containers share one contents array and each write clobbers the other. Neither failure exists today. The design's own reasoning for why Q-4 need not be answered is the inverse of the safety argument.",
     "fix": "Add slot-ownership checks to §2.5, all cheap and all closable in review: refuse the apply when the resolved Index is 0; refuse when the resolved slot pointer equals the local personal-store slot (ue_wrap::inventory::ReadLivePersonalStore already carries the address assertion in the other direction); refuse when it equals the GObjStackSlot of any other live world container (SnapshotWorldContainers, container_contents_sync.h:56-68, is the shipped enumerator); and print the resolved Index on the [CUSTODY] reattached line so a rig arm can see aliasing without a stack probe. Alternatively make Q-2/Q-4 a precondition of implementation rather than an open question."
    },
    {
     "id": "BF-3",
     "requirement": "§2.4's stated postcondition -- \"APPLIED must be followed, on the same tick or the next contents sweep, by the host's ordinary ContainerContents fan-out for the new eid\" -- must be implementable at the insertion point §5.2 specifies. Regression: the fix must not create a host/client divergence where none exists today.",
     "evidence": "§5.2 inserts TryReattachForSpawnedContainer(actor, key) inside HostSpawnPlacedProp after :230 and before the return at :231, and §5.1 provides MarkDirty(uint32_t eid) as the fan-out trigger -- but neither the call signature nor the plan says where that eid comes from. On the source the eid does not exist yet at that statement: HostSpawnPlacedProp deliberately does not mark an incoming spawn so \"the host's own finish-spawn watcher catches it\" (prop_drop_intent.cpp:158-161), and that watcher's callback (host_spawn_watcher.cpp OnFinishSpawnFunc) \"only enqueues\" into g_pendingFinished -- its own header comment says \"The drain, next pump tick, adopts once the whole blueprint call has completed.\" The element is minted later, in DrainPendingSpawns -> prop_lifecycle::ExpressSpawnedProp -> MarkPropElement (prop_lifecycle.cpp:214, :371-373). prop_lifecycle's Init POST body also returns early for an Aprop_C whose key is not yet set (\"An Aprop_C mints on a later Init pass after loadData or setKey\"), and the construction-script Init dispatch is the route the same file records as invisible to the POST observer. So PT::GetPropElementIdForActor(actor) is kInvalidId at prop_drop_intent.cpp:231.",
     "consequence": "As specified, the host restores the contents locally and never publishes them: no eid to mark dirty, no retry described. Every client keeps showing the empty box while the host shows a full one -- a new user-visible divergence, since today both peers agree the box is empty. Additionally g_publishedHash (container_contents_sync.cpp:364) gains no entry for the new eid, so the first client-authored write against that container is refused as STALE BASE by HostAcceptsClientWrite (:528-558) and host truth is re-published over that player's action. The draft's §2.4 asks the reviewer the wrong question (whether stale hash entries exist for a freshly minted eid) when the actual defect is that no eid exists at the apply point.",
     "fix": "Move the reattach to the drain that mints the element (immediately after ExpressSpawnedProp in DrainPendingSpawns, or a same-tick post-drain hook keyed on the actor), or keep the apply where it is and defer only the MarkDirty until GetPropElementIdForActor(actor) resolves, with a bounded retry and a refusal log if it never does. Add an acceptance observable that the ContainerContents fan-out for the new eid actually shipped (container_contents_sync.cpp:369-372 already logs it)."
    },
    {
     "id": "BF-4",
     "requirement": "§4.1's control-regression arm A2 must be a must-fire control: its named observable must actually be producible by a correct implementation on the CONTROL chain. The draft states \"A1 without A2 is not a result\".",
     "evidence": "A2's pass criterion is the presence of the line \"[CUSTODY] HOST park DISCARDED key='<K>' -- fresh slot already holds N records\". Neither seam can fire on a wholly host-authored chain. The capture seam DestroyResolvedLocalActor_ (remote_prop_destroy.cpp:73-99) is reached only from remote_prop::OnDestroy / OnDestroyImpl_ (:108-198), i.e. a PropDestroy RECEIVED from a peer; a host's own pickup goes out through prop_destroy_seam.cpp:128-149 (s->SendPropDestroy) and never re-enters its own receive path -- and NoteClientKeyedDestroy at :154-156 is explicitly gated on role == Client. The apply seam HostSpawnPlacedProp is reached only from OnPropDropIntent (prop_drop_intent.cpp:394) and OnReelEjectIntent (:427), both host handlers for CLIENT intents; a host's own place is expressed by the finish-spawn watcher instead. The R3Q CONTROL arm is entirely host-authored (REVIEW.md: \"Joined C1 content establishment to C2-C5 paired logs... host-authored grab/inventory/place/pickup chain\"; draft §0.1 C2-C5).",
     "consequence": "A correct implementation produces ZERO [CUSTODY] lines on A2, so A2 fails its stated pass criterion. The programme then either rejects a correct fix, or is \"corrected\" by making the seams fire on host-authored chains -- the single change most likely to put the working CONTROL_PASS path at risk. The DISCARDED transition in §2.4 and standalone assertion 2 also have no rig arm that can exercise them, so the draft's claim that assertion 2 is \"the assertion A2 exists to confirm in the world\" is not true.",
     "fix": "Restate A2's pass criterion as the correct must-fire negative: zero [CUSTODY] lines anywhere in the host log for the host-authored chain, AND drive GCPcVmpTGXXJQmxep5xtXg present at C5 -- with the grep shown matching a planted [CUSTODY] line first, per §4.1's own probe-before-claim rule. Then either name a real chain that exercises the discard branch (a client-authored re-place where the host's native path restores first, if one exists) or declare that branch unit-tested only and say so in §4.1."
    },
    {
     "id": "BF-5",
     "requirement": "§4.2's omission discipline -- \"Each must turn at least one named assertion red\" -- must hold for all ten listed omission cases.",
     "evidence": "The proposed harness is, by §4.2 and §5.1, a header-only engine-free CustodyParkStore plus save_record_wire codec controls; assertions 1-12 are entirely about the store's FIFO/map/TTL/hash behaviour and the SerSave/DeSave neuter flag. Four of the ten omission cases live only in the engine-touching container_custody.cpp / the container_contents_sync.h wrappers and therefore cannot be linked into that harness, and no assertion 1-12 names them: \"delete the capture call\", \"park on a Player=true inventory\", \"apply without the allocation pre-flight\", \"apply without RederiveManagedState\". A5 nominally covers the Player refusal but is written as \"place a personal-inventory-backed container path through the same code (or plant a Player=true component in the dev instrument)\" while §5.1's complete file list adds no dev-instrument change that could produce one.",
     "consequence": "Two of the four unproven gates are the ones that prevent a wrong write: the Player==true refusal at the NEW entry point (whose violation destroys a player's inventory, per container_contents_sync.cpp:191-195) and the EngineAlloc pre-flight, whose shipped comment states \"without it an empty array would silently replace real contents\" (:495-500). The draft would ship them in the state its own A5 rationale condemns -- \"a boundary that has never been seen refusing is a boundary nobody has tested\" -- while claiming the omission harness covers them.",
     "fix": "Give the two engine-touching seams a thin injectable slot-reader/slot-writer pair the harness can stub, so Player-refusal, pre-flight-refusal and rederive-called become named assertions in candidate_17; and give A5 a concrete instrument listed in §5.1. If neither is affordable, delete those four rows from the omission list and state explicitly that they are unproven."
    }
   ],
   "optional_improvements": [
    "§2.3 borrows NoteJoinSnapshotBracket's \"parks do not age inside the bracket\" rule for a HOST-side store, but the shipped bracket is client-side: container_contents_sync.cpp:667 is fed only from event_feed.cpp:361 and :424, the client's SnapshotBegin/SnapshotComplete dispatch (the function's own comment says \"From event_feed's client-side ... dispatch\"). On the host g_joinBracketOpen is always false, so standalone assertion 7 (\"does not expire while the bracket is open\") tests a state the host cannot reach -- a test mirroring the implementation rather than a control. Either drop the bracket clause and use a plain TTL, or define what opens a host-side bracket.",
    "Add an absolute age ceiling independent of any bracket, and clear the park on a host world-load episode / reconcile window (coop::world_load_episode::InEpisode / InReconcileWindow, the same gates used at prop_destroy_seam.cpp:84-107). Session-exit clearing is already covered -- container_contents_sync::OnDisconnect is called from the whole-session teardown at subsystems.cpp:422, not per-peer -- but a mid-session world reload is not, and a park surviving one can apply pre-reload records to a post-reload container carrying the same key.",
    "§2.6 and §5.4's \"no wire change at all\" / \"adds zero bytes\" should be reworded to \"no new ReliableKind, no new op, no format or size change\". The mechanism does produce ContainerContents traffic that is not sent today (a restored container is marked dirty and fanned out where today an empty one is silent). That is wire-compatible with official b150 peers -- ContainerContents = 118 and the kOpContents grammar are identical on all three pins per §0.5, which I re-checked -- but it is not byte-identical traffic, and the b150 interop claim should be stated as format compatibility.",
    "§2.3's neuterNested=false capture is right for the host-local path, but the design should state what happens to a NESTED container's own slot: destroying the outer container does not destroy the inner container's GObjStack slot, and re-attaching the outer record set re-points at inner slots whose contents may have been rewritten in the interval. Add the invariant, and add an assertion that a parked outer set whose preserved ints[0][0] no longer resolves is refused rather than written.",
    "§2.11's park line carries index=%d but the reattach line does not. Print the resolved Index on the apply line -- that single field is what lets a rig arm detect the BF-2 aliasing case without the Q-2 stack probe.",
    "§0.5 calls prop_destroy_seam.cpp \"Not on the BUG-017 chain\" while §1.4 step 1 and §8 cite it as chain step 1. Reconcile the wording. The underlying identity claim holds: I measured git diff -w ba6d8c39..3af5ddae on that file and its only executable change is the removal of the dead SyncDestroyedTrackedProp function.",
    "§5.2 gives insertion anchors as 3af5ddae line numbers; those offsets do not hold on ba6d8c39 (prop_drop_intent.cpp differs by 103/106 lines b150->3af5ddae, all comment text -- measured). For the b150 cherry-pick, name the anchors by statement (\"before R::CallFunction(actor, g_destroyActorFn, nullptr)\", \"before return actor\"), which §5.2 already does in prose; drop the bare line numbers from the plan."
   ],
   "citations_checked": [
    {
     "citation": "§0.1 R3Q values: CONTROL_PASS, EXACT_REPRODUCTION, PERSISTENCE_RESULT=NOT_RECOVERABLE_AFTER_RELOAD; ABSENT_WITHIN_SEARCHED_SCOPE, PERMANENT_DATA_LOSS=ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE; UNKNOWN_BEYOND_SCOPE, RUNTIME_RERUN_REQUIRED=none",
     "holds": true,
     "note": "Verbatim match to R3Q/PACKET2/r3q-runtime-packet-2/RESULT.json and to ws-runtime-review-codex/VERDICT.txt. The subject key HlwQ1k8xmW4zxWDHX-VZFw, drive GCPcVmpTGXXJQmxep5xtXg and class prop_box_C also match."
    },
    {
     "citation": "§0.1 GAP quotation: the two arms vary the acting peer for the entire grab/inventory/place chain",
     "holds": true,
     "note": "REVIEW.md primary-arm GAP, verbatim in substance."
    },
    {
     "citation": "§0.2 GATE-OBSERVATION-CLOSURE=PASS; neither observation load-bearing",
     "holds": true,
     "note": "REVIEW.md observation-closure claim, exact."
    },
    {
     "citation": "§0.5 b150 vs 3af5ddae non-comment drift in prop_drop_intent.cpp (14), remote_prop_destroy.cpp (2), container_contents_sync.cpp (7), prop_destroy_seam.cpp (31)",
     "holds": true,
     "note": "Re-measured with git diff -w ba6d8c39 3af5ddae plus a comment filter. The two files the patch touches show only include/comment text changes; prop_destroy_seam.cpp's only executable change is the dead SyncDestroyedTrackedProp removal; container_contents_sync.cpp's is the warn-once latch. The code-identity conclusion stands."
    },
    {
     "citation": "§1.1 Boundary 1 = IsWorldContainerInventory at container_contents_sync.cpp:191-195, fail-closed on an unresolvable Player offset",
     "holds": true,
     "note": "Exact. But it tests the component, not the slot the component's Index names -- see BF-2."
    },
    {
     "citation": "§1.1 Boundary 2 = NeuterNestedIndex at container_contents_sync.cpp:252-262 and the loadData/init reading",
     "holds": true,
     "note": "Exact, including the quoted GObjStack[0] rationale."
    },
    {
     "citation": "§1.3 / §5.2 HostSpawnPlacedProp body: FindClass, BeginDeferredSpawn, setKey, WriteSpParityIdentity, FinishDeferredSpawn, SetActorScale3D, ApplySavedScalarForClass, return actor -- no loadData, no Index write, no GObjStack touch",
     "holds": true,
     "note": "Read in full at prop_drop_intent.cpp:158-232. Confirmed."
    },
    {
     "citation": "§1.4 step 2: host routes PropDestroy -> remote_prop::OnDestroy -> OnDestroyImpl_ -> DestroyResolvedLocalActor_ (clear drives, release grab, echo-suppress, unpin, K2_DestroyActor)",
     "holds": true,
     "note": "Exact. Note the resolution order is eid-first then key-fallback, which the draft cites but does not carry into the park key -- see BF-1."
    },
    {
     "citation": "§2.3 park store mirrors prop_drop_intent.cpp:68-79's set+FIFO invariant, kMaxParked=64",
     "holds": true,
     "note": "Exact, including UnparkKey's one-copy invariant."
    },
    {
     "citation": "§2.3 apply reuses SR::AllocZeroed / WriteSaveRecord / WriteArrHeader at container_contents_sync.cpp:502-517 with the EngineAlloc pre-flight at :495-500 and RederiveManagedState",
     "holds": true,
     "note": "Exact."
    },
    {
     "citation": "§2.4 g_sentHash / g_publishedHash / g_appliedHash are keyed by eid and carry no entry for a freshly minted eid",
     "holds": true,
     "note": "They are std::map<uint32_t,uint64_t> keyed by eid (:73-87) and cleared only at OnDisconnect (:824-827). The reviewer check the draft asks for passes -- but it is the wrong question, because there is no eid at the apply point at all (BF-3)."
    },
    {
     "citation": "§2.3 reuse NoteJoinSnapshotBracket's reasoning (container_contents_sync.cpp:667-677) for the custody park's TTL",
     "holds": false,
     "note": "The bracket is fed only from the CLIENT-side SnapshotBegin/Complete dispatch (event_feed.cpp:361, :424) and the function's own comment says so. On the host the flag never opens, so the borrowed rule and standalone assertion 7 are inapplicable to a host-side park."
    },
    {
     "citation": "§7 Q-8 CLOSED: git grep ReadContents returns exactly the definition at :271 and one call at :334",
     "holds": true,
     "note": "Re-ran git grep -n ReadContents 3af5ddae -- src/votv-coop: exactly those two lines."
    },
    {
     "citation": "§2.7 protocol mismatch refused before join (session.cpp:262-267), ContainerContents = 118 identical on all three pins",
     "holds": true,
     "note": "Kind number and grammar identical; the wire-compatibility conclusion for the b150 line holds at the format level."
    },
    {
     "citation": "§3 OBS-R3Q-C01: prop_destroy_seam.cpp:131 writes elementId 0 as the sender-had-no-Element sentinel; prop_drop_intent.h:12-15 is the client-side ordering guard",
     "holds": true,
     "note": "Exact, including the header's verbatim reasoning. The claim that C01 is off the primary arm's critical path is consistent with the code, and scoring A1-A3 on the host only is the right call."
    },
    {
     "citation": "§4.1 A2 pass criterion: the [CUSTODY] park DISCARDED line fires on the host-authored CONTROL chain",
     "holds": false,
     "note": "Neither seam is reachable on a wholly host-authored chain -- see BF-4."
    },
    {
     "citation": "§5.2 insert TryReattachForSpawnedContainer(actor, key) before return actor at prop_drop_intent.cpp:231, then MarkDirty(eid) per §2.4",
     "holds": false,
     "note": "No element id exists at that statement; the host's element is minted next pump tick in DrainPendingSpawns -> ExpressSpawnedProp -> MarkPropElement. See BF-3."
    },
    {
     "citation": "§1.4 step 5 / §7 Q-4: the fresh propInventory_C takes the allocate-a-fresh-slot branch",
     "holds": true,
     "note": "Correctly tagged INFERRED and correctly left open as Q-4. The error is not the tag but §9 item 4's conclusion that the recommended strategy does not depend on the answer -- it does, for slot ownership (BF-2)."
    }
   ],
   "claims_overstated": [
    "§2.6 \"Under the primary (host-local) mechanism: no wire change at all\" and §2.7's \"It adds zero bytes\" -- the format, kinds and ops are unchanged and b150 interop is preserved, but a restored container is marked dirty and fanned out as a ContainerContents blob that is not sent today. Say \"no new kind, no new op, no format or size change\", not \"zero bytes\".",
    "§7 Q-4's \"Blocks: the strength tag on §1.4 step 5; not the patch\" and §9 item 4's \"The design does not depend on it being right -- the recommended strategy writes into whatever slot the fresh component resolved\". Writing into whatever slot the fresh component resolved is precisely what makes the answer load-bearing: the safety of the write depends on that slot being exclusively owned.",
    "§4.1 A2's pass criterion, and §4.2 assertion 2's gloss \"this is the assertion A2 exists to confirm in the world\" -- presented as an in-world measurement of the control path when the seams cannot fire on that path at all.",
    "§2.3's appeal to NoteJoinSnapshotBracket's \"already-shipped reasoning\" for a host-side park. The shipped bracket is client-side only; the reasoning is not transferable as written.",
    "§0.5's \"prop_destroy_seam.cpp ... Not on the BUG-017 chain\", contradicted by §1.4 step 1 and §8, which cite prop_destroy_seam.cpp:128-149 and :154-156 as chain step 1. The code-identity conclusion the sentence supports is nonetheless correct.",
    "§4.2's closing \"Each must turn at least one named assertion red\" for the ten omission cases -- four of them cannot, given the engine-free harness the same section specifies."
   ],
   "implementation_go": false,
   "summary": "DESIGN_NOT_CLEAN. The draft honours the reviewed direction cleanly on the axes the task named as prohibitions: no transport rewrite, no Relay artifact, no universal serializer, no new ReliableKind or op or protocol bump under the primary mechanism, and the private b150 line stays wire-format-compatible (ContainerContents = 118 and its kOpContents grammar are identical on all three pins -- re-measured). Its persistence wording is correct and disciplined: it quotes RESULT.json verbatim, repeats the bounded scope, and twice forbids restating ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE as unrestricted permanent loss (§0.1, §5.4). Its INFERRED claims are tagged, including the load-bearing ones in §1.4 steps 4-6 and the blanket tag on §2. Nineteen of its source citations that I checked on 3af5ddae hold exactly.\n\nFive findings block implementation, four of them under the data-loss/regression lens. (BF-1) The custody park is addressed by logical key alone, with no class binding, no generation token, no causal binding to the destroy that filled it, and no distinction between a pickup and a permanent destruction -- on a line where Report 3 measures one key carried by >=4 distinct live actors with all destroy traffic at eid=0, and where the consuming spawn's class and transform are entirely client-chosen. That resurrects a broken container's contents into an unrelated one and lets a client direct where a host container's contents land. (BF-2) The apply writes into whatever GObjStack slot the fresh component resolves, and Boundary 1 tests the component's Player flag, not the slot: an Index of 0 reaches GObjStack[0], the host player's own inventory, which the fresh-empty check waves through whenever that player is carrying nothing. The draft's §9 explicitly argues this dependency away. (BF-3) The apply point sits before the host's element is minted -- OnFinishSpawnFunc only enqueues, and ExpressSpawnedProp mints next pump tick -- so §2.4's mandatory ContainerContents fan-out has no eid, and the host ends up holding contents no client ever sees. (BF-4) Acceptance arm A2, the control-regression gate, asserts a [CUSTODY] DISCARDED line that a correct implementation cannot produce on a wholly host-authored chain, because neither seam is reachable there; as written it fails a working fix or invites a change that endangers CONTROL_PASS. (BF-5) Four of the ten declared omission cases -- including the Player==true refusal and the allocation pre-flight, the two gates that prevent a wrong write -- cannot turn any named assertion red in the engine-free harness the draft specifies.\n\nEach finding has a bounded fix stated against a cited source line. None requires abandoning the host-local approach or reopening the architecture direction; BF-2, BF-4 and BF-5 are closable in the document, BF-1 and BF-3 change the consume predicate and the apply point. Re-review after those, then implementation is a reasonable next step."
  }
 }
]
```
