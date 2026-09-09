# C1 delta brief — 2026-09-09 (Claude-native continuation)

Measured facts are in CONTEXT.md; this file records decisions and the running batch. Update in place
as results land; every sha below was pasted from git output in this session.

## Verified bases

| line | head | note |
|---|---|---|
| upstream main (VOTV-MP) | 3af5ddae (2026-09-08, proto 152) | FORCE-REWRITTEN since f690c017; b150 tag not an ancestor; no b151/b152 release; tools/ tree deleted upstream, CI gates moved to .github/ci/ |
| b150 release tag | ba6d8c39 (proto 150) | what players run; code-identical to f690c017 except the version constant |
| private/b150-aligned | 998c43b6 | reviewed NOT_CLEAN on packaging provenance only; untouched |
| private/b150-aligned-fixes | **e2277707** | 998c43b6 + four clean cherry-picks (39c1fb88, e917477f, 653be7cc, d437aefc; blob-verified) + the C01 gate (f4624265, 413c3d06, ece4d354; clean) + candidate 8 (e4191f3b, f1d22695 [one comment-only conflict in prop_drop_intent.cpp resolved: b150 comment kept, fix code taken], c3706d9c, 0061c852, a557a7cf, e2277707). All nine new picks verified code-identical to their originals with comments stripped; protocol.h blob unchanged. Checked out in S:\GAMEMODDING\Multivoid-Private-B150. Rebuild at e2277707 EXIT=0 (2026-09-09 08:06:33Z): main.dll sha256 58d0d365fc99eb1611c67f3487dfb07ee726efdf61fb7ba74b96b50afd528b89 (18299392 bytes), embeds MULTIVOID_SOURCE_COMMIT=e22777072ca89d4b604e0b3218044a03b8955cba; policy selftest PASS/PASS/PASS; drills running (log C1\EVIDENCE\drills-b150-aligned-fixes-e2277707.log). This is the rig-test candidate. |
| fix/retarget-3af5ddae | 48497664 | REVIEWED CLEAN (fresh Opus reviewer): four picks code-identical, harness 10/10 with a negative control built from 3af5ddae's pre-fix blob, standalone runner both arms, protocol.h identical blob. Worktree C1\WORKTREES\retarget-upstream |
| fix/c01-keyed-destroy-gate | a482a5d7 | REVIEWED CLEAN twice (two fresh reviewers, 0 blocking). Worktree C1\WORKTREES\fix-c01 |
| fix/place-queue-admission | d8cd3c1e | REVIEWED CLEAN after two comment-only respins (125f174c → 4d6e8437 → d8cd3c1e); shape authorisation = orchestrator adjudication (eviction accepted, below). Worktree C1\WORKTREES\fix-cand08 |

## Upstream changes that matter (from the Opus rewrite audit)

- Only 141 of 670 changed src files carry code changes; ~30 behavioural.
- **86d04953 portable interactable identity** (`mv_<16 hex>`, coop/element/portable_identity.*): doors,
  lights, lightgroups, containers, garage, appliance, door-box now keyed by a peer-independent identity.
  This is upstream's fix for the field defect the reporter's Reports 2 and 4 describe (per-peer
  keysHash for world-load-added actors). Proto 152 = wire-MEANING change; a 152 peer cannot play
  with b150 peers. Unreleased.
- interactable_channel.h: pending-apply expiry moved from 25 s TTL to 5 stable index passes with a
  10-minute backstop.
- cfd1d7f2: base power lane refuses on unresolved offsets (regression risk on b150 game bytes).
- 385c1eee unresolved_pose_ledger (telemetry only); 65ce2999 kerfur id release; 3b5a60aa join
  Downloading/LoadingWorld phase; d77add6f push-to-talk X→G.
- NONE of the four local fixes present upstream. BUG-1/BUG-2/BUG-017/OBS-R3Q-C01/P01 mechanism files
  are code-identical to b150.

## Existing work: status

| item | status |
|---|---|
| four reviewed fixes (10777571, 46b77b92, 15589b7f, 1c393401) | still valid; defects present at both targets; applied to b150 line (clean); upstream retarget in progress (8 comment-only conflicts, recorded resolutions in C1\EVIDENCE\retarget-*.patch) |
| R3Q BUG-017 runtime results | upheld; RUNTIME_RERUN_REQUIRED=none; used as design input |
| BUG-017 contract-layer design | DRAFT written (C1\LANES\bug017-design\BUG017-CONTRACT-DESIGN-DRAFT.md, host-local custody park, zero wire bytes); independent two-lens review in progress |
| OBS-R3Q-C01 | source chain established (host emitter stamps eid=0 + key for the unexpressed hand husk; client resolves by key onto the just-placed mirror); emitter gate being implemented |
| player-log candidate 8 (32-entry place queue cap tested before eligibility) | SOURCE_PROVEN, authorised, being implemented |
| candidates 3,4,5,6,7,9 | NEEDS_RUNTIME; no fix work |
| hygiene branches | published earlier (experimental/origin); no action |
| codex launcher qualification chain | preserved, not resumed |

## BugDropbox (2026-09-07 drop) — intake in progress

11 structured reports from one reporter (official b150, paired host/client logs, videos) plus 16
community screenshots and one video. Themes: per-peer key instability (Reports 2, 4 — upstream fixed
in 86d04953, unreleased); destroy-by-key collision (Report 3 — same class as C01); drive payload lost
across hand transitions (Report 5); client-authored native effects not replicated (Reports 6, 8, 9);
death/explosion → session teardown (Report 10, Eggy); pose pipeline dead after re-host (Report 11);
client object duplication → FPS collapse (several); inventory replaced by host's after rejoin
(several); UI unreachable above 1080p. Logs unpacked at C1\EVIDENCE\doctaaaa-logs\.

## Batch 1 (running)

1. b150 retarget — DONE (d437aefc). Incremental Release build EXIT=0 at 2026-09-09 03:13:59Z;
   `build/votv-coop-b150/Release/main.dll` sha256
   fe0fde750ca30313e3a88c4491ddef6c29b1805b59cb3f471eb37804bd90dc0a (18298368 bytes), embeds
   `MULTIVOID_SOURCE_COMMIT=d437aefc225f4b87e952f38b6006834c7da2d4d3` (1 ascii occurrence);
   multivoid_policy_tests.exe PASS/PASS/PASS. Identity + provenance + git-failure drills on this build ALL EXIT 0
   (2026-09-09 08:02Z; 83 PASSED, 53 controls FIRED, git-failure 5/5; log
   C1\EVIDENCE\drills-b150-aligned-fixes-d437aefc.log) — the added headers and uncompiled
   tests/standalone/*.cpp did not move the provenance gate. Runtime effect UNMEASURED until a rig arm.
2. Upstream retarget of the four fixes + harness stub extension + standalone test runner → fresh review.
3. OBS-R3Q-C01 emitter gate + pure policy header + standalone test → fresh review.
4. Candidate 8 placement-queue admission + header + test → fresh review.
5. BUG-017 design: two-lens independent review → reconciled verdict **DESIGN_NOT_CLEAN, seven
   blocking findings RB-1..RB-7** (`C1\LANES\bug017-design-review\VERDICT.md`): key-only consume on a
   line with measured non-unique keys; no eid at the apply point so the fan-out cannot fire; slot
   ownership unproven (Index 0 aliases the personal store); silent apply refusal; A2 control
   unsatisfiable (seams unreachable on the host-authored chain); join-bracket TTL guard dead on the
   host; four omission cases with no named assertion. Core host-local park idea upheld. Round 2
   (fresh author → two fresh lenses → reconciler) launched 2026-09-09 after the reset; revision file
   `C1\LANES\bug017-design\BUG017-CONTRACT-DESIGN-R2.md`, verdict `VERDICT-R2.md`.

Verification target for 2–4: standalone tests both arms (pre-fix arm fails, fix arm passes), production
TU compiles, protocol.h blob unchanged, reviewer CLEAN. Runtime effect stays UNMEASURED until a rig
arm; only one rig owner; operator participation required.

## Orientation critic outcomes (acted on 2026-09-09)

- Drift table in CONTEXT.md was inflated by trailing-comment edits; corrected and pasted (session.cpp,
  event_dispatch_entity.cpp, prop_drop_intent.cpp, drive_sync.cpp, call.cpp are all 0).
- b150 retarget verified at BLOB level: 3 of 4 picks blob-identical for every touched file; for
  1c393401 only session.cpp differs because the private overlay already modifies that file on the
  b150 line (auto-merged); the added/removed hunk lines are compared separately (see below).
- 1c393401 on 3af5ddae: the PendingDelivery key is `d.first` from the channel's own byKey_ index,
  which is the portable key where one exists; lastKnown_ and the wire use the same namespace, so the
  retarget stays mechanical.
- Candidate 8 hazard for the reviewer: at drain, an entry NOT flagged containerExtract may still be
  a legitimate PARKED re-place (parked is only known once the key restores), so "evict the oldest
  ineligible" can drop a real place; the fix must reserve/extend capacity for container-extract
  arrivals rather than evict.
  **ADJUDICATED 2026-09-09 (orchestrator): eviction ACCEPTED.** Reasoning: the 32-entry queue only
  fills inside a spawn burst (a container spill or a save/world load), and burst occupants are
  entries the drain drops at :309 anyway (not parked, not whitelisted, not extraction); a player
  cannot author 32 genuine places in ~8 ticks. The author's dominance argument (the extraction
  arrival's authoring condition is a strict superset of a latch-less occupant's) is source-backed,
  the bound stays absolute, and eviction happens only at a full queue. Residual risk: one genuine
  parked place evicted during a burst — the same class of loss the cap already causes today.
  Reserve/extend capacity was the alternative; not pursued.
- Strategic: private/b150-aligned is wire-forked from new upstream (b150 tag not an ancestor; proto
  152 changed WireKey meaning); it stays the playable line and does NOT track upstream. The public
  hygiene branch is mostly dead (16 of 18 paths absent upstream) — retire, do not port; the private
  hygiene branch is unaffected.
- The provenance/identity gate on the b150 line must be re-run after the fixes build (added headers
  + uncompiled tests/standalone/*.cpp may move the critical-graph manifest).
- Do not schedule a separate C01 source-reading lane; it was done (inv3) and is being implemented.
- C01 emitter-gate premise settled from source (3af5ddae): the host's Init POST express path calls
  `PT::MarkPropElement(self, keyStr, cls, kExpressSeam)` (prop_lifecycle.cpp ~:215) BEFORE building
  and logging the SPAWN broadcast; MarkPropElement's winning path ends in
  `IndexKeyForActor_(actor, enrollKey, internalIdx)` (prop_element_tracker.cpp:312-313); the R3Q
  host log shows that SPAWN line (C4-HOST-PLACE-host.log:4706) before the two eid=0 DESTROY lines
  (:4708-4709). On the host, the key index is written only on that Element-winning path, and the
  husk's destroy carried eid=0 read BEFORE the unmark (prop_destroy_seam.cpp:45 vs :47), so the husk
  was never indexed and cannot satisfy the gate against itself. Residual edge: the post-alloc race
  branch returns without indexing (prop_element_tracker.cpp:305-309) — not the measured case.
- 1c393401 on the b150 line: session.cpp blob differs only because the private overlay adds 33 lines
  there; the fix's added/removed lines are IDENTICAL between the original and the pick.

## Batch 2 candidates (intake source traces, ranked by the critic; T6 trace lost to a lane failure)

Traces at `C1\EVIDENCE\intake	races\` (published too). Critic ranking:
1. T1 doors/lights keysHash — UPSTREAM ALREADY FIXED (86d04953 portable identity); a b150-line backport would change wire meaning → design decision, not a fix.
2. T3 pose pipeline dead after in-process re-host — READY: ResetPeerRemoteState for every slot at Session::Start().
3. T5 drive payload lost across hand transitions — READY: hoist NoteLocalDriveBirth out of the freshBirth-only block.
4. T2 destroy-by-key collision (floppy) — READY: same family as the C01 gate; suppress the outbound keyed destroy when the dying host actor has no Element and is still pending in the spawn watcher.
5–6. T7 client duplication / rejoin inventory — needs rig first.
7–8. T4 death→menu / listener death — diagnostic first, then keepalive.
Implementation of any of these needs author + fresh reviewer lanes → blocked by the operator's no-new-agents rule until re-authorised.

## Published (operator-authorised 2026-09-09)

- `SeaTurtleManOG/Multivoid-Experimental@findings/c1-intake-20260909` head 9eddc7db (docs only, 35
  files, rooted on findings/verified 546419cd): intake ledger, the reporter's 11 reports as text,
  CONTEXT/DELTA-BRIEF/CHECKPOINT (transcript path scrubbed), five orientation findings, BUG-017
  draft 1 + reconciled verdict. Videos, screenshots and the log bundle NOT published (sha256 in the
  ledger). Purpose: readable by ChatGPT desktop. Later drops append to the same branch.

## Not done / constraints

- Nothing pushed, merged, PR'd or released. All branches local.
- No rig run this session yet. A future b150 rig test needs the operator.
- RAM ~5.7 GB free (foreign codex/blender processes present); builds serialized.
