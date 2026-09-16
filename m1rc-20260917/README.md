# Reconciliation findings drop — 2026-09-17

This is a docs-only drop from the reconciliation that followed the M1 mapping and architecture session. It is not a release, a merge
or a pull request, and no code is published here.

**How this differs from the older material on this branch:**
- `findings/MVF-*` are single findings, each passed by a per-file `FINDING_CLEAN` review. This drop is NOT one of those findings and
  did not receive that per-file review.
- `c1-20260909/` records the project state of 2026-09-09 on protocol-150 lines. For project state (candidate contents, upstream
  overlap, fix dispositions), this drop supersedes it. Per-bug evidence in `c1-20260909/` keeps its own scope and strength tags.

| path | what |
|---|---|
| `FINDINGS.md` | the corrected summary, with each claim labelled by evidence type: what upstream `ba3bf74a` (protocol 162) already provides; upstream changes that alter private premises; open defects and limits; tests actually executed; unknowns |

**Review status:**
- The underlying register had one combined independent check. Its verdict was NOT_CLEAN, with 1 blocking and 8 non-blocking
  findings, all about wording or cost. Every finding was corrected before publication, including the blocking one, which had
  overstated how the private fixes were compared with upstream.
- No disposition changed.
- There was no further review of the corrected text.

**Qualifications that bind every statement in `FINDINGS.md`:**
- Nothing is runtime-verified after 2026-09-11, and no game, rig, build or test ran in the sessions summarised.
- Upstream comparisons are bounded by their stated searches. "No equivalent found" is not proof of absence.
- The private candidate's fixes are counted as reconciled items, not as runtime-verified fixes, and individual review gaps remain.
