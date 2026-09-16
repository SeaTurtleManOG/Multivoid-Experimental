# Multivoid findings after the M1 reconciliation (2026-09-17)

This drop supersedes the project-state statements in `c1-20260909/` (CHECKPOINT, orientation 01-02). Those files record the C1
state on protocol 150 lines. The candidate, upstream and fix dispositions have changed since then. The per-bug findings under
`findings/MVF-*` keep their own scope.

## Evidence labels

- **SOURCE**: read in the mod's source at the named commit, or in the game's decoded Blueprint bytecode (marked "bytecode"). This is static analysis and does not mean the code was run.
- **HEADLESS**: a test suite executed without the game (named run).
- **RUNTIME**: observed in the game. Nothing below is RUNTIME-verified after 2026-09-11.
- **INFERRED**: follows from SOURCE facts but has not been read end to end or observed.
- **UNKNOWN**: not established.

## Pins

- Private candidate `eeace033f4b1baac93f9894d969a53424bca760d`: upstream `a84376581fbd` plus 48 private commits, protocol 155.
- Upstream reference `VOTV-MP/Multivoid` main `ba3bf74aec84a0e28705af14bb1ca31260021567`, protocol 162. Main was unchanged when
  checked on 2026-09-17.

## 1. What upstream already provides

| finding | label |
|---|---|
| OV-F1 (the trash-bits cleaner cancel never registers) is fixed upstream by 4f2fcb54/36c60041, with an exact-class filter. No private fix exists or is needed. | SOURCE |
| Upstream bb75833e clears the NPC and world-actor pose batches and two pose queues at session Stop. That is half of the private session-watermark reset 7e2fda9c. The per-slot, host-clock, desk-sim, dish, reel and desk-cursor watermarks are cleared only by the private fix. | SOURCE |
| An earlier statement said "upstream reimplements none of our fixes". It is withdrawn, because of the bb75833e overlap above. | SOURCE |
| The remaining private changes were re-checked against upstream main. Gameplay fixes were checked by the state, member or message they guard, searched at the pin rather than by title or file name. Comment-only and test-only changes were checked through the fix they belong to. No further equivalent was found. This is bounded by the probes and is not proof of absence. | SOURCE (bounded) |
| Three upstream changes earlier labelled "partial overlap" (4d502ec0 end-reason codes, 120a6796 disc eject lineage, 8aa211c2 trash mirror binding) only move code the private fixes touch. They provide none of the fixes' behaviour. | SOURCE |

## 2. Upstream changes that alter private premises

| change | effect | label |
|---|---|---|
| 6cbe985f: posted tasks drain only at the outermost dispatch | removes the nested-drain window the kitchen repair fix (FIX-4) was blocked on | SOURCE (drain); INFERRED (window closed) |
| 54e425b5: Blueprint script-body gate with per-call cancel | makes Blueprint-to-Blueprint producers watchable. Reach through ProcessEvent in the shipping binary is unverified | SOURCE at source; INFERRED at binary |
| 5eb7689c: UObject create/delete listeners | observes every object birth and death, including math-library spawns; cannot cancel; only the scan hub consumes it | SOURCE |
| 2f34f8fa: container arbitration and slice relay | changes where a container-custody fix must write; does not carry contents across pickup/re-place | SOURCE |
| protocol 155 -> 162 | the private R8 grid experiment's wire kinds 130/131 collide with upstream's save-data kinds | SOURCE |

## 3. Open defects and limits

| id | finding | label |
|---|---|---|
| BUG-017 | Container contents are lost when a client picks up and re-places a world container. No carry exists in upstream at base or pin. The private candidate carries a custody park and reducer tests, but the fix's acceptance arms A0-A5 were never run in game. | mechanism SOURCE; tests HEADLESS (reducer only); fix RUNTIME-unverified |
| AR-B1 | The container-extraction save-record publisher is a ProcessEvent observer on `takeObj`, but every stored call of `takeObj` in the game's bytecode is a local virtual call, so that observer never fires. A host-side extraction therefore publishes no save record on any stored route. | SOURCE (mod source and bytecode); host routing INFERRED |
| AL-B1 (corrected) | `takeObj` spawns the item itself when `removeVol` is true and the index is valid. The extraction latch is left armed with no birth only by `findInventoryObject(removeVol=false)` (no stored caller) or an invalid index. This corrects an earlier statement that two game callers arm it without spawning. | SOURCE (bytecode); hook behaviour INFERRED |
| AL-B2 (corrected) | The mod's record apply splices the receiver's own base record, so no sender lifespan crosses a record apply. This corrects an earlier statement that every record apply restarts the lifespan. | SOURCE |
| FIX-5 | Wallunit phantom reel: the defect stands at upstream main. The private fix's review was NOT_CLEAN and its remediation is unreviewed. | SOURCE (mod source and bytecode); fix unresolved |
| Key-only destroys | The host broadcasts eid-0 destroys that receivers resolve by key against any live actor, and this still stands at upstream main. The private candidate gates two cases (C01, T2). | SOURCE; C01 measured RUNTIME on b150 in an earlier session |

## 4. Tests and runs actually executed

- HEADLESS: the private standalone suite on the candidate's parent commit 4a252279 (the candidate head differs by comments only)
  ran on 2026-09-11: exit 0, 11 expected-pass, 10 expected-fail arms. It built as b155 the same day.
- HEADLESS: the baseline-delivery qualification ran on 2026-09-11, exit 0.
- No game, rig, build or test ran in the mapping (M1) or reconciliation (M1-RC) sessions.

## 5. Unknowns that decide the next steps

- Whether the CEF boot crash still occurs on the current build.
- Whether the upstream script gate reaches ProcessEvent routes in the shipping binary.
- How host container-extraction births reach the host spawn watcher.
- Native or by-name callers of `takeObj` and `findInventoryObject`.
- Every in-game acceptance arm listed above.
