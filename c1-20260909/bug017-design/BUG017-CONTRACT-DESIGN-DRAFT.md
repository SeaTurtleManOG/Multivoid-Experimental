# BUG-017 — container custody contract, FIRST DRAFT design

```
STATUS            FIRST_DRAFT — DESIGN ONLY. IMPLEMENTATION NOT AUTHORISED.
DIRECTION         ADD_CONTRACT_LAYER_KEEP_CURRENT_TRANSPORT (reviewed, f3d6b59d)
                  no transport rewrite, no Relay dependency, no universal serializer
CODE PIN          3af5ddae (upstream/main, kProtocolVersion=152)  — every file:line below is on this ref
COMPARE PINS      ba6d8c39 (v0.9.0n-b150-dev, proto 150), f690c017 (old main, proto 151)
AUTHORED          2026-09-09, lane C1/bug017-design
REVIEW REQUIRED   yes — design review before any patch, per MVF-0004's exact handoff
```

This document is a design input set plus a proposed contract. It approves no patch. Every fact
carries a strength tag and, where it is a source fact, a `file:line` on `3af5ddae`. Anything the
source does not settle is marked `UNKNOWN` and carries the observable that would settle it.

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
PACKET VERDICT        NOT_CLEAN (binding review) — but the gameplay arms are upheld:
                      the seven findings are in the tool-policy and cleanup aggregates and in
                      non-load-bearing record defects, not in the arm outcomes.
BUILD IDENTITY        both roles' main.dll = CFD1A037F073D07F3EAE0E7B189BD70294A5E899E8E9CA8D51DA72731951289C
                      = the official b150 DLL
SUBJECT               CLASS=prop_box_C  LOGICAL_KEY=HlwQ1k8xmW4zxWDHX-VZFw
                      DRIVE_ID=GCPcVmpTGXXJQmxep5xtXg
```

**The exact reproduction chain (PRIMARY arm, client-authored).** Verbatim structure from
`REVIEW.md` ("The primary arm exactly reproduces the specified empty-box outcome"):

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
grab/inventory/place chain.** The review states the resulting limit explicitly and this design
inherits it:

> `GAP` — The two arms vary the acting peer for the entire grab/inventory/place chain; they do not
> isolate which transition loses the content. There is one run per route.

**Persistence.** After the primary arm the host saved natively, both peers quit natively, the host
relaunched **from the same primary role root** (world hash `2CBFFC12…`, not the seed `CCCB6FCE…`),
the client rejoined, and the same key reminted as EID 4202: the box was empty; both players'
personal stores held Crowbar x1 and Hook x2 and no drive; the immediate floor region was
inspected. `NOT_RECOVERABLE_AFTER_RELOAD; ABSENT_WITHIN_SEARCHED_SCOPE`. The declared searched
scope was the subject box, both players' searched inventory/store surfaces, the immediate searched
world/floor region, and the reload state actually inspected. **Everything outside that scope is
UNKNOWN.** This design must not be read as, and must not be written up as, unrestricted permanent
data loss.

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

### 0.3 Source-position facts carried forward (R3Q `ws-implementation-readiness`, MVF-0004)

`STRENGTH=SOURCE_PROVEN at the old pin; RE-VERIFIED here on 3af5ddae` (§1 below re-cites each on
the new pin):

- `PropDropIntentPayload` is exactly 172 bytes; class, stable key, prop row, transform, flags,
  one optional scalar. **No complete record, no contents, no portable association.**
- The client-carried `SaveRecord` contains a **client-local** `propInventory.Index`; the contained
  rows live separately in that peer's `GObjStack[Index].obj`.
- `HostSpawnPlacedProp` sets generic identity, finishes the spawn, sets scale and the optional
  scalar. It does **not** call `loadData`, install an old index, read contents, or reattach by key.
  Fresh host backing follows.
- The searched host destroy paths contain **no explicit whole-slot clear/removal**. Positive
  survival of the old slot is therefore `INFERRED`, not implementation evidence.

### 0.4 The reviewed architecture direction

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

### 0.5 Code identity across the three pins

`STRENGTH=MEASURED` — `git diff -w --numstat` and a non-comment line filter over the mechanism
files:

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
  `SyncDestroyedTrackedProp` function (its `mirror_manager.h` include went with it). Not on the
  BUG-017 chain.
- `protocol.h`: `PropDropIntentPayload`'s body is byte-for-byte identical on b150 and 3af5ddae
  (only its trailing comment and the `static_assert` message text changed).
  `PropDropIntent = 90`, `ReelEjectIntent = 104` and `ContainerContents = 118` are the same numbers
  on all three pins. `kProtocolVersion` is 150 / 151 / 152 respectively.

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
  all**, is refused. Personal inventory shares the same global `GObjStack`, and
  `GObjStack[0]` is the local player's inventory by construction; writing it from the host would
  wipe that peer's inventory. Applied on the read side (`:418`), on the apply side (`:486-490`), on
  the join seed (`:776`) and in the dev seams (`:797`, `:809`).
- **Boundary 2** — `NeuterNestedIndex` (`container_contents_sync.cpp:252-262`): a nested
  container's `ints[0][0]` is its own `GObjStack` index, a slot number in the **sender's** array,
  so it ships as the sentinel `-1`. The comment records why clearing `ints[]` entirely is wrong:
  *"prop_container::loadData reads ints[0][0] unguarded, Array_Get zero-fills an out-of-range read,
  so an empty array yields index 0, which propInventory::init's `index >= 0` guard passes, and the
  container reuses GObjStack[0], a slot owned by someone else."*

### 1.2 The addressing mismatch: contents are eid-addressed, custody is key-addressed

`STRENGTH=SOURCE_PROVEN`.

`ContainerContents` is **element-id addressed** end to end:
- `protocol.h:589` — "A world container's contents as state, **eid-addressed**."
- `PackContents(eid, baseHash, recs)` — `container_contents_sync.cpp:307-316`, blob
  `[u8 op=0][u32 eid][u64 baseHash][u16 n][n × SerSave]`.
- `ApplyContents(eid, …)` resolves `LivePropActor(eid)` first — `:471-473`.
- The dirty edge resolves identity **at the verb edge** from the actor's eid —
  `OnVerbEntry`, `:651-654`.

`PropDropIntent` is **logical-key addressed** end to end (`protocol.h:1099-1112`): `className`,
`key` (the persistent cross-peer save key), `propName`, transform, scale, `physFlags`,
`savedScalar`. No element id, by design — "the host allocates its own".

A pickup/re-place cycle **destroys the actor and its eid and mints a new one**. The R3Q content
frames show exactly that: same logical key, EID 4129 → 5975 (control), EID 5968 → 4202 across the
reload (primary/persistence). So the one identity that survives the custody transition — the
logical key — is the one identity the contents lane never speaks, and the one identity the contents
lane does speak — the eid — is destroyed by the transition. **That is the contract violation.**

### 1.3 Which message carries the client's intent, and why the host cannot rebuild contents from it

`STRENGTH=SOURCE_PROVEN`.

The intent is `ReliableKind::PropDropIntent = 90` (`protocol.h:483`) carrying
`PropDropIntentPayload` (`protocol.h:1099-1112`, `static_assert(sizeof(...) == 172)` at `:1111`,
and `<= 256 - 20 - 8` — one datagram — at `:1112`). Authored by the client in
`prop_drop_intent.cpp:311-353`; routed at `event_dispatch_intent.cpp:258-280`; handled by
`OnPropDropIntent` at `prop_drop_intent.cpp:377-400`.

The payload's complete field list (`protocol.h:1100-1110`) is `className[64]`, `key[32]`,
`propName[32]`, 3 loc + 3 rot + 3 scale floats, `physFlags` + 3 pad, `savedScalar`. **There is no
contents field, no record array, no index, and no room for one**: the struct is a fixed 172-byte
POD on the reliable-datagram lane whose ceiling is 228 bytes (`protocol.h:1112`), while a container
may legally hold up to 512 records (`container_contents_sync.cpp:55`) of a variable-length record
grammar (`save_record_wire.h:55-56`, `save_record.h:29-45`). Contents cannot ride this payload
under any encoding.

The host's materialization is `HostSpawnPlacedProp` (`prop_drop_intent.cpp:158-232`). Its complete
body is: `FindClass` (`:160`), `BeginDeferredSpawn` (`:168`), `setKey` through the prop base's
setter (`:181-197`), `WriteSpParityIdentity` for the props-table row and the four physics bools
(`:201-212`), `FinishDeferredSpawn` (`:214`), `SetActorScale3D` (`:220-222`), and
`ApplySavedScalarForClass` (`:226-230`). **No `loadData` call, no `propInventory.Index` write, no
`GObjStack` touch, no contents read, no reattach by key.** The host therefore rebuilds the
container from class + key + transform + parity flags alone, and the fresh `propInventory_C::init`
runs inside the finish spawn with the CDO's default index.

`OnPropDropIntent` (`:377-400`) validates key/class non-empty (`:383-386`), refuses a duplicate if
the host still has the key live (`:390-393`), and calls `HostSpawnPlacedProp` (`:394`). Nothing in
that path consults the destroyed container's prior contents.

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
   `prop_destroy_seam.cpp:128-149`; the key is then parked for a later re-place at `:154-156`.
   `STRENGTH=SOURCE_PROVEN`.
2. The host routes it at `event_dispatch_entity.cpp:215-256` → `remote_prop::OnDestroy`
   (`remote_prop_destroy.cpp:196-198`) → `OnDestroyImpl_` (`:108-192`), which resolves by eid then
   by key (`:133-151`) and terminates in `DestroyResolvedLocalActor_` (`:73-99`): clear drives,
   release a grab, echo-suppress, unpin, `K2_DestroyActor` (`:98`). `STRENGTH=SOURCE_PROVEN`.
3. **Nothing in steps 1–2 clears, frees, or re-indexes the host's `GObjStack` slot.** The word
   `GObjStack` does not appear in `remote_prop_destroy.cpp`, `prop_destroy_seam.cpp` or
   `event_dispatch_entity.cpp`. `STRENGTH=SOURCE_PROVEN (absence over the searched paths)`.
4. The host's box actor dies. Its `propInventory_C` component dies with it. The slot it addressed
   in the host's `saveSlot.GObjStack` **is not addressed by anything any more.**
   `STRENGTH=INFERRED` — the coop layer does not clear it; whether the native
   `prop_container_C`/`propInventory_C` teardown or the engine's own save-object management clears,
   compacts or reuses it is **UNKNOWN** (Blueprint side, not read by this task).
5. The re-place spawns a fresh `prop_container_C` with a fresh `propInventory_C` whose `Index` is
   the CDO default. The in-tree bytecode reading recorded at `container_contents_sync.cpp:252-257`
   states that `propInventory::init` guards on `index >= 0` and that the CDO default sentinel is
   `-1`; with `loadData` never called (`HostSpawnPlacedProp` above), the guard fails and init takes
   its allocate-a-fresh-slot branch. `STRENGTH=INFERRED (from an in-tree bytecode-derived comment,
   not re-measured here)`.
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
> key. No message carries the association across the gap, and the host's materialization path
> deliberately reconstructs the container from identity alone. Actor lifetime is therefore treated
> as state lifetime — which `MVF-0004`'s required contract rule 1 forbids in as many words.**

The doctrine says the same thing prospectively: *"A destroy-and-recreate transition, hold to drop
to store to equip, carries the identity across the actor gap"* (`docs/coop-sync-doctrine.md:96-97`).
Identity does carry. **State does not.**

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

`f3d6b59d:design/relay-adoption/MULTIVOID-CURRENT-MODEL.md:23-42` fixes the concern each axis must
preserve. This design is checked against it row by row before anything else, because the axis
table — not this document — is the reviewed artifact:

| Axis | Concern the declaration must preserve (`MULTIVOID-CURRENT-MODEL.md:28-42`) | Where this design honours it |
|---|---|---|
| IDENTITY | "Logical key, EID and live actor binding must not collapse into one value." | §2.1 keeps custody on the key, contents on the eid, and forbids transmitting the peer-local index — three domains, not one |
| PRESENCE | native / mirrored / adopted / absent / retired differ | §2.9 — untouched; a park belongs to the host process, not to any expression |
| AUTHORITY | "Host routing, origin validation and feature-specific authorship are separate decisions." | §2.2 — the host still arbitrates; the client causes but never asserts |
| DURABLE_STATE | "Native save bytes and per-player data are not session event streams." | §2.3 — the park is session-local and never persisted; the player's own store is never written (§5.4) |
| EVENTS | "One-shot intents/results are not durable fields." | the drop intent stays a one-shot intent; the contents stay durable state on their own lane |
| TRANSITIONS | "Multi-expression state changes need ordered postconditions." | §2.4's state machine, with the fresh-empty precondition and the fan-out postcondition |
| MATERIALIZATION | "Actor creation/adoption/destruction occurs on the game/world side, not the network thread." | both seams are game-thread, inside the existing spawn/destroy paths; no network-thread work is added |
| JOIN_SEED | "Native save load and ordered per-slot replay form a staged bootstrap." | §2.8 — the restored container is an ordinary live container by seed time; no new bootstrap stage |
| RESYNC | "Current convergence is feature-specific and must stay explicit." | §2.8 — the shipped CAS + re-publish path is reused verbatim, not generalised |
| VALIDATION | "Size/version/sender/range/liveness/ownership checks remain per contract." | §2.5's table, all fail-closed, all per-entry-point |

`MULTIVOID-CURRENT-MODEL.md:44` records at `SOURCE_PROVEN` that identity already has four
non-substitutable domains plus per-player ownership as a fifth. **The defect in §1 is precisely a
collapse between two of them** — the design's job is to bridge them without merging them.

`PILOT-DESIGN.md` is not applicable here: its G01–G17 gates
(`f3d6b59d:design/relay-adoption/PILOT-DESIGN.md:21-37`) all govern OPTION_3, the Relay pilot, and
this design introduces no Relay artifact, no external package and no rule profile. Two of its
gates are still worth borrowing as *discipline*: G10's "exactly one enroller, writer, seed source
and materializer" (satisfied — the host is all four for a restored container) and G16's one-switch
backout (satisfied — the whole mechanism is two three-line call sites, §5.2).

### 2.1 IDENTITY

- The container's **persistent logical key** (`WireKey`, `ue_wrap::prop::GetInteractableKeyString`,
  the same value `PropDropIntentPayload.key` already carries) is the custody identity. It survives
  the actor gap; R3Q measured it surviving three actor gaps and a reload.
- The element id remains the **expression** identity and keeps owning steady-state
  `ContainerContents`. It is never used to address a custody record.
- The peer-local `GObjStack` **`Index` is never transmitted and never installed from the wire.**
  (`MVF-0004` required-contract rule 3; Boundary 2's existing argument.)
- One row per actor: the custody record is keyed, and a second custody record for a key that
  already has one **replaces** it (latest wins on the same peer), never accumulates.

### 2.2 AUTHORITY

- **World containers are host-authoritative.** No change: the host already arbitrates
  (`container_contents_sync.cpp:528-558`).
- A client may *cause* a custody transition (it authors `PropDropIntent`); it may never *assert*
  container contents on the host outside the existing CAS-gated `ContainerContents` path.
- `propInventory_C.Player == true` — or an unresolvable `Player` flag — is **refused at every new
  entry point**, exactly as `IsWorldContainerInventory` does today
  (`container_contents_sync.cpp:191-195`). This is a hard invariant, not a policy: a violation
  destroys a player's inventory.

### 2.3 DURABLE_STATE — the host-side custody park (the primary mechanism)

The design's core claim is that **BUG-017 is closable entirely on the host, with zero wire bytes**,
because in the measured chain the host held the contents before the client took the box.

**Capture point (host).** In `DestroyResolvedLocalActor_`
(`remote_prop_destroy.cpp:73-99`) — the actor is still live from `:79` through `:97`; the engine
call that kills it is `:98`. Before that call, if the dying actor is a container
(`IsContainerActor`, `container_contents_sync.cpp:247-250`) whose inventory passes Boundary 1,
capture:

```
CustodyPark {
    key            the logical key (WireKey / std::wstring)
    records        the ordered std::vector<SaveRecord> read from GObjStack[Index].obj
    recordCount    int32
    contentHash    FNV-64 over the canonical pack (the existing ContentHash shape)
    parkedIndex    int32 — the host's own slot number, recorded for diagnostics and for the
                   optional reattach strategy; NEVER transmitted
    at             steady_clock::time_point
}
```

**Capture must not neuter nested indices.** `ReadContents`
(`container_contents_sync.cpp:271-289`) calls `NeuterNestedIndex` at `:285` because its output is
destined for the wire. A **host-local** park keeps the same host's array, so a nested container's
`ints[0][0]` is still valid and neutering it would silently empty every nested container on
re-attach. The capture therefore needs a `bool neuterNested` parameter on `ReadContents` (default
`true`, wire behaviour unchanged) or a sibling reader. **This is the sharpest trap in the design;
a reviewer should check it first.**

**Park store.** A bounded FIFO keyed by logical key, mirroring the shipped park set's invariant
exactly (`prop_drop_intent.cpp:68-79`: set and FIFO hold the same keys; every insert, consume and
evict touches both, because a consume that dropped only the set desynced them). Bounds:
`kMaxCustodyParks = 32` (a container is bulkier than a key string; the shipped key park is 64),
`kCustodyParkTtlSec = 60`, aged only outside a join-snapshot bracket — reuse
`NoteJoinSnapshotBracket`'s already-shipped reasoning (`container_contents_sync.cpp:667-677`:
parks do not age while the bracket is open because a slow link can exceed any fixed TTL with no
loss at all).

**Apply point (host).** In `HostSpawnPlacedProp`, **after** `FinishDeferredSpawn`
(`prop_drop_intent.cpp:214`) so `propInventory_C::init` has already run, and after the scale and
saved-scalar writes (`:220-230`), immediately before the `return actor` at `:231`. Resolve the
fresh container's own inventory locally — `InventoryOf(actor)` → `IsContainerActor` →
`IsWorldContainerInventory` → `GObjStackSlot` — and write the parked records into **the slot the
host just resolved for its own fresh component**, through the shipped apply primitives
(`SR::AllocZeroed`, `SR::WriteSaveRecord`, `SR::WriteArrHeader`,
`container_contents_sync.cpp:502-517`), then `RederiveManagedState` (`:453-457`).

Writing into the *fresh* slot, rather than re-pointing `Index` at the parked slot, is the
recommended strategy because (a) it reuses a shipped, reviewed write path verbatim, (b) it needs no
answer to the orphan-slot question, and (c) it cannot alias a slot another component now owns. The
alternative — re-install `parkedIndex` and orphan the fresh slot — is smaller but is gated on §7
Q-2 and on an orphan-accounting rule (`MVF-0004` rule 11: never remove middle `GObjStack` slots and
shift live indices).

**Refusal rule — never overwrite newer state.** The park is consumed only if the fresh container's
slot resolves **and reads as empty** (`SR::ReadArr(slot, 0).num == 0`). If the native path already
restored contents — which is what makes the CONTROL arm pass today — the park is **discarded with a
log line and no write.** Without this rule the fix would double-apply on the host-authored chain
and could revert a newer host state. This rule is what makes the mechanism safe to run on *both*
authorship directions with one code path.

### 2.4 TRANSITIONS and MATERIALIZATION

The custody state machine, per key:

```
LIVE ──(host destroys a keyed world container)──▶ PARKED{records, hash, at}
PARKED ──(keyed host spawn, fresh slot empty)───▶ APPLIED ─▶ LIVE (contents restored)
PARKED ──(keyed host spawn, fresh slot NOT empty)▶ DISCARDED (native loadData already won)
PARKED ──(TTL, bracket closed)──────────────────▶ EXPIRED  (logged; contents stay lost)
PARKED ──(park cap exceeded)────────────────────▶ EVICTED  (oldest first; logged)
PARKED ──(session teardown)─────────────────────▶ CLEARED
```

`APPLIED` must be followed, on the same tick or the next contents sweep, by the host's ordinary
`ContainerContents` fan-out for the new eid, so every peer converges on the restored contents
through the shipped lane. The simplest correct trigger is to mark the new eid dirty (the same set
`OnVerbEntry` writes at `container_contents_sync.cpp:654`) and let `DrainDirty` (`:400-421`) ship
it; **`force=false` is wrong here** because `g_sentHash` may still hold the pre-destroy hash for a
different eid — the new eid has no entry, so the ordinary path already sends. Reviewer check:
confirm `g_sentHash`/`g_publishedHash`/`g_appliedHash` are keyed by eid and therefore carry no
stale entry for a freshly minted eid (`:339-346`, `:359-364`, `:483-484`, `:518-520`).

### 2.5 EVENTS and VALIDATION

No new event kind under the primary mechanism. Validation at the apply point, all fail-closed:

| Check | Refusal |
|---|---|
| dying/spawning actor is not a container | no park / no apply |
| `IsWorldContainerInventory` false, **including an unresolvable `Player` offset** | refuse, log, no write |
| `GObjStackSlot` returns null (save slot, offsets or index unresolved) | refuse, log, no write |
| fresh slot's record count `!= 0` | discard the park, log, no write |
| `EngineAlloc` probe fails | refuse, log, no write (`container_contents_sync.cpp:495-500`) |
| record count `> kMaxRecordsPerContainer` (512) | refuse to park at all (`:275-279`) |
| park older than TTL with the join bracket closed | expire, log |

### 2.6 The wire shape — and whether a new kind is needed

**Under the primary (host-local) mechanism: no wire change at all.** No new `ReliableKind`, no new
payload, no new blob op, no protocol bump. This is the whole reason to prefer it.

**If the reviewer or the stack probe shows the host-local park is insufficient** — the identified
case is a container whose contents the host *never held*, e.g. a client filled it while carrying
it, or the container was born on the client (the `ReelEjectIntent` / container-extract birth path,
`prop_drop_intent.cpp:298-309`) — then a bounded transfer is needed. In that case:

- **Do not add a field to `PropDropIntentPayload`.** It is a fixed 172-byte single-datagram POD
  (`protocol.h:1111-1112`); there is no trailing-blob mechanism on that lane and contents are
  unbounded up to 512 variable-length records.
- **Do not add a new `ReliableKind`.** Reuse `ReliableKind::ContainerContents = 118`
  (`protocol.h:589`), which is already a chunked `BlobChunkPayload` blob (220 data bytes per chunk,
  `protocol.h:1367-1374`; ceiling 220 × 255 = 56,100 bytes, `blob_chunks.h:45-47`) and already owns
  a reassembler, a sender-slot acceptance matrix (`container_contents_sync.cpp:735-742`) and a
  park/TTL path.
- **Add one op byte to that blob's existing grammar.** The current grammar is
  `[u8 op=0][u32 eid][u64 baseHash][u16 n][n × SerSave]`
  (`PackContents`, `container_contents_sync.cpp:307-316`). Add:

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

- **Version gate.** The op byte is the gate on the receive side: the shipped parser already refuses
  an unknown op — `if (!W::RdU8(blob, o, op) || op != kOpContents) return Ingest::Handled;`
  (`container_contents_sync.cpp:563`) — so an old peer treats a custody blob as `Handled` and never
  relays it. That is a graceful *degrade*, not compatibility: the fix silently does nothing.
  Doctrine Step 7 (`coop-sync-doctrine.md:112-113`) requires the bump anyway: *"A new wire format or
  field bumps the build number in the same commit."*

### 2.7 Compatibility — two lines, two answers

`STRENGTH=SOURCE_PROVEN` on the mechanism: a protocol mismatch is refused before a peer joins.
`session.cpp:262-267` peeks the peer's version on an unparsed header and retires the pending
connection with `"protocol mismatch: peer=v%u, ours=v%u"`; `peer_admission_internal.h:57-58` binds
`kProtocolVersion` into the admission preimage; `session_manager.cpp:592` refuses a browser join
whose advertised `proto` differs. There is no mixed-version lobby to break.

| Line | Rule | What this design allows |
|---|---|---|
| **Upstream** (`3af5ddae`, proto 152) | mismatched peers are refused before joining, so a bump costs a cohort update, not correctness | Either option is acceptable. If the custody op is ever added, bump to 153 in the same commit and add the router/`code-map.md` row. |
| **PRIVATE b150 line** (`ba6d8c39` + `private/b150-aligned`) | **the wire must not change** — this line has to interoperate with the shipped b150 build players actually have | **Only the primary host-local mechanism is admissible.** It adds zero bytes, zero kinds, zero ops, and leaves `kProtocolVersion = 150` untouched. The custody op is forbidden on this line. |

Because the mechanism files are code-identical across the three pins (§0.5), the same patch text
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
- Resync: unchanged. The host's CAS + re-publish-to-the-author path
  (`container_contents_sync.cpp:569-578`) already converges a diverged author.

### 2.9 PRESENCE

Not touched. No new presence obligation; a park belongs to the host process, not to a peer.

### 2.10 Entry, exit and error behaviour

**Entry conditions** (all must hold before a park is taken):
session connected; role Host; the dying actor is live and is a container; its inventory resolves,
is a `propInventory_C`, and passes Boundary 1; `GObjStackSlot` resolves; record count ≤ 512.

**Exit conditions** (the park is consumed):
a keyed host spawn under the identical key; the fresh container resolves and passes the same
boundary checks; the fresh slot reads empty; the allocation pre-flight succeeds. On success the
park is removed from both the map and the FIFO in one operation.

**Error behaviour** — every failure is a refusal that leaves state exactly as it is today. A failed
park means the bug reproduces (no regression). A failed apply means the bug reproduces (no
regression). **No path may write a partial record set, write into a slot it could not verify, or
write into an inventory whose `Player` flag it could not read.** Doctrine's forbidden-patterns list
(`coop-sync-doctrine.md:130-138`) rules out the shortcuts: no receive-side gate protecting shared
state, no second implementation of the record codec, no per-frame array scan.

### 2.11 Observability — what lets the rig prove it

Every line prefixed `[CUSTODY]` so one grep separates the lane, and every one of them carries the
logical key, so the rig can bind a line to the R3Q subject key `HlwQ1k8xmW4zxWDHX-VZFw`:

| Event | Line (shape) | Why the rig needs it |
|---|---|---|
| park taken | `[CUSTODY] HOST parked key='%ls' records=%d hash=%llu index=%d` | proves the capture point fired on the destroy, with the record count that must reappear |
| park refused | `[CUSTODY] HOST park REFUSED key='%ls' reason=<not-container\|boundary1\|slot-unresolved\|cap-512>` | a silent non-park is indistinguishable from a dead hook; this is the falsifier's home |
| apply | `[CUSTODY] HOST reattached key='%ls' eid=%u records=%d hash=%llu` | the primary observable: same key, **new** eid, same record count and hash |
| discard | `[CUSTODY] HOST park DISCARDED key='%ls' -- fresh slot already holds %d records (native restore won)` | proves the CONTROL arm still takes the native path and is not double-applied |
| expire / evict | `[CUSTODY] HOST park EXPIRED key='%ls' after %ds` / `... EVICTED (cap %zu)` | bounds the store; an expiry during a rig arm invalidates that arm |
| first entry | one-shot `[CUSTODY] capture hook ENTERED for the first time on this peer (role=%s)` | the shipped lane's own convention (`container_contents_sync.cpp:633-639`): *"a resolved verb name does not prove this callback runs"* — if this line is absent, the lane is dead |

Counters, readable from the log at teardown: `parksTaken`, `parksRefused` (by reason),
`parksApplied`, `parksDiscarded`, `parksExpired`, `parksEvicted`. Plus the already-shipped
`ContentsDigest(eid, count, vol)` (`container_contents_sync.cpp:803-815`) as the cross-peer
comparison the two-peer smoke prints on both roles.

---

## 3. How OBS-R3Q-C01 interacts with this chain

`STRENGTH=MEASURED (the observation); INFERRED (the interaction)`.

**What C01 is.** A HOST-placed box vanishes on the CLIENT: the host broadcasts `PropDestroy` with
`elementId = 0` and the box's logical key — the destroy is aimed at the in-hand display husk, which
carries the same key as the box — and the client, finding no eid to resolve, falls back to
`ResolveLiveActorByKey` (`remote_prop_destroy.cpp:136-151`) and destroys the just-placed box mirror.

**Why the zero eid is legitimate wire.** `prop_destroy_seam.cpp:131` writes
`dp.elementId = (destroyEid == kInvalidId) ? 0u : destroyEid;` — 0 is the protocol's
"sender had no Element" sentinel (`protocol.h:1088-1092`) — and the husk is untracked, so 0 is
correct for it. `event_dispatch_entity.cpp:236-242` accepts `elementId == 0` deliberately.

**Why the host has a guard and the client does not.** For the *client-place* direction the guard is
ordering, and it is documented in the header: `prop_drop_intent.h:12-15` — the place also destroys
the in-hand husk, and if that crossed as `DESTROY(key)` first, "the host processes it against a
rock it no longer has -- a no-op -- and only then spawns from the intent, and the spawn survives.
Authoring in the same tick lets the husk-destroy kill it instead." The client-side `Tick` drain
(`prop_drop_intent.cpp:263-363`) is what implements that "one tick late". **There is no equivalent
for the host-place direction**: the host spawns immediately inside `OnPropDropIntent`/its own place,
its `FinishSpawn` watcher broadcasts `PropSpawn`, and the husk destroy follows with the same key
and eid 0.

**Interaction with BUG-017, and the answer.**

- **On the primary (client-authored) chain, C01 is not on the critical path.** The placing peer is
  the client; the measurement peer is the host; the host resolves the client's husk destroy against
  a key it no longer holds (a no-op, by the header's own argument). The R3Q primary arm recorded
  "one client drop intent plus one host materialization" — exactly one — and the box existed on the
  host at P5. **C01 does not need to be fixed first for the contents fix to be designed, built, or
  proven on a host-side observable.** `FALSIFIER`: a host log in a fixed-build arm showing the
  host's own box mirror destroyed by a keyed eid-0 destroy after the intent materialised it.
- **C01 *is* on the path of any CLIENT-side acceptance observable.** An arm that says "the client
  now sees the restored contents" can fail for a C01 reason with the contents fix working perfectly.
  So: **acceptance arms A1–A3 below are scored on the HOST only.** A client-side arm (A4) is
  listed as `BLOCKED_ON_OBS_R3Q_C01` and must not be scored until C01 is separately closed.
- **C01 also sits on the CONTROL arm's client half.** The control arm passed on the host observable,
  which is consistent: the control's finding was about the host's box, not the client's mirror.
- **C01 has an independent cost even after this fix.** Restoring contents into a box the client
  then loses the mirror of is a worse user-visible outcome than an empty box the client can see.
  Recommendation: **fix C01 in a separate, small, separately-reviewed patch, sequenced before the
  client-side acceptance arm and before any release note.** It is not a blocker for this design and
  must not be merged into this patch — one patch, one defect.

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
| **A1 — primary** | FIXED | R3Q PRIMARY, verbatim | host log: `[CUSTODY] HOST parked key='<K>' records=N` at the client's pickup, then `[CUSTODY] HOST reattached key='<K>' eid=<new> records=N hash=<H>` at the materialization; **and** the host's opened box shows the same drive ID as P0 | both lines present with equal `records=N`; drive ID at P5 == drive ID at P0 |
| **A2 — control regression** | FIXED | R3Q CONTROL, verbatim (host-authored) | host log: `[CUSTODY] HOST park DISCARDED key='<K>' -- fresh slot already holds N records`; host's box still shows the drive | discard line present (the native path still wins), drive preserved. **A1 without A2 is not a result**: it cannot distinguish a fix from a fix that also broke the working path. |
| **A3 — persistence** | FIXED | A1, then native save, native quit both roles, relaunch host from the same role root, client rejoins | the same drive ID in the same keyed box after reload | drive present. Declare the searched scope **before** the arm, exactly as R3Q did, and report absence only within it. |
| **A4 — client view** | FIXED | A1, then read the CLIENT's view of the box | client's `ContentsDigest` count equals the host's | `BLOCKED_ON_OBS_R3Q_C01` — do not score until C01 is closed |
| **A5 — refusal control** | FIXED | place a **personal-inventory-backed** container path through the same code (or plant a `Player=true` component in the dev instrument) | `[CUSTODY] HOST park REFUSED ... reason=boundary1` | the refusal fires. A boundary that has never been seen refusing is a boundary nobody has tested. |

**Probe-before-claim.** Per the programme's standing rule and its own recorded defect class, each
observable's detector must be shown returning TRUE on a known-true input before its FALSE is
believed. Concretely: grep the A0 (unfixed) host log for `[CUSTODY]` first — it must return **zero**
lines, which proves the grep is looking at the right file and the fixed build's lines are new; and
the `parked`/`reattached` grep must be shown matching a planted line before an arm's absence is
scored.

### 4.2 Standalone tests that run without the game

In the style of `1c393401:src/votv-coop/tests/baseline_delivery/` (a `run.ps1` that locates the
MSVC toolchain via `vswhere`, compiles `qualification.cpp` against a `stubs/` tree with
`/std:c++20 /EHsc /W4 /WX`, and runs both the fixed header and the **pinned pre-fix header** to
show the negative) and of `15589b7f:src/votv-coop/tests/standalone/candidate_02_prop_spawn_gate.cpp`
(a single `main()` of `Check(condition, label)` lines, with an `#if __has_include` fallback that
inlines the pinned pre-fix policy so one source proves both polarities).

Proposed: `src/votv-coop/tests/custody_park/` with `run.ps1`, `qualification.cpp` and a `stubs/`
tree, plus one standalone `candidate_17_container_custody.cpp` for the pure policy predicate.

The design must therefore put the decidable logic behind a **header-only, engine-free predicate and
reducer** — the same shape as `dispatch/prop_spawn_gate.h` and `interactables/pending_retry.h` — so
these tests are possible at all:

```
coop/props/container_custody.h        (proposed, header-only, no engine types)
    struct CustodyRecordSet { std::vector<Rec> recs; uint64_t hash; int32_t index; };
    class CustodyParkStore {           // key -> record set, bounded FIFO + map, TTL
        bool Park(key, set, now);
        bool TryConsume(key, int32_t freshSlotCount, now, CustodyRecordSet& out, Reason& why);
        void Sweep(now, bool bracketOpen);
        void Clear();
        counters...
    };
    enum class Reason { Applied, DiscardedFreshNotEmpty, Expired, NotParked, Evicted };
```

Positive controls (each must pass on the fixed logic **and fail on the pinned pre-fix stub**):

1. `Park(K, {r1,r2}) → TryConsume(K, freshSlotCount=0)` yields `Applied` with the identical ordered
   record set and the identical FNV-64 hash. *The core claim.*
2. `TryConsume(K, freshSlotCount=3)` yields `DiscardedFreshNotEmpty` and **writes nothing**.
   *The CONTROL-arm regression guard; this is the assertion A2 exists to confirm in the world.*
3. `TryConsume(K')` for a key never parked yields `NotParked` and writes nothing.
4. Double consume: a second `TryConsume(K)` after `Applied` yields `NotParked` — the park is
   consumed exactly once (idempotence of a duplicated intent, `MVF-0004` rule 8).
5. Re-park of the same key replaces, does not accumulate: `Park(K,A); Park(K,B); TryConsume(K)`
   yields `B`, and the FIFO holds exactly one entry for `K` (the shipped park set's desync bug,
   `prop_drop_intent.cpp:71-77`, reproduced as a test rather than as a comment).
6. Cap: `kMaxCustodyParks + 1` distinct keys evicts exactly the oldest, and the map and FIFO agree
   afterwards on membership, size and order.
7. TTL: a park older than the TTL sweeps to `Expired` when the bracket is closed, and **does not**
   expire while the bracket is open (`NoteJoinSnapshotBracket`'s shipped reasoning).
8. Ordering: a 512-record set round-trips in order; the hash is order-sensitive (swap two records →
   different hash).
9. Boundary at the cap: a 513-record set is refused at `Park` (`kMaxRecordsPerContainer`), and a
   512-record set is accepted.
10. `Clear()` empties both containers and zeroes the counters; nothing survives a session.

Codec controls (pure byte work; `save_record_wire` is engine-free by contract,
`save_record_wire.h:14-17`):

11. `SerSave`/`DeSave` round-trip of a record carrying a nested-container `ints[0][0]` **preserves**
    it when `neuterNested=false` (the host-local path) and **replaces it with `-1`** when
    `neuterNested=true` (the wire path). *This is the trap in §2.3 turned into an assertion.*
12. A truncated / oversized blob fails cleanly through the existing `Feasible` bound
    (`save_record_wire.h:36-38`) — the hostile-input contract is not weakened.

Omission cases (the harness must fail if the fix is removed, per `1c393401`'s ten-omission
discipline): delete the capture call; delete the fresh-empty check; delete the FIFO eviction;
delete the TTL sweep; delete the map/FIFO pairing on consume; neuter the nested index on the
host-local path; park on a `Player=true` inventory; apply without the allocation pre-flight; apply
without `RederiveManagedState`; consume twice. Each must turn at least one named assertion red.

---

## 5. Implementation plan (bounded; NOT authorised by this document)

### 5.1 Files touched — the whole set

| File | Change | Size |
|---|---|---|
| `src/votv-coop/include/coop/props/container_custody.h` | **new**, header-only: `CustodyRecordSet`, `CustodyParkStore`, `Reason`. No engine types, so it compiles in the test harness. | ~150 lines |
| `src/votv-coop/src/coop/props/container_custody.cpp` | **new**: the two engine-touching seams — `CaptureForDyingContainer(actor)` and `TryReattachForSpawnedContainer(actor, key)` — plus the log lines and counters. Everything else lives in the header. | ~180 lines |
| `src/votv-coop/src/coop/props/remote_prop_destroy.cpp` | **one call** added inside `DestroyResolvedLocalActor_` (`:73-99`), host-role-gated, before `R::CallFunction(actor, g_destroyActorFn, nullptr)` at `:98`. | +3 lines |
| `src/votv-coop/src/coop/props/prop_drop_intent.cpp` | **one call** added inside `HostSpawnPlacedProp` (`:158-232`), after `ApplySavedScalarForClass` (`:226-230`), before `return actor` (`:231`). | +3 lines |
| `src/votv-coop/include/coop/props/container_contents_sync.h` | declare the seams the custody module must reuse instead of reimplementing. `ReadContents` (`:271`) and `ApplyContents` (`:471`) are both in that file's **anonymous namespace**, so reuse needs public wrappers, not a copy: `ReadWorldContainerRecords(void* actor, std::vector<SaveRecord>& out, bool neuterNested)`, `WriteWorldContainerRecords(void* actor, const std::vector<SaveRecord>&)`, `MarkDirty(uint32_t eid)`. RULE 2 — "ONE implementation" (`save_record_wire.h:8`) — is the reason this is three wrappers and not a second reader. | +3 declarations |
| `src/votv-coop/src/coop/props/container_contents_sync.cpp` | thread `neuterNested` through `ReadContents` (`:271-289`, default `true` so wire behaviour is unchanged — measured: exactly one caller, `:334`); define the three wrappers over the existing private functions. | ~25 lines |
| `docs/devices.md` | add the missing container-contents row to "Who owns what" (`:171-183`), the `ContainerContents` row to "Wire messages" (`:185-196`), and a custody paragraph after `:71`. | docs |
| `docs/status.md`, `docs/code-map.md` | the lane's late-join row (doctrine Step 6) and the code-map entry. | docs |
| `src/votv-coop/tests/custody_park/**` + `tests/standalone/candidate_17_container_custody.cpp` | the harness of §4.2. | tests |

Two production files change by three lines each. Everything else is new, isolated, and testable
without the engine.

### 5.2 Exact functions to touch

```
remote_prop_destroy.cpp
  DestroyResolvedLocalActor_(void*, const std::wstring&, const PropDestroyPayload&, void*)   :73-99
      insert after :97 (native_pile_mirror::Unpin) and before :98 (K2_DestroyActor):
          if (IsHost()) coop::props::container_custody::CaptureForDyingContainer(actor, keyW);

prop_drop_intent.cpp
  HostSpawnPlacedProp(const PropDropIntentPayload&, const std::wstring& cls,
                      const std::wstring& key)                                              :158-232
      insert after :230 and before :231 (return actor):
          coop::props::container_custody::TryReattachForSpawnedContainer(actor, key);

container_contents_sync.cpp
  ReadContents(void* inv, std::vector<SaveRecord>& out)                                     :271-289
      -> ReadContents(void* inv, std::vector<SaveRecord>& out, bool neuterNested = true)
         guard the NeuterNestedIndex call at :285 on that flag. MEASURED: one caller (:334),
         which takes the default, so wire behaviour is unchanged.
  + ReadWorldContainerRecords / WriteWorldContainerRecords — public wrappers that apply
    IsContainerActor (:247), InventoryOf (:168), IsWorldContainerInventory (:191) and
    GObjStackSlot (:200) and then delegate to ReadContents (:271) / the raw-write block
    inside ApplyContents (:502-521, including the EngineAlloc pre-flight at :495-500 and
    RederiveManagedState at :521). Both private functions stay private.
  + MarkDirty(uint32_t eid) — a two-line public wrapper over the private g_dirty insert at :654.
```

### 5.3 Sequencing

1. Land the header + tests **first**, red, against the pinned pre-fix logic (the `baseline_delivery`
   discipline: show every gate red before trusting it).
2. Land the module and the two call sites; the tests go green; MSVC `/W4 /WX`.
3. Docs rows.
4. Rig arms A0 (cite) → A2 → A1 → A3 → A5. **A2 before A1**, because a fix that breaks the working
   path is worse than the bug.
5. `private/b150-aligned` cherry-pick with `kProtocolVersion` untouched at 150 and the constraint
   restated in the commit message.

### 5.4 Explicitly out of scope

- **No transport change, no Relay, no universal serializer** (the reviewed direction's three
  prohibitions).
- **No new `ReliableKind`, no protocol bump, no wire byte** under the primary mechanism.
- **The custody op (`kOpCustody`)** — designed in §2.6, *not implemented* by this plan. It is a
  second, separately-reviewed increment, gated on §7 Q-3.
- **OBS-R3Q-C01** — separate patch, separate review (§3).
- **OBS-R3Q-P01** (held-lid state) — untouched. It is `MVF-0002`/`PENDING-BUG-025` territory and
  `container_contents_sync.h`'s own rule keeps lid state a separate correlated contract.
- **Personal inventories.** Never authored, never repaired, never copied. `MVF-0004` rule 7.
- **Contents the host never held** — a client filling a carried container, a client-born container.
  Named as a known non-coverage, with its observable in §7 Q-3.
- **Orphan-slot reclamation.** No slot is removed, compacted, or index-shifted. `MVF-0004` rule 11.
- **Reroll suppression for rematerialized loot containers** (`MVF-0004` rule 5) — a distinct defect
  class; not addressed and not claimed.
- **The 45 causally-unresolved ledger rows.** This design claims one chain, measured once per route.
- **Any claim of unrestricted permanent data loss.** The persistence result is bounded and stays
  bounded.

---

## 6. Invariants, restated as one list

1. Host-authoritative world containers. A client causes a transition; it never asserts contents
   outside the shipped CAS path.
2. `propInventory_C.Player == true`, or an unresolvable `Player` flag, is refused everywhere,
   always, fail-closed.
3. A peer-local `GObjStack` index is never transmitted and never installed from the wire.
4. Actor lifetime is not state lifetime: contents survive the actor gap, keyed by the logical key.
5. A park is consumed at most once, by exactly one keyed spawn.
6. A park never overwrites a non-empty fresh slot. Native restore always wins.
7. Every store is bounded: 32 parks, 512 records, a 60 s TTL that does not run inside a join
   bracket.
8. Every refusal is logged with a named reason. A silent refusal is a defect.
9. Nested indices are preserved on the host-local path and neutered on any wire path. The two must
   not share a default.
10. No session state survives a session.

---

## 7. Open questions, each with the observable that settles it

| # | Question | Observable that settles it | Blocks |
|---|---|---|---|
| **Q-1** | Do the R3Q arms promote `BUG-017`'s applicability cell from `U2` to a named contract failure? The frozen matrix says `U2` and was frozen before the arms ran. | A design review that reads `REVIEW.md`'s primary/control claims against the matrix's `U2` definition ("the row does not establish a named contract failure") and rules. This is a **review decision, not a measurement**; the design does not self-promote. | the ledger row, not the patch |
| **Q-2** | Does the host's old `GObjStack` slot survive the container's destruction (orphaned) or is it cleared/reclaimed by the native teardown? | The bounded full-stack probe `MVF-0004` already specifies: before the client take, record host `{key, index, stack length, ordered hash}`; after the host destroy, read the same slot; after materialization, read the new index and hash; repeat after save/reload. Requires separate rig authorisation. | only the *alternative* reattach strategy (re-install `parkedIndex`). The recommended strategy (write into the fresh slot) does not need this answer. |
| **Q-3** | Is a wire custody transfer needed at all — i.e. is there a real chain where the host never held the contents? | A rig arm: client takes a **client-extracted** item into a container it is carrying, then places it. If the host's box shows those contents under the host-local fix, no wire is needed. If it does not, `kOpCustody` is justified. | the second increment only |
| **Q-4** | Does `propInventory_C::init` allocate a fresh slot, reuse a free one, or read `ints[0][0]` when `loadData` never ran? | The same stack probe as Q-2, plus a bytecode read of `propInventory_C::init` and `prop_container_C::loadData`. Currently `INFERRED` from the in-tree comment at `container_contents_sync.cpp:252-257`. | the strength tag on §1.4 step 5; not the patch |
| **Q-5** | Can the capture point read `GObjStack[Index].obj` safely at `remote_prop_destroy.cpp:97`, i.e. is the component still fully valid after `Unpin` and before `K2_DestroyActor`? | A dev-instrument arm: log `ContentsDigest(eid)` immediately before the destroy call and compare it with the value logged one tick earlier. Equal on every sample = safe. | the capture point; a fallback exists: the destroy seam is post-native and `prop_lifecycle.cpp:280-283` states *"K2_DestroyActor only marks PendingKill, so reads on the actor are still valid"*) |
| **Q-6** | Which transition actually loses the content? The two R3Q arms vary all three (grab, inventory, place) together. | Three single-transition arms on the unfixed build: client-grab-only + host-place; host-grab + client-place; client-grab + client-inventory + host-place. **Not required for this design** — the fix covers the whole custody window — but required before any causal statement is published. | published causal wording only |
| **Q-7** | Does a restored container's fan-out interact with `g_sentHash` / `g_publishedHash` / `g_appliedHash` entries left over from the pre-destroy eid? | Read the three maps' keying (`container_contents_sync.cpp:339-346, 359-364, 483-484, 518-520`) and assert in the harness that a fresh eid has no entry in any of them. Cheap; do it in review. | the fan-out step of §2.4 |
| **Q-8** | ~~Are there other callers of `ReadContents` whose behaviour a `neuterNested` default could change?~~ **CLOSED, `STRENGTH=MEASURED`.** `git grep -n ReadContents 3af5ddae -- src/votv-coop` returns exactly two lines: the definition at `container_contents_sync.cpp:271` and one call at `:334` (`BroadcastContainer`). A defaulted parameter changes nothing. **But** `ReadContents` lives in that file's anonymous namespace, so the custody module cannot call it directly — the reuse must go through a new declared seam in `container_contents_sync.h`, not through a copy of the reader. | closed |

---

## 8. Appendix — citation index (all on `3af5ddae`)

```
include/coop/net/protocol.h
  :30      kProtocolVersion = 152
  :483     ReliableKind::PropDropIntent = 90
  :535     ReliableKind::ReelEjectIntent = 104
  :589     ReliableKind::ContainerContents = 118 ("eid-addressed")
  :1083    propspawn_flags::kHasSavedScalar
  :1088-94 PropDestroyPayload (40 B; elementId 0 = "sender had no Element")
  :1099-112 PropDropIntentPayload (172 B; the complete field list; one-datagram assert)
  :1367-74 BlobChunkPayload (228 B; 220 data bytes per chunk)

include/coop/net/blob_chunks.h
  :45-47   MaxBlobBytes() = 220 * 255 = 56,100

include/coop/props/prop_drop_intent.h
  :12-15   why the client's intent is authored a tick LATE (the husk-destroy ordering guard)
  :44-50   OnPropDropIntent's contract

src/coop/props/prop_drop_intent.cpp
  :53-79   PendingPlace, kMaxPending=32, kMaxKeyTries=8, the park set/FIFO invariant, kMaxParked=64
  :107-152 OnClientFinishSpawn (the client place detector; :148-151 the container-extract latch)
  :158-232 HostSpawnPlacedProp  <-- THE APPLY POINT (:214 finish, :231 return)
  :263-363 Tick (the client's intent author; :311-353 payload fill and send)
  :365-375 NoteClientKeyedDestroy
  :377-400 OnPropDropIntent (:390-393 the duplicate guard)
  :402-428 OnReelEjectIntent (the class-whitelisted client-birth door)
  :430-435 Reset

src/coop/props/prop_destroy_seam.cpp
  :84-107  the client's load-episode / reconcile-window destroy suppression
  :128-149 the PropDestroy payload build and send (:131 the eid-0 sentinel)
  :154-156 the client's key park

src/coop/props/remote_prop_destroy.cpp
  :73-99   DestroyResolvedLocalActor_  <-- THE CAPTURE POINT (:98 K2_DestroyActor)
  :108-192 OnDestroyImpl_ (:133-151 eid-then-key resolution; :150 the key fallback that OBS-R3Q-C01 rides)

src/coop/dispatch/event_dispatch_entity.cpp
  :215-256 the PropDestroy case (:236-242 accepts elementId 0 deliberately)

src/coop/dispatch/event_dispatch_intent.cpp
  :258-280 the PropDropIntent case
  :281-302 the ReelEjectIntent case

include/coop/props/container_contents_sync.h
  :1-13    the lane's model and its two rules
  :56-68   the dev-instrument seams (SnapshotWorldContainers, ContentsDigest)

src/coop/props/container_contents_sync.cpp
  :41      kOpContents = 0            :55  kMaxRecordsPerContainer = 512
  :101     kConflictWindowMs = 1500   :117 kParkTtlSec = 30
  :191-195 IsWorldContainerInventory  <-- BOUNDARY 1
  :200-211 GObjStackSlot
  :252-262 NeuterNestedIndex          <-- BOUNDARY 2 + the loadData/init reading
  :271-289 ReadContents (:285 the neuter call the host-local path must skip)
  :307-316 PackContents (the blob grammar)      :321-323 ContentHash
  :332-376 BroadcastContainer         :400-421 DrainDirty
  :453-457 RederiveManagedState       :471-524 ApplyContents (the raw-write path to reuse)
  :528-558 HostAcceptsClientWrite (the CAS)     :560-600 ParseAndApply (:563 the unknown-op refusal)
  :629-662 OnVerbEntry (:633-639 the "if this line is absent, the lane is dead" convention)
  :667-677 NoteJoinSnapshotBracket    :729-759 OnContentsChunk (:735-742 the acceptance matrix)
  :761-780 QueueConnectBroadcastForSlot (the join seed)   :803-815 ContentsDigest
  :817-831 OnDisconnect

include/coop/items/save_record_wire.h
  :14-17   the hostile-input contract     :31-38 the caps and Feasible()
  :55-56   SerSave / DeSave

include/ue_wrap/actors/save_record.h
  :29-45   the SaveRecord POD             :55-57 kSaveStride 0x100 / kMxStride 0x10
  :93-102  ReadSaveRecord / WriteSaveRecord (the orphan-the-old-buffer rule)

src/coop/items/inventory_wire.cpp
  :36-45   the player envelope over the same per-record codec

src/coop/items/player_inventory_sync.cpp
  :391-435 OnReliable — the host persists a client's blob to that player's file and applies nothing

src/coop/net/session.cpp
  :262-267 the protocol-mismatch refusal on an unadmitted peer

docs/devices.md
  :41-71   "What is inside a container, and who may write it"
  :171-183 "Who owns what" (no container-contents row)
  :185-196 "Wire messages" (no ContainerContents row)

docs/coop-sync-doctrine.md
  :40-53   Step 2, exactly one owner per element
  :94-101  Step 5, identity at birth; identity across the actor gap
  :103-110 Step 6, the late-join row is part of the lane
  :111-119 Step 7, wire discipline (a new format bumps the build number in the same commit)
  :120-128 Step 8, evidence (every gate shown red before it is trusted)
  :129-138 Forbidden patterns
```

---

## 9. What a reviewer should attack first

1. **The neuter asymmetry** (§2.3, test 11). If the host-local capture reuses `ReadContents`
   unchanged, every nested container silently empties on re-attach and the patch ships a new data
   loss.
2. **The fresh-empty refusal** (§2.3, test 2, arm A2). Without it the fix double-applies on the
   host-authored path.
3. **Q-5**, the read validity at the capture point. If the component is not readable there, the
   whole capture moves to the destroy seam and the design changes shape.
4. **§1.4 step 5's strength.** It is `INFERRED` from an in-tree comment about Blueprint bytecode
   that this task did not re-measure. The design does not depend on it being right — the
   recommended strategy writes into whatever slot the fresh component resolved — but the *narrative*
   does, and a narrative stated at the wrong strength is this programme's standing defect class.
5. **Q-1.** Do not let this document's existence promote `BUG-017`'s matrix cell. That is a
   reviewer's call.

---

## 10. Provenance of this document

```
INPUTS READ (read-only, no checkout of the main repo, no branch switch, no commit)
  88841428:handoffs/stages/20260903T222051Z/{HANDOFF.md,RESULT.json,PLAYER-LOG-CANDIDATES-CROSSREF.md}
  88841428:handoffs/CORRECTIONS.md
  R3Q/LANES/ws-runtime-review-codex/{VERDICT.txt,STATUS.json,REVIEW.md,LAST-MESSAGE.md}
  R3Q/PACKET2/r3q-runtime-packet-2/{RESULT.json,SUMMARY.md}
  R3Q/CAMPAIGN-FROZEN/ws-implementation-readiness/READY-NOW-BRIEFS.md  (MVF-0001 :83-153, MVF-0004 :218-305)
  f3d6b59d:design/relay-adoption/{DECISION.md,BUG-APPLICABILITY-MATRIX.md,
                                 MULTIVOID-CURRENT-MODEL.md,PILOT-DESIGN.md}
  3af5ddae and ba6d8c39 and f690c017: the source files in the §8 citation index
NOT READ / NOT DONE
  No game process was started. No rig arm was run. No build was configured.
  The bounded full-stack GObjStack probe (Q-2/Q-4) was NOT run; it needs separate authorisation.
  No worktree was created; the main repo's checkout, branch and index were not touched.
STRENGTHS USED
  MEASURED       the R3Q arm outcomes, the build identity, the diffstats in §0.5
  SOURCE_PROVEN  every file:line read on 3af5ddae in §1 and §8
  INFERRED       the orphan-slot reading, the CONTROL-arm explanation, every proposal in §2
  UNKNOWN        Q-2 through Q-6, and everything outside R3Q's declared searched scope
```
