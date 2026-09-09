# BUG-017 — container custody contract, REVISION 2

```
STATUS            REVISION_2 — DESIGN ONLY. IMPLEMENTATION NOT AUTHORISED.
SUPERSEDES        BUG017-CONTRACT-DESIGN-DRAFT.md (draft 1, immutable, not edited)
ANSWERS           C1/bug017-design-review/VERDICT.md — DESIGN_NOT_CLEAN, RB-1..RB-7
DIRECTION         ADD_CONTRACT_LAYER_KEEP_CURRENT_TRANSPORT (reviewed, f3d6b59d)
                  no transport rewrite, no Relay dependency, no universal serializer
CODE PIN          3af5ddae (upstream/main, kProtocolVersion=152) — every file:line below was
                  RE-OPENED on this ref while writing this revision; nothing is carried from
                  draft 1 on trust (three of draft 1's citations had drifted, and two more are
                  corrected here that the verdict did not catch)
COMPARE PINS      ba6d8c39 (v0.9.0n-b150-dev, proto 150), f690c017 (old main, proto 151)
AUTHORED          2026-09-09, lane C1/bug017-design
REVIEW REQUIRED   yes — re-review of this revision before any patch
```

This document approves no patch. Every fact carries a strength tag and, where it is a source fact,
a `file:line` on `3af5ddae`. Anything the source does not settle is `UNKNOWN` and carries the
observable that would settle it. §11 is the RB closure table.

**Three things the verdict asked for are delivered in a form that differs from the letter of its
§4 change list, each with the reason stated at the point of change and repeated in §11:** the
consume's short clock window (RB-1 item 3, see §2.4), the host world-load clear (RB-6, see §2.3 —
the gates the verdict named are themselves client-only, which would have reproduced RB-6's own
defect), and the apply point (RB-2, see §5.2 — the arm belongs in `OnPropDropIntent`, not at
`HostSpawnPlacedProp`'s `return`, because the authoring peer slot exists only in the former).

---

## 0. The measured inputs this design rests on

### 0.1 Runtime result (R3Q, 2026-09-04, official b150 bytes)

`STRENGTH=MEASURED`. Source: `R3Q/PACKET2/r3q-runtime-packet-2/RESULT.json`,
`R3Q/LANES/ws-runtime-review-codex/{VERDICT.txt,REVIEW.md,STATUS.json}`.

```
CONTROL_RESULT        CONTROL_PASS
PRIMARY_RESULT        EXACT_REPRODUCTION          EXACT_REPRODUCTION=TRUE
PERSISTENCE_RESULT    NOT_RECOVERABLE_AFTER_RELOAD; ABSENT_WITHIN_SEARCHED_SCOPE
PERMANENT_DATA_LOSS   ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE; UNKNOWN_BEYOND_SCOPE
RUNTIME_RERUN_REQUIRED  none
PACKET VERDICT        NOT_CLEAN (binding review) — the gameplay arms are upheld: the seven
                      findings are in the tool-policy and cleanup aggregates and in
                      non-load-bearing record defects, not in the arm outcomes.
BUILD IDENTITY        both roles' main.dll = CFD1A037F073D07F3EAE0E7B189BD70294A5E899E8E9CA8D51DA72731951289C
                      = the official b150 DLL
SUBJECT               CLASS=prop_box_C  LOGICAL_KEY=HlwQ1k8xmW4zxWDHX-VZFw
                      DRIVE_ID=GCPcVmpTGXXJQmxep5xtXg
```

**The exact reproduction chain (PRIMARY arm, client-authored).** Verbatim structure from
`REVIEW.md:143`:

| Marker | Actor | Event |
|---|---|---|
| P0 | — | content established: the keyed box holds drive `GCPcVmpTGXXJQmxep5xtXg`, level 0, size 1.1207 MB |
| P1 | CLIENT | hand-grab of the box |
| P2 | CLIENT | inventory transfer (the box goes into the client's personal store) |
| P3 | CLIENT → HOST | **one** client drop intent; **one** host materialization at `(-1214.3, 2241.6, 6234.1)` |
| P4 | — | untouched interval |
| P5 | HOST | host pickup and open. Same logical key, expressed at EID 5968. **Open, empty, no drive object, no drive panel.** |

**The CONTROL arm (host-authored, same three transitions).** C1 establishes the content; C2–C5 are
the host's own grab / inventory / place / pickup. The key stays `HlwQ1k8xmW4zxWDHX-VZFw`; the
expression remints from EID 4129 to EID 5975; C5 again shows drive `GCPcVmpTGXXJQmxep5xtXg`,
level 0, size 1.1207 MB. `CONTROL_RESULT=CONTROL_PASS`.

The two arms differ in exactly one declared dimension: **which peer authored the whole
grab/inventory/place chain.** The review states the resulting limit and this design inherits it:

> `GAP` — The two arms vary the acting peer for the entire grab/inventory/place chain; they do not
> isolate which transition loses the content. There is one run per route.

**A second GAP this revision declares, which draft 1 did not.** `STRENGTH=UNKNOWN`. The R3Q record
carries **no measured interval between P2 (the box enters the client's personal store) and P3 (the
client re-places it)**. `REVIEW.md:143` names P4 as the untouched interval, which sits *after* the
materialization, not inside the carry. Every timing decision in §2.3 is therefore made against an
unmeasured carry duration, and this revision refuses to bound the mechanism by a wall clock as a
result (RB-6).

**Persistence.** After the primary arm the host saved natively, both peers quit natively, the host
relaunched **from the same primary role root** (world hash `2CBFFC12…`, not the seed `CCCB6FCE…`),
the client rejoined, and the same key reminted as EID 4202: the box was empty; both players'
personal stores held Crowbar x1 and Hook x2 and no drive; the immediate floor region was inspected.
`NOT_RECOVERABLE_AFTER_RELOAD; ABSENT_WITHIN_SEARCHED_SCOPE`. The declared searched scope was the
subject box, both players' searched inventory/store surfaces, the immediate searched world/floor
region, and the reload state actually inspected. **Everything outside that scope is UNKNOWN.** This
design must not be read as, and must not be written up as, unrestricted permanent data loss.

### 0.2 The two incidental observations

`STRENGTH=MEASURED (as observations); CONTEXT_ONLY (as packet inputs)`. The binding review's
`GATE-OBSERVATION-CLOSURE=PASS` records that neither observation is load-bearing for `CONTROL_PASS`,
`EXACT_REPRODUCTION` or the bounded persistence conclusion.

- **OBS-R3Q-C01** — a HOST-placed box vanishes on the CLIENT. The host broadcasts `PropDestroy`
  with `elementId=0` and the box's logical key (the destroy is aimed at the in-hand display husk);
  the client resolves by key and destroys the just-placed box mirror. The host side has a guard,
  the client side does not. Preserved as "the same-key client mirror-loss observation".
- **OBS-R3Q-P01** — connect-replay carries held-lid state that the live update path drops.
  Explicitly incidental in the primary records; narrowed only by the persistence join context.

### 0.3 The measured key non-uniqueness input (new in this revision, and load-bearing)

`STRENGTH=MEASURED (a player-supplied log pair from the shipped b150 build; not produced by this
programme's rig)`. Source:
`C:\AgenticStaging\BugDropbox\Doctaaaa's reports\Report 3\bug_03_floppy_dissolves_deferred_destroy_key_collision.md`.

Four facts from it that this design is built against, each quoted from the file's own evidence rows:

1. `:14, :57` — one logical key, `0zlKei78xf4Ek_bPrYWV0A`, carried by **at least four distinct live
   client actors plus one host actor** inside a three-minute window. Two further keys show the same
   pattern.
2. `:58` — *"All destroy traffic in this incident carries `eid=0`"* — identity on that traffic is
   key-only, so destroy-by-key is ambiguous across same-keyed instances.
3. `:38-40` — the host's **deferred** destroy path armed a destroy-by-key at 21:51:10 and applied it
   at 21:51:16 against a *different* actor that had since taken the key. **The interval is ~6
   seconds.** This is the fact that decides the TTL question in §2.3 and §2.4.
4. `:47-49` — in the same second, `[PROP-DROP] HOST spawned client-placed prop key='0zlKei78…'`
   is immediately followed by `grab_hook[destroy-seam]: HOST broadcasting DESTROY … key='0zlKei78…'
   eid=0`. That is the exact shape of the seam pair this design hooks, on the exact chain, with a
   colliding key.

`FALSIFIER` for treating this as the design's key-uniqueness input: a demonstration that these five
same-keyed actors were the same actor observed at different addresses.

### 0.4 Source-position facts carried forward (R3Q `ws-implementation-readiness`, MVF-0004)

`STRENGTH=SOURCE_PROVEN at the old pin; RE-VERIFIED here on 3af5ddae` (§1 re-cites each):

- `PropDropIntentPayload` is exactly 172 bytes (`protocol.h:1099-1111`); class, stable key, prop row,
  transform, flags, one optional scalar. **No complete record, no contents, no portable
  association.**
- The client-carried `SaveRecord` contains a **client-local** `propInventory.Index`; the contained
  rows live separately in that peer's `GObjStack[Index].obj`.
- `HostSpawnPlacedProp` sets generic identity, finishes the spawn, sets scale and the optional
  scalar. It does **not** call `loadData`, install an old index, read contents, or reattach by key
  (`prop_drop_intent.cpp:158-232`, re-read in full).
- The searched host destroy paths contain **no explicit whole-slot clear/removal**. Positive
  survival of the old slot is therefore `INFERRED`, not implementation evidence.

### 0.5 The reviewed architecture direction

`STRENGTH=SOURCE_PROVEN`. `f3d6b59d:design/relay-adoption/DECISION.md`:
`RELAY_DECISION=ADD_CONTRACT_LAYER_KEEP_CURRENT_TRANSPORT`, labelled `[INFERRED]`, retaining
current transport and keeping Relay separately gated, approving no specific patch. The layer
"must keep independent declarations for IDENTITY, PRESENCE, AUTHORITY, DURABLE_STATE, EVENTS,
TRANSITIONS, MATERIALIZATION, JOIN_SEED, RESYNC and VALIDATION. It must not be a universal
reflection serializer."

`f3d6b59d:design/relay-adoption/BUG-APPLICABILITY-MATRIX.md:51,84,87` — the BUG-017 rows:

| Identity | Class / contract | OPTION_1 | OPTION_2 | OPTION_3 | OPTION_4 |
|---|---|---|---|---|---|
| `BUG-017` | UNKNOWN | U1 | U2 | U3 | U4 |
| `MVF-0001` | CONTRACT_GAP / MATERIALIZATION | U1 | **H-M** | U3 | U4 |
| `MVF-0004` | UNKNOWN; exact reproduction rejected/not reproduced | U1 | U2 | U3 | U4 |

Cell key (`:16-22`): `U2` = "the row does not establish a named contract failure";
`H-M` = "Option 2 can declare and validate the verified host-to-client content-view materialization
obligation". **BUG-017 itself is credited nothing by the frozen matrix.** That matrix was frozen
before the R3Q runtime arms ran. This design does not silently upgrade the cell; it states the new
input (§7, Q-1) and leaves the promotion to the reviewer.

`DECISION.md`, "What proceeds and what waits": *"A small, isolated, confirmed fix may proceed when
a diff-scoped review proves it does not conflict with the ten-axis contract direction and its exact
regression passes. This decision approves no specific patch."* That is the clause this design is
written to satisfy.

### 0.6 Code identity across the three pins

`STRENGTH=MEASURED` — `git diff -w --numstat` and a non-comment line filter over the mechanism
files. **Upheld verbatim from draft 1; both reviews independently re-measured and reproduced it.**

| File | `-w` changed b150→3af5ddae | non-comment changed lines | b150 vs f690c017 |
|---|---:|---:|---|
| `props/container_contents_sync.cpp` | 185/292 | **7** | **identical (0/0)** |
| `props/container_contents_sync.h` | 31/87 | **0** | identical |
| `props/prop_drop_intent.cpp` | 103/106 | **14** | identical |
| `props/prop_drop_intent.h` | 28/41 | **0** | identical |
| `items/save_record_wire.{h,cpp}` | 0/0 | **0** | identical |
| `dispatch/event_dispatch_intent.cpp` | 58/59 | **14** | identical |
| `props/remote_prop_destroy.cpp` | 89/104 | **2** | identical |
| `props/prop_destroy_seam.cpp` | 69/161 | **31** | identical |

**b150 (`ba6d8c39`) and old main (`f690c017`) are byte-identical in every file above.** The
b150→3af5ddae non-comment differences are, in full:

- `container_contents_sync.cpp`: a warn-once latch added around the verb-registration failure log
  (now `container_contents_sync.cpp:698-705`), plus one comment-tag removal. **No mechanism change.**
- `prop_drop_intent.cpp`, `event_dispatch_intent.cpp`, `remote_prop_destroy.cpp`: **include-comment
  and case-comment text only. Zero executable change.**
- `prop_destroy_seam.cpp`: one comment read-once note, plus the removal of a dead
  `SyncDestroyedTrackedProp` function (its `mirror_manager.h` include went with it). **It is that
  removed function — not the file — that is off the BUG-017 chain.** (D-4 wording fix; the file
  itself IS chain step 1, per §1.4 and §8.)
- `protocol.h`: `PropDropIntentPayload`'s body is byte-for-byte identical on b150 and 3af5ddae
  (only its trailing comment and the `static_assert` message text changed). `PropDropIntent = 90`
  (`:483`), `ReelEjectIntent = 104` (`:535`) and `ContainerContents = 118` (`:589`) are the same
  numbers on all three pins. `kProtocolVersion` is 150 / 151 / 152 (`:30`).

`CONCLUSION` — **the BUG-017 mechanism is code-identical between `ba6d8c39`, `f690c017` and
`3af5ddae`.** A fix designed against 3af5ddae describes the same defect that reproduced on the
official b150 bytes. `FALSIFIER`: any executable line above that a reviewer finds materially changed.

---

## 1. The defect as a contract violation

### 1.1 Who owns container contents today

`STRENGTH=SOURCE_PROVEN`.

`docs/devices.md:41-71` is the model: *"A container's contents are not on the container. Every one
of them reads from a single global per-peer array, `saveSlot.GObjStack`, addressed by an index the
actor holds alongside a cached volume."* The lane is
`coop/props/container_contents_sync` (`container_contents_sync.h:1-13`).

The authority rule as shipped (`devices.md:50-57`, `container_contents_sync.cpp:528-558`):

> "There is no request the host could refuse: the item has already moved on the presser before any
> seam of ours exists. So the host arbitrates instead, accepting a client slice only if its author
> edited the truth the host last published and no host-side change is in flight, and relaying to
> every peer except the author."

So the owner is: **the host arbitrates world-container contents; the peer whose verb fired authors
the slice; the host's compare-and-swap decides.** `HostAcceptsClientWrite`
(`container_contents_sync.cpp:528-558`) refuses a stale-base or in-flight-conflict write and
re-publishes host truth to the author.

Two fail-closed boundaries the design must not weaken:

- **Boundary 1** — `IsWorldContainerInventory` (`container_contents_sync.cpp:191-195`): a component
  whose `propInventory_C.Player` reads non-zero, **or whose `Player` offset will not resolve at
  all**, is refused. Its own comment (`:183-190`) states why: *"GObjStack[0] is the local player's
  inventory by construction, baked by the player container's component template"*, and *"the same
  flag is the address assertion in `ue_wrap::inventory::ReadLivePersonalStore`, fail-closed in the
  other direction."* **Six non-definition sites, not five** (draft 1 omitted `:573`; the list read
  as exhaustive): the read side `:418`, the apply side `:486`, the refuse-and-re-publish branch
  inside `ParseAndApply` `:573`, the join seed `:776`, and the dev seams `:797` and `:809`.
- **Boundary 2** — `NeuterNestedIndex` (`container_contents_sync.cpp:258-262`, comment `:252-257`):
  a nested container's `ints[0][0]` is its own `GObjStack` index, a slot number in the **sender's**
  array, so it ships as the sentinel `-1`. The comment records why clearing `ints[]` entirely is
  wrong: *"prop_container::loadData reads ints[0][0] unguarded, Array_Get zero-fills an
  out-of-range read, so an empty array yields index 0, which propInventory::init's `index >= 0`
  guard passes, and the container reuses GObjStack[0], a slot owned by someone else."*

### 1.2 The addressing mismatch: contents are eid-addressed, custody is key-addressed

`STRENGTH=SOURCE_PROVEN`.

`ContainerContents` is **element-id addressed** end to end:
- `protocol.h:589` — "A world container's contents as state, **eid-addressed**."
- `PackContents(eid, baseHash, recs)` — `container_contents_sync.cpp:307-316`, blob
  `[u8 op=0][u32 eid][u64 baseHash][u16 n][n × SerSave]`.
- `ApplyContents(eid, …)` resolves `LivePropActor(eid)` first — `:471-473`.
- The dirty edge resolves identity **at the verb edge** from the actor's eid — `OnVerbEntry`, `:651-654`.

`PropDropIntent` is **logical-key addressed** end to end (`protocol.h:1099-1112`): `className`,
`key` (the persistent cross-peer save key), `propName`, transform, scale, `physFlags`,
`savedScalar`. No element id, by design — *"No element id (the host allocates its own)"*
(`protocol.h:1098`).

A pickup/re-place cycle **destroys the actor and its eid and mints a new one**. The R3Q content
frames show exactly that: same logical key, EID 4129 → 5975 (control), EID 5968 → 4202 across the
reload (primary/persistence). So the one identity that survives the custody transition — the
logical key — is the one identity the contents lane never speaks, and the one identity the contents
lane does speak — the eid — is destroyed by the transition. **That is the contract violation.**

**And the surviving identity is not unique.** §0.3 measures one key on five live actors with all
destroy traffic at `eid=0`. Draft 1 designed a key-only consume predicate against that line. The
whole of §2's binding set exists because of this sentence.

### 1.3 Which message carries the client's intent, and why the host cannot rebuild contents from it

`STRENGTH=SOURCE_PROVEN, except the final clause, which is INFERRED and marked.`

The intent is `ReliableKind::PropDropIntent = 90` (`protocol.h:483`) carrying
`PropDropIntentPayload` (`protocol.h:1099-1112`, `static_assert(sizeof(...) == 172)` at `:1111`,
and `<= 256 - 20 - 8` — one datagram — at `:1112`). Authored by the client in
`prop_drop_intent.cpp:311-353`; routed at `event_dispatch_intent.cpp:258-280`; handled by
`OnPropDropIntent` at `prop_drop_intent.cpp:377-400`.

The payload's complete field list (`protocol.h:1100-1109`) is `className[64]`, `key[32]`,
`propName[32]`, 3 loc + 3 rot + 3 scale floats, `physFlags` + 3 pad, `savedScalar`. **There is no
contents field, no record array, no index, and no room for one**: the struct is a fixed 172-byte
POD on the reliable-datagram lane whose ceiling is 228 bytes (`protocol.h:1112`), while a container
may legally hold up to 512 records (`container_contents_sync.cpp:55`) of a variable-length record
grammar (`save_record_wire.h:55-56`, `save_record.h:29-45`). Contents cannot ride this payload
under any encoding.

The host's materialization is `HostSpawnPlacedProp` (`prop_drop_intent.cpp:158-232`, re-read in
full for this revision). Its complete body is: `FindClass` (`:160`), `BeginDeferredSpawn` (`:168`),
`setKey` through the prop base's cached setter (`:181-197`), `WriteSpParityIdentity` for the
props-table row and the four physics bools (`:201-212`), `FinishDeferredSpawn` (`:214`),
`SetActorScale3D` (`:220-222`), and `ApplySavedScalarForClass` (`:226-230`), then `return actor`
(`:231`). **No `loadData` call, no `propInventory.Index` write, no `GObjStack` touch, no contents
read, no reattach by key, and no element id minted.** The host therefore rebuilds the container from
class + key + transform + parity flags alone.

> **`STRENGTH=INFERRED` (re-tagged from draft 1's `SOURCE_PROVEN`, per RB-4 / verdict §6 item 1).**
> That the fresh `propInventory_C::init` runs inside the finish spawn *with the CDO's default
> index*, and therefore takes an allocate-a-fresh-slot branch, is an inference from the in-tree
> Blueprint-bytecode reading recorded at `container_contents_sync.cpp:252-257`. It was not
> re-measured by this task and it is not re-measured here. It is **load-bearing** for the apply
> strategy (§2.3, §2.5) and it is Q-4, which §7 now files in the blocking column.

`OnPropDropIntent` (`:377-400`) is host-role-gated at `:380`, validates key/class non-empty
(`:383-386`), refuses a duplicate if the host still has the key live (`:390-393`), and calls
`HostSpawnPlacedProp` at `:394`. **It also carries `senderSlot` (`:378`), which
`HostSpawnPlacedProp` does not receive.** That single fact relocates this design's arm point
(§5.2). Nothing in that path consults the destroyed container's prior contents.

`git grep HostSpawnPlacedProp 3af5ddae -- src/votv-coop` returns the definition at `:158` and
**exactly one call, `:394`**. (`:427` is a call to `OnPropDropIntent` from `OnReelEjectIntent`, with
`HostSpawnPlacedProp` named only in its trailing comment; the verdict's ":427 via OnReelEjectIntent"
is transitive, and this revision states it precisely.) `OnReelEjectIntent` (`:402-428`) is the
class-whitelisted client-birth door — reels, desk modules and drives (`:411-421`) — and reaches
`HostSpawnPlacedProp` only through `:427`.

Nor does the client's personal inventory close the gap. The client's pocketed box **is** serialized
as a `SaveRecord` including its `ints[0][0]` index and streamed to the host
(`inventory_wire.cpp:36-45`, `player_inventory_sync.cpp:391-435`), but the host's receive path
**writes the blob to that player's per-slot file and nothing else** — `e.blob = std::move(blob);
… FlushSlot(senderPeerSlot);` (`player_inventory_sync.cpp:426-434`). The index inside it is
client-local and would be actively harmful to install (Boundary 2's argument, applied one level
up). So: the client holds a complete record of the box, the host holds those bytes on disk, and no
code path turns either into the host's world state.

### 1.4 Exactly where the host's contents go

The chain on the host, with a strength tag per link:

1. The client's pickup destroys its local box and broadcasts `PropDestroy{key, elementId}` —
   `prop_destroy_seam.cpp:128-149`. The key is read off the **dying actor** at `:60`
   (`ue_wrap::prop::GetInteractableKeyString`) and **truncated to 31 characters** into the
   `WireKey` at `:119-121`; the key is then parked for a later re-place at `:154-156`
   (`NoteClientKeyedDestroy`, `role() == Role::Client` gated). `STRENGTH=SOURCE_PROVEN`.
2. The host routes it at `event_dispatch_entity.cpp:215-256` (`:254` is the dispatch call) →
   `remote_prop::OnDestroy` (`remote_prop_destroy.cpp:196-198`) → `OnDestroyImpl_` (`:108-192`),
   which builds `keyW` from the wire payload at `:123`, resolves **by eid first** (`:133-135`) and
   **by key only as a fallback** (`:150`), and terminates in `DestroyResolvedLocalActor_`
   (`:73-99`, called at `:190`): log, clear drives (`:87`), release a local grab (`:90`),
   echo-suppress (`:93`), unpin (`:97`), `K2_DestroyActor` (`:98`). `STRENGTH=SOURCE_PROVEN`.
3. **Nothing in steps 1–2 clears, frees, or re-indexes the host's `GObjStack` slot.** The word
   `GObjStack` does not appear in `remote_prop_destroy.cpp`, `prop_destroy_seam.cpp` or
   `event_dispatch_entity.cpp`. `STRENGTH=SOURCE_PROVEN (absence over the searched paths)`.
4. The host's box actor dies. Its `propInventory_C` component dies with it. The slot it addressed
   in the host's `saveSlot.GObjStack` **is not addressed by anything any more.**
   `STRENGTH=INFERRED` — the coop layer does not clear it; whether the native
   `prop_container_C`/`propInventory_C` teardown or the engine's own save-object management clears,
   compacts or reuses it is **UNKNOWN** (Blueprint side, not read by this task).
5. The re-place spawns a fresh `prop_container_C` with a fresh `propInventory_C` whose `Index` is
   the CDO default. `STRENGTH=INFERRED (from the in-tree bytecode-derived comment at
   container_contents_sync.cpp:252-257, not re-measured here)`. See the boxed re-tag in §1.3.
6. The host's box is therefore **empty, not cleared**: a fresh empty slot, with the populated old
   slot **orphaned** — present in the array, addressed by nothing.
   `STRENGTH=INFERRED`. The competing reading — the old slot is emptied or reclaimed by the native
   teardown — is `UNKNOWN` and is exactly what the stack probe in §7 Q-2 settles.
7. The observable end state is `MEASURED`: R3Q P5 and the reload both show the same logical key,
   an open box, no drive object and no drive panel.

**Why the CONTROL arm passes.** `STRENGTH=INFERRED`. When the *host* pockets the box, the game's
own single-player path serialises it into the host's personal store **with its `ints[0][0]` index
intact**, and the host's own re-place runs the native `prop_container::loadData`, which re-installs
that index — the same read `container_contents_sync.cpp:252-257` documents. The index is host-local
and the slot it names is the host's own, so it resolves and the drive is still there. The client
path has no equivalent: `HostSpawnPlacedProp` is not `loadData`, and the client's index would be
invalid on the host even if it were transmitted.

### 1.5 The violation, stated

> **A world container's contents are host-arbitrated state addressed by a transient element id,
> while custody of that container transfers across an actor gap addressed by a persistent logical
> key that is measurably not unique. No message carries the association across the gap, and the
> host's materialization path deliberately reconstructs the container from identity alone. Actor
> lifetime is therefore treated as state lifetime — which `MVF-0004`'s required contract rule 1
> forbids in as many words.**

The doctrine says the same thing prospectively: *"A destroy-and-recreate transition, hold to drop
to store to equip, carries the identity across the actor gap"*
(`docs/coop-sync-doctrine.md:98-99` — corrected from draft 1's `:96-97`). Identity does carry.
**State does not.** The same Step 5 adds, at `:99-101`, *"Keys are load-bearing, case included"*
and the rule that a design must say which of two same-named objects is canonical **before** it
ships — which is the rule §2.4's consume predicate is written to satisfy.

### 1.6 The documentation gap

`STRENGTH=SOURCE_PROVEN`. `docs/devices.md:171-183` ("Who owns what") has **no row for container
contents**, and `docs/devices.md:185-196` ("Wire messages") has **no row for `ContainerContents`**,
although §41-71 of the same file describes the lane in full. Any patch here owes both rows plus a
custody row.

---

## 2. The proposed contract

Ten axes, in the order `DECISION.md` names them. Everything here is a proposal at
`STRENGTH=INFERRED` unless it restates a shipped, cited behaviour.

### 2.0 Conformance to the reviewed axis table

`f3d6b59d:design/relay-adoption/MULTIVOID-CURRENT-MODEL.md:27-38` fixes the concern each axis must
preserve. This design is checked against it row by row before anything else, because the axis
table — not this document — is the reviewed artifact:

| Axis | Concern the declaration must preserve | Where this design honours it |
|---|---|---|
| IDENTITY | "Logical key, EID and live actor binding must not collapse into one value." | §2.1 keeps custody on the key **plus class, author slot and world generation**, contents on the eid, and forbids transmitting the peer-local index |
| PRESENCE | native / mirrored / adopted / absent / retired differ | §2.9 — untouched; a park belongs to the host process, not to any expression |
| AUTHORITY | "Host routing, origin validation and feature-specific authorship are separate decisions." | §2.2 — the host still arbitrates; the client causes but never asserts; the author slot is now bound |
| DURABLE_STATE | "Native save bytes and per-player data are not session event streams." | §2.3 — the park is session-local and never persisted; the player's own store is never written (§2.5, §5.4) |
| EVENTS | "One-shot intents/results are not durable fields." | the drop intent stays a one-shot intent; the contents stay durable state on their own lane |
| TRANSITIONS | "Multi-expression state changes need ordered postconditions." | **§2.4's three-phase machine (PARKED → ARMED → APPLIED), whose APPLIED phase runs at the next-tick adoption where the eid exists.** Draft 1's single-phase machine could not obtain the eid its own postcondition named (RB-2); that row is corrected here |
| MATERIALIZATION | "Actor creation/adoption/destruction occurs on the game/world side, not the network thread." | all three seams are game-thread, inside the existing spawn/destroy/pump paths; no network-thread work is added |
| JOIN_SEED | "Native save load and ordered per-slot replay form a staged bootstrap." | §2.8 — the restored container is an ordinary live container by seed time; no new bootstrap stage |
| RESYNC | "Current convergence is feature-specific and must stay explicit." | §2.8 — the shipped CAS + re-publish path is reused verbatim, not generalised |
| VALIDATION | "Size/version/sender/range/liveness/ownership checks remain per contract." | §2.5's table, all fail-closed, all per-entry-point, now including **slot ownership** |

`MULTIVOID-CURRENT-MODEL.md:40` (corrected from draft 1's `:44`) records at `SOURCE_PROVEN` that
identity already has at least four non-substitutable domains plus per-player ownership as a fifth.
**The defect in §1 is precisely a collapse between two of them** — the design's job is to bridge
them without merging them.

`PILOT-DESIGN.md` is not applicable here: its G01–G17 gates
(`f3d6b59d:design/relay-adoption/PILOT-DESIGN.md:21-37`) all govern OPTION_3, the Relay pilot, and
this design introduces no Relay artifact, no external package and no rule profile. Two of its
gates are worth borrowing as *discipline*:

- **G10** — "exactly one enroller, writer, seed source and materializer". Satisfied: the host is all
  four for a restored container.
- **G16** — one-switch backout. **Draft 1's claim that "the whole mechanism is two three-line call
  sites" is retracted.** The real source footprint is five shipped files gaining a call or a
  declaration, plus a Tick registration (§5.1). G16 is satisfied *at runtime and not in the diff*:
  every one of those five call sites is an unconditional call into the custody module, and the
  module self-gates on a single `container_custody::Enabled()` read (role, session, and a
  compile-time constant). Flipping that one read is the backout; reverting the diff is five files.
  **Say it that way in the commit message; do not claim a one-line revert.**

### 2.1 IDENTITY

- The container's **persistent logical key** is the custody identity, but **never on its own**. It
  survives the actor gap; R3Q measured it surviving three actor gaps and a reload; §0.3 measures it
  colliding across five live actors. The custody record is therefore a **tuple**, not a key.
- The key is stored **wire-normalised**: read off the dying actor with
  `ue_wrap::prop::GetInteractableKeyString` and truncated to 31 characters, the same normalisation
  both senders already apply (`prop_destroy_seam.cpp:119-121`; `FillWireStr`'s `N - 1` cap at
  `prop_drop_intent.cpp:90-93`). An implementer who stores the untruncated actor key silently
  breaks every comparison against a wire key. **Normalise, then compare.**
- The element id remains the **expression** identity and keeps owning steady-state
  `ContainerContents`. It is never used to address a custody record — but it is now the value the
  consume *waits for* (§2.4).
- The peer-local `GObjStack` **`Index` is never transmitted and never installed from the wire.**
  (`MVF-0004` required-contract rule 3; Boundary 2's existing argument.)
- One row per key: a second park for a key that already has one **replaces** it (latest wins on the
  same peer, logged as `REPLACED`), never accumulates. The replaced set is dropped — which is
  today's outcome for both containers, so the replacement is not a regression.

The custody record, in full:

```
CustodyPark {
    key          std::wstring  the DYING ACTOR's own key, wire-normalised (<= 31 chars)
    cls          std::wstring  R::ClassNameOf(dying actor)                    <-- RB-1 item 2
    records      std::vector<SaveRecord>, ordered, nested indices PRESERVED
    recordCount  int32
    packedBytes  size_t        the size PackContents would produce            <-- see 2.3 bounds
    contentHash  uint64        FNV-64 over the canonical pack (ContentHash's shape)
    parkedIndex  int32         the host's own slot number; diagnostics only; NEVER transmitted
    authorSlot   uint8         the peer slot whose PropDestroy caused this    <-- RB-1 item 3
    worldGen     uint32        ue_wrap::world_identity::Generation() at capture
    at           steady_clock::time_point
    armed        bool          set once, by the first matching host spawn     <-- RB-1 item 3
}
```

### 2.2 AUTHORITY

- **World containers are host-authoritative.** No change: the host already arbitrates
  (`container_contents_sync.cpp:528-558`).
- A client may *cause* a custody transition (it authors `PropDropIntent`); it may never *assert*
  container contents on the host outside the existing CAS-gated `ContainerContents` path. Draft 1
  broke this without meaning to: a key-only park plus a client-chosen class and transform let a
  client direct where a host container's contents landed. The class binding (§2.1) and the author
  binding (§2.4) are what restore it.
- **The authoring peer is bound.** The custody transition's two halves — the destroy that fills the
  park and the drop intent that consumes it — must come from the **same peer slot**. Neither half
  carries a slot to the seam today: `PropDestroyPayload` has no sender field (`protocol.h:1088-1093`)
  and `remote_prop::OnDestroy` does not take one (`remote_prop_destroy.cpp:196`), while
  `OnPropDropIntent` does (`prop_drop_intent.cpp:378`). §5.2 adds one latch line at the destroy
  dispatch (`event_dispatch_entity.cpp:253-254`, where `msg.senderPeerSlot` is in scope) to carry it.
  **A capture with no latched slot is refused** (`reason=author-unknown`), which is fail-closed and
  has a deliberate, valuable side effect: the deferred destroy-by-key path
  (`quiescence_drain.cpp:249` → `TryApplyDestroy`, `remote_prop_destroy.cpp:204-207`) has no sender
  slot, so **the very path §0.3 measures firing against the wrong actor can never fill a park.**
- `propInventory_C.Player == true` — or an unresolvable `Player` flag — is **refused at every new
  entry point**, exactly as `IsWorldContainerInventory` does today
  (`container_contents_sync.cpp:191-195`). This is a hard invariant, not a policy: a violation
  destroys a player's inventory.

### 2.3 DURABLE_STATE — the host-side custody park (the primary mechanism)

The design's core claim, unchanged and upheld by the verdict: **BUG-017 is closable entirely on the
host, with no new wire kind, op, format or size**, because in the measured chain the host held the
contents before the client took the box.

**Capture point (host).** In `DestroyResolvedLocalActor_` (`remote_prop_destroy.cpp:73-99`) — the
actor is dereferenced live from `:79` (the log line) through `:97`, and the engine call that kills
it is `:98`. The capture goes **before `:98`**, after `native_pile_mirror::Unpin` at `:97`.

`remote_prop_destroy.cpp` has **no `IsHost()` and no `role()` accessor at all** (`git grep
'IsHost\|role()\|Role::' 3af5ddae -- …/remote_prop_destroy.cpp` returns nothing — re-verified here).
Draft 1's `if (IsHost()) …` three-liner cannot be written there. The custody module holds its own
session pointer instead (the shipped `session_holder` pattern, `prop_drop_intent.cpp:46-47`) and
self-gates, so the call site is **one unconditional line**.

**Capture entry conditions (all must hold; each failure is a named, logged refusal):**

| # | Condition | Refusal reason |
|---|---|---|
| 1 | module enabled; session connected; `role() == Host` | *(silent — the module is not installed for this role)* |
| 2 | a sender slot was latched for this destroy (§2.2) | `author-unknown` |
| 3 | the dying actor's own key resolves, is non-empty and is not `"None"` | `key-empty` |
| 4 | that key, wire-normalised, **equals `payload.key`** (`remote_prop_destroy.cpp:123`) | `key-mismatch` |
| 5 | the dying actor is a container (`IsContainerActor`, `container_contents_sync.cpp:247-250`) | `not-container` |
| 6 | its inventory resolves and passes **Boundary 1** (`:191-195`) | `boundary1` |
| 7 | `GObjStackSlot` resolves (`:200-211`) | `slot-unresolved` |
| 8 | record count ≤ `kMaxRecordsPerContainer` (512, `:55`; `ReadContents` already refuses above it at `:275-279`) | `cap-512` |
| 9 | the packed size ≤ `blob_chunks::MaxBlobBytes()` (56,100 B) | `over-blob-ceiling` |

Conditions 3 and 4 are RB-1 item 1. Condition 3 also closes the third capture entry the
reconciliation found — `kerfur_convert_client.cpp:310-313` synthesises a `PropDestroyPayload` with
`dp.key.len = 0` and calls `remote_prop::OnDestroy` locally. **Re-verified here and narrowed:** the
enclosing `OnKerfurConvert` returns early unless `role() == Role::Client`
(`kerfur_convert_client.cpp:269`), so on the host that entry is unreachable in the first place;
condition 3 is defence-in-depth against a future caller, not a live hole.

Condition 9 is new in this revision and is the host-local answer to the verdict's optional finding
about `kOpCustody` having no size bound. `BroadcastContainer` refuses an over-ceiling blob at
`container_contents_sync.cpp:348-352`. A record set the host could park but never fan out would be
restored into a container **no peer would ever see** — the exact host/client divergence RB-2 is
about. Refusing at capture keeps the mechanism's coverage equal to what the lane can actually
publish, and it gives the park store a hard byte bound: 32 × 56,100 B ≈ 1.8 MB worst case.

**Capture must not neuter nested indices.** `ReadContents` (`container_contents_sync.cpp:271-289`)
calls `NeuterNestedIndex` at `:285` — under a `RecordIsNestedContainer(r)` test (`:228-233`) — because
its output is destined for the wire. A **host-local** park keeps the same host's array, so a nested
container's `ints[0][0]` is still valid and neutering it would silently empty every nested container
on re-attach. The capture therefore needs a `bool neuterNested` parameter on `ReadContents` (default
`true`, wire behaviour unchanged; `git grep -n ReadContents` returns the definition at `:271` and
exactly one caller, `BroadcastContainer` at `:334`, which takes the default). **This is still the
sharpest trap in the design; a reviewer should check it first.** It is assertion 11 in §4.2.

**Park store bounds — and why there is no wall-clock safety window.**

Draft 1 aged the park on a 60 s TTL, "aged only outside a join-snapshot bracket". The bracket half
is dead on the host, and the clock half is the wrong instrument. Both are replaced:

- **`kMaxCustodyParks = 32`**, a bounded FIFO plus a map holding exactly the same keys, mirroring
  the shipped park set's invariant (`prop_drop_intent.cpp:67-86`: set and FIFO hold the same keys,
  and `UnparkKey` at `:81-86` touches both, because a consume that dropped only the set desynced
  them and the evict later popped a stale copy). Overflow evicts the oldest, logged.
- **The join-snapshot bracket clause is deleted.** `NoteJoinSnapshotBracket` is client-only:
  its own definition comment reads *"From event_feed's client-side SnapshotBegin and
  SnapshotComplete dispatch"* (`container_contents_sync.cpp:666`); `git grep` returns exactly two
  call sites, `event_feed.cpp:361` and `:424`; and both handlers return early on the host —
  `if (session.role() == net::Role::Host) break;  // self-echo guard (host doesn't load-screen)`
  at `:341`, and the same break at `:413`. `g_joinBracketOpen` is therefore permanently false on the
  host, which is the only role that holds a custody park. The shipped park sweep it guards is itself
  host-excluded: `if (!IsHost()) SweepParked();` (`container_contents_sync.cpp:726`). Importing that
  reasoning was a gate that cannot fire.
- **No wall-clock consume window.** RB-1 item 3 asked for a window "materially shorter than 60 s".
  §0.3 fact 3 measures the collision this would guard against firing **6 seconds** after arming —
  inside any such window. A short clock is therefore neither necessary nor sufficient for safety
  here, and it is actively harmful for coverage, because the reported chain is store-then-re-place
  (BUG-3 / R3Q P2, *"the box goes into the client's personal store"*) with an **unmeasured** carry
  duration (§0.1). The safety property is supplied causally instead, in §2.4, and it is strictly
  stronger than a clock.
- **What bounds a park instead**, all host-valid, each a named refusal or clear:
  - **World generation.** `ue_wrap::world_identity::Generation()` (`world_identity.h:55-58`,
    *"Bumped every time CurrentWorld is observed to change: a cheap staleness token"*) is stamped at
    capture. A park whose stamp differs from the current generation is cleared on the next sweep,
    `reason=world-changed`. **This replaces the verdict's suggested
    `world_load_episode::InEpisode` / `InReconcileWindow` clear, which would have repeated RB-6's
    own defect.** Re-verified: `world_load_episode` is by its header's first line *"the **client**
    world-load lifecycle"*; `InEpisode`'s only arm is `session_runtime.cpp:317`, whose own comment
    at `:315` calls it *"The sole, client-only arm site"*; `RaiseReconcileForReload` sits behind
    `if (session.role() == Role::Host) return;` (`net_pump.cpp:234`, raised at `:243`); and
    `NoteReconcileBegin` / `NoteReconcileComplete` are behind the same host breaks at
    `event_feed.cpp:341` and `:413`. Both predicates read false on a host, always.
  - **Author liveness.** A park whose `authorSlot` is no longer connected (`Session::IsSlotConnected`,
    the same call `RelayToOthers` uses at `container_contents_sync.cpp:386`) is cleared,
    `reason=author-left`. A peer that disconnected cannot author the matching place.
  - **An absolute age ceiling, `kCustodyParkCeilingSec = 900`,** as a leak guard only, not a safety
    bound — the memory is already bounded harder by the park cap and condition 9. Logged as
    `EXPIRED`.
  - **Session teardown** clears everything (`OnDisconnect`, the shape at
    `container_contents_sync.cpp:817-831`).
- **Where the sweep runs.** The host runs no contents park sweep today (`:726`), so the custody
  sweep lives in the custody module's own `Tick` (§5.2), which is where the consume lives anyway.

**Apply strategy.** Write the parked records into **the slot the host's own fresh component
resolved**, through the shipped apply primitives — the `EngineAlloc` pre-flight
(`container_contents_sync.cpp:494-500`), `SR::AllocZeroed` (`:503`), `SR::WriteSaveRecord`
(`:508-510`), `SR::WriteArrHeader` (`:516`) — then `RederiveManagedState` (`:453-457`, called at
`:521`).

Writing into the *fresh* slot, rather than re-pointing `Index` at `parkedIndex`, is still the
recommended strategy because (a) it reuses a shipped, reviewed write path verbatim and (b) it needs
no answer to the orphan-slot question (Q-2). **Draft 1's third reason — "it cannot alias a slot
another component now owns" — is retracted.** `IsWorldContainerInventory` reads the `Player` byte
off the *component* (`:191-195`); it says nothing about which slot that component's `Index` names,
and `GObjStackSlot` refuses only `idx < 0` (`:207`) and `idx >= stack.num` (`:209`) — **index 0 is
accepted**, and index 0 is the host player's own inventory by construction
(`container_contents_sync.cpp:183-186`; `ue_wrap/actors/inventory.h:64-72`, whose
`LivePersonalStore::slotIndex` comment reads *"propInventory.Index -- measured to be 0 by
construction"*). Aliasing is therefore possible and must be refused explicitly. §2.5 does that.

The alternative — re-install `parkedIndex` and orphan the fresh slot — remains gated on Q-2 and on
`MVF-0004` rule 11 (never remove middle `GObjStack` slots and shift live indices).

**Refusal rule — never overwrite newer state.** The park is consumed only if the fresh container's
slot resolves **and reads as empty** (`SR::ReadArr(slot, 0).num == 0`). If something already
restored contents, the park is **discarded with a log line and no write.**

Draft 1 justified this rule as preventing a double-apply "on the host-authored chain". **That
justification is withdrawn**: neither seam is reachable on a wholly host-authored chain (§4.1, A2),
so the mechanism never runs on that direction and the sentence *"safe to run on both authorship
directions with one code path"* is simply false. The rule is kept as defence-in-depth against the
cases that **can** reach it:

1. a duplicated or re-delivered `PropDropIntent` (the shipped duplicate guard at `:390-393` catches
   the common case, but only while the host still holds the key live);
2. a future native restore inside `FinishDeferredSpawn` — i.e. if Q-4 resolves the other way and
   `propInventory_C::init` does restore, the discard branch is what keeps this patch inert rather
   than destructive;
3. an inbound `ContainerContents` blob that landed on the new eid between the adoption and the
   custody Tick.

### 2.4 TRANSITIONS and MATERIALIZATION — the three-phase machine

Draft 1 applied at `prop_drop_intent.cpp:231` and then marked the new eid dirty. **There is no eid
at that statement.** Re-verified: `HostSpawnPlacedProp`'s body ends at `return actor` (`:231`) with
no element minted; the host's `FinishSpawningActor` callback *"only ENQUEUES, and
DrainPendingSpawns adopts on the next tick"* (`host_spawn_watcher.h:29-32`), and
`OnFinishSpawnFunc` (`host_spawn_watcher.cpp:219-238`) does exactly that — `g_pendingFinished.push_back`
at `:237`; `prop_drop_intent.cpp:223-225` says *"the next-tick express drain re-reads the live
actor"*; the eid is minted inside that drain, `DrainPendingSpawns` (`host_spawn_watcher.cpp:352-383`)
→ `prop_lifecycle::ExpressSpawnedProp` (`:374`; defined `prop_lifecycle.cpp:371-373`) → the
Init-POST funnel → `MarkPropElement` (`prop_lifecycle.cpp:214`). `DrainDirty`
(`container_contents_sync.cpp:400-421`) is edge-driven from `g_dirty`, written only at
`OnVerbEntry:654`, and resolves forward from the eid at `:415`. So the machine gains a phase:

```
LIVE
 └─(host RECEIVES a keyed PropDestroy from slot S for a live world container; §2.3 conditions 1-9)
      ──▶ PARKED{key, cls, records, hash, parkedIndex, authorSlot=S, worldGen, at, armed=false}

PARKED
 ├─(host spawns from slot S's drop intent under the same key and class; §2.4 arm conditions)
 │    ──▶ ARMED{park, actor, actorInternalIdx, armPumpTick}         park.armed := true, ONCE
 ├─(a second park for the same key)              ──▶ REPLACED (latest wins; logged)
 ├─(authorSlot disconnects)                      ──▶ CLEARED  (author-left)
 ├─(worldGen != Generation())                    ──▶ CLEARED  (world-changed)
 ├─(age > kCustodyParkCeilingSec)                ──▶ EXPIRED  (leak guard; logged)
 └─(park cap exceeded)                           ──▶ EVICTED  (oldest first; logged)

ARMED  (evaluated in the custody module's own Tick, AFTER host_spawn_watcher::DrainPendingSpawns)
 ├─(eid resolves; every §2.5 check passes)       ──▶ APPLIED ─▶ MarkDirty(eid) ─▶ LIVE
 ├─(eid resolves; fresh slot NOT empty)          ──▶ DISCARDED (something already restored)
 ├─(eid resolves; any §2.5 check fails)          ──▶ REFUSED(reason)
 └─(eid unresolved after kMaxAdoptTries ticks)   ──▶ REFUSED(eid-unresolved)

EVERY exit from ARMED retires the park. Arm once, consume once.
```

**The arm conditions (RB-1 item 3, delivered causally rather than on a clock):**

1. The spawning peer slot equals `park.authorSlot`. Available because the arm is placed in
   `OnPropDropIntent`, which has `senderSlot` (`prop_drop_intent.cpp:378`), not in
   `HostSpawnPlacedProp`, which does not.
2. `R::ClassNameOf(actor)` equals `park.cls` (RB-1 item 2).
3. The intent key, wire-normalised, equals `park.key`.
4. `park.armed == false` — **only the first matching host spawn after that destroy may arm.**
5. `park.worldGen == world_identity::Generation()`.

Together these are strictly stronger than the "single generation / same authoring peer / short
window" the verdict asked for: same peer, same class, same key, first spawn only, same world. The
clock is not part of the predicate, for the reason given in §2.3.

**The `MarkPropElement` re-key interaction (RB-1 item 4).** `prop_lifecycle.cpp:211-214` states
*"Mark may re-key a duplicate (the host is the key authority), so the payload carries the enrolled
key"*, and the re-key runs in the next-tick express drain — **after** the arm and **before** the
consume. Re-read at `prop_element_tracker.cpp:243-275`: the host re-keys only when
`FindLiveActorByKey(enrollKey)` returns a **different live actor** (`:253-254`), and then mints a
fresh key (`:261`) and enrols under it (`:267`). The rule:

> **At the consume, re-read the adopted actor's enrolled key and compare it with `park.key`. If they
> differ, `MarkPropElement` re-keyed this container because another live actor already carried the
> key — i.e. exactly the collision family §0.3 measures. REFUSE the consume, `reason=key-rekeyed`,
> and log both keys.** The park is retired. This is not a lost opportunity; it is the one case where
> writing the parked contents into this container would be provably guessing.

That check is also the cheapest key-collision detector the programme has: a `key-rekeyed` refusal in
a host log is a measurement that a collision family is live in that world.

**The consume's own postcondition — the fan-out.** `APPLIED` must be followed by the host's ordinary
`ContainerContents` fan-out for the new eid, so every peer converges through the shipped lane. The
trigger is to mark the new eid dirty (the set `OnVerbEntry` writes at
`container_contents_sync.cpp:654`) and let `DrainDirty` (`:400-421`) ship it. `force=false` is
correct: `BroadcastContainer` skips only when `g_sentHash` already holds this eid's hash
(`:344-347`), and a freshly minted eid has no entry in any of the four maps — they are keyed by eid
and written only at `:359` (`g_sentHash`), `:364` (`g_publishedHash`), `:518` (`g_appliedHash`) and
`:520` (`g_baseHash`). **That closes Q-7 at `SOURCE_PROVEN`, and it closes RB-2's second
consequence**: the fan-out's own success path sets `g_publishedHash[eid] = h` at `:364`, so the
first client-authored write against the restored container is judged against a real published base
by `HostAcceptsClientWrite` (`:528-558`) instead of being refused as a stale base against 0.

**Latency, stated honestly.** The custody Tick runs at `subsystems.cpp:547`, immediately after
`DrainPendingSpawns`; `container_contents_sync::Tick()` — which owns `DrainDirty` — already ran
earlier in the same pass, at `:515`, behind a 250 ms sweep gate (`kSweepMs = 250`,
`container_contents_sync.cpp:51,717-719`). So the fan-out ships on the **next** contents sweep:
one pump tick plus up to 250 ms after the adoption. That is invisible in play and it must be stated,
because an arm that greps for the fan-out line in the same log second as the reattach line would
score a false negative.

### 2.5 EVENTS and VALIDATION

No new event kind under the primary mechanism. Validation at both new entry points, all fail-closed.
Capture-side conditions are §2.3's table. Consume-side:

| # | Check | Refusal reason |
|---|---|---|
| 1 | the adopted actor is still live (`R::IsLiveByIndex`, the guard `DrainPendingSpawns` uses at `host_spawn_watcher.cpp:360`) | `actor-dead` |
| 2 | the eid resolves (`Registry::EidForActor` / `PT::GetPropElementIdForActor`, as at `host_spawn_watcher.cpp:363,375`) | `eid-unresolved` (after `kMaxAdoptTries`) |
| 3 | the actor's **enrolled** key equals `park.key` (post-`MarkPropElement`) | `key-rekeyed` |
| 4 | `R::ClassNameOf(actor)` equals `park.cls` | `class-mismatch` |
| 5 | the actor is a container (`IsContainerActor`, `:247-250`) | `not-container` |
| 6 | its inventory resolves and passes **Boundary 1** (`:191-195`) | `boundary1` |
| 7 | `GObjStackSlot` resolves (`:200-211`) | `slot-unresolved` |
| 8 | **the resolved `Index` is not 0** | `slot-aliased` |
| 9 | **the resolved `Index` != the live personal store's index** (`ue_wrap::inventory::ReadLivePersonalStore(out).slotIndex`, `inventory.h:87`, `inventory.cpp:138-178`; the reader is fail-closed on `Player == 0` at `inventory.cpp:155-165`) | `slot-aliased` |
| 10 | **the resolved `Index` != any other live world container's `Index`** (enumerate with `SnapshotWorldContainers`, `container_contents_sync.h:62-63`, which applies Boundary 1 itself at `:797`, so it enumerates world containers only) | `slot-aliased` |
| 11 | the fresh slot reads empty (`SR::ReadArr(slot, 0).num == 0`) | `fresh-not-empty` → `DISCARDED` |
| 12 | the `EngineAlloc` pre-flight succeeds (`:494-500`) | `alloc-preflight` |
| 13 | `SR::AllocZeroed` succeeds for `n > 0` (`:503-507`) | `alloc-failed` |

Rows 8, 9 and 10 are RB-3, and each is cheap. Row 8's justification is `inventory.h:64-72`:
*"the class's component template serializes index=0, player=true, customVolume=50000, so
GObjStack[0] IS the player's inventory"*. Row 10 needs one accessor that does not exist today —
`GObjStackSlot` is private to `container_contents_sync.cpp` — so §5.1 adds a public
`WorldContainerSlotIndex(void* inv, int32_t& out)` beside the other wrappers rather than a second
reader (RULE 2, "ONE implementation", `save_record_wire.h:8`).

### 2.6 The wire shape — and whether a new kind is needed

**Under the primary (host-local) mechanism: no new `ReliableKind`, no new payload, no new blob op,
no format or size change, and no protocol bump.** That is the whole reason to prefer it.

**It is not, however, "zero bytes" or "no wire change at all", and draft 1's wording is corrected
here.** A restored container is marked dirty and fanned out as a `ContainerContents` blob that the
unfixed build does not send at that moment. The traffic is **wire-format compatible with b150** —
same kind (118), same op (0), same grammar, same size rules — but it is not byte-identical traffic.
State it as format compatibility.

**If the reviewer or the stack probe shows the host-local park is insufficient** — the identified
case is a container whose contents the host *never held*, e.g. a client filled it while carrying it,
or the container was born on the client (the `ReelEjectIntent` / container-extract birth path,
`prop_drop_intent.cpp:298-309`) — then a bounded transfer is needed. In that case:

- **Do not add a field to `PropDropIntentPayload`.** It is a fixed 172-byte single-datagram POD
  (`protocol.h:1111-1112`); there is no trailing-blob mechanism on that lane and contents are
  unbounded up to 512 variable-length records.
- **Do not add a new `ReliableKind`.** Reuse `ReliableKind::ContainerContents = 118`
  (`protocol.h:589`), already a chunked `BlobChunkPayload` blob (220 data bytes per chunk,
  `protocol.h:1367-1374`; ceiling 220 × 255 = 56,100 bytes, `blob_chunks.h:45-47`) with a
  reassembler, a sender-slot acceptance matrix (`container_contents_sync.cpp:735-742`) and a
  park/TTL path.
- **Add one op byte to that blob's existing grammar.** The current grammar is
  `[u8 op=0][u32 eid][u64 baseHash][u16 n][n × SerSave]` (`PackContents`, `:307-316`). Add:

```
kOpCustody = 1
[u8 op=1][u8 keyLen][keyLen × char  (the logical key, <= 31, WireKey grammar)]
[u64 custodyToken]   host-issued; 0 = "the author had none"
[u64 baseHash]       the last host truth the author applied for this container
[u16 n][n × SerSave] the ordered records, nested indices NEUTERED (Boundary 2 applies)
```

  Direction: **client → host only**. The host answers with an ordinary `kOpContents` fan-out for
  the new eid; it never relays a custody blob. The host validates the key, resolves the container
  **locally** by key (`prop_element_tracker::ResolveLiveActorByKey`, already used at
  `prop_drop_intent.cpp:390`), applies Boundary 1, and runs the same CAS shape as
  `HostAcceptsClientWrite` against the custody token / base hash.
- **Size bound, required** (the verdict's optional finding, adopted): check the packed blob against
  `blob_chunks::MaxBlobBytes()` **before** `ChunkAndSend`, and refuse with a named warning, exactly
  as `BroadcastContainer` does at `container_contents_sync.cpp:348-352`. 512 variable-length records
  can exceed 56,100 bytes.
- **Version gate.** The op byte is the gate on the receive side: the shipped parser already refuses
  an unknown op — `if (!W::RdU8(blob, o, op) || op != kOpContents) return Ingest::Handled;`
  (`:563`) — so an old peer treats a custody blob as `Handled` and never relays it. That is a
  graceful *degrade*, not compatibility: the fix silently does nothing. Doctrine Step 7
  (`coop-sync-doctrine.md:111-119`) requires the bump anyway: *"A new wire format or field bumps the
  build number in the same commit."*

### 2.7 Compatibility — two lines, two answers

`STRENGTH=SOURCE_PROVEN` on the mechanism: a protocol mismatch is refused before a peer joins.
`session.cpp:262-267` peeks the peer's version on an unparsed header and retires the pending
connection with `"protocol mismatch: peer=v%u, ours=v%u"`; `peer_admission_internal.h:57-58` binds
`kProtocolVersion` into the admission preimage; `session_manager.cpp:592` refuses a browser join
whose advertised `proto` differs. There is no mixed-version lobby to break.

| Line | Rule | What this design allows |
|---|---|---|
| **Upstream** (`3af5ddae`, proto 152) | mismatched peers are refused before joining, so a bump costs a cohort update, not correctness | Either option is acceptable. If the custody op is ever added, bump to 153 in the same commit and add the router/`code-map.md` row. |
| **PRIVATE b150 line** (`ba6d8c39` + `private/b150-aligned`) | **the wire format must not change** — this line has to interoperate with the shipped b150 build players actually have | **Only the primary host-local mechanism is admissible.** It adds no kind, no op, no format change and no size change, and leaves `kProtocolVersion = 150` untouched. The custody op is forbidden on this line. |

Because the mechanism files are code-identical across the three pins (§0.6), the same patch text
applies to both lines; only the protocol constant and the docs rows differ. **State the b150
constraint in the commit message**: a later contributor who adds the custody op to the private line
silently breaks every b150 peer's ability to join.

### 2.8 JOIN_SEED and RESYNC

- A custody park is **session-local and never persisted**. It is cleared on disconnect alongside
  the lane's other session state (`container_contents_sync.cpp:817-831` is the shape;
  `prop_drop_intent.cpp:430-435` is the park equivalent).
- A joiner is seeded by the shipped connect broadcast (`QueueConnectBroadcastForSlot`,
  `container_contents_sync.cpp:761-780`), which walks every live world container and ships a forced
  slice. **A restored container is just a live world container by then, so the late-join row needs
  no new mechanism** — but doctrine Step 6 (`coop-sync-doctrine.md:103-110`) requires the row to be
  written down and listed in `status.md` anyway.
- Resync: unchanged. The host's CAS + re-publish-to-the-author path (`:569-578`) already converges a
  diverged author, and §2.4 shows the restored eid now enters that path with a real published base.

### 2.9 PRESENCE

Not touched. No new presence obligation; a park belongs to the host process, not to a peer.

### 2.10 Entry, exit and error behaviour

**Entry conditions for a park:** §2.3's nine-row table. The four that draft 1 did not have are
"a sender slot was latched", "the dying actor's own key is non-empty", "that key equals
`payload.key`", and "the packed set fits the blob ceiling".

**Exit conditions (the park is consumed):** §2.4's arm conditions, then §2.5's thirteen consume
checks. On any outcome the park is removed from both the map and the FIFO in one operation.

**Error behaviour** — every failure is a refusal that leaves state exactly as it is today. A failed
park means the bug reproduces (no regression). A failed apply means the bug reproduces (no
regression). **No path may write a partial record set, write into a slot it could not verify, write
into a slot it could not prove exclusively owned, or write into an inventory whose `Player` flag it
could not read.** Doctrine's forbidden-patterns list (`coop-sync-doctrine.md:129-138`) rules out the
shortcuts: no receive-side gate protecting shared state, no second implementation of the record
codec, no per-frame array scan. Note that §2.5 row 10's enumeration is per-consume, not per-frame:
a consume happens at most once per drop intent.

### 2.11 Observability — what lets the rig prove it

Every line prefixed `[CUSTODY]` so one grep separates the lane, and every one carries the logical
key, so the rig can bind a line to the R3Q subject key `HlwQ1k8xmW4zxWDHX-VZFw`:

| Event | Line (shape) | Why the rig needs it |
|---|---|---|
| park taken | `[CUSTODY] HOST parked key='%ls' cls='%ls' records=%d bytes=%zu hash=%llu index=%d slot=%u gen=%u` | proves the capture point fired, with the record count that must reappear and the orphaned index Q-2/Q-4 want |
| park refused | `[CUSTODY] HOST park REFUSED key='%ls' reason=<author-unknown\|key-empty\|key-mismatch\|not-container\|boundary1\|slot-unresolved\|cap-512\|over-blob-ceiling>` | a silent non-park is indistinguishable from a dead hook; this is the falsifier's home |
| park armed | `[CUSTODY] HOST park ARMED key='%ls' actor=%p slot=%u (awaiting adoption)` | separates "the intent matched" from "the eid resolved"; without it RB-2's failure mode is invisible |
| **apply refused** | `[CUSTODY] HOST reattach REFUSED key='%ls' reason=<not-container\|boundary1\|slot-unresolved\|slot-aliased\|alloc-preflight\|actor-dead\|eid-unresolved\|key-rekeyed\|class-mismatch\|alloc-failed> index=%d` | **RB-4.** The first five reasons are the verdict's required set; the rest are this revision's added refusals. Without this row, A1 cannot tell "no slot yet" from "the hook never ran" |
| apply | `[CUSTODY] HOST reattached key='%ls' eid=%u records=%d hash=%llu index=%d` | the primary observable: same key, **new** eid, same record count and hash, **and the resolved index** (RB-3) |
| fan-out | `[CUSTODY] HOST fanout QUEUED key='%ls' eid=%u records=%d` | RB-2's postcondition, scorable on the host without a client arm; the shipped `container_contents: eid=%u shipped %zu records` line (`:370-373`) is the confirmation on the next sweep |
| discard | `[CUSTODY] HOST park DISCARDED key='%ls' -- fresh slot already holds %d records` | proves nothing was double-applied |
| expire / evict / clear | `[CUSTODY] HOST park EXPIRED key='%ls' after %ds` / `... EVICTED (cap %zu)` / `... CLEARED reason=<author-left\|world-changed\|session>` | bounds the store; an expiry or clear during a rig arm invalidates that arm |
| first entry | one-shot `[CUSTODY] capture hook ENTERED for the first time on this peer (role=%s)` and the same for the Tick | the shipped lane's own convention (`container_contents_sync.cpp:630-639`: *"a resolved verb name does not prove this callback runs … If this line is absent from a log, the lane is dead"*) |

Counters, readable from the log at teardown: `parksTaken`, `parksRefused` (by reason),
`parksArmed`, `parksApplied`, `parksRefusedAtApply` (by reason), `parksDiscarded`, `parksExpired`,
`parksEvicted`, `parksCleared`. Plus the already-shipped `ContentsDigest(eid, count, vol)`
(`container_contents_sync.cpp:803-815`) as the cross-peer comparison the two-peer smoke prints on
both roles.

---

## 3. How OBS-R3Q-C01 interacts with this chain

`STRENGTH=MEASURED (the observation); INFERRED (the interaction)`.

**What C01 is.** A HOST-placed box vanishes on the CLIENT: the host broadcasts `PropDestroy` with
`elementId = 0` and the box's logical key — the destroy is aimed at the in-hand display husk, which
carries the same key as the box — and the client, finding no eid to resolve, falls back to
`ResolveLiveActorByKey` (`remote_prop_destroy.cpp:136-150`) and destroys the just-placed box mirror.

**Corroborated by §0.3.** Report 3 captures the same pair in one second on the client-place
direction: `[PROP-DROP] HOST spawned client-placed prop key='0zlKei78…'` immediately followed by
`grab_hook[destroy-seam]: HOST broadcasting DESTROY … key='0zlKei78…' eid=0`
(`bug_03_…md:47-49`), and the host itself broadcasting a same-key `eid=0` destroy during trash
disposal at 22:11:03 (`:60`). C01 is not a one-run curiosity.

**Why the zero eid is legitimate wire.** `prop_destroy_seam.cpp:131` writes
`dp.elementId = (destroyEid == kInvalidId) ? 0u : destroyEid;` — 0 is the protocol's
"sender had no Element" sentinel (`protocol.h:1088-1092`) — and the husk is untracked, so 0 is
correct for it. `event_dispatch_entity.cpp:236-242` accepts `elementId == 0` deliberately.

**Why the host has a guard and the client does not.** For the *client-place* direction the guard is
ordering, documented in the header: `prop_drop_intent.h:12-15` — the place also destroys the in-hand
husk, and if that crossed as `DESTROY(key)` first, "the host processes it against a rock it no
longer has -- a no-op -- and only then spawns from the intent, and the spawn survives. Authoring in
the same tick lets the husk-destroy kill it instead." The client-side `Tick` drain
(`prop_drop_intent.cpp:263-363`) implements that "one tick late". **There is no equivalent for the
host-place direction.**

**Interaction with BUG-017, and the answer.**

- **On the primary (client-authored) chain, C01 is not on the critical path.** The placing peer is
  the client; the measurement peer is the host; the host resolves the client's husk destroy against
  a key it no longer holds (a no-op, by the header's own argument). The R3Q primary arm recorded
  "one client drop intent plus one host materialization" — exactly one — and the box existed on the
  host at P5. **C01 does not need to be fixed first for the contents fix to be designed, built, or
  proven on a host-side observable.** `FALSIFIER`: a host log in a fixed-build arm showing the
  host's own box mirror destroyed by a keyed eid-0 destroy after the intent materialised it.
- **A new interaction this revision must name.** A husk destroy that reaches the *host's* receiver
  under the box's key would now also reach the **capture point**. §2.3's conditions are what stop it
  filling a park with the wrong contents: the husk is not a container (condition 5, `not-container`),
  and if it somehow resolved to the freshly placed box instead, the arm-once rule and the
  `fresh-not-empty` discard prevent a write. A `[CUSTODY] HOST park REFUSED … reason=not-container`
  line under the subject key in arm A1 is therefore expected, not alarming, and A1's scoring must
  say so.
- **C01 *is* on the path of any CLIENT-side acceptance observable.** An arm that says "the client
  now sees the restored contents" can fail for a C01 reason with the contents fix working perfectly.
  So: **acceptance arms A1–A3 are scored on the HOST only.** A4 is `BLOCKED_ON_OBS_R3Q_C01` and must
  not be scored until C01 is separately closed. §4.1 adds A4b, a host-scored substitute.
- **C01 has an independent cost even after this fix.** Restoring contents into a box the client then
  loses the mirror of is a worse user-visible outcome than an empty box the client can see.
  Recommendation: **fix C01 in a separate, small, separately-reviewed patch, sequenced before the
  client-side acceptance arm and before any release note.** Not a blocker for this design; must not
  be merged into this patch — one patch, one defect.

---

## 4. Acceptance tests

### 4.1 Rig arms (four peers available; two are enough)

Every arm reuses the R3Q rig shape and the same subject class. Roles: `Game_0.9.0n_HOST` and
`Game_0.9.0n_CLIENT_1`, per-role state via `-saveddirsuffix` only. **No arm below re-runs anything
R3Q already established** — `RUNTIME_RERUN_REQUIRED=none` stands for the *unfixed* build; these are
arms on a *fixed* build, which is a different subject.

| Arm | Build | Chain | Named observable | Pass |
|---|---|---|---|---|
| **A0 — negative control** | UNFIXED (the b150 oracle bytes, already measured) | R3Q PRIMARY | host P5 box open and empty | already `EXACT_REPRODUCTION`; cited, not re-run |
| **A1 — primary** | FIXED | R3Q PRIMARY, verbatim | host log, in order: `park ARMED`, then `reattached key='<K>' eid=<new> records=N hash=<H> index=<I>`, then `fanout QUEUED`; **and** the host's opened box shows the same drive ID as P0 | all three lines present with equal `records=N`; `I != 0`; drive ID at P5 == drive ID at P0. A `park REFUSED … reason=not-container` line under the same key is expected (§3) and does not fail the arm |
| **A2 — control regression** (restated, RB-5) | FIXED | R3Q CONTROL, verbatim (host-authored) | host log grepped for `[CUSTODY]` | **PASS = ZERO `[CUSTODY]` lines of any kind in the host log, AND the drive still present at C5.** The grep must be shown matching a **planted** `[CUSTODY]` line in that same file first (§4.1's probe-before-claim rule) before its zero is believed. **A1 without A2 is not a result.** |
| **A3 — persistence** | FIXED | A1, then native save, native quit both roles, relaunch host from the same role root, client rejoins | the same drive ID in the same keyed box after reload | drive present. Declare the searched scope **before** the arm, exactly as R3Q did, and report absence only within it |
| **A4 — client view** | FIXED | A1, then read the CLIENT's view of the box | client's `ContentsDigest` count equals the host's | `BLOCKED_ON_OBS_R3Q_C01` — do not score until C01 is closed |
| **A4b — client convergence, host-scored substitute** | FIXED | A1 with `container_selftest=1` | both roles print `ContentsDigest(eid)` (`container_contents_sync.cpp:803-815`, `container_selftest.h:13-15`); compare the counts | counts equal after the next contents sweep. Does not depend on the client's mirror surviving C01, because it reads the client's own digest rather than its visual box |
| **A5 — refusal control** | FIXED | with `container_selftest=1`, the instrument calls `container_custody::WouldParkForInventory()` on the **live personal inventory component** (§5.1) | `[CUSTODY] HOST park REFUSED … reason=boundary1` | the refusal fires. A boundary that has never been seen refusing is a boundary nobody has tested. **The predicate is read-only and reaches no write path** (§5.1) |

**Why A2 changed, stated at the arm.** `STRENGTH=SOURCE_PROVEN`. Neither seam is reachable on a
wholly host-authored grab/inventory/place chain, so a correct fix emits **no** `[CUSTODY]` line on
the CONTROL arm, and draft 1's pass condition ("discard line present") was unsatisfiable:

- **Apply seam.** `HostSpawnPlacedProp` has one call site, `prop_drop_intent.cpp:394`, inside
  `OnPropDropIntent`, which is role-gated to Host at `:380` and entered only from
  `event_dispatch_intent.cpp:258-280` — explicitly `CLIENT→HOST`, dropping on a client. The reel
  door (`:402-428`) reaches it only through `OnPropDropIntent` at `:427`, same shape. A host's own
  place is expressed by the `FinishSpawningActor` seam → `DrainPendingSpawns` →
  `ExpressSpawnedProp` (`prop_lifecycle.cpp:371-373`), never by `HostSpawnPlacedProp`.
- **Capture seam.** `DestroyResolvedLocalActor_` (`remote_prop_destroy.cpp:73-99`) is
  anonymous-namespace and called only from `OnDestroyImpl_:190`, entered from `remote_prop::OnDestroy`
  (`:196-198`, called at `event_dispatch_entity.cpp:254` on a **received** `PropDestroy`), from
  `TryApplyDestroy` (`:204-207`, from `quiescence_drain.cpp:249`, also a received destroy), and from
  `kerfur_convert_client.cpp:313`, which is client-only (`:269`). A host's own pickup runs
  `DestroySeamBody` (`prop_destroy_seam.cpp:36-157`), which only **sends**, at `:149`; the host never
  receives its own destroy.
- The same correction propagates into §2.3 (the fresh-empty rule's re-justification) and §4.2
  test 2's gloss.

**Probe-before-claim.** Per the programme's standing rule and its own recorded defect class, each
observable's detector must be shown returning TRUE on a known-true input before its FALSE is
believed. Concretely: (a) grep the A0 (unfixed) host log for `[CUSTODY]` — it must return zero
lines, which proves the grep is looking at the right file and the fixed build's lines are new;
(b) for **A2 specifically**, plant a `[CUSTODY]` line in the fixed-build host log and show the same
grep matching it, **before** the zero-line result is scored; (c) the `parked` / `armed` /
`reattached` greps must each be shown matching a planted line before an arm's absence is scored.

### 4.2 Standalone tests that run without the game

In the style of `1c393401:src/votv-coop/tests/baseline_delivery/` (a `run.ps1` that locates the
MSVC toolchain via `vswhere`, compiles `qualification.cpp` against a `stubs/` tree with
`/std:c++20 /EHsc /W4 /WX`, and runs both the fixed header and the **pinned pre-fix header** to
show the negative) and of `15589b7f:src/votv-coop/tests/standalone/candidate_02_prop_spawn_gate.cpp`.

Proposed: `src/votv-coop/tests/custody_park/` with `run.ps1`, `qualification.cpp` and a `stubs/`
tree, plus one standalone `candidate_17_container_custody.cpp`.

The decidable logic goes behind a **header-only, engine-free predicate and reducer** — the same
shape as `dispatch/prop_spawn_gate.h` and `interactables/pending_retry.h`:

```
coop/props/container_custody.h        (proposed, header-only, no engine types)
    struct CustodyRecordSet { std::vector<Rec> recs; uint64_t hash; int32_t index; size_t bytes; };
    struct CustodyKeyT     { std::wstring key; std::wstring cls; };

    // RB-7: the two engine-touching operations are INJECTED, so the harness can stub them and
    // every refusal below becomes a named assertion instead of engine-only code.
    struct SlotIo {
        bool  (*IsWorldContainer)(void* inv);                       // Boundary 1
        bool  (*ReadSlotIndex)(void* inv, int32_t& outIndex);       // GObjStackSlot's index half
        bool  (*ReadRecords)(void* inv, CustodyRecordSet& out);     // neuterNested = FALSE
        int32_t (*ReadSlotCount)(void* inv);                        // fresh-empty test
        bool  (*AllocPreflight)();                                  // EngineAlloc probe
        bool  (*WriteRecords)(void* inv, const CustodyRecordSet&);
        bool  (*Rederive)(void* owner, void* inv);
        size_t (*OtherWorldContainerIndices)(int32_t* out, size_t want);
        bool  (*PersonalStoreIndex)(int32_t& out);
    };

    class CustodyParkStore {           // key -> park, bounded FIFO + map
        bool Park(CustodyKeyT, CustodyRecordSet, uint8_t authorSlot, uint32_t worldGen, Now);
        bool Arm(CustodyKeyT, uint8_t spawnSlot, uint32_t worldGen, void* actor, Now, Reason&);
        bool TryConsume(void* actor, void* inv, const SlotIo&, Now, Reason&);
        void Sweep(Now, uint32_t worldGen, const bool* slotConnected);
        void Clear();
        counters...
    };
    enum class Reason { Applied, DiscardedFreshNotEmpty, NotParked, AlreadyArmed,
                        AuthorMismatch, ClassMismatch, KeyRekeyed, WorldChanged, AuthorLeft,
                        Boundary1, SlotUnresolved, SlotAliased, AllocPreflight, Expired, Evicted };
```

Positive controls (each must pass on the fixed logic **and fail on the pinned pre-fix stub**):

1. `Park(K,cls,{r1,r2},slot=1) → Arm(K,cls,slot=1) → TryConsume(freshCount=0)` yields `Applied` with
   the identical ordered record set and the identical FNV-64 hash. *The core claim.*
2. `TryConsume` with `freshCount=3` yields `DiscardedFreshNotEmpty` and **writes nothing** (the
   stub's write counter stays 0). *Pure logic; this is NOT what A2 measures in the world — see §4.1.*
3. `Arm` for a key never parked yields `NotParked`; nothing is written.
4. Double consume: a second `TryConsume` after `Applied` yields `NotParked` — consumed exactly once
   (`MVF-0004` rule 8).
5. Re-park of the same key replaces, does not accumulate: `Park(K,A); Park(K,B); Arm; TryConsume`
   yields `B`, and the FIFO holds exactly one entry for `K` (the shipped park set's desync bug,
   `prop_drop_intent.cpp:70-77`, reproduced as a test rather than as a comment).
6. Cap: `kMaxCustodyParks + 1` distinct keys evicts exactly the oldest; map and FIFO agree
   afterwards on membership, size and order.
7. **(retargeted, RB-6)** Three aging assertions, none involving a join bracket: (a) a park older
   than `kCustodyParkCeilingSec` sweeps to `Expired`; (b) a park whose `worldGen` differs from the
   swept generation clears with `WorldChanged`; (c) a park whose `authorSlot` reads not-connected
   clears with `AuthorLeft`. **No assertion in this suite references a join-snapshot bracket** —
   that predicate is client-only and cannot be true on the role that holds a park.
8. Ordering: a 512-record set round-trips in order; the hash is order-sensitive (swap two records →
   different hash).
9. Boundary at the cap: a 513-record set is refused at `Park`; a 512-record set is accepted. A set
   whose packed size exceeds `MaxBlobBytes()` is refused at `Park`.
10. `Clear()` empties both containers and zeroes the counters; nothing survives a session.

Binding controls (RB-1):

11. `Arm(K, cls, spawnSlot=2)` against `Park(K, cls, authorSlot=1)` yields `AuthorMismatch`;
    nothing is written; the park is retired.
12. `Arm(K, cls='prop_box_C')` against `Park(K, cls='prop_crate_C')` yields `ClassMismatch`.
13. A second `Arm` for an already-armed park yields `AlreadyArmed` — first spawn only.
14. `TryConsume` where the actor's enrolled key differs from `park.key` yields `KeyRekeyed`;
    nothing is written. *The `MarkPropElement` re-key interaction, §2.4.*
15. `Park` with no latched author slot is refused outright.

Slot-ownership controls (RB-3, made assertable by `SlotIo`):

16. `ReadSlotIndex` returns 0 → `SlotAliased`; write counter 0.
17. `ReadSlotIndex` returns the value `PersonalStoreIndex` returns → `SlotAliased`; write counter 0.
18. `ReadSlotIndex` returns a value present in `OtherWorldContainerIndices` → `SlotAliased`;
    write counter 0.

Engine-seam controls, previously unassertable (RB-7):

19. `IsWorldContainer` returns false → `Boundary1`; write counter 0. *The `Player == true` refusal.*
20. `AllocPreflight` returns false → `AllocPreflight`; write counter 0. *The pre-flight whose
    shipped comment (`container_contents_sync.cpp:494`) says that without it "an empty array would
    silently replace real contents".*
21. `Rederive` is called exactly once on `Applied` and **zero times** on every refusal above.

Codec controls (pure byte work; `save_record_wire` is engine-free by contract,
`save_record_wire.h:14-17`):

22. `SerSave`/`DeSave` round-trip of a record carrying a nested-container `ints[0][0]` **preserves**
    it when `neuterNested=false` (the host-local path) and **replaces it with `-1`** when
    `neuterNested=true` (the wire path). *The §2.3 trap turned into an assertion.*
23. A truncated / oversized blob fails cleanly through the existing `Feasible` bound
    (`save_record_wire.h:36-38`) — the hostile-input contract is not weakened.

**Omission cases** (the harness must fail if the fix is removed, per `1c393401`'s ten-omission
discipline). Each row names the assertion it turns red:

| Omission | Assertion turned red |
|---|---|
| delete the capture call | 1 (nothing parked → `NotParked`) |
| delete the author-slot latch | 15 |
| delete the class binding | 12 |
| delete the arm-once rule | 13 |
| delete the enrolled-key re-check | 14 |
| delete the fresh-empty check | 2 |
| delete the FIFO eviction | 6 |
| delete the ceiling / world-gen / author-left sweep | 7a / 7b / 7c |
| delete the map/FIFO pairing on consume | 5 |
| neuter the nested index on the host-local path | 22 |
| park on a `Player=true` inventory | 19 |
| apply without the allocation pre-flight | 20 |
| apply without `RederiveManagedState` | 21 |
| refuse nothing on `Index == 0` | 16 |
| refuse nothing on the personal-store index | 17 |
| refuse nothing on another container's index | 18 |
| consume twice | 4 |

**Seventeen omissions, seventeen named assertions.** Draft 1 declared ten and four of them could not
turn any assertion red in the engine-free harness it specified; the `SlotIo` injection is what
closes that (RB-7). The remaining engine-only behaviour — that the *production* `SlotIo` binding
actually calls the shipped `IsWorldContainerInventory` / `GObjStackSlot` / `EngineAlloc` /
`RederiveManagedState` and not something else — is **not** covered by this harness and is proven
only by A1, A5 and code review. Said plainly here, per RB-7's closing requirement.

---

## 5. Implementation plan (bounded; NOT authorised by this document)

### 5.1 Files touched — the whole set

**Draft 1's "two production files change by three lines each" is withdrawn.** Satisfying RB-1
through RB-4 needs the authoring peer slot (which only the intent handler has), the element id
(which only the next-tick adoption has), and a host-side sweep (which the contents lane does not run
on the host). That is three more seams than draft 1 counted. The honest set:

| File | Change | Size |
|---|---|---|
| `include/coop/props/container_custody.h` | **new**, header-only: `CustodyRecordSet`, `CustodyKeyT`, `SlotIo`, `CustodyParkStore`, `Reason`. No engine types, so it compiles in the test harness. | ~230 lines |
| `src/coop/props/container_custody.cpp` | **new**: the session holder + `Enabled()` self-gate; the production `SlotIo` binding over the `container_contents_sync` wrappers; the three seam entries `NoteInboundDestroySlot(uint8_t)`, `CaptureForDyingContainer(void* actor, const std::wstring& payloadKey)`, `NoteHostSpawnForIntent(void* actor, key, cls, uint8_t senderSlot)`; `Tick(Session*)`; `WouldParkForInventory(void* inv, Reason&)` for A5; `OnDisconnect()`; the log lines and counters. | ~300 lines |
| `src/coop/dispatch/event_dispatch_entity.cpp` | **one call** + include: `container_custody::NoteInboundDestroySlot(msg.senderPeerSlot)` immediately before `remote_prop::OnDestroy(p, localPlayer)` at `:254`. The payload carries no sender field, and this is the only scope that has one. | +2 lines |
| `src/coop/props/remote_prop_destroy.cpp` | **one call** + include, inside `DestroyResolvedLocalActor_`, after `native_pile_mirror::Unpin(actor)` (`:97`) and before `R::CallFunction(actor, g_destroyActorFn, nullptr)` (`:98`). **Unconditional** — the module self-gates, because this file has no role accessor at all. | +2 lines |
| `src/coop/props/prop_drop_intent.cpp` | **one call** + include, inside `OnPropDropIntent`'s `if (actor)` block (`:395-399`), **not** at `HostSpawnPlacedProp`'s `return` (`:231`): only this scope has `senderSlot` (`:378`). | +2 lines |
| `src/coop/session/subsystems.cpp` | **one Tick registration**, immediately after `host_spawn_watcher::DrainPendingSpawns(&session)` (`:547`) and before `prop_drop_intent::Tick(&session)` (`:548`), so the consume runs on the same pass that mints the eid. Plus the `Install`/`OnDisconnect` wiring alongside the lane's siblings. | +3 lines |
| `include/coop/props/container_contents_sync.h` | declare the seams the custody module must reuse instead of reimplementing: `ReadWorldContainerRecords(void* actor, std::vector<SaveRecord>&, bool neuterNested)`, `WriteWorldContainerRecords(void* actor, const std::vector<SaveRecord>&)`, `WorldContainerSlotIndex(void* inv, int32_t& out)`, `MarkDirty(uint32_t eid)`. `ReadContents` (`:271`) and `ApplyContents` (`:471`) are in that file's **anonymous namespace**, so reuse needs public wrappers, not a copy — RULE 2, "ONE implementation" (`save_record_wire.h:8`). | +4 declarations |
| `src/coop/props/container_contents_sync.cpp` | thread `neuterNested` through `ReadContents` (`:271-289`, guarding the `NeuterNestedIndex` call at `:285`; default `true`, so wire behaviour is unchanged — MEASURED: exactly one caller, `:334`); define the four wrappers over the existing private functions. | ~45 lines |
| `include/ue_wrap/actors/inventory.h` + `src/ue_wrap/actors/inventory.cpp` | **one read-only accessor** `void* ResolveLivePersonalInventoryComponent();` — the same walk `ReadLivePersonalStore` already performs at `inventory.cpp:142-165`, returning the component instead of its contents. Used **only** by the A5 instrument. The header's own rule (`inventory.h:84-86`, *"READ-ONLY BY CONSTRUCTION: there is no live-store writer in this header"*) is preserved: this returns a pointer for a predicate to inspect, and the predicate reaches no writer. | ~14 lines |
| `include/coop/dev/container_selftest.h` + `src/coop/dev/container_selftest.cpp` | the A5 instrument, under the **already-shipped** `container_selftest=1` gate (`container_selftest.h:1-2,23-28`): once per session, call `container_custody::WouldParkForInventory(ResolveLivePersonalInventoryComponent(), reason)` and print the result. No new dev module, no new env flag, no write path. | ~20 lines |
| `docs/devices.md` | add the missing container-contents row to "Who owns what" (`:171-183`), the `ContainerContents` row to "Wire messages" (`:185-196`), and a custody paragraph after `:71`. | docs |
| `docs/status.md`, `docs/code-map.md` | the lane's late-join row (doctrine Step 6) and the code-map entry. | docs |
| `src/votv-coop/tests/custody_park/**` + `tests/standalone/candidate_17_container_custody.cpp` | the harness of §4.2 (23 assertions, 17 omissions). | tests |

**Count it honestly: two new module files, seven shipped files edited (five of them by one or two
lines, two by a small block), one ue_wrap accessor, two docs files, one test tree.** The runtime
backout is still one switch (§2.0, G16); the diff is not.

### 5.2 Exact functions to touch — symbol anchors, not line anchors

Line anchors are 3af5ddae-only and drift materially on the b150 line that §2.7 says the same patch
text applies to. Draft 1's §5.2 already reasoned in prose; §5.3 now does too. **Anchor on symbols
and statements:**

```
dispatch/event_dispatch_entity.cpp
  in `case net::ReliableKind::PropDestroy:`, in the statement IMMEDIATELY BEFORE
      remote_prop::OnDestroy(p, localPlayer);                                     [3af5ddae :254]
  insert:
      coop::props::container_custody::NoteInboundDestroySlot(msg.senderPeerSlot);
  Rationale: PropDestroyPayload has no sender field (protocol.h:1088-1093) and
  remote_prop::OnDestroy takes none. The latch is read-and-cleared by the capture; a capture with
  no latch refuses (author-unknown), which also excludes the deferred quiescence path.

props/remote_prop_destroy.cpp
  in `DestroyResolvedLocalActor_`, between the call to
      coop::native_pile_mirror::Unpin(actor);                                     [3af5ddae :97]
  and
      R::CallFunction(actor, g_destroyActorFn, nullptr);                          [3af5ddae :98]
  insert ONE unconditional call:
      coop::props::container_custody::CaptureForDyingContainer(actor, keyW);
  The module self-gates on role/session (this TU has no role accessor -- MEASURED: `git grep
  'IsHost\|role()\|Role::'` over this file returns nothing).

props/prop_drop_intent.cpp
  in `OnPropDropIntent`, inside the `if (actor) { ... }` block that follows
      void* actor = HostSpawnPlacedProp(p, cls, key);                             [3af5ddae :394]
  insert:
      coop::props::container_custody::NoteHostSpawnForIntent(actor, key, cls, senderSlot);
  NOT at HostSpawnPlacedProp's `return actor` (draft 1's site): that function receives no
  senderSlot, and there is no element id there either (host_spawn_watcher.h:29-32).

session/subsystems.cpp
  in the per-tick block, between
      coop::host_spawn_watcher::DrainPendingSpawns(&session);                     [3af5ddae :547]
  and
      coop::prop_drop_intent::Tick(&session);                                     [3af5ddae :548]
  insert:
      coop::props::container_custody::Tick(&session);
  Ordering rationale: DrainPendingSpawns is where the eid is minted
  (-> prop_lifecycle::ExpressSpawnedProp -> MarkPropElement); the custody Tick reads
  Registry::EidForActor immediately after. container_contents_sync::Tick (which owns DrainDirty)
  already ran earlier in the same pass [:515], so the fan-out ships on the NEXT contents sweep
  (<= 250 ms, kSweepMs). State that in the arm's scoring.

props/container_contents_sync.cpp
  ReadContents(void* inv, std::vector<SaveRecord>& out)
      -> ReadContents(void* inv, std::vector<SaveRecord>& out, bool neuterNested = true)
         guard the `if (RecordIsNestedContainer(r)) NeuterNestedIndex(r);` statement on the flag.
         MEASURED: exactly one caller (BroadcastContainer), which takes the default, so wire
         behaviour is unchanged.
  + ReadWorldContainerRecords / WriteWorldContainerRecords -- public wrappers that apply
    IsContainerActor, InventoryOf, IsWorldContainerInventory and GObjStackSlot and then delegate
    to ReadContents / the raw-write block inside ApplyContents (the EngineAlloc pre-flight,
    AllocZeroed, WriteSaveRecord, WriteArrHeader, RederiveManagedState). Both private functions
    stay private.
  + WorldContainerSlotIndex(void* inv, int32_t& out) -- the index half of GObjStackSlot, needed by
    the slot-aliasing refusals (RB-3 rows 8-10) which cannot read a private function.
  + MarkDirty(uint32_t eid) -- a two-line public wrapper over the private g_dirty insert.
```

### 5.3 Sequencing

1. Land the header + tests **first**, red, against the pinned pre-fix logic (the `baseline_delivery`
   discipline: show every gate red before trusting it). All 23 assertions and all 17 omissions.
2. Land the module and the five call sites; the tests go green; MSVC `/W4 /WX`.
3. Land the `container_contents_sync` wrappers and the `neuterNested` thread.
4. Land the ue_wrap accessor and the `container_selftest` A5 hook.
5. Docs rows.
6. Rig arms in this order: **A0 (cite) → A2 → A1 → A4b → A3 → A5.** **A2 before A1**, because a fix
   that breaks the working path is worse than the bug, and because A2's detector must be proven on a
   planted line before its zero means anything.
7. `private/b150-aligned` cherry-pick with `kProtocolVersion` untouched at 150 and the constraint
   restated in the commit message. Verify by symbol, not by line number.

### 5.4 Explicitly out of scope, and the residual coverage bound

**Out of scope:**

- **No transport change, no Relay, no universal serializer** (the reviewed direction's three
  prohibitions).
- **No new `ReliableKind`, no new blob op, no protocol bump, no format or size change** under the
  primary mechanism. (Not "zero bytes" — see §2.6.)
- **The custody op (`kOpCustody`)** — designed in §2.6, *not implemented* by this plan. A second,
  separately-reviewed increment, gated on Q-3.
- **OBS-R3Q-C01** — separate patch, separate review (§3).
- **OBS-R3Q-P01** (held-lid state) — untouched; `MVF-0002`/`PENDING-BUG-025` territory.
- **Personal inventories.** Never authored, never repaired, never copied. `MVF-0004` rule 7, and now
  three explicit refusals (§2.5 rows 6, 8, 9).
- **Orphan-slot reclamation.** No slot is removed, compacted, or index-shifted. `MVF-0004` rule 11.
- **Reroll suppression for rematerialized loot containers** (`MVF-0004` rule 5) — a distinct defect
  class; not addressed and not claimed.
- **Key uniqueness itself.** This design *detects* a collision (`key-rekeyed`) and *refuses* on it.
  It does not fix the underlying non-uniqueness §0.3 measures, and it must not be described as
  doing so.
- **The 45 causally-unresolved ledger rows.** This design claims one chain, measured once per route.
- **Any claim of unrestricted permanent data loss.** The persistence result is bounded and stays
  bounded: `ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE; UNKNOWN_BEYOND_SCOPE`.

**The residual coverage bound (RB-6's closing requirement), stated positively and negatively.**

*Covered:* a world container whose contents **the host held**, destroyed on the host by a
`PropDestroy` **received from peer slot S**, then re-materialised by **that same peer's**
`PropDropIntent` under the **same wire-normalised key** and the **same class**, as the **first**
such spawn after that destroy, in the **same world generation**, while **S stayed connected**, where
the fresh component's slot resolves to an index that is not 0, not the personal store's, and not any
other live world container's, and reads empty.

*Not covered, each with its refusal:*

| Case | Refusal / reason | Why it is out |
|---|---|---|
| contents the host never held (client fills a carried container; client-born container) | never parked | Q-3; the wire increment of §2.6 |
| peer A takes, peer B (or the host) places | `AuthorMismatch` | a client may not direct where another peer's container contents land (§2.2) |
| a destroy that reached the host through the deferred quiescence path | `author-unknown` | that path is measured firing against the wrong actor (§0.3 fact 3); fail-closed is correct |
| the container's key was re-keyed at adoption | `key-rekeyed` | a live collision family; writing would be guessing (§2.4) |
| a second container parked over the first under one key | `REPLACED` | today both are empty; not a regression |
| a host world reload between take and place | `world-changed` | the slot geometry the park recorded is gone |
| the taking peer disconnects and rejoins | `author-left` | the park cannot be matched to a new slot safely |
| a record set larger than the blob ceiling | `over-blob-ceiling` | it could be restored but never fanned out — a divergence, not a fix |
| the fresh component's slot aliases anything | `slot-aliased` | RB-3; refusing is strictly better than a wrong write |
| the eid never resolves within `kMaxAdoptTries` | `eid-unresolved` | RB-2; the fan-out could not fire, so the apply would create a divergence |

**What the bound is NOT.** Draft 1's 60 s TTL would have covered only a short-carry subset of the
reported store-then-re-place chain, and did not say so. This revision removes the wall clock from
the predicate entirely, so **carry duration is no longer a coverage limit within a connection**. The
limits above are causal, not temporal, and each is observable in the host log.

---

## 6. Invariants, restated as one list

1. Host-authoritative world containers. A client causes a transition; it never asserts contents
   outside the shipped CAS path.
2. `propInventory_C.Player == true`, or an unresolvable `Player` flag, is refused everywhere,
   always, fail-closed.
3. A peer-local `GObjStack` index is never transmitted and never installed from the wire.
4. Actor lifetime is not state lifetime: contents survive the actor gap, keyed by a **tuple** — key,
   class, authoring peer slot, world generation — never by the key alone.
5. A park is armed at most once and consumed at most once, by exactly one keyed spawn from the peer
   that authored the destroy.
6. A park never overwrites a non-empty fresh slot, and never writes into a slot it has not proved
   is neither index 0, nor the local personal store's, nor another live world container's.
7. Every store is bounded, and **no bound is a wall-clock safety window**: 32 parks, 512 records,
   the blob ceiling in bytes, a world-generation stamp, the authoring peer's connection, and a
   900 s absolute ceiling as a leak guard.
8. Every refusal is logged with a named reason, at **both** the capture and the apply. A silent
   refusal is a defect.
9. Nested indices are preserved on the host-local path and neutered on any wire path. The two must
   not share a default.
10. No session state survives a session.
11. **No invariant in this list may be expressed by a predicate that cannot be true on the host.**
    (The rule RB-6 exists to install; it is why invariant 7 names a world generation and not a join
    bracket, and why §2.3 rejects `world_load_episode` as well.)

---

## 7. Open questions, each with the observable that settles it

| # | Question | Observable that settles it | Blocks |
|---|---|---|---|
| **Q-1** | Do the R3Q arms promote `BUG-017`'s applicability cell from `U2` to a named contract failure? | A design review that reads `REVIEW.md`'s primary/control claims against the matrix's `U2` definition and rules. A **review decision, not a measurement**; the design does not self-promote. | the ledger row, not the patch |
| **Q-2** | Does the host's old `GObjStack` slot survive the container's destruction (orphaned) or is it cleared/reclaimed by the native teardown? | The bounded full-stack probe `MVF-0004` specifies: before the client take, record host `{key, index, stack length, ordered hash}`; after the host destroy, read the same slot; after materialization, read the new index and hash; repeat after save/reload. Requires separate rig authorisation. **Cheaper first pass:** §2.11's park line already prints `index=`, and the reattach line prints the new index, so A1's own log answers the "did the index change" half without a new instrument. | only the *alternative* reattach strategy (re-install `parkedIndex`) |
| **Q-4** | **(moved to the blocking column, RB-3/RB-4)** Does `propInventory_C::init` allocate a fresh slot, reuse a free one, leave `Index` at the CDO default, or read `ints[0][0]`, when `loadData` never ran? | The same stack probe as Q-2, plus a bytecode read of `propInventory_C::init` and `prop_container_C::loadData`. Currently `INFERRED` from the in-tree comment at `container_contents_sync.cpp:252-257`. **In the meantime the patch does not depend on the answer being favourable:** if `init` defers allocation, `GObjStackSlot` returns null and every consume refuses with `slot-unresolved`, logged, and the build behaves exactly as the unfixed build. **Draft 1's §9 item 4 — "the design does not depend on it being right" — is retracted as written:** the *safety* of the write depends on the resolved slot being exclusively owned, which is why §2.5 rows 8-10 exist. | the apply strategy's expected yield, and the strength tag on §1.4 step 5 |
| **Q-3** | Is a wire custody transfer needed at all — is there a real chain where the host never held the contents? | A rig arm: client takes a **client-extracted** item into a container it is carrying, then places it. If the host's box shows those contents under the host-local fix, no wire is needed. | the second increment only |
| **Q-5** | **(largely closed by re-reading)** Can the capture read `GObjStack[Index].obj` safely at the capture point? | `STRENGTH=SOURCE_PROVEN`: the capture sits **before** `K2_DestroyActor` (`remote_prop_destroy.cpp:98`), not after, so the actor and its component are simply live — `:79-97` all dereference the actor. Draft 1 argued from `prop_lifecycle.cpp:280-283` (*"K2_DestroyActor only marks PendingKill, so reads on the actor are still valid"*), which is about the **sender-side** post-native seam and is a weaker, second line of defence here. Residual `UNKNOWN`: whether `ClearAnyDriveFor` / `ReleaseMainPlayerGrabIfHolding` at `:87-90` can mutate the inventory. A dev-instrument sample of `ContentsDigest(eid)` one tick earlier vs. at the capture settles it. | nothing; a fallback exists — move the capture to immediately after the entry log (`:79-80`) and before `ClearAnyDriveFor` (`:87`) |
| **Q-6** | Which transition actually loses the content? The two R3Q arms vary all three together. | Three single-transition arms on the unfixed build. **Not required for this design** — the fix covers the whole custody window — but required before any causal statement is published. | published causal wording only |
| **Q-7** | ~~Does a restored container's fan-out interact with hash-map entries left over from the pre-destroy eid?~~ **CLOSED, `STRENGTH=SOURCE_PROVEN`.** All four maps are keyed by eid and written only at `container_contents_sync.cpp:359` (`g_sentHash`), `:364` (`g_publishedHash`), `:518` (`g_appliedHash`) and `:520` (`g_baseHash`). A freshly minted eid has no entry in any of them, so `force=false` sends (`:344-347`), and the fan-out itself establishes `g_publishedHash[eid]` at `:364`, which is what keeps the first client write from being refused as a stale base. | closed |
| **Q-8** | ~~Other callers of `ReadContents` whose behaviour a `neuterNested` default could change?~~ **CLOSED, `STRENGTH=MEASURED.**` `git grep -n ReadContents 3af5ddae -- src/votv-coop` returns exactly two lines: the definition at `container_contents_sync.cpp:271` and one call at `:334` (`BroadcastContainer`). A defaulted parameter changes nothing. It is in the anonymous namespace, so reuse goes through a declared seam, not a copy. | closed |
| **Q-9** | *(new)* How long is a real store-then-re-place carry? | Not measured by R3Q (§0.1). **This revision no longer needs the answer**, because no wall clock is in the consume predicate; it is recorded so nobody re-derives a TTL from an assumption. | nothing |
| **Q-10** | *(new)* Can two live world containers on the host hold the **same** `propInventory.Index`? | §2.5 row 10 refuses it either way. The enumeration itself is the measurement: a `slot-aliased` refusal naming two eids is the observation. | nothing; the refusal is the answer |

---

## 8. Appendix — citation index (all re-opened on `3af5ddae` for this revision)

```
include/coop/net/protocol.h
  :30       kProtocolVersion = 152
  :483      ReliableKind::PropDropIntent = 90
  :535      ReliableKind::ReelEjectIntent = 104
  :589      ReliableKind::ContainerContents = 118
  :1086-94  PropDestroyPayload (40 B; elementId 0 = "sender had no Element"; NO sender-slot field)
  :1096-112 PropDropIntentPayload (172 B; complete field list; ":1098 No element id (the host
            allocates its own)"; :1111 the 172 assert; :1112 the one-datagram assert)
  :1367-74  BlobChunkPayload (228 B; 220 data bytes per chunk)

include/coop/net/blob_chunks.h
  :45-47    MaxBlobBytes() = 220 * 255 = 56,100

include/ue_wrap/engine/world_identity.h
  :29       CurrentWorld()
  :48-53    WorldKind / CurrentWorldKind()
  :55-58    Generation() -- "Bumped every time CurrentWorld is observed to change"  <-- THE HOST-VALID
            AGING TOKEN that replaces the client-only join bracket

include/ue_wrap/actors/inventory.h
  :58-70    the live personal store's address chain and why GObjStack[0] IS the player's inventory
  :71-74    LivePersonalStore { slotIndex "measured to be 0 by construction"; records }
  :81-87    ReadLivePersonalStore + the address-assertion / refusal symmetry with Boundary 1
src/ue_wrap/actors/inventory.cpp
  :138-178  ReadLivePersonalStore (:155-165 the fail-closed Player==0 refusal; :167-169 the index)

include/coop/props/prop_drop_intent.h
  :12-15    why the client's intent is authored a tick LATE (the husk-destroy ordering guard)
  :54       "through the SAME HostSpawnPlacedProp path" (the reel door's own comment)
src/coop/props/prop_drop_intent.cpp
  :46-47    the session_holder pattern this design reuses for the custody module
  :53-86    PendingPlace, kMaxPending=32, kMaxKeyTries=8; the park set/FIFO invariant + UnparkKey
  :88-93    FillWireStr -- the N-1 wire-key truncation
  :107-152  OnClientFinishSpawn (:145-151 the container-extract latch: TakeObjInFlight at :148)
  :158-232  HostSpawnPlacedProp (:168 BeginDeferredSpawn, :214 FinishDeferredSpawn,
            :220-222 scale, :226-230 savedScalar, :231 return actor -- NO eid minted)
  :223-225  "the next-tick express drain re-reads the live actor"
  :263-363  Tick (the client's intent author)
  :365-375  NoteClientKeyedDestroy
  :377-400  OnPropDropIntent (:378 senderSlot, :380 host gate, :383-386 key/class,
            :390-393 the duplicate guard, :394 the ONE HostSpawnPlacedProp call)  <-- THE ARM POINT
  :402-428  OnReelEjectIntent (:411-421 the class whitelist; :427 calls OnPropDropIntent)
  :430-435  Reset

src/coop/props/prop_destroy_seam.cpp
  :36-157   DestroySeamBody (the general destroy observer, SEND side)
  :54       IsKeyedInteractable        :60  the dying actor's own key
  :84-107   the CLIENT-ONLY load-episode / reconcile-window suppression (":85 role()==Client")
  :115      the kerfur first refusal
  :119-121  the 31-char WireKey truncation (the wire normalisation to match)
  :128-149  the payload build and send (:131 the eid-0 sentinel; :149 SendPropDestroy)
  :146-147  the coin-gun sale on the same body
  :154-156  NoteClientKeyedDestroy, Role::Client gated -- fires for ANY keyed destroy this seam sends

src/coop/props/remote_prop_destroy.cpp
  :73-99    DestroyResolvedLocalActor_  <-- THE CAPTURE POINT (:97 Unpin, :98 K2_DestroyActor)
  :108-192  OnDestroyImpl_ (:123 keyW from the WIRE payload; :133-135 eid resolve FIRST;
            :150 the key fallback; :190 the only call to DestroyResolvedLocalActor_)
  :196-198  OnDestroy      :204-207  TryApplyDestroy (the deferred re-apply; NO sender slot)
  MEASURED: no IsHost(), no role(), no Role:: anywhere in this file

src/coop/props/host_spawn_watcher.{h,cpp}
  h:29-32   "that callback only ENQUEUES, and DrainPendingSpawns adopts on the next tick"
  h:43-49   DrainPendingSpawns -- "ONE express attempt per entry"
  cpp:205-211  the enqueue rationale (the key is still a placeholder at the finish's return)
  cpp:219-238  OnFinishSpawnFunc (:223 host-only; :237 push_back -- enqueue only)
  cpp:352-383  DrainPendingSpawns (:355 host gate, :360 IsLiveByIndex, :363 EidForActor,
            :374 ExpressSpawnedProp, :375-379 the eid EXISTS here)   <-- THE CONSUME'S PRECONDITION

src/coop/props/prop_lifecycle.cpp
  :199      GetInteractableKeyString at the express seam
  :211-214  "Mark may re-key a duplicate (the host is the key authority)" + MarkPropElement
  :216-217  the 31-char key truncation into the PropSpawn payload
  :280-283  "K2_DestroyActor only marks PendingKill, so reads on the actor are still valid"
  :371-373  ExpressSpawnedProp -> GrabObserver_Aprop_Init_POST_Body
  :375-395  RegisterHostPropSilent (the other MarkPropElement caller)

src/coop/props/prop_element_tracker.cpp
  :192-198  MarkPropElement's contract (the one place a local Prop Element is minted)
  :199-275  MarkPropElement (:228 the already-tracked early-out; :230 the host role read;
            :243-250 the key-uniqueness doctrine; :252-254 the live-incumbent test;
            :261-267 MintFreshKeyForDuplicate and the re-key)   <-- THE RE-KEY INTERACTION

src/coop/dispatch/event_dispatch_entity.cpp
  :215-256  the PropDestroy case (:221-231 length/key validation; :236-242 elementId 0 accepted;
            :247-252 the keyed echo guard; :254 remote_prop::OnDestroy)   <-- THE SLOT LATCH POINT
            `msg.senderPeerSlot` is in scope here and nowhere downstream

src/coop/dispatch/event_dispatch_intent.cpp
  :258-280  the PropDropIntent case (CLIENT->HOST; drops on a client)
  :281-302  the ReelEjectIntent case

src/coop/element/quiescence_drain.cpp
  :242-256  ApplyPendingDestroys -> TryApplyDestroy (the deferred path; no sender slot)

include/coop/props/container_contents_sync.h
  :1-13     the lane's model and its two rules
  :43-47    NoteJoinSnapshotBracket's contract ("Called from the client-side snapshot dispatch")
  :51-68    the dev-instrument seams (WorldContainer, SnapshotWorldContainers, ContentsDigest)

src/coop/props/container_contents_sync.cpp
  :41       kOpContents = 0        :51  kSweepMs = 250       :55  kMaxRecordsPerContainer = 512
  :101      kConflictWindowMs = 1500                         :117 kParkTtlSec = 30 (the SHIPPED park)
  :168-181  InventoryOf / OwnerOf
  :183-195  IsWorldContainerInventory  <-- BOUNDARY 1 (:185 "GObjStack[0] is the local player's
            inventory by construction"; :187-188 the ReadLivePersonalStore symmetry)
  :200-211  GObjStackSlot (:207 idx<0 refused; :209 idx>=num refused; INDEX 0 ACCEPTED)
  :228-233  RecordIsNestedContainer      :247-250 IsContainerActor
  :252-262  NeuterNestedIndex          <-- BOUNDARY 2 + the loadData/init reading
  :271-289  ReadContents (:275-279 the 512 cap; :285 the neuter call the host-local path must skip)
  :303-316  PackContents (the blob grammar)        :318-323 ContentHash
  :332-376  BroadcastContainer (:344-347 the force/sentHash gate; :348-352 THE BLOB CEILING REFUSAL;
            :359 g_sentHash; :364 g_publishedHash; :369 Rederive; :370-373 the shipped ship line)
  :400-421  DrainDirty (edge-driven; :415 resolves forward from the eid; :418 BOUNDARY 1)
  :453-457  RederiveManagedState
  :471-524  ApplyContents (:483-484 the applied-hash no-op; :486-490 BOUNDARY 1;
            :491-492 slot resolve; :494-500 the EngineAlloc PRE-FLIGHT; :502-517 the raw write;
            :518-520 the hash records; :521 Rederive)
  :528-558  HostAcceptsClientWrite (the CAS; :535 baseMatches; :543 accept; :546-557 the refusal)
  :560-600  ParseAndApply (:563 the unknown-op refusal; :569-578 refuse-and-re-publish, :573 BOUNDARY 1)
  :602-620  SweepParked (the shipped park sweep)
  :629-662  OnVerbEntry (:630-639 "If this line is absent from a log, the lane is dead"; :654 g_dirty)
  :666-677  NoteJoinSnapshotBracket (:666 "From event_feed's CLIENT-SIDE ... dispatch")
  :690-727  Tick (:717-719 the 250 ms sweep gate; :725 DrainDirty; :726 `if (!IsHost()) SweepParked()`)
  :729-759  OnContentsChunk (:735-742 the acceptance matrix)
  :761-780  QueueConnectBroadcastForSlot (:776 BOUNDARY 1)
  :784-801  SnapshotWorldContainers (:797 BOUNDARY 1)     :803-815 ContentsDigest (:809 BOUNDARY 1)
  :817-831  OnDisconnect

src/coop/dispatch/event_feed.cpp
  :341      SnapshotBegin: `if (session.role() == net::Role::Host) break;` -- the host early-out
  :361      NoteJoinSnapshotBracket(true)      :364  NoteReconcileBegin()
  :413      SnapshotComplete: the same host early-out
  :424      NoteJoinSnapshotBracket(false)     :447  NoteReconcileComplete()

include/coop/session/world_load_episode.h
  :1        "the CLIENT world-load lifecycle"
  :50-53    InEpisode -- "True while a CLIENT world-load episode is in progress"
  :86-91    InReconcileWindow
src/coop/session/net_pump.cpp
  :233-243  MaybeRequestReAnnounce -- `:234 if (role() == Host) return;`, RaiseReconcileForReload at :243
src/harness/session_runtime.cpp
  :311-317  world_load_episode::Arm() -- "The sole, CLIENT-ONLY arm site"

src/coop/creatures/kerfur_convert_client.cpp
  :264-269  OnKerfurConvert -- `:269 if (s->role() != Role::Client) return;`
  :310-313  the synthesised PropDestroyPayload with dp.key.len = 0 -> remote_prop::OnDestroy

src/coop/session/subsystems.cpp
  :515      container_contents_sync::Tick()   (owns DrainDirty; 250 ms gate)
  :547      host_spawn_watcher::DrainPendingSpawns(&session)   <-- THE TICK INSERTION POINT (after)
  :548      prop_drop_intent::Tick(&session)

include/coop/dev/container_selftest.h
  :1-15     the shipped instrument for this lane and its env flag
  :23-31    Install / Tick / OnDisconnect

include/coop/items/save_record_wire.h
  :8        RULE 2 -- "ONE implementation"
  :14-17    the hostile-input contract     :31-38 the caps and Feasible()     :55-56 SerSave / DeSave
include/ue_wrap/actors/save_record.h
  :29-45    the SaveRecord POD    :55-57 kSaveStride 0x100 / kMxStride 0x10
  :93-102   ReadSaveRecord / WriteSaveRecord (the orphan-the-old-buffer rule)

src/coop/items/inventory_wire.cpp        :36-45   the player envelope over the same per-record codec
src/coop/items/player_inventory_sync.cpp :391-435 the host persists a client's blob and applies nothing
src/coop/net/session.cpp                 :262-267 the protocol-mismatch refusal on an unadmitted peer

docs/devices.md
  :41-71    "What is inside a container, and who may write it"
  :171-183  "Who owns what" (no container-contents row)
  :185-196  "Wire messages" (no ContainerContents row)
docs/coop-sync-doctrine.md
  :40-53    Step 2, exactly one owner per element
  :94-101   Step 5 (:98-99 the actor-gap sentence -- CORRECTED from draft 1's :96-97;
            :99-101 "Keys are load-bearing" and the say-which-is-canonical rule)
  :103-110  Step 6, the late-join row is part of the lane
  :111-119  Step 7, wire discipline
  :120-128  Step 8, evidence (:125-126 "a gate that cannot fire passes forever")
  :129-138  Forbidden patterns

f3d6b59d:design/relay-adoption/MULTIVOID-CURRENT-MODEL.md
  :27-38    the ten-axis table         :40  the SOURCE_PROVEN identity-domains claim
                                            (CORRECTED from draft 1's :44)

C:\AgenticStaging\BugDropbox\Doctaaaa's reports\Report 3\bug_03_...key_collision.md
  :14,:57   one key on >=4 live client actors + 1 host actor; two more keys, same pattern
  :38-40    the deferred destroy armed 21:51:10, applied 21:51:16 -- SIX SECONDS
  :47-49    HOST spawned client-placed prop, then HOST broadcasting DESTROY, same key, same second
  :58       "All destroy traffic in this incident carries eid=0"
  :60       the host's own same-key eid=0 destroy at 22:11:03
```

---

## 9. What a reviewer should attack first

1. **The neuter asymmetry** (§2.3, assertion 22). If the host-local capture reuses `ReadContents`
   unchanged, every nested container silently empties on re-attach and the patch ships a new data
   loss. Unchanged from draft 1, and still first.
2. **The slot-ownership refusals** (§2.5 rows 8–10, assertions 16–18). These are what stand between
   the apply and the host player's own inventory. Check that the enumeration in row 10 really uses
   `SnapshotWorldContainers` (which applies Boundary 1 itself) and not a home-rolled walk, and that
   the personal-store comparison uses `ReadLivePersonalStore`'s index rather than a hard-coded 0.
3. **The consume point** (§2.4, §5.2). Confirm for yourself that `HostSpawnPlacedProp` mints no eid
   and that `DrainPendingSpawns` does, and that the custody Tick is registered *after* it. If the
   consume is moved back to `:231` for tidiness, the fan-out silently stops and the host holds
   contents no peer sees — a divergence the unfixed build does not have.
4. **The author-slot latch** (§2.2, §5.2). One line in a dispatch case, carrying the only piece of
   provenance the mechanism has. If it is dropped as "an extra seam", the class binding is all that
   is left against §0.3's collision families, and that is not enough.
5. **A2's polarity** (§4.1). The arm passes on **zero** `[CUSTODY]` lines. Anyone who reads it as
   "the discard line must appear" will either fail a correct fix or, worse, make the seams fire on
   the host-authored chain — the single change most likely to endanger `CONTROL_PASS`.
6. **Q-4's strength** (§1.3, §7). It is `INFERRED` from an in-tree comment about Blueprint bytecode
   that neither this task nor draft 1 re-measured. The patch degrades safely if it is wrong
   (`slot-unresolved`, logged), but the *narrative* in §1.4 depends on it, and a narrative stated at
   the wrong strength is this programme's standing defect class.
7. **Q-1.** Do not let this document's existence promote `BUG-017`'s matrix cell. That is a
   reviewer's call.

---

## 10. Provenance of this document

```
INPUTS READ (read-only; no checkout of the main repo, no branch switch, no commit, no game process)
  C1/CONTEXT.md
  C1/LANES/bug017-design-review/VERDICT.md            (the binding verdict, read in full)
  C1/LANES/bug017-design/BUG017-CONTRACT-DESIGN-DRAFT.md  (draft 1, read in full, NOT modified)
  R3Q/LANES/ws-runtime-review-codex/{VERDICT.txt,REVIEW.md}
  BugDropbox/Doctaaaa's reports/Report 3/bug_03_floppy_dissolves_deferred_destroy_key_collision.md
  f3d6b59d:design/relay-adoption/{DECISION.md,BUG-APPLICABILITY-MATRIX.md,MULTIVOID-CURRENT-MODEL.md}
  3af5ddae: every file in the §8 citation index, re-opened at the cited lines for THIS revision
            (git show / git grep only; the working tree was not touched)
NOT READ / NOT DONE
  No game process was started. No rig arm was run. No build was configured. No commit was made.
  The bounded full-stack GObjStack probe (Q-2/Q-4) was NOT run; it needs separate authorisation.
  No worktree was created; the main repo's checkout, branch and index were not touched.
CORRECTIONS MADE TO DRAFT 1'S CITATIONS (five; three from the verdict, two found here)
  coop-sync-doctrine.md actor-gap sentence   :96-97  ->  :98-99          (verdict D-5)
  MULTIVOID-CURRENT-MODEL.md identity claim  :44     ->  :40             (verdict D-5)
  container_contents_sync.cpp Boundary 1 site list: :573 added (six sites, not five)  (verdict D-5)
  HostSpawnPlacedProp call sites: "two (:394, :427)" -> ONE (:394); :427 calls OnPropDropIntent,
      and names HostSpawnPlacedProp only in a trailing comment                    (found here)
  world_load_episode::InEpisode / InReconcileWindow, proposed by the verdict as the host-side
      park clear, are CLIENT-ONLY on every raise path (session_runtime.cpp:315-317,
      net_pump.cpp:234, event_feed.cpp:341/:413) and would have repeated RB-6     (found here)
STRENGTHS USED
  MEASURED       the R3Q arm outcomes, the build identity, the diffstats in §0.6, the key
                 non-uniqueness and the 6-second deferred-destroy interval in §0.3, the grep
                 results labelled MEASURED in §5.2
  SOURCE_PROVEN  every file:line read on 3af5ddae in §1, §2, §4.1 and §8
  INFERRED       the orphan-slot reading, the CONTROL-arm explanation, §1.3's closing clause
                 (re-tagged from draft 1), and every proposal in §2
  UNKNOWN        Q-2, Q-3, Q-4, Q-6, Q-9, Q-10, the carry duration, and everything outside R3Q's
                 declared searched scope
```

---

## 11. RB closure table

| RB | What it required | Where revision 2 closes it | Closed? |
|---|---|---|---|
| **RB-1** — key-only consume predicate on a line with measured key collisions | (1) park under the dying actor's own key; refuse empty key or mismatch with `payload.key`; add "key non-empty" to the entry conditions (closing `kerfur_convert_client.cpp:313`). (2) record the class; refuse the consume on mismatch. (3) bind to a single generation / same authoring peer slot / short window. (4) state the `MarkPropElement` re-key interaction. | (1) §2.3 capture conditions 3 and 4; §2.10; the kerfur entry additionally shown unreachable on the host at `kerfur_convert_client.cpp:269`. (2) §2.1 `CustodyPark.cls`; §2.4 arm condition 2; §2.5 row 4; assertion 12. (3) §2.4 arm conditions 1–5 — same peer slot (via the §2.2 latch), same class, same key, **first spawn only**, same world generation, plus author-connection continuity. (4) §2.4's boxed rule and §2.5 row 3, from `prop_lifecycle.cpp:211-214` and `prop_element_tracker.cpp:243-275`; assertion 14. | **YES**, with item 3 delivered causally rather than as a clock window. Reason stated at §2.3 and §2.4: §0.3 measures the collision firing **6 s** after arming, inside any window "materially shorter than 60 s", so a short clock is neither necessary nor sufficient; the arm-once + same-peer + same-class + same-generation predicate is strictly stronger, and removing the clock is also what closes RB-6's coverage complaint. |
| **RB-2** — no element id at the apply point, so the mandatory fan-out cannot fire | move the consume (or at minimum the `MarkDirty`) to the next-tick adoption, with a bounded retry and a named refusal; update the file set, call sites, the TRANSITIONS row and the G16 claim; add a host-side observable naming the fan-out | §2.4's three-phase machine (PARKED → ARMED → APPLIED) with the consume in the custody module's own `Tick`, registered at `subsystems.cpp:547` immediately after `DrainPendingSpawns`, where `host_spawn_watcher.cpp:375` shows the eid resolving. Bounded retry `kMaxAdoptTries`, refusal `eid-unresolved` (§2.5 row 2, §2.11). §2.0's TRANSITIONS row rewritten; §2.0's G16 claim retracted and restated as a runtime switch; §5.1 and §5.2 rebuilt. Fan-out observable `[CUSTODY] HOST fanout QUEUED …` in §2.11, plus the ≤250 ms sweep latency stated in §2.4. The second consequence (`g_publishedHash`) closed at §2.4 via `container_contents_sync.cpp:364`, which also closes Q-7. | **YES** |
| **RB-3** — the apply writes into a slot it has not proved is exclusively owned | add slot-ownership rows: refuse `Index == 0`; refuse a slot equal to the local personal store's; refuse a slot equal to any other live world container's. Print the resolved `Index` on the reattached line. Retract §9 item 4; move Q-4 into the blocking column | §2.5 rows 8, 9, 10, each with its source (`inventory.h:64-72`, `inventory.cpp:138-178`, `container_contents_sync.h:62-63` / `:797`). `index=%d` printed on the park, reattach **and apply-refusal** lines (§2.11). §2.3's third justification for the fresh-slot strategy explicitly retracted. §9 item 4 replaced (new §9 items 2 and 6). Q-4 moved to the blocking column in §7 with the degradation path named. Assertions 16–18. A new public `WorldContainerSlotIndex` listed in §5.1. | **YES** |
| **RB-4** — the apply may refuse silently, with no refusal observable | re-tag §1.3's closing clause `INFERRED`; add the apply-refusal log row with reasons `<not-container\|boundary1\|slot-unresolved\|slot-aliased\|alloc-preflight>` and add it to A1's scoring; name the fallback | §1.3's closing clause is boxed and tagged `INFERRED`, with the load-bearing note. §2.11's fourth row carries the five required reasons verbatim plus this revision's added ones. A1's row scores the refusal lines and A1's `not-container` expectation is stated in §3. The fallback is the next-tick consume RB-2 installs, where the express drain has demonstrably read the actor (`host_spawn_watcher.cpp:374-375`). | **YES** |
| **RB-5** — A2 asserts an observable a correct implementation cannot produce | restate A2 as a must-fire negative control: FIXED build, R3Q CONTROL verbatim, pass = **zero** `[CUSTODY]` lines **and** the drive present at C5, with the grep shown matching a planted line first. Keep the fresh-empty refusal as defence-in-depth and re-justify it. Keep test 2 as pure logic; name a real chain for the DISCARDED branch or declare it unit-tested only | §4.1's A2 row restated exactly so, with the reachability proof beneath it ("Why A2 changed") and the planted-line requirement in the probe-before-claim paragraph, item (b). §2.3's fresh-empty rule re-justified against three cases that can actually reach it. §4.2 test 2 relabelled *"Pure logic; this is NOT what A2 measures in the world"*. The DISCARDED branch is declared reachable only by the three §2.3 cases and otherwise **unit-tested only**. | **YES** |
| **RB-6** — the join-bracket guard is dead on the host and the real coverage bound is undeclared | delete the bracket clause; retarget test 7; decide the TTL against a measured carry duration or an event anchor; add an absolute age ceiling and a world-load-episode / reconcile-window clear; state the residual coverage bound in §5.4 | Bracket clause deleted, with the host-side proof (`container_contents_sync.cpp:666`, `event_feed.cpp:341`/`:424`/`:413`, and the shipped sweep's own `if (!IsHost())` at `:726`). Test 7 retargeted to three host-valid aging assertions. The TTL is **removed from the predicate**: the carry duration is UNKNOWN (§0.1, Q-9) and §0.3 shows a clock would not have caught the collision anyway; expiry is anchored to world generation, author-slot connection, and a 900 s ceiling as a leak guard. §5.4's residual-coverage table replaces "short-carry only" with ten named causal limits, each with its refusal. | **YES**, with one deliberate substitution: the verdict's suggested `world_load_episode::InEpisode` / `InReconcileWindow` clear is **not** used, because re-reading shows every raise path is client-only (`session_runtime.cpp:315-317`, `net_pump.cpp:234`, `event_feed.cpp:341`/`:413`) — adopting it would have reproduced RB-6 itself. `ue_wrap::world_identity::Generation()` (`world_identity.h:55-58`) is used instead; it is role-agnostic and can actually be observed to change on a host. |
| **RB-7** — four omission cases cannot turn any named assertion red | give the two engine-touching seams an injectable slot-reader/slot-writer so Player-refusal, pre-flight-refusal and rederive-called become named assertions in `candidate_17`; give A5 a concrete instrument listed in §5.1. Otherwise delete those rows and say they are unproven | The `SlotIo` struct in §4.2 injects all nine engine operations; assertions 19, 20, 21 name the Player refusal, the pre-flight refusal and the rederive call, and 16–18 name the slot refusals. The omission table maps **17 omissions to 17 assertions**. A5 gets a concrete instrument — `container_custody::WouldParkForInventory` driven from the **already-shipped** `container_selftest` gate over a new read-only `ResolveLivePersonalInventoryComponent()` — both listed in §5.1, with the read-only constraint stated. §4.2 closes by declaring what the harness still does **not** prove (that the production `SlotIo` binding calls the shipped functions), so nothing is claimed that is not covered. | **YES** |

**Also closed from the verdict's §4 items 11–13 and §6 overstatement list:** §2.3's "both authorship
directions with one code path" and "double-apply on the host-authored chain" (withdrawn, §2.3);
§4.2 test 2's gloss (§4.2); §2.0's TRANSITIONS row and G16 claim, §5.1's file set and §5.2's call
sites (rebuilt); §2.6/§2.7/§5.4's "no wire change at all" / "zero bytes" (restated as format
compatibility); §1.1's Boundary 1 site list (six sites); §0.6's `prop_destroy_seam.cpp` antecedent
(D-4); the `kOpCustody` size bound (§2.6, and adopted for the host-local path as capture condition
9); the wire-normalised key rule (§2.1); the `session_holder` self-gate instead of an `IsHost()`
that does not exist in that TU (§2.3, §5.2); symbol anchors instead of line anchors for the b150
cherry-pick (§5.2, §5.3 step 7); the park/reattach lines carrying the index for Q-2/Q-4 (§2.11, Q-2);
and a host-scored substitute for the C01-blocked client arm (A4b).

**Still open, deliberately, with the consequence stated:** the **nested container's own slot** — the
verdict's optional finding that destroying an outer container does not destroy an inner container's
`GObjStack` slot, and that re-attaching an outer record set re-points at inner slots whose contents
may have been rewritten in the interval. §2.3 preserves nested indices (it must, or every nested
container empties), and §4.2 assertion 22 proves the preservation, but **no rule in this revision
validates that a preserved `ints[0][0]` still names the same inner contents at consume time.**
*Consequence:* a container holding a nested container, taken and re-placed while a third party
rewrote the inner container, can be restored pointing at an inner slot whose contents have changed —
stale, not destroyed, and no worse than the empty box the unfixed build gives, but not correct
either. *What would close it:* an assertion that a parked outer record set whose preserved
`ints[0][0]` no longer resolves to a live world container is **refused** rather than written, plus
the invariant written into §6. It is left open because it needs the nested-slot liveness reader that
does not exist yet, and because refusing on it without that reader would refuse every nested case.
**A reviewer may reasonably make this an eighth blocker.**
