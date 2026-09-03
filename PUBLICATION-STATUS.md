# Publication status

This branch carries only the finding files that an independent reviewer marked `FINDING_CLEAN`.

Each reviewer authored none of the files it judged, ran read-only, and had no authority to change
them or to order a rerun. Each returned one verdict per file, so a defect in one file could not
sink another.

## Published

| File | Verdict | Review |
|---|---|---|
| `findings/MVF-0001-drive-box-contents-not-replicated.md` | `FINDING_CLEAN` | `ws-f-review-01` |
| `findings/MVF-0002-drive-box-lid-state-divergence.md` | `FINDING_CLEAN` | `ws-f-review-01` |
| `findings/MVF-0003-food-eat-progress-stale-on-remote-peer.md` | `FINDING_CLEAN` | `r3o-wsf-review-01` |
| `findings/MVF-0004-bug017-client-inventory-placement-empty-host-box.md` | `FINDING_CLEAN` | `r3o-wsf-review-01` |
| `README.md` | no defect found | it is a legend and makes no per-finding claim |

## What changed since the previous publication

MVF-0003 and MVF-0004 were rejected `FINDING_NOT_CLEAN` by `ws-f-review-01` and were **not**
published at that time. They have since been revised and passed by a **second, independent**
review, `r3o-wsf-review-01`, whose lane authored none of the bytes it judged.

The rejected versions were not repaired in the session that authored them. They were preserved as
rejected, and the revision was made in a later session against the recorded reviewer notes. This
matters because a verdict applies to the exact bytes it was issued against.

MVF-0001 and MVF-0002 were **not** touched by the revision. Their published bytes still hash to
the digests `ws-f-review-01` recorded, so the verdict that published them still applies:

```
MVF-0001  0393b6ad05690adedc5810a09d7cde6e04686dd48e70622a26ff808c2e497cac
MVF-0002  f5915f23f37d7490bd57c44206c3033d4fb4af0b7f940d796e24e4d7c91dbd4f
```

### What the revision corrected

**MVF-0003.** Its `STATUS` was `VERIFIED` and its `CAUSE_STRENGTH` was `INFERRED`. The cited role
logs record a keyed client drop and a host materialisation, and they record **no food-progress
value**; no food-progress image exists in its evidence set. The displayed value sequence was
therefore reported behaviour, not a measurement. The file now carries `STATUS=REPORTED` and
`CAUSE_STRENGTH=HYPOTHESIS`, and its result field attributes the value sequence to the report.

**MVF-0004.** Its rejection record understated two of its own binding grounds, and its
`PROMOTION_CONDITION` required only that a future run have "no **undisclosed** state-mutating
tool". That would have permitted a **disclosed but active** one — including the vitals keepalive
whose activity was itself a binding rejection ground. Disclosure is not permission. The condition
now requires every state-mutating tool to remain **inactive**, and requires a complete activity
manifest and a complete artifact manifest listing every cited artifact. The record now also states
plainly that the vitals keepalive was active and what its restore action did, and that cited
artifacts were outside the manifest.

`MVF-0004` remains `PARTIALLY_VERIFIED`. It was **not** promoted, because the run that might have
promoted it was itself independently rejected.

## Still withheld, with the reason

| File | Defect | Why it is not here |
|---|---|---|
| `CURRENT.json` | corrected, not published | The defect the previous review found — a title stronger than the file it indexed — has been fixed and the fix was verified by `r3o-wsf-review-01`. The file is nonetheless not published here: it is an index, not a finding, and adding a new file class to this branch was outside the revision's scope. A later session may publish it. |
| `BUG-STATUS.md` | corrected, not published | Same. Identity coverage was re-counted at 53 identities with none missing and none extra, and the `MVF-0003` and `PENDING-BUG-024` rows no longer carry `VERIFIED`. Not published for the same scope reason. |

## What was not done

No upstream issue was opened. No upstream pull request was opened. This branch is not merged into
`main`, and `main` remains a clean mirror of canonical upstream.

No finding on this branch rests on the `R3N` evidence packet. That packet was independently
reviewed and rejected (`DIAGNOSIS_NOT_CLEAN`, both arms `INCONCLUSIVE`,
`EXACT_REPRODUCTION=NOT_ESTABLISHED`), and it is cited nowhere as verification.

## Reviewer output, verbatim

`ws-f-review-01`, which published MVF-0001 and MVF-0002:

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

`r3o-wsf-review-01`, which published MVF-0003 and MVF-0004:

```
REVIEW_RUN_ID=r3o-wsf-review-01
MVF_0001_UNCHANGED=true
MVF_0002_UNCHANGED=true
MVF_0003_VERDICT=FINDING_CLEAN
MVF_0004_VERDICT=FINDING_CLEAN
BUG_STATUS_IDENTITIES=53  MISSING=0  EXTRA=0
CURRENT_JSON_TITLES_NO_STRONGER_THAN_FILES=true
EVIDENCE_IDS_THAT_DID_NOT_RESOLVE=0
SOURCE_CITATIONS_THAT_DID_NOT_RESOLVE=0
PROMOTION_CONDITION_ADMITS_A_DISCLOSED_ACTIVE_TOOL=false
FORBIDDEN_PUBLIC_CONTENT_INSTANCES=0
```

Reviewed artifact digests, as recorded by the reviewers:

```
README.md      4ebdce0299d4df35c1cafd3de74454063f9d3fbc10da1679dd71ca7504ca2fc8
MVF-0001       0393b6ad05690adedc5810a09d7cde6e04686dd48e70622a26ff808c2e497cac
MVF-0002       f5915f23f37d7490bd57c44206c3033d4fb4af0b7f940d796e24e4d7c91dbd4f
MVF-0003       a45f291e9be8d89f54eac9bab187ce3a21ec8fc5644af14d1311287d42a64f1e
MVF-0004       e97ffcdeff7e99d1000b2b6a4b588511f096a1b315367f75c9afd59aa61c5975
```

## Affected coordinates

```
AFFECTED_RELEASE      v0.9.0n-b150-dev
RELEASE_SOURCE        ba6d8c39d64a48a6262565208bafacad306c6e1e
RELEASE_DLL_SHA256    cfd1a037f073d07f3eae0e7b189bd70294a5e899e8e9ca8d51da72731951289c
AFFECTED_MAIN_HEAD    f690c017e8e55f2996d0e9121956aae66486742e
```
