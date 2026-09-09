# C1 CHECKPOINT — 2026-09-09 (written at the operator's usage-limit stop)

State: RESUMED 2026-09-09 after the allowance reset (the earlier PAUSED_FOR_USAGE_LIMIT stop is
superseded). At the stop, the session limit had killed: the retarget-upstream review, the cand08
re-review, all seven intake source traces, the rank critic, and the drills (operator-stopped). On
resume: both workflows were resumed with resumeFromRunId (cached lanes replay; only the killed
lanes run), the drills were re-run locally, candidate 8's eviction shape was ADJUDICATED ACCEPTED
(see DELTA-BRIEF), and BUG-017 design round 2 was launched (wf_e898dd0e-0f6).

## Standing instruction (operator, 2026-09-09 after the reset)

NO NEW AGENTS OR WORKFLOWS. Existing background lanes may finish; everything else is done by the
orchestrator directly (git, builds, drills, reading journals). Ultracode is off.

## Resume procedure

1. Read `C1\CONTEXT.md` (measured brief), `C1\DELTA-BRIEF.md` (decisions), this file.
2. Read the journals for anything that finished after this checkpoint was written
   (`<transcripts>\subagents\workflows\<run>\journal.jsonl`, one `{"type":"result",...}` line per
   agent). Transcript root:
   `<local-transcripts>\`
   Scripts under `...\workflows\scripts\`. A workflow can be resumed with
   `Workflow({scriptPath, resumeFromRunId})`; unchanged agents replay from cache.
3. Do not restart the orientation (wf_e6ce3b5e-bf7, COMPLETE) or the intake archive lane.
4. Native Opus subagents remain the authorized backend for this continuation (owner instruction
   2026-09-09); no codex lanes, no launcher qualification.

## Workflows

| run | purpose | state at checkpoint |
|---|---|---|
| wf_e6ce3b5e-bf7 | orientation (5 investigations + critic) | COMPLETE; critic outcomes acted on (see DELTA-BRIEF) |
| wf_0f33d071-bd0 | dropbox intake + 7 source traces + ranking critic | intake lane DONE (64 items archived to `C:\AgenticStaging\BugDropboxArchive\20260909T0302Z`, 0 hash mismatches, ledger `C1\EVIDENCE\intake\INTAKE-LEDGER-20260909.md`, 12 themes); traces T1–T7 RUNNING; rank critic not started |
| wf_1493ded8-3da | BUG-017 design review (2 lenses + reconciler) | both lenses returned DESIGN_NOT_CLEAN, implementation_go=false (source-correctness: 4 blocking, 4 of 28 citations failed; data-loss/regression: 5 blocking, 3 of 18 citations failed); reconciler RUNNING → `C1\LANES\bug017-design-review\VERDICT.md` when done |
| wf_11293548-44d | batch-1 implementation (3 authors → fresh reviewers → one remediation round) | see table below |

## Batch-1 implementation state (all local, nothing pushed) — UPDATED after the resume

FINAL VERDICTS (workflow wf_11293548-44d completed after the resume):
- fix/c01-keyed-destroy-gate a482a5d7 — CLEAN (two fresh reviewers, 0 blocking).
- fix/retarget-3af5ddae 48497664 — CLEAN (fresh reviewer; code preservation measured with the
  reviewer's own comment-stripping probe; harness and standalone runners re-run).
- fix/place-queue-admission d8cd3c1e — CLEAN after two comment-only respins; the remaining
  "shape authorisation" precondition is the orchestrator's eviction adjudication (ACCEPTED, see
  DELTA-BRIEF). Ready to cherry-pick onto private/b150-aligned-fixes together with C01.
Intake workflow wf_0f33d071-bd0 completed: six of seven traces + the rank critic
(`C1\EVIDENCE\intake	races\`, published on experimental findings/c1-intake-20260909 @ 0e646684);
T6 (client native effects not replicated) was lost to a lane failure and stays untraced.
The table below is the pre-resume snapshot.


| branch / worktree | head | author | review |
|---|---|---|---|
| fix/c01-keyed-destroy-gate — `C1\WORKTREES\fix-c01` | a482a5d7 (3 commits: policy header, DestroySeamBody host gate, candidate_11 test) | DONE; both test arms shown (pre-fix defect case fails, fixed 7/7 pass); prop_destroy_seam.cpp TU compiles /W4 clean; protocol.h unchanged | **CLEAN** at a482a5d7 (0 blocking) |
| fix/place-queue-admission — `C1\WORKTREES\fix-cand08` | 4d6e8437 (3 reviewed commits at 125f174c + 1 comment-only respin) | DONE; test both arms (pre-fix 4 FAIL/15 PASS, fixed 19 PASS); TU compiles; protocol.h unchanged | NOT_CLEAN at 125f174c on ONE claim-strength wording finding (CLAIM-STRENGTH-01, not behavioural) → remediated by 4d6e8437 → re-review RUNNING. Open adjudication for the orchestrator: the author kept admit-by-eviction (the evicted latch-less occupant may be a real parked place, so it is a trade); the alternative named in the brief was reserved/extended capacity for container-extract arrivals. Decide before merging anywhere. |
| fix/retarget-3af5ddae — `C1\WORKTREES\retarget-upstream` | 48497664 (4 cherry-picks 0ddf4aaa, fd55b86a, dce0a271, 76054b75 + 2 test commits 98542b68, 48497664) | DONE; exactly the 8 predicted comment-adjacent conflicts; baseline_delivery harness 10/10 with a negative control built from 3af5ddae's own pre-fix blob; standalone runner both arms; protocol.h unchanged. Known gap: run.ps1 leaks $LASTEXITCODE=1 on success (pre-existing) | review RUNNING |
| private/b150-aligned-fixes — `S:\GAMEMODDING\Multivoid-Private-B150` | d437aefc | four clean cherry-picks, blob-verified; Release build EXIT=0, main.dll sha256 fe0fde750ca30313e3a88c4491ddef6c29b1805b59cb3f471eb37804bd90dc0a embeds MULTIVOID_SOURCE_COMMIT=d437aefc…; policy selftest 3/3 | reviews of the four fixes carry over (byte-identical hunks); identity/provenance/git-failure drills: see `C1\EVIDENCE\drills-b150-aligned-fixes-d437aefc.log` (tallies at the bottom of this file) |

`private/b150-aligned` itself is untouched at 998c43b6. Scratch worktrees
`scratch-upstream-3af5ddae` and `scratch-b150-998c43b6` are detached and clean.

## BUG-017 design: what the two reviewers found (before reconciliation)

Both lenses: direction compliance honoured (no transport rewrite, no Relay, no serializer, no wire
change; persistence wording disciplined). Blocking, summarised:
- A container destroyed for good (hammer break, sale) followed by a key reuse can resurrect or
  duplicate parked contents (key-generation binding missing).
- The apply may write into a GObjStack slot it cannot prove is exclusively the fresh container's
  (propInventory_C::init slot behaviour UNKNOWN; possible silent no-op or wrong-slot write).
- The host restores contents locally but never publishes them (MarkDirty has no valid eid at that
  seam) — clients keep the empty box.
- Arm A2's pass criterion expects [CUSTODY] lines that a correct implementation never emits (control
  arm mis-specified); omission discipline does not cover the Player==true refusal at the new entry.
- A 60 s TTL guard imported from the client-side bracket is dead on the host.
Next design round must fix these; implementation is NOT authorised. Do not restate the orphan-slot
premise above INFERRED.

## Dropbox intake (2026-09-07 drop) — where it stands

Archived and classified (64 files, 12 themes). Reporter's Bugs 11–14 are not in the drop
(Discord-only); 13 videos unviewed; Report 3's multivoid.log is byte-identical to the already
archived PENDING-20260904-008. Upstream 86d04953 (portable identity, proto 152, unreleased) is the
fix for theme T1 (doors/lights keysHash); it cannot go on the private b150 line without breaking
b150 compatibility. Source traces T1–T7 were running at checkpoint; read their journal results
before ranking batch 2.

## Next tasks, in order

1. Read the pending results: retarget-upstream review, cand08 re-review, design reconciler, T1–T7
   traces and the rank critic. Resolve any NOT_CLEAN with a bounded remediation + delta re-review.
2. Adjudicate the candidate-8 eviction-vs-reserve question; if reserve is chosen, one more bounded
   round on fix/place-queue-admission.
3. Apply the two new CLEAN fixes (C01 gate; cand08 once clean) to private/b150-aligned-fixes by
   cherry-pick (files are code-identical between 3af5ddae and b150), rebuild, rerun selftest/drills.
4. Rig test of the b150 fixes build (needs the operator; one rig owner): C01 re-run of
   R3Q-CONTROL-01 C0..C5 with the three paired log assertions; a four-fix smoke; NOT a BUG-017 rerun.
5. BUG-017 design round 2 against the reconciled verdict, then fresh review, then implementation.
6. Batch 2 from the intake ranking (likely: death→menu client arming, pose pipeline after re-host,
   drive payload across hand transitions, loot spill authority).

## Constraints unchanged

No push, no merge to public/default branches, no PR, no release, no master-server contact, no rig
run without the operator, no Astra/AgentHarness work. RAM ~5.7 GB free; builds serialized.
