# Multivoid-Experimental: proposal index

This is the one current index of the draft proposals staged in this repository. It is a navigation page, not a finding: each
proposal's status is in its pull request, and each finding keeps its own status label in `findings/` and in the dated drops
(`c1-20260909/`, `m1rc-20260917/`). Nothing listed here has been submitted to VOTV-MP/Multivoid.

## How this repository is arranged

- `main` is byte-for-byte upstream VOTV-MP/Multivoid at `ba3bf74aec84a0e28705af14bb1ca31260021567` (protocol 162). No fork commits
  live on it, so each proposal's diff is only its own change.
- The fork's earlier `main` (a different, older history) is preserved as tag `archive/main-f690c017-20260917`.
- Findings and their evidence summaries live on this branch (`findings/c1-intake-20260909`) and on `findings/verified`.

## Draft proposals (updated 2026-09-17)

Every proposal is a DRAFT and NOT GAMEPLAY-VERIFIED: none has been run in the game. Each was cut from upstream `ba3bf74a`; the
standalone candidate test runner comes from proposal 1, so proposals 2 to 6 are stacked on it and target its branch.

| # | pull request | branch | base | what it proposes | checks run on the branch |
|---|---|---|---|---|---|
| 1 | [#1](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/1) | `proposal/call-frame-write-refusal` | `main` | a reflected call whose parameter write failed is refused; the standalone candidate test runner | repo gates; standalone suite |
| 2 | [#2](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/2) | `proposal/session-reuse-reset` | proposal 1 | a reused session starts clear of the previous session's watermarks and reliables; a refused connect snapshot is retried for the same peer; the baseline-delivery qualification | repo gates; standalone suite; baseline-delivery qualification |
| 3 | [#3](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/3) | `proposal/keyed-zero-eid-prop-spawn` | proposal 1 | a PropSpawn with element id 0 is admitted when its key carries the identity | repo gates; standalone suite |
| 4 | [#4](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/4) | `proposal/keyed-destroy-gates` | proposal 1 | the host does not broadcast a key-only destroy that names the wrong actor | repo gates; standalone suite |
| 5 | [#5](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/5) | `proposal/container-custody` | proposal 1 | place-queue admission and drive-place note; a world container keeps its contents across a client's take and place; a client does not respawn a drone sack another peer took | repo gates; standalone suite |
| 6 | [#6](https://github.com/SeaTurtleManOG/Multivoid-Experimental/pull/6) | `proposal/device-apply-fixes` | proposal 1 | drive apply resolves before its deadline; appliance rows apply through each class's own verb; a kitchen oven is repaired on an applied ON | repo gates; standalone suite |

No branch was built into a DLL; the combined line these were cut from built without errors, and each pull request says which
evidence is reused from that line rather than run on its branch.

## Known work not proposed here

| work | status |
|---|---|
| wall unit phantom reel tick | unresolved: its fix has not passed review |
| drain-time birth record | unresolved: design and census only, no code |
| host power grid experiment | a separate experiment on a private wire; its message kinds collide with upstream's, so it is not a proposal |
| CEF hook arming during MinHook setup | deferred: whether the crash still occurs on the current build is unknown, and the change also repoints the MinHook submodule |
| trash-bits cleaner cancel | already in upstream |
