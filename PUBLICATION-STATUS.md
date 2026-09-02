# Publication status

This branch carries only the finding files that an independent reviewer marked `FINDING_CLEAN`.

The reviewer authored none of these files, ran read-only, and had no authority to change them or
to order a rerun. It returned one verdict per file, so a defect in one file could not sink
another.

## Published

| File | Verdict |
|---|---|
| `findings/MVF-0001-drive-box-contents-not-replicated.md` | `FINDING_CLEAN` |
| `findings/MVF-0002-drive-box-lid-state-divergence.md` | `FINDING_CLEAN` |
| `README.md` | no defect found; it is a legend and makes no per-finding claim |

## Withheld, with the reason

| File | Verdict or defect | Why it is not here |
|---|---|---|
| `findings/MVF-0003-food-eat-progress-stale-on-remote-peer.md` | `FINDING_NOT_CLEAN` | The evidence records a keyed client drop and a host reconstruction. It does not record the displayed food-progress values. The quoted result is therefore reported behaviour, not a direct measurement. `STATUS=VERIFIED` should be `REPORTED` and `CAUSE_STRENGTH=INFERRED` should be `HYPOTHESIS`. |
| `findings/MVF-0004-bug017-client-inventory-placement-empty-host-box.md` | `FINDING_NOT_CLEAN` | Its partial status and evidence limits are correct, and it does not use the rejected predecessor display as proof of the defect. It does not name the active vitals keepalive or the incomplete manifest at the required level. Its promotion condition would also permit a disclosed active state-mutating tool, so a later reviewer could promote a run that repeats a binding rejection ground. |
| `CURRENT.json` | defect found | The MVF-0003 title states "can overwrite the host value". The finding limits the observation to the host display. The title is stronger than the file it indexes. |
| `BUG-STATUS.md` | defect found | Identity coverage is complete: 53 identities expected, 53 present, none missing, none extra, and no row claims a report is absent or resolved. The `MVF-0003` and `PENDING-BUG-024` rows use `VERIFIED`, which inherits the unsupported food-progress measurement. |

## What was not done

The withheld files were **not** repaired and resubmitted in the session that authored them. A
rejected artifact is preserved as rejected. Repairing it in the same session would mean the
reviewer's verdict applies to bytes that no longer exist.

No upstream issue was opened. No upstream pull request was opened. This branch is not merged into
`main`, and `main` remains a clean mirror of canonical upstream.

## Reviewer output, verbatim

```
REVIEW_RUN_ID=ws-f-review-01
MVF_0001_VERDICT=FINDING_CLEAN
MVF_0002_VERDICT=FINDING_CLEAN
MVF_0003_VERDICT=FINDING_NOT_CLEAN
MVF_0004_VERDICT=FINDING_NOT_CLEAN
BUG_STATUS_COMPLETE=true
BUG_IDS_EXPECTED=53
BUG_IDS_MISSING=0
EVIDENCE_IDS_THAT_DID_NOT_RESOLVE=0
CAUSE_STRENGTH_UPGRADES_FOUND=1
FORBIDDEN_PUBLIC_CONTENT_INSTANCES=0
```

Reviewed artifact digests, as recorded by the reviewer:

```
README.md      4ebdce0299d4df35c1cafd3de74454063f9d3fbc10da1679dd71ca7504ca2fc8
MVF-0001       0393b6ad05690adedc5810a09d7cde6e04686dd48e70622a26ff808c2e497cac
MVF-0002       f5915f23f37d7490bd57c44206c3033d4fb4af0b7f940d796e24e4d7c91dbd4f
```

## Affected coordinates

```
AFFECTED_RELEASE      v0.9.0n-b150-dev
RELEASE_SOURCE        ba6d8c39d64a48a6262565208bafacad306c6e1e
RELEASE_DLL_SHA256    cfd1a037f073d07f3eae0e7b189bd70294a5e899e8e9ca8d51da72731951289c
AFFECTED_MAIN_HEAD    f690c017e8e55f2996d0e9121956aae66486742e
```
