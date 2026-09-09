# REPORT-2 (bug_02 door key mismatch) + REPORT-4 (bug_04 light switch) — ONE defect, two surfaces

STRENGTH=MEASURED  FIX_READINESS=UPSTREAM_ALREADY_FIXED

## Defect
In the reporter's terms: after a world load, every door / light switch / light group that is not part of the base set is addressable only by the peer it was loaded on. The client's E-press on a radio-telescope door emits `door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'`, that key occurs zero times in the host log, no `door: applied` ever comes back, and the host's own toggles of the same physical door ride a key ('f2kqYIFFMbZ5pm8ZO-ZlUw') the client has never seen — so the host walks through a door that stays shut on the client. The identical shape holds for the light switch ('BqgD3X87kKv8T_DyLebx2g', 0 host occurrences, host retry queue reports `dropped 1 expired`), and the whole join snapshot for these families dies the same way (`dropped 31 / 27 / 27 expired` at 21:49:55). In source terms: the mod does not derive interactable keys at all — it READS the game's own instance Key. `interactable_sync.cpp` hands the door channel `ue_wrap::door::GetKeyString` and the light/lightgroup channels `ue_wrap::lightswitch::GetSwitchKeyString` / `GetKeyString`, each a raw read of the `AtriggerBase_C::Key` FName; `Channel::HubMatch` puts that string straight into `byKey_`, and `ResolveFast` is a plain map lookup on it, with a miss going to `pending_` and expiring on a 25-second wall clock. VOTV assigns that Key through `lib_C::assignKey`, whose bytecode is `if (keyIn == None) keyIn := generateRandomKey()` — a fresh crypto-random 22-char base64url FName minted INSIDE the process that ran the load. Actors whose Key is baked into the cooked level (the 14 `basedoor_*`/`alphaBunkerDoor*` names and 5 author-baked base64url keys) are byte-identical on both peers; the instances the world load brings in — measured in-tree at b150 as child-actor instances for lights — carry Key=None in the asset and therefore get a per-process random key. The wire is fine, the transport is fine, the lane is fine: the ADDRESS is private to each process, so the message names nothing on arrival and is dropped without a warning.

## Log verification
RE-DERIVED BY ME, from the raw logs, not taken from the reporter.

(1) Index stability, all five logs at once —
  cd C:/AgenticStaging/VOTV/Coordination/C1/EVIDENCE/doctaaaa-logs
  grep -hoE "(door|light|lightgroup): index rebuilt -- [0-9]+ live" *.log | sort | uniq -c
    1 door:0  8 door:19  8 door:50 | 1 light:0  8 light:15  8 light:42 | 1 lightgroup:0  8 lightgroup:15  8 lightgroup:42
  for ch in door light lightgroup; do for n in 19 15 50 42; do grep -hE "$ch: index rebuilt -- $n live" *.log | grep -oE "keysHash=0x[0-9A-F]{16}" | sort -u | wc -l; done; done
    door 19 -> 1 distinct hash over 8 rebuilds (0xB177A4B1F440E053)
    light 15 -> 1 distinct (0x125425EF3DB15927); lightgroup 15 -> 1 distinct (0x55CFFE39361F71BF)
    door 50 -> 8 distinct over 8 rebuilds; light 42 -> 8 distinct; lightgroup 42 -> 8 distinct
  STRONGER THAN THE REPORT, which claimed 4 distinct over 2 loads: it is 8 of 8, over 8 world loads spanning both peers and four sessions, with ZERO repeats. Per-load AND per-peer.

(2) The arithmetic closes exactly. client_040920262246 line 15410-15412 [21:49:55]:
  `door: retry tick -- applied 0 deferred, dropped 31 expired, 0 still pending` / `light: ... dropped 27 expired` / `lightgroup: ... dropped 27 expired`
  and before that tick the client applied `door: applied` for 19 distinct keys and `lightgroup: applied` for 15 distinct keys (awk NR<15410 + grep -oE "key='[^']+'" | sort -u | wc -l). 50-19=31, 42-15=27, 42-15=27. Every non-baked instance in the host's connect snapshot died unapplied.

(3) Cross-peer key occurrence (grep -c on the whole file):
  pDcZUv8cxnVTI-ey_0Jo0w  client_s1=18  host=0   (client's telescope door)
  f2kqYIFFMbZ5pm8ZO-ZlUw  host=8        client=0 (host's key for the same door)
  I6J8HCQDcitfWptyjA4Jwg  client_s2=2   host=0   (the delta-door addendum)
  BqgD3X87kKv8T_DyLebx2g  client_s1=3   host=0   (the light switch, Report 4)
  All four reporter claims of "0 occurrences on the other peer" CONFIRMED.

(4) The 19 stable door keys, listed: 14 human names (basedoor_signalroom/garage/entrance/...; alphaBunkerDoor, alphaBunkerDoor_in) + 5 base64url (2IVTljm57lxfJVNyipZsLA, cINjJa7Yd_VZwzGaOAIm4Q, opu4dmuubchlNraKht3k2w, vfHBOoFDLBu-MbDb1iRrNQ, xuHKjzEpmWoHshjk9wT77A). So key FORMAT is not the discriminator — provenance is. Confirms the reporter's addendum finding 2.

WHAT I COULD NOT RE-DERIVE FROM THE LOGS: which physical door each key belongs to (no probe line in these runs); whether the session-1 door was opened by the host's toggle or by the native hammer-break (the reporter already marks this unconfirmed); and why the delta door's native use ran unsuppressed while the session-1 door's did not — none of these are logged. They do not bear on the key defect.

## Source chain
- The door channel's key function is a RAW READ of the game's AtriggerBase_C::Key FName — the mod derives nothing. Adapter registration, then the reader.
    b150: src/votv-coop/src/coop/interactables/interactable_sync.cpp:53 (`&ue_wrap::door::GetKeyString`); src/votv-coop/src/ue_wrap/devices/door.cpp:206-211
    main: src/votv-coop/src/coop/interactables/interactable_sync.cpp:44; src/votv-coop/src/ue_wrap/devices/door.cpp:198-203
    same_code: True
- Light and lightgroup do the same: the switch's Key and the trigger_lightRoot_C's Key, both raw AtriggerBase_C::Key reads.
    b150: src/votv-coop/src/coop/interactables/interactable_sync.cpp:119 (`GetSwitchKeyString`), :158 (`lightswitch::GetKeyString`); src/votv-coop/src/ue_wrap/devices/lightswitch.cpp:99-104
    main: src/votv-coop/src/coop/interactables/interactable_sync.cpp:86, :111; src/votv-coop/src/ue_wrap/devices/lightswitch.cpp:96-101
    same_code: True
- That string IS the index key and IS the wire key. HubMatch drops the game key straight into byKey_; ResolveFast is a bare map lookup; a miss becomes a pending entry that expires on a 25-second wall clock and logs the `retry tick -- dropped N expired` line the reporter quotes.
    b150: src/votv-coop/include/coop/interactables/interactable_channel.h:517 (`std::wstring key = a_.GetKey(obj);`), :561 ResolveFast, :231 pending_ + :58 kPendingTTL=25s, :454 expired log, :463 retry-tick summary
    main: interactable_channel.h:473 (same `a_.GetKey(obj)` line) BUT :479 now overrides it with `coop::element::PortableWireKey(obj)`; :575 ResolveFast; :196 pending_ + :52 kPendingTTL=10min; :389 expired-on-settle log; :411 retry-tick summary
    same_code: False
- The `keysHash` the reporter compares is an order-independent XOR of FNV-1a-64 over the CURRENT key set, logged only when (count,hash) changes — i.e. it is exactly a cross-peer key-set digest, which is what makes the reporter's comparison valid.
    b150: src/votv-coop/include/coop/interactables/interactable_channel.h:535 (`keysHash ^= FnvKey(kv.first)`), :545 log; src/votv-coop/include/coop/net/wire_key_util.h:38-45 FnvKey
    main: interactable_channel.h:502, :518; wire_key_util.h:40 (file byte-identical between the two refs: `git diff -w ba6d8c39 3af5ddae -- .../wire_key_util.h` is EMPTY)
    same_code: True
- The wire carries the key verbatim as a 32-byte WireKey (len + 31 chars) with no transformation, and the header states outright that these are the game's own instance Keys.
    b150: src/votv-coop/include/coop/net/wire_key_util.h:7-9, :21-27 WireKeyFromString
    main: src/votv-coop/include/coop/net/wire_key_util.h:7-9, :21-27 (identical file)
    same_code: True
- WHY the world-load-added instances get per-peer keys — the game mints them. `lib_C::assignKey` @81-133 is `if (keyIn == None) keyIn := generateRandomKey()`, a crypto-random base64url FName produced in whichever process ran the load; level-PLACED actors carry author-baked keys that DO match. (b150 evidence is disassembly recorded in-tree; the same mechanism is asserted in upstream's own new header.)
    b150: docs/vehicles/ATV.md:1388 (the assignKey bytecode table) and :1424 (22-char base64 GUID confirmation from a field log); docs/piles/findings/votv-snapshot-adoption-root-causes-2026-06-10.md:73 (`lib_C::assignKey -> generateRandomKey` Base64Url FNames; level-PLACED actors carry author-baked keys that DO match) and :192 (`Door keysHash divergence host-vs-client ... a subset of triggerBase keys is runtime-generated ... NOT user-reported yet; investigate separately`); docs/LESSONS.md:5380-5390 (the keying pass is `mainGamemode::loadObjects -> GetAllActorsWithInterface + loadTriggers`, sublevel-gated)
    main: src/votv-coop/include/coop/element/portable_identity.h:3-8 — `lib_C::assignKey` mints a random 16-byte base64url FName for any `triggerBase` whose Key is None at load ... both PER PROCESS ... 31 doors of 50, 27 lights of 42, 27 light groups of 42, 25 containers of 56
    same_code: False
- The affected LIGHT set was already measured and written down at b150, with the reporter's exact numbers, and marked pre-existing: 15 of 42 lightRoots cross-peer stable, the other 27 child-actor instances with per-peer keys, unreachable by any Key lane.
    b150: docs/COOP_SYNC_MAP.md:142
    main: file removed by the doc sweep on new main; the equivalent statement is portable_identity.h:3-8 and the 86d04953 commit body
    same_code: False
- The in-tree PRECEDENT that a placed actor should be keyed by something other than the game Key already exists at b150 and is shipped for two other channels: garage and door_box are addressed by their baked level-export FName via GetNameKey, explicitly because the gamemode's one-shot Key pass is unreliable.
    b150: src/votv-coop/src/coop/interactables/interactable_sync.cpp:191 (the rationale comment), :203 `garage::GetNameKey`, :237 `door_box::GetNameKey`; src/votv-coop/src/ue_wrap/devices/garage.cpp:68-74; src/votv-coop/src/ue_wrap/devices/door_box.cpp:158-161
    main: interactable_sync.cpp:147, :175; garage.cpp:69; door_box.cpp:158 — both GetNameKey bodies are comment-stripped-identical between the refs (0 code-diff lines)
    same_code: True

## Upstream status
ALREADY FIXED UPSTREAM, after b150, by the exact mechanism this trace points at — and upstream's own commit names this very field report.

Commits on 3af5ddae, none of them ancestors of ba6d8c39:
- 40cc6cc8 (2026-09-05) `[coop] portable identity: our layer names an actor across two processes, and the game's Key is never written` — adds src/votv-coop/include/coop/element/portable_identity.h + src/votv-coop/src/coop/element/portable_identity.cpp (absent at b150: `git ls-tree -r --name-only ba6d8c39 | grep portable_identity` is empty).
- 86d04953 (2026-09-05) `[coop] B2: the interactable channels address by portable identity, and four channels agree cross-peer for the first time`. Its body: "Field defect B2 -- a client whose radiotelescope door would not open, 72 presses, `door: applied` zero times ... 110 interactables per peer carried a key the other peer had never heard of", and its measured table shows door/light/lightgroup/container keysHash now BYTE-IDENTICAL host vs client where all four had DIFFERED, with the `dropped N expired` counter that "read 31/27/27/25 = 110 on every join" logging zero lines. Those are the reporter's 31/27/27 to the instance.
- e8484fca (2026-09-07) demotes the run counts from contract to evidence (residual: 109 of 110 instances got an identity in that run).
- b91777ba (2026-09-07) pins the anchor ORDER: key before name, because a level-placed anchor is destroyed during the world load and replaced by its save-loaded twin, so the name changes within one session and the key does not.

The rule (portable_identity.h:23-33, .cpp:42-100): portable(a) = child actor -> portable(parent)+"/"+component; RF_WasLoaded -> "n:"+UObject name; Key!=None -> "k:"+Key; else "" (no identity, said out loud, never guessed). Wire form = "mv_" + 16 hex of FNV-1a-64 over the UTF-16 code units, 19 chars. Wired in at interactable_channel.h:479 inside HubMatch (the ONE derivation site) with Channel::KeyForActor(actor) at :459 for the E-press observer, and interactable_sync.cpp:169/190 now asks the channel rather than the game.

Two things upstream fixed alongside, both visible in these logs: the deferred-apply expiry moved off a 25-second WALL CLOCK (b150 interactable_channel.h:58) to "the channel's live count unchanged for 5 passes AND the key still does not resolve", with the clock kept only as a 10-minute backstop logging a different line (main :52, :389, :401) — the b150 bound is on the wrong quantity and a field join is far slower than the lab one; and an E-press that names nothing now WARNs instead of returning silently (main interactable_sync.cpp:190-202).

Upstream also found a SECOND, independent defect while measuring this, which bears directly on Report 2's "the base doors sync fine": the door index went 50 -> 57 under portable identity because 57 doors carry only 50 distinct game keys, and ONE key — `cINjJa7Yd_VZwzGaOAIm4Q` — is shared by EIGHT different doors. `byKey_` is a std::map, so seven of those eight were unreachable to either peer, in single-player too. That key is in the reporter's own list of five "working" base doors, and I confirmed it appears exactly once in the client log and zero times in the host log.

PROTOCOL: 86d04953 bumped kProtocolVersion 151 -> 152 with the note "THE INTERACTABLE WIRE KEY CHANGES MEANING for 110 instances per world". No payload or struct changed; the bump is a compatibility gate, not a format change. b150 is at kProtocolVersion 150 (protocol.h:710); new main is 152 (protocol.h:30).

CONSEQUENCE FOR A CONTRIBUTION: there is nothing to send upstream for this defect. The remaining work is a BACKPORT onto the private b150-aligned line (which is what players actually run), plus independent verification, plus the residual cases upstream itself left open.

## Proposed fix
THE ANSWER TO "which peer-stable key input" IS ALREADY DECIDED AND SHIPPED UPSTREAM; for the b150 line the bounded action is a backport, not a design.

What to backport (files/functions):
1. NEW src/votv-coop/include/coop/element/portable_identity.h + src/votv-coop/src/coop/element/portable_identity.cpp verbatim from 3af5ddae (post-b91777ba, so the key-before-name anchor order is included). Add to src/votv-coop/CMakeLists.txt as 40cc6cc8 did, and call coop::element::RunSelfTest() once per session from src/votv-coop/src/harness/session_runtime.cpp.
2. src/votv-coop/include/coop/interactables/interactable_channel.h — in HubMatch (b150 :515-521), after `std::wstring key = a_.GetKey(obj);`, override with PortableWireKey(obj) when non-empty (7 lines, main :479-485); add `std::wstring KeyForActor(void* actor) const` (main :459-465) and make indexMutex_ mutable.
3. src/votv-coop/src/coop/interactables/interactable_sync.cpp — the door E-press observer (b150 :403 and :435) must ask `g_door.KeyForActor(door)` instead of `ue_wrap::door::GetKeyString(door)`, so the request names what the index holds. ONE derivation site is load-bearing: ParentActorOf reads a weak pointer, so two derivation sites can disagree during join GC churn.
4. Two small ue_wrap additions b150 lacks: `ue_wrap::engine::ParentActorOf(actor, &componentName)` (with its property offset CACHED — it is now called per instance per hub pass) and `ue_wrap::prop::GetActorSaveKeyString` (narrow, for the Aactor_save_C lineage that GetInteractableKey cannot read and reports as "None"). b150 already has `IsChildActor` (engine.h:163) and `GetInteractableKeyString` (prop.h:115).
5. Bring the expiry change with it (interactable_channel.h :52/:196/:389/:401/:411): settle-passes conjunction plus a 10-minute backstop on a distinct line. Without it a slow field join can still expire a snapshot that WOULD have resolved.

Why this cannot regress the legitimate path: PortableWireKey returns "" for any actor with no derivable identity, and the override is `if (!portable.empty())` — so every instance that works today (the 14 named base doors, the 5 baked base64url keys, `lightswitch_outside`, garage and door_box which are on GetNameKey and not on this path at all) keeps a key. For a WasLoaded anchor the identity is "k:"+its own Key, i.e. the same discriminating input, merely rehashed; for a child actor it is parent-identity + component name, and UE guarantees component names unique within an actor, so uniqueness is structural rather than measured. Nothing writes the game's Key field, so no game state and no save is touched, and prop_element_tracker's host-only re-key invariant is untouched. The one real behaviour change is intended: seven doors that shared `cINjJa7Yd_VZwzGaOAIm4Q` and were map-overwritten become individually addressable.

The three candidate inputs the task names, judged:
- SAVE-RECORD IDENTITY — REJECTED for this family. Doors/lights are AtriggerBase_C; their persistence is triggersData keyed BY the same Key, not an objectsData ordinal, so the save record is downstream of the broken key rather than upstream of it; and b91777ba measured that a level-placed anchor is destroyed and replaced by its save-loaded twin during the world load, so no single save-record ordinal survives the load as one identity.
- CLASS + QUANTISED LOCATION — REJECTED, and already rejected in-tree: it is the "key by WORLD POSITION" fallback that /qf R2-Q2 called a RULE-1 hope (a property of the current save's geometry, not an invariant) — see the LESSONS.md entry at :5395-5405. It also cannot separate co-located child actors of one dish.
- HOST-ASSIGNED KEY EXCHANGE AT JOIN — workable in principle but strictly worse here: it needs a new message kind and therefore a real protocol change, it re-introduces a join-window ordering dependency, and it gives no identity to anything spawned after the snapshot. Portable identity needs no exchange at all: two peers with the same world compute the same 19 characters independently.

WIRE COMPATIBILITY, precisely. The wire FORMAT does not change: WireKey is len + 31 chars (wire_key_util.h:21-27) and "mv_"+16 hex is 19 characters, so no payload, no struct, no size, no new kind — nothing a protocol constant describes moves. But the MEANING of the field changes for 110 instances, so a peer on the old scheme and a peer on the new one address different namespaces and would silently fail exactly as today. That is why upstream bumped 151->152 anyway. Honest answer to the question as asked: compatible with the existing wire without any payload change, NOT compatible without a version gate — and on the b150 line the same reasoning forces a kProtocolVersion bump from 150 for any build that carries this.

## CLAIM/FALSIFIER/PROBE/GAP
CLAIM: The door/light/lightgroup keyed indexes diverge cross-peer because the mod addresses instances by the GAME's own instance Key, and VOTV mints that Key at random inside each process (`lib_C::assignKey`: `if (keyIn == None) keyIn := generateRandomKey()`) for every instance whose Key is not baked into the cooked level — so the divergent set is exactly the non-baked instances, and the failure is an ADDRESSING failure, not a transport or lane failure.

FALSIFIER: (a) if the divergence were transport/lane breakage, a STABLE-keyed instance of the same family would fail the same way — so a stable-keyed light switch that round-trips client->host end to end falsifies it; (b) if the keys were merely per-SESSION rather than per-PROCESS-per-load, the same peer would reproduce its own hash on a second load of the same world; (c) if key FORMAT (base64url vs human name) were the discriminator, no base64url key would ever sync; (d) if the mod derived or transformed keys, a derivation site would exist in the source between GetKey and the wire.

PROBE: (1) count distinct `keysHash=` values among the `<channel>: index rebuilt -- N live` lines, bucketed by N, across both peers and all loads; (2) grep -c each disputed key in the other peer's whole log; (3) read the key path end to end in source from adapter registration to WireKeyFromString.

PROBE POSITIVE CASE — demonstrated before any negative was believed:
 - The probe DOES report stability when stability exists. The pre-world-load index (19 doors / 15 lights / 15 lightgroups) yields exactly ONE distinct hash per channel across 8 rebuilds spanning both peers and four sessions: door 0xB177A4B1F440E053, light 0x125425EF3DB15927, lightgroup 0x55CFFE39361F71BF. A probe that simply always says "different" could not produce that.
 - The MECHANISM works end to end when the key is stable. Client `client_040920262246:20513 [21:58:58] light: sent ON key='lightswitch_outside'` is answered by host `host_050920260342:9713 [21:58:58] light: applied ON key='lightswitch_outside' ok=1 (from slot 1)`, four sends and four applies in the same two seconds. So falsifier (a) is tested and does not fire: the client->host light lane is alive; only the ADDRESS is dead.
 - Falsifier (c) is tested and does not fire either: five base64url-format door keys (2IVTljm57lxfJVNyipZsLA, cINjJa7Yd_VZwzGaOAIm4Q, opu4dmuubchlNraKht3k2w, vfHBOoFDLBu-MbDb1iRrNQ, xuHKjzEpmWoHshjk9wT77A) are among the 19 that apply ok=1 at join. Format is not the discriminator; provenance is.
 - Falsifier (b) is tested and DOES fire in the claim's favour: the host reloaded the same world four times and produced four different door hashes (0xF1E83244393256B6, 0xEA9927171B688840, 0x8D9AFA3D17720825, 0xA5343A3239B50A2A) — per-load, not merely per-peer, which is what "minted at load" predicts and "assigned once and cached" does not.
 - Falsifier (d) is tested in source and does not fire: door.cpp:206-211 and lightswitch.cpp:99-104 are raw FName reads at a resolved offset, HubMatch stores the string unchanged, WireKeyFromString copies bytes. There is no derivation site to blame.

GAP: I did not observe a single door instance carrying two different keys on the two peers — no probe line (`door[probe]: key=... actor=...`) is enabled in these runs, so instance-level identity is inferred from the count arithmetic (50-19=31 = the exact expired count) and from the in-tree measurement that 27 of 42 lightRoots are child-actor instances, not seen directly. The keysHash is an XOR fold, so it proves key-SET inequality only to ~2^-64 collision probability and says nothing about which keys differ.

## Standalone test shape
Two tests, both host-free, matching the tests/baseline_delivery/ shape already in this tree (a qualification.cpp + run.ps1 + stubs, no engine).

TEST 1 — the pure half, no game at all. Link coop/element/portable_identity.cpp against a stub reflection layer and assert the cross-machine contract directly: IdentityHash("") == 0xcbf29ce484222325, IdentityHash("a") == 0x089be207b544f1e4, IdentityHash("n:dish19/lightroot") == 0x5824277a40808c78; wire token is exactly 19 chars and matches ^mv_[0-9a-f]{16}$; "n:dish19/door" != "n:dish5/door" (parent distinguishes) and != "n:dish19/lightswitch" (component distinguishes); a Cyrillic pair one UTF-16 code unit apart does not collide. Upstream already ships this as coop::element::RunSelfTest() (portable_identity.cpp:129-176) — the standalone test is that function plus a RED arm: mutate the hash to fold only the low byte of each wchar_t and assert the Cyrillic check FAILS, so the test is shown to be able to fail before its PASS is trusted.

TEST 2 — the addressing property, with a fake actor graph. Stub IsChildActor / ParentActorOf / GetInteractableKeyString / NameOf / ObjectFlags and build two "peers" over the same synthetic world: 19 anchors with baked keys and RF_WasLoaded set, plus 31 child actors under named dish parents whose Key is a DIFFERENT random string on each peer (that is the defect, injected). Drive Channel::HubMatch/HubPassComplete on both and assert: (a) with a_.GetKey alone the two keysHash values differ and a request minted on peer A resolves to nullptr on peer B — the RED arm reproducing Report 2/4 without a game; (b) with PortableWireKey the two keysHash values are byte-equal and every peer-A request resolves on peer B; (c) the 19 baked anchors keep resolving in BOTH arms (the no-regression assertion); (d) an actor that is top-level, not RF_WasLoaded and keyless yields "" and is EXCLUDED rather than given a guessed key; (e) the eight-doors-one-key case: eight distinct child actors sharing one parent-baked game key collapse to one byKey_ entry in the RED arm and produce eight entries in the GREEN arm.

## Rig observable
On the four-copy rig, with NO code change, on b150 bytes (the oracle zip), the defect is observable in one join and needs no player skill:

1. Host and client both boot to the MENU and log `door: index rebuilt -- 19 ... keysHash=0xB177A4B1F440E053` (and light/lightgroup 15). BASELINE CONTROL — these three hashes must be IDENTICAL on both peers; if they are not, the rig is wrong, not the game.
2. Load the world. Both peers log `door: index rebuilt -- 50 ...`, `light: 42`, `lightgroup: 42`. PREDICTION: all three hashes DIFFER between host and client, and differ again from either peer's previous load of the same save.
3. Client joins. PREDICTION: within ~30 s the client logs `door: retry tick -- applied 0 deferred, dropped 31 expired` and `light`/`lightgroup: ... dropped 27 expired`, and exactly 19 `door: applied ... ok=1` / 15 `lightgroup: applied ... ok=1` lines precede it.
4. Client presses E on any radio-telescope door: `door: use-input hook -> toggle request key='<22-char base64url>'`; grep that key in the host log — zero hits, no `door: applied`.
5. POSITIVE CONTROL in the same session: client flips the outside light switch and both logs show `light: sent ... key='lightswitch_outside'` on the client and `light: applied ... ok=1 (from slot 1)` on the host. This separates "the lane is broken" from "the address is private".
6. Cheap extra evidence this run should collect and the reporter's runs did not: set the channel probe log on both peers so `door[probe]: key=... idx=... actor=...` prints the full key list, then diff the two lists — that turns the inferred 31/19 split into a named per-instance split, and on b150 it should also show `cINjJa7Yd_VZwzGaOAIm4Q` bound to only ONE door while eight exist.

AFTER a backport, the same script is the acceptance test: step 2's three hashes byte-equal cross-peer, step 3 logging zero expired, step 4's key present in the host log with a matching `door: applied ... ok=1`, and step 5 still passing (no regression on the already-working set).

Nothing here needs the master server, and it uses only -saveddirsuffix state separation.

## Related ledger ids
REPORT-2 / bug_02 door key mismatch (this report), REPORT-4 / bug_04 light switch (SAME defect, different channel — do not track as two), upstream field defect B2 — commits 40cc6cc8, 86d04953, e8484fca, b91777ba on 3af5ddae, b150 in-tree deferred item: docs/piles/findings/votv-snapshot-adoption-root-causes-2026-06-10.md:192 'Door keysHash divergence host-vs-client ... NOT user-reported yet; investigate separately' — this report IS that user report, 2026-09-04, b150 in-tree measurement: docs/COOP_SYNC_MAP.md:142 '15 of the level's 42 lightRoots are cross-peer stable; the other 27 are child-actor instances with per-peer keys' (pre-existing, known, unfixed at b150), NEW, not in the reporter's material: 57 doors share 50 game keys and cINjJa7Yd_VZwzGaOAIm4Q addresses EIGHT doors — a single-player addressing defect inside the reporter's 'these 19 work' set, adjacent same-class: 25 of 56 containers carry per-peer keys (portable_identity.h:3-8) — the container channel has this defect too and no report covers it

## Gaps
- No per-instance proof: the runs carry no `door[probe]`/`light[probe]` lines, so 'the 31 divergent doors ARE the non-baked ones' rests on the count arithmetic (50-19=31 exactly matching the expired count) plus the in-tree child-actor measurement, not on a key-by-key cross-peer diff. A rig run with the probe log on would close this.
- The `lib_C::assignKey -> generateRandomKey` bytecode is cited from in-tree disassembly records (docs/vehicles/ATV.md:1388, votv-snapshot-adoption-root-causes-2026-06-10.md:73) and from upstream's own header. I did not re-disassemble the blueprint myself this session, and no game bytecode is readable from this repo.
- Why the 5 base64url base-door keys are stable is INFERRED (author-baked into the cooked .umap and therefore RF_WasLoaded on both peers) rather than measured; I did not open the .umap. The alternative — that they were minted once and persisted into a save both peers loaded — is not excluded by anything I checked, and it would not change the fix.
- Report 2's two non-key questions remain unanswered and are NOT addressed by this fix: which path finally opened the session-1 door (host toggle vs native hammer-break), and why the client's native door use was suppressed for the session-1 door but ran free for the delta door at 22:53:18 producing a silent client-local state fork. The second one matters — after the key fix the delta case becomes a normal synced open, but if the suppression rule itself is instance-dependent there is a residual behind it.
- Neither report's reverse direction (host->client broadcasts for a specific hash-keyed instance) is observed per message; it is inferred from the same key-set divergence and from the 31/27/27 join burst. This is sound but it is inference.
- Upstream's own residual: PortableIdentity returns "" for an actor that is top-level, runtime-created and keyless (the header names a crematorium door), and one run measured 109 of 110 covered (e8484fca). I have not enumerated which b150 instances fall in that hole.
- The fix has been measured only by upstream's own autonomous two-peer smoke (86d04953: NOT hands-on). It has never been run on the b150 line, on the oracle bytes, or with these players' save. Backport correctness on b150 is UNKNOWN until the rig run above.
- keysHash is an XOR fold of FNV-1a-64, so hash inequality proves key-set inequality only to ~2^-64; and hash EQUALITY after a fix would not by itself prove per-instance correctness, only set equality. The acceptance test should assert the applied/expired counts too, not the hash alone.
- I did not build, run the game, or write into any repository this session, as instructed; every source claim is from `git show`/`git grep` at ba6d8c39 and 3af5ddae, and every log claim from grep over the five unpacked logs.
