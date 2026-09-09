# C1 findings drop — 2026-09-09

Docs-only publication from the Claude-native Multivoid continuation (C1). Nothing here is a
release, a merge or a PR; all code sits on local branches named in CHECKPOINT.md.

| path | what |
|---|---|
| `intake/INTAKE-LEDGER-20260909.md` | the sorted BugDropbox drop of 2026-09-07: 64 items archived and hash-verified, classified against BUG-001..020, PENDING ids assigned, 12 cross-cutting themes, screenshot transcriptions |
| `reports/Report-N/` | the reporter's eleven structured b150 bug reports (text only; videos and the 70 MB log bundle are referenced by sha256 in the ledger, not published) |
| `CONTEXT.md` | measured session brief: upstream force-rewrite (f690c017 → 3af5ddae, proto 152), local lines, corrected code-drift table |
| `DELTA-BRIEF.md` | decisions and batch state, incl. the orientation critic's outcomes and the candidate-8 adjudication |
| `CHECKPOINT.md` | resumable state: branches, heads, review verdicts, next tasks |
| `orientation/01..05` | five independent source investigations: upstream rewrite audit; fix retarget feasibility; OBS-R3Q-C01 destroy trace; BUG-017 design inputs; player-log candidate inventory |
| `bug017-design/` | BUG-017 contract-layer design draft 1 and its reconciled binding verdict (DESIGN_NOT_CLEAN, seven blocking findings; round 2 in progress) |

Strength tags (MEASURED / SOURCE_PROVEN / INFERRED / UNKNOWN) inside each document are binding;
do not quote a claim above its tag. BUG-017 persistence is
ABSENT_WITHIN_DECLARED_SEARCHED_SCOPE, not unrestricted data loss.
