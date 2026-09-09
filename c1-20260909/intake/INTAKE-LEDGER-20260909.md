# BUGDROPBOX INTAKE LEDGER — 2026-09-09 — session C1-2026-09-09

**Status: SIDECAR.** These entries carry **no binding verdict**. They are input to a later
`PUBLIC_BUG_INTAKE_DELTA_AUDIT`, which is the only place permanent `BUG-0NN` numbers may be
assigned. **No permanent BUG number is assigned by this file.** The reviewed `BUG-001`–`BUG-020`
ledger is not edited, renumbered or appended to.

Authorities read before this intake ran:

```
CONVENTION        S:\GAMEMODDING\Multivoid-B150-Evidence\pending-intake-2026-09-02\PENDING-BUG-LEDGER.md
ARCHIVE_ROOT      C:\AgenticStaging\BugDropboxArchive
INDEX             C:\AgenticStaging\BugDropboxArchive\INDEX.jsonl        (16 rows before, 80 after)
INBOX             C:\AgenticStaging\BugDropbox
REVIEWED_LEDGER   Multivoid-Private@d6728c66:docs/handoffs/PUBLIC-B150-BUG-AUDIT-2026-09-02-R2.md
PUBLISHED         Multivoid-Private@experimental/findings/verified:findings/MVF-0001..MVF-0004
TAXONOMY          DUPLICATE_OR_CORROBORATION | NEW_BUG_CANDIDATE | NEW_FINDING_FOR_EXISTING_BUG
                  | NON_BUG | INSUFFICIENT | FEEDBACK | EXTERNAL_WORK_POINTER
```

### Strength limits that apply to every row in this file

- Everything in the drop is **third-party reported behaviour**. Nothing here was reproduced,
  instrumented or confirmed by this programme. Every row is `strength=REPORTED`,
  `confirmation=UNPROVEN`.
- The *transcriptions* below are `MEASURED` from the files: text files were read, PNGs were read
  as images. The *reported behaviour* inside them is not.
- The reporter's own `bug_NN_*.md` analyses are **hypotheses**. Where they say "log-proven", that
  is the reporter's reading of his own logs; this intake did **not** open the log bundle and did
  **not** verify any quoted line, and did **not** check any claim against source.
- No repository was modified. No branch was switched. Nothing was pushed. No master server was
  contacted. No network fetch was performed, so the Discord and GitHub links quoted below were
  **not** followed.
- `PERMANENT_DATA_LOSS` is `UNKNOWN` everywhere it could be read in and is asserted nowhere.

---

## 1. Archive record

```
ARCHIVE_DIR=C:\AgenticStaging\BugDropboxArchive\20260909T0302Z
CREATED_FROM=`date -u` -> Wed Sep  9 03:02:01 UTC 2026
FILES_IN_INBOX_AT_CHECK=64
FILES_COPIED=64          (relative paths preserved, `cp -p`)
FILES_HASHED_SOURCE=64
FILES_HASHED_COPY=64
HASH_MISMATCHES=0
INDEX_ROWS_APPENDED=64   (PENDING-20260909-001 .. PENDING-20260909-064)
INBOX_COPY_DELETED_AFTER_ARCHIVE=false
```

**The inbox was not touched.** Nothing was deleted, moved or renamed in
`C:\AgenticStaging\BugDropbox`. Per the standing arrangement, **the operator deletes the inbox
after review**; this intake only copied out of it.

### 1.1 Verification method

Source and archive digests were computed independently and compared as whole listings, so a
mismatch in *any* file or a missing/extra path would show as a diff line:

```
$ cd C:/AgenticStaging/BugDropbox        && find . -type f -print0 | sort -z | xargs -0 sha256sum > src.sha256
$ cd C:/AgenticStaging/BugDropboxArchive/20260909T0302Z && find . -type f -print0 | sort -z | xargs -0 sha256sum > dst.sha256
$ diff src.sha256 dst.sha256 && echo "IDENTICAL"
IDENTICAL
$ wc -l src.sha256 dst.sha256
   64 src.sha256
   64 dst.sha256
  128 total
```

`diff` produced no output and the guard echoed `IDENTICAL`, so all 64 relative paths and all 64
digests match between the inbox and the archive. Every digest below is pasted from the
`sha256sum` run above; none was typed by hand.

### 1.2 sha256sum output, source tree (pasted verbatim)

```
d92aa84a5aaaae2994167b81a384feab70c0de3fa034999e9b34169460ae468f *./Doctaaaa's reports/Report 1/bug_01_spurious_signal_catch_on_connect.md
24b8436fdcdacc56d0742feae251d5f9cf47dedd65f00e508f05b8a0fe8ec180 *./Doctaaaa's reports/Report 1/bug_01_spurious_signal_catch_on_connect.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 1/client_host_050920260342_multivoid.zip
6bb2a1bcb5f565dd73eaec68516dc200dbf93b1d063baca413e120f59606b409 *./Doctaaaa's reports/Report 1/report.txt
4e77cd20616447776dc2302461196ec6d2cb28816bb9be8b74717064e4d5684f *./Doctaaaa's reports/Report 10/bug_10_client_death_quits_to_menu_host_unjoinable.md
77bcbc03ca55bcb8ccfeea1b267c7fbc67de87032672a54410ceb420ab030a1c *./Doctaaaa's reports/Report 10/bug_10_client_death_quits_to_menu_host_unjoinable_compressed.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 10/client_host_050920260342_multivoid.zip
6fe0642054b68480b2b2139f7a1a32eafdebd17e4966a65497fafb1277d7f42a *./Doctaaaa's reports/Report 10/report.txt
c0aabba43dd79dbd42e1eb2ff39f49910db6b07697d8180b2d66b4bf1f79de2e *./Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin.md
b4b6ec760c5f1992900e440285b5a3227926be21077fb0d8aaf0ba057da3892c *./Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin_client_compressed.mp4
d1aa3c33d21e7ce8eb6b01fe9e5d61e3e0d6631b72d0a76d1006e8df7b004b70 *./Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin_host_compressed.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 11/client_host_050920260342_multivoid.zip
f2456ae9e33bcf09e815f9f0e67aa26b309edc194612ddb046a32b442af98b55 *./Doctaaaa's reports/Report 11/report.txt
2df126c422ba9e8977f5fc5823486552d2e9c7cae6a0018618c6dcada282adb3 *./Doctaaaa's reports/Report 2/bug_02_door_key_mismatch_client_cannot_open.md
159d9071781adadb234a0767e5c2738217445106a90fb5348512ef8c4f421245 *./Doctaaaa's reports/Report 2/bug_02_door_key_mismatch_client_cannot_open.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 2/client_host_050920260342_multivoid.zip
f6ad531532b28ddc7a42aff9b9d68e954379e28d3b406959420910294eebf32a *./Doctaaaa's reports/Report 2/report.txt
aaf093de9812c4ac2544bd72079ac02d5088af409c623d9623b08ee27eff5055 *./Doctaaaa's reports/Report 3/bug_03_floppy_dissolves_deferred_destroy_key_collision.md
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 3/client_host_050920260342_multivoid.zip
26e781e218773af5505b45784423731a23f10e28f231f0c76e1cae4a981ff9bc *./Doctaaaa's reports/Report 3/multivoid.log
97bf4ef53038c0b76f5ef253aa77dbceb587af87576560425d5c14b25fc29357 *./Doctaaaa's reports/Report 3/report.txt
0230fe2947ee50f425411e42fbf06d8b2c71f321012a08e18b1f7dd2f4e61afc *./Doctaaaa's reports/Report 3/ну то самое.mov
3a71fabbdf7276ead5c68819c61a7bebecac831dd510fd22d7eae9a3d6927813 *./Doctaaaa's reports/Report 4/bug_04_light_switch_client.md
08f00528c98afc8f34c4cd7e5bba5ebcc562e689302c090ca6147226834a21ac *./Doctaaaa's reports/Report 4/bug_04_light_switch_client.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 4/client_host_050920260342_multivoid.zip
82ffc97430ab7330b4e3fd0467a8cec3e5d612b551c9974f67603fb567645a7f *./Doctaaaa's reports/Report 4/report.txt
99b8e463ddd90d688144ad47d7a90f9713d61a73f3bc15a22ee2a394840cedcf *./Doctaaaa's reports/Report 5/bug_05_disc_signal_lost_hand_transitions.md
1ea0cd8d66372ba7b929a5e818be7551325dfc6c1ac86da14b4419331fcff303 *./Doctaaaa's reports/Report 5/bug_05_disc_signal_lost_hand_transitions.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 5/client_host_050920260342_multivoid.zip
a1b045631f6233b55f29c55e377463468bc1147b6255193fb049b0db0d6de62e *./Doctaaaa's reports/Report 5/report.txt
6494a3b42d07977b7f740ac11e4f0a4b88c3a2537a34f93fdecdaf4af2af9e1a *./Doctaaaa's reports/Report 6/bug_06_client_broken_container_loot_invisible_to_host.md
16a8d6b7d5eb2a8952990a2dd680db979440621549046f5869693366204c3d66 *./Doctaaaa's reports/Report 6/bug_06_client_broken_container_loot_invisible_to_host.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 6/client_host_050920260342_multivoid.zip
82227e45812813283283896dd598943a13d60620933dd2123420aff7146dbda4 *./Doctaaaa's reports/Report 6/report.txt
500b3a155c9bd7e1a2323c7c4699c43cbb3824abd50fc11355105763c21e628b *./Doctaaaa's reports/Report 7/Report.txt
dad95479dfb1fd78fa08c8eedf4f8f9a9767f02156d746647283ca254d9affb6 *./Doctaaaa's reports/Report 7/bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.md
154c37ac1be89cb9f37bc1d0208e3845dc8658fc88da59b6916761701d3bb369 *./Doctaaaa's reports/Report 7/bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 7/client_host_050920260342_multivoid.zip
68deb05a71a52a39ccf5e686261de0e670343fd2fe0e18485cd5e07cda21d038 *./Doctaaaa's reports/Report 8/bug_08_consoles_off_after_power_restore_client.md
3048013447ee924731004a108b1b7b1ff622ab2a51cc822e40793c39b18633a0 *./Doctaaaa's reports/Report 8/bug_08_consoles_off_after_power_restore_client_compressed.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 8/client_host_050920260342_multivoid.zip
4f946f0a7d84756ed8c91a51789134d6b4f02b5b193d52cc5d86b0185b27c9a2 *./Doctaaaa's reports/Report 8/report.txt
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 *./Doctaaaa's reports/Report 9/New Text Document.txt
24b7189d82b386857a2954db4f0196719a6013871236cdd96314613a4757648e *./Doctaaaa's reports/Report 9/bug_09_barrel_explosion_chain_tr1_client_only.md
0dc4d6720b0a2bf760accf6a6e096c7d547d3dd1c600ff94e0286895d7cca3c0 *./Doctaaaa's reports/Report 9/bug_09_barrel_explosion_chain_tr1_client_only.mp4
5ac2b8a87c8549bf8dbd7fd45986bd2f80d5966a03dae3ab81143601e608b7c0 *./Doctaaaa's reports/Report 9/client_host_050920260342_multivoid.zip
14dc0d8508aab312160e9c4337498754560d3c4d758b9e1c7d75205044532619 *./Doctaaaa's reports/Report 9/report.txt
6bf9f0cc81b1eb8819676a1e753da2ac0b826b9dbc1e507d2f9f39acb8c31aa3 *./Screenshot 2026-09-04 110239.png
1c141f78d2742dd6190da5ee8964b623f19af8aba77d9c4c4715d6c9182d076a *./Screenshot 2026-09-04 110249.png
d0dad649abd5b5e328fcc285865f595b3f58546d20ad4db80928f382846cb570 *./Screenshot 2026-09-04 110302.png
fb98ca069c577b6ede226692d1793b601064fb3c73369f60a6789d4d29a89990 *./Screenshot 2026-09-04 121754.png
2570de1279d5a7b74fff2677adf8c7855fd9029118eeda4558628b31651259d4 *./Screenshot 2026-09-04 121811.png
cb0385a9b205866b7c6d166d76effea345c1b52a506368396593df5d373ac89f *./Screenshot 2026-09-05 063921.png
986bdd9803b27a969567612b68c8b77b830713d7b37414ad31a13905168e751b *./Screenshot 2026-09-05 131410.png
70ce4c9a072e73179dcb5f6d8593a359ded9f6c3419202618ce8ce76a182f900 *./Screenshot 2026-09-05 181159.png
91ab4ab8dd965095501cf2abe287a9303975c281d24c6673a9b9a80f5dd98582 *./Screenshot 2026-09-06 010331.png
4ce69fd2f121d37af753dbdc76099adadd092e2efc17c2613237b6e9d6da451a *./Screenshot 2026-09-06 034445.png
a99ff950cb035b411d9f51ec71a4f37b79051d1401e0719fe48aae19abe033a6 *./Screenshot 2026-09-06 034454-EXPLOSIONS.png .png
0c08e5aca26a12a7f029297b7df9bdc026c9da2c799acd0305afd60f0398f253 *./Screenshot 2026-09-06 034600.png
a4dcee6654ca1586b34ee48941c2693a149297a77d78506e43e1cd182b9438ad *./Screenshot 2026-09-06 052818.png
796d6c01320091e8fd970b584f1839e7c14007b18e2ffa1eb592457a8fce8b8f *./Screenshot 2026-09-06 052830.png
c35bd4ada330c3dbce091d1f8e53e2767d6a2834d7c62d30744e78b0dba4f6af *./Screenshot 2026-09-06 052842.png
182619a5b4416dc6d035f543f133a56d6b18913a29bc9354733c81ade628a1f8 *./Screenshot 2026-09-07 002454.png
dc282a253e589afd8c6dfe5de5ac000bfaec8ef46e0ea824a995058591c9c942 *./Votv 2026.09.05 - 19.11.18.14.DVR- EXPLOSIONS - Trim.mp4
```

### 1.3 Facts the digests themselves establish

1. **The eleven `client_host_050920260342_multivoid.zip` files are one file, copied eleven times.**
   All eleven carry `5ac2b8a8…`, 7,584,525 bytes. `unzip -l` on one of them lists 5 logs,
   70,877,190 bytes uncompressed: `client_050920260342_multivoid.log` (18,789,833),
   `host_050920260342_multivoid.log` (31,033,499), `client_040920262246_multivoid.log`
   (6,176,538), `client_050920260005_multivoid.log` (12,228,380),
   `client_050920260012_multivoid.log` (2,648,940). **The bundle was not extracted and not
   censused by this intake**, so every log line quoted in the reports below is the reporter's
   quotation, unverified here.
2. **`Report 3/multivoid.log` is a re-submission.** Its digest `26e781e2…` is byte-identical to
   the already-archived `PENDING-20260904-008`
   (`multivoid(Screenshot 2026-09-04 002050.png).log`), whose recorded census was 152,903 lines
   / 148,410 INFO / 1,270 WARN / 3,222 ERROR, clock 22:11:23 → 00:06:29 over a midnight rollover.
3. **`Report 9/New Text Document.txt` is empty by digest.**
   `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` is the sha256 of the empty
   string, and the file is 0 bytes. It is not an unreadable file; it has no content.
4. `Report 3/report.txt` is the same single Russian sentence already archived as
   `PENDING-20260904-003`.

### 1.4 Timestamps

Filesystem modification times were recorded per file in `INDEX.jsonl` as
`modified_filesystem_local`, with an explicit `time_zone_note`: they come from the local clock and
**no UTC offset was established**. In-image message timestamps are in the capturing client's own
zone and are likewise not normalised. The archive directory name uses the measured UTC clock
(`Wed Sep  9 03:02:01 UTC 2026`).

---

## 2. Per-item table (64 rows)

Ids run `PENDING-20260909-001` … `-064` in sorted path order, one per file, matching the
`INDEX.jsonl` rows appended by this intake. Sub-ids (`-001a`, `-052j`, …) are recorded inside the
`INDEX.jsonl` rows and in §4/§5 below; the table lists one row per *file*.

| PENDING_ID | Source file (relative to the inbox root) | Category | One line | Related ledger / pending / finding ids |
|---|---|---|---|---|
| PENDING-20260909-001 | `Doctaaaa's reports/Report 1/bug_01_spurious_signal_catch_on_connect.md` | NEW_BUG_CANDIDATE | Bug 01: a signal catch is published on client connect with no player action, in the same second as the join-time slew of all 24 dishes. | PENDING-20260904-002f, PENDING-20260909-060 |
| PENDING-20260909-002 | `Doctaaaa's reports/Report 1/bug_01_spurious_signal_catch_on_connect.mp4` | INSUFFICIENT | UNVIEWED video for Bug 01. | PENDING-20260909-001 |
| PENDING-20260909-003 | `Doctaaaa's reports/Report 1/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-001 |
| PENDING-20260909-004 | `Doctaaaa's reports/Report 1/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 01; states build b150, roles, log windows and the log-proven vs hypothesis split. | PENDING-20260909-001 |
| PENDING-20260909-005 | `Doctaaaa's reports/Report 10/bug_10_client_death_quits_to_menu_host_unjoinable.md` | NEW_BUG_CANDIDATE | Bug 10: client death quits the client to the main menu and tears down the live session; the host's own death is correctly vetoed, and the orphaned host stays listed but unjoinable. | PENDING-20260909-058, PENDING-20260909-009 |
| PENDING-20260909-006 | `Doctaaaa's reports/Report 10/bug_10_client_death_quits_to_menu_host_unjoinable_compressed.mp4` | INSUFFICIENT | UNVIEWED video for Bug 10. | PENDING-20260909-005 |
| PENDING-20260909-007 | `Doctaaaa's reports/Report 10/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-005 |
| PENDING-20260909-008 | `Doctaaaa's reports/Report 10/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 10, including the reporter's caveat that an unreleased fix may exist. | PENDING-20260909-005 |
| PENDING-20260909-009 | `Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin.md` | NEW_BUG_CANDIDATE | Bug 15: on the first join after a host-death tear the host's pose pipeline for that slot dies for 5 min while the transport and the prop ferry stay healthy; a later control join is clean. | PENDING-20260902-002b, PENDING-20260909-062, PENDING-20260909-061 |
| PENDING-20260909-010 | `Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin_client_compressed.mp4` | INSUFFICIENT | UNVIEWED video for Bug 15 (client view). | PENDING-20260909-009 |
| PENDING-20260909-011 | `Doctaaaa's reports/Report 11/bug_15_puppet_frozen_at_spawn_after_rejoin_host_compressed.mp4` | INSUFFICIENT | UNVIEWED video for Bug 15 (host view). | PENDING-20260909-009 |
| PENDING-20260909-012 | `Doctaaaa's reports/Report 11/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-009 |
| PENDING-20260909-013 | `Doctaaaa's reports/Report 11/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 15. | PENDING-20260909-009 |
| PENDING-20260909-014 | `Doctaaaa's reports/Report 2/bug_02_door_key_mismatch_client_cannot_open.md` | NEW_FINDING_FOR_EXISTING_BUG | Bug 02: the two peers rebuild the same 50 doors under different keys, so client door toggles are unaddressable at the host; only the 31 world-load-added doors are affected. | BUG-012, BUG-014, PENDING-20260904-002b, PENDING-20260903-002, PENDING-20260909-023, PENDING-20260909-055, PENDING-20260909-061, PENDING-20260902-001b |
| PENDING-20260909-015 | `Doctaaaa's reports/Report 2/bug_02_door_key_mismatch_client_cannot_open.mp4` | INSUFFICIENT | UNVIEWED video for Bug 02. | PENDING-20260909-014 |
| PENDING-20260909-016 | `Doctaaaa's reports/Report 2/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-014 |
| PENDING-20260909-017 | `Doctaaaa's reports/Report 2/report.txt` | NEW_FINDING_FOR_EXISTING_BUG | Bilingual cover note for Bug 02; cites two similar Discord threads. | BUG-012, PENDING-20260909-014 |
| PENDING-20260909-018 | `Doctaaaa's reports/Report 3/bug_03_floppy_dissolves_deferred_destroy_key_collision.md` | NEW_FINDING_FOR_EXISTING_BUG | Bug 03: an extracted floppy dissolves because a deferred destroy-by-key armed against an earlier same-keyed actor fires post-bind on the newly spawned prop; keys are not instance-unique and eid is 0. | BUG-018, PENDING-20260904-002a, PENDING-20260904-003a, PENDING-20260904-005a, PENDING-20260904-005b, PENDING-20260909-057, PENDING-20260909-052 |
| PENDING-20260909-019 | `Doctaaaa's reports/Report 3/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-018 |
| PENDING-20260909-020 | `Doctaaaa's reports/Report 3/multivoid.log` | INSUFFICIENT | Runtime log, byte-identical to the already-archived PENDING-20260904-008; no new census run. | PENDING-20260904-008, PENDING-20260909-018 |
| PENDING-20260909-021 | `Doctaaaa's reports/Report 3/report.txt` | DUPLICATE_OR_CORROBORATION | One Russian sentence saying the floppy very often gets stuck in the server; same wording as PENDING-20260904-003. | BUG-018, PENDING-20260904-003a, PENDING-20260909-018 |
| PENDING-20260909-022 | `Doctaaaa's reports/Report 3/ну то самое.mov` | INSUFFICIENT | UNVIEWED video for Bug 03 (Cyrillic filename). | PENDING-20260909-018 |
| PENDING-20260909-023 | `Doctaaaa's reports/Report 4/bug_04_light_switch_client.md` | NEW_BUG_CANDIDATE | Bug 04: the same per-peer key instability proven on the light and lightgroup indexes; a client switch key appears 0 times at the host and the message expires unapplied. | PENDING-20260909-014, PENDING-20260909-054, PENDING-20260909-039 |
| PENDING-20260909-024 | `Doctaaaa's reports/Report 4/bug_04_light_switch_client.mp4` | INSUFFICIENT | UNVIEWED video for Bug 04. | PENDING-20260909-023 |
| PENDING-20260909-025 | `Doctaaaa's reports/Report 4/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-023 |
| PENDING-20260909-026 | `Doctaaaa's reports/Report 4/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 04. | PENDING-20260909-023 |
| PENDING-20260909-027 | `Doctaaaa's reports/Report 5/bug_05_disc_signal_lost_hand_transitions.md` | NEW_FINDING_FOR_EXISTING_BUG | Bug 05: a recorded disc payload does not follow the prop; each hand transition destroys and recreates the world actor as a new generation with empty state, and the re-inserted disc is invisible to the client. | BUG-005, BUG-018, MVF-0001, MVF-0004, PENDING-20260909-055, PENDING-20260909-057 |
| PENDING-20260909-028 | `Doctaaaa's reports/Report 5/bug_05_disc_signal_lost_hand_transitions.mp4` | INSUFFICIENT | UNVIEWED video for Bug 05. | PENDING-20260909-027 |
| PENDING-20260909-029 | `Doctaaaa's reports/Report 5/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-027 |
| PENDING-20260909-030 | `Doctaaaa's reports/Report 5/report.txt` | NEW_FINDING_FOR_EXISTING_BUG | Bilingual cover note for Bug 05. | BUG-005, PENDING-20260909-027 |
| PENDING-20260909-031 | `Doctaaaa's reports/Report 6/bug_06_client_broken_container_loot_invisible_to_host.md` | DUPLICATE_OR_CORROBORATION | Bug 06: a client-broken container spills 12 items client-side while the host gets only a bare keyed destroy; the loot crosses only one item per client pickup via the held-item broadcast. | BUG-016, PENDING-20260904-006a, PENDING-20260909-044, PENDING-20260909-061, PENDING-20260909-052 |
| PENDING-20260909-032 | `Doctaaaa's reports/Report 6/bug_06_client_broken_container_loot_invisible_to_host.mp4` | INSUFFICIENT | UNVIEWED video for Bug 06. | PENDING-20260909-031 |
| PENDING-20260909-033 | `Doctaaaa's reports/Report 6/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-031 |
| PENDING-20260909-034 | `Doctaaaa's reports/Report 6/report.txt` | DUPLICATE_OR_CORROBORATION | English cover note for Bug 06. | BUG-016, PENDING-20260909-031 |
| PENDING-20260909-035 | `Doctaaaa's reports/Report 7/Report.txt` | DUPLICATE_OR_CORROBORATION | Bilingual cover note for Bug 07 (capitalised filename; this folder has no lowercase report.txt). | PENDING-20260904-002h, PENDING-20260904-006d, PENDING-20260909-036 |
| PENDING-20260909-036 | `Doctaaaa's reports/Report 7/bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.md` | DUPLICATE_OR_CORROBORATION | Bug 07: a pile grab-intent camera cone hijacks the client's E-presses, cancels the aimed native use, produces dual carry, and the thrown clump nativises into a new world pile for both peers; 33+ hijacks. | BUG-013, BUG-015, PENDING-20260904-002h, PENDING-20260904-006d, PENDING-20260904-002i, PENDING-20260909-052, PENDING-20260909-054, PENDING-20260909-055, PENDING-20260909-061 |
| PENDING-20260909-037 | `Doctaaaa's reports/Report 7/bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.mp4` | INSUFFICIENT | UNVIEWED video for Bug 07. | PENDING-20260909-036 |
| PENDING-20260909-038 | `Doctaaaa's reports/Report 7/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-036 |
| PENDING-20260909-039 | `Doctaaaa's reports/Report 8/bug_08_consoles_off_after_power_restore_client.md` | NEW_BUG_CANDIDATE | Bug 08: the power mask and the lights restore correctly on the client, but the client's consoles stay dead because the mod has no console subsystem and the native restoration chain never runs there. | BUG-019, PENDING-20260904-002e, PENDING-20260909-054, PENDING-20260909-031, PENDING-20260909-044 |
| PENDING-20260909-040 | `Doctaaaa's reports/Report 8/bug_08_consoles_off_after_power_restore_client_compressed.mp4` | INSUFFICIENT | UNVIEWED video for Bug 08. | PENDING-20260909-039 |
| PENDING-20260909-041 | `Doctaaaa's reports/Report 8/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-039 |
| PENDING-20260909-042 | `Doctaaaa's reports/Report 8/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 08. | PENDING-20260909-039 |
| PENDING-20260909-043 | `Doctaaaa's reports/Report 9/New Text Document.txt` | INSUFFICIENT | ZERO-BYTE file; empty by digest, carries no content. | PENDING-20260909-044 |
| PENDING-20260909-044 | `Doctaaaa's reports/Report 9/bug_09_barrel_explosion_chain_tr1_client_only.md` | NEW_BUG_CANDIDATE | Bug 09: a client-lit barrel chain destroys 11 keyed props, kills and ignites the transformer, all client-only; the host receives bare keyed destroys and no explosion, fire or loot spill. | BUG-016, PENDING-BUG-022, PENDING-20260909-031, PENDING-20260909-058, PENDING-20260909-064 |
| PENDING-20260909-045 | `Doctaaaa's reports/Report 9/bug_09_barrel_explosion_chain_tr1_client_only.mp4` | INSUFFICIENT | UNVIEWED video for Bug 09. | PENDING-20260909-044 |
| PENDING-20260909-046 | `Doctaaaa's reports/Report 9/client_host_050920260342_multivoid.zip` | INSUFFICIENT | Duplicate copy of the one shared 5-log bundle; unextracted, uncensused. | PENDING-20260909-044 |
| PENDING-20260909-047 | `Doctaaaa's reports/Report 9/report.txt` | NEW_BUG_CANDIDATE | Bilingual cover note for Bug 09. | PENDING-20260909-044 |
| PENDING-20260909-048 | `Screenshot 2026-09-04 110239.png` | NEW_BUG_CANDIDATE | Discord post: cleaning surfaces with water is not synced to other players, even when the host cleans. | none |
| PENDING-20260909-049 | `Screenshot 2026-09-04 110249.png` | DUPLICATE_OR_CORROBORATION | Discord post: 2-9 fps when not hosting. | PENDING-20260902-002a, PENDING-20260903-002, PENDING-20260909-051, PENDING-20260909-056, PENDING-20260909-063 |
| PENDING-20260909-050 | `Screenshot 2026-09-04 110302.png` | NEW_BUG_CANDIDATE | Discord post: the bottom servers in the mod UI cannot be clicked at 2560x1600 or 2560x1440; the reporter concludes the UI cannot handle anything above 1080p. | none |
| PENDING-20260909-051 | `Screenshot 2026-09-04 121754.png` | NEW_FINDING_FOR_EXISTING_BUG | The same fps thread later, with a reply attributing it to every item on the map being duplicated for non-host players. | PENDING-20260902-002a, PENDING-20260909-049, PENDING-20260909-056, PENDING-20260909-063 |
| PENDING-20260909-052 | `Screenshot 2026-09-04 121811.png` | MULTI_REPORT_CAPTURE: DUPLICATE_OR_CORROBORATION + NEW_FINDING_FOR_EXISTING_BUG + NEW_BUG_CANDIDATE + NON_BUG | Discord post "Some bugs I've found": ten items covering trash duplication, drone send, floppy/drive eject, stuck trash pile, a wrong satellite terminal identity, client shovelling, scrap box, doors staying open, host-must-touch items, and inventory replaced after a host save/reload. | BUG-013, BUG-014, BUG-015, BUG-018, BUG-019, PENDING-20260903-002d, PENDING-20260903-002e, PENDING-20260903-003a, PENDING-20260904-002i, PENDING-20260904-002j, PENDING-20260904-006f, PENDING-20260909-018, PENDING-20260909-036, PENDING-20260909-057, PENDING-20260909-059, PENDING-20260909-061, PENDING-BUG-023 |
| PENDING-20260909-053 | `Screenshot 2026-09-05 063921.png` | DUPLICATE_OR_CORROBORATION | Discord reply: the singleplayer world-settings workaround does NOT carry over to multiplayer, contradicting the workaround recorded under PENDING-20260902-004. | PENDING-20260902-004a |
| PENDING-20260909-054 | `Screenshot 2026-09-05 131410.png` | MULTI_REPORT_ISSUE: DUPLICATE_OR_CORROBORATION + NEW_FINDING_FOR_EXISTING_BUG + NEW_BUG_CANDIDATE | GitHub issue 14: seven items covering flashlight flicker, client-only duplicated objects, grabbing through walls, an unseen generator toggle, points desync, clients unable to call the drone, and ATV lag. | BUG-002, BUG-009, BUG-015, PENDING-20260902-002a, PENDING-20260903-002d, PENDING-20260904-002j, PENDING-20260909-036, PENDING-20260909-023, PENDING-20260909-039, PENDING-20260909-051, PENDING-20260909-055 |
| PENDING-20260909-055 | `Screenshot 2026-09-05 181159.png` | MULTI_REPORT_CAPTURE: DUPLICATE_OR_CORROBORATION + NEW_BUG_CANDIDATE + FEEDBACK | Discord post "Data sync issue": upgrades, cords, shop queue, trash through walls, multiple objects at once, container/drive rack contents, satellite and other main-facility doors; closing with "only positioning of objects works, but not their data". | BUG-012, BUG-017, PENDING-20260903-002f, PENDING-20260904-004a, MVF-0001, MVF-0004, PENDING-20260909-014, PENDING-20260909-027, PENDING-20260909-036, PENDING-20260909-054, PENDING-20260909-061 |
| PENDING-20260909-056 | `Screenshot 2026-09-06 010331.png` | DUPLICATE_OR_CORROBORATION | Discord post: on joining a friend's game the fps fell to 2.5, then basement crates duped and the friend saw the joiner noclipping and moving them effortlessly. | PENDING-20260902-002a, PENDING-20260902-002c, PENDING-20260909-049, PENDING-20260909-051, PENDING-20260909-063, PENDING-20260909-052 |
| PENDING-20260909-057 | `Screenshot 2026-09-06 034445.png` | DUPLICATE_OR_CORROBORATION | Discord post: spamming R stops floppies disappearing, but moving data to a zip disk yields INVALID_DATA_BLOCK and blocks task completion. | BUG-018, PENDING-20260904-002a, PENDING-20260904-003a, PENDING-20260904-005a, PENDING-20260909-018, PENDING-20260909-052 |
| PENDING-20260909-058 | `Screenshot 2026-09-06 034454-EXPLOSIONS.png .png` | NEW_BUG_CANDIDATE | Discord post: explosions from lightning strikes or explosive barrels black-screen and boot everyone to the main screen. | PENDING-20260909-005, PENDING-20260909-044, PENDING-20260909-064 |
| PENDING-20260909-059 | `Screenshot 2026-09-06 034600.png` | DUPLICATE_OR_CORROBORATION | Discord post: after rejoining, everyone gets the host's items and those items may disappear when dropped, even by the host. | PENDING-20260903-002e, PENDING-BUG-023, PENDING-20260909-052, PENDING-20260909-061 |
| PENDING-20260909-060 | `Screenshot 2026-09-06 052818.png` | NEW_BUG_CANDIDATE | Discord post: someone joining resets an in-progress satellite download. | PENDING-20260904-002f, PENDING-20260909-001 |
| PENDING-20260909-061 | `Screenshot 2026-09-06 052830.png` | MULTI_REPORT_CAPTURE: DUPLICATE_OR_CORROBORATION + NEW_FINDING_FOR_EXISTING_BUG + NEW_BUG_CANDIDATE | Discord post "Various Sync Issues": twelve items from a four-player lobby, including inventory replacement, a bricked host inventory from a duped shovel, garbage duplication and loss, supply-box loot lost across save load, keypad/transformer/satellite doors, per-observer invisibility, global instead of proximal audio, drone send, and vitals copied from the host at join. | BUG-002, BUG-012, BUG-013, BUG-015, BUG-016, PENDING-20260902-001a, PENDING-20260902-001b, PENDING-20260902-002b, PENDING-20260903-002e, PENDING-20260904-002j, PENDING-20260904-006f, PENDING-BUG-023, PENDING-20260909-009, PENDING-20260909-014, PENDING-20260909-031, PENDING-20260909-052, PENDING-20260909-059, PENDING-20260909-062 |
| PENDING-20260909-062 | `Screenshot 2026-09-06 052842.png` | NEW_BUG_CANDIDATE | Discord post: a leaked "remote player" placeholder occupies the slot of anyone who leaves or fails to join, and that player's movement can no longer be seen although grabbing seems to still work. | PENDING-20260902-002b, PENDING-20260909-009, PENDING-20260909-061 |
| PENDING-20260909-063 | `Screenshot 2026-09-07 002454.png` | DUPLICATE_OR_CORROBORATION | Discord reply: the client-side low FPS is attributed to most physics objects being duplicated on clients. | PENDING-20260902-002a, PENDING-20260909-049, PENDING-20260909-051, PENDING-20260909-056, PENDING-20260909-054 |
| PENDING-20260909-064 | `Votv 2026.09.05 - 19.11.18.14.DVR- EXPLOSIONS - Trim.mp4` | INSUFFICIENT | UNVIEWED loose video; associated with the EXPLOSIONS capture by filename only. | PENDING-20260909-058, PENDING-20260909-044 |


---

## 3. Transcriptions

### 3.1 The eleven structured reports — common frame

Every one of the eleven `report.txt` / `Report.txt` cover notes states the same setup line, in
English and (except Report 6) again in Russian:

> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1.

**REPORTER'S STATED BUILD = `0.9.0n b150` for all eleven reports** — the required build. No
screenshot in the drop states any build at all.

The peers are named in the `bug_NN_*.md` metadata: host `Mikuzavr` (slot 0), client
`PlayerNickname2` (slot 1). Reports 1–10 draw on logs from **2026-09-04, 21:49–23:45**; Report 11
from **2026-09-05, 00:07–00:12**. All eleven are the same two people in one continuous evening.

Each `bug_NN_*.md` carries a metadata table with `Video window`, `Log window`, `Log files`,
`Reporter`, `Peers`, `Subsystem` and per-report extras, then a body that separates evidence from
a section the reporter titles **"What the logs cannot show"**. That structure is the reason this
drop is treated as higher-grade than the loose captures — but it does **not** make any of it
measured by this programme.

| Report | md file | Build | Log window (2026-09-04 unless noted) | Subsystem the reporter names | Log-proven / player-reported split, in the reporter's own terms |
|---|---|---|---|---|---|
| 1 | `bug_01_spurious_signal_catch_on_connect.md` | 0.9.0n b150 | 21:49:25–21:49:36; recurrence 22:48:53–22:49:00 | `signal_catch` (client catch detector) × `dish_sync` (join baseline slew) + feed | LOG-PROVEN (both peers): the false catch fires in the same second as the join-time slew storm; all 24 dish mirrors slew while the client's dish tickers are parked; the host then applies "catch replay" and publishes the feed message. NOT PROVABLE: the detector's internals; the trigger mechanism is "our hypothesis from timing, not a code reading". |
| 2 | `bug_02_door_key_mismatch_client_cannot_open.md` | 0.9.0n b150 | 21:50:24–21:51:48; addendum 22:53:18 | door keyed index (`keysHash`) + use-input hook suppression | LOG-PROVEN: the client's door key `'pDcZUv8…'` appears 0 times in the host log; the host uses `'f2kqYIFF…'` for the same door; the mod's own "compare host vs client for cross-peer Key stability" check FAILS — 50 doors rebuilt with different keys, 4 distinct `keysHash` over 2 world loads; the 19 named base doors sync, the 31 world-load-added doors do not. NOT PROVABLE: how door keys are derived (hypothesis: non-stable runtime identity); why the native use ran unsuppressed for one door and not the other. |
| 3 | `bug_03_floppy_dissolves_deferred_destroy_key_collision.md` | 0.9.0n b150 | 21:51:07–21:51:16 | prop identity (`eid=0`, key-only) + deferred destroy-by-key (`[DESTROY-DEFER]` / destroy-seam) | LOG-PROVEN: the whole causal chain (client destroy → host arms deferred destroy → client drop → host spawns → same-tick destroy). PLAYER-REPORTED (other sessions, not in these logs): the same dissolution after **host-side** floppy interactions. NOT PROVABLE: why the drop-intent path allocates no eid; whether key sharing is intended; whether `[DESTROY-DEFER]` has any expiry rule. |
| 4 | `bug_04_light_switch_client.md` | 0.9.0n b150 | 21:51:28–21:51:57 | `light` / `lightgroup` keyed indexes + deferred-apply retry queue | LOG-PROVEN: 3 "light: sent ON/OFF" in 3 s with zero effect; switch key `'BqgD3X87kKv8T_DyLebx2g'` 0 times in the host log; host retry queue "applied 0 deferred, dropped 1 expired"; 42 instances, 4 hashes over 2 loads; 27 expired light applies dropped right after connect; named `'lightswitch_outside'` works. NOT PROVABLE: the host→client direction per-message for this switch; whether a louder WARN exists but is disabled. |
| 5 | `bug_05_disc_signal_lost_hand_transitions.md` | 0.9.0n b150 | 22:01:02–22:01:44 | `drive_sync` (recorded payload) vs `hand_item`/`remote_prop` actor generations + re-insert fuzzy match | LOG-PROVEN: payload bound to eid=3517 does not follow the prop; eid drift 3517 → 5279 → 5280; final rebind wire-destroyed the same second; host payload apply expires "actor never resolved"; pose stream "no local match" ~13+/s. NOT PROVABLE: where the payload physically lives; whether per-generation binding is intentional. |
| 6 | `bug_06_client_broken_container_loot_invisible_to_host.md` | 0.9.0n b150 | 22:07:36–22:07:37; ferry 22:07:47–22:08:55 | wire destroy vs native container death effects; re-seed authority + held-broadcast ferry | LOG-PROVEN: 12 items spill client-side; the host gets only a bare keyed destroy `eid=0`; no `container_contents`, no `PropSpawn`; the mod logs "a client authors no PropSpawn"; the ferry crosses 13 items on client pickups, eids 42703–42715, each triggering a host-side OnSpawn. NOT PROVABLE: the native loot-spill logic itself; whether the authority split is intended. |
| 7 | `bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.md` | 0.9.0n b150 | 22:09:04–22:11:43; recurrence 23:24:53–23:44:56 | pile grab-intent (camera-ray cone) vs native use; throw-intent `release(E)`; clump→pile NATIVIZE | LOG-PROVEN: `[GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy (camera-ray cone) -> native use CANCELLED (no use_deny)`; `paired E-release CANCELLED (its press was intercepted)`; cone matched a pile proxy ~3.1 m away while the cursor was elsewhere; dual carry log-proven; 33+ hijacks. NOT PROVABLE: the source of one switch-adjacent clump; any host-side hijack; whether the cone has a reach bound. |
| 8 | `bug_08_consoles_off_after_power_restore_client.md` | 0.9.0n b150 | blackout 22:14:46, breakers 22:17:24–22:17:26, consoles 22:20:05 | `power` keyed mask sync vs the native power-restoration event chain; **no console/computer sync path exists** | LOG-PROVEN: blackout `mask=0x00` + 11 lightgroup OFF applies; every restore mask bit `ok=1`; lights and breaker states return on the client; the keyed-subsystem index has no console entry. PLAYER-REPORTED: the client's consoles and main computer stay dead, the host's work, only the processing panel works on the client. NOT PROVABLE: the native restoration chain (console power is unlogged on both sides); whether the client's consoles ever recovered. |
| 9 | `bug_09_barrel_explosion_chain_tr1_client_only.md` | 0.9.0n b150 | 22:34:52–22:36:09 | wire destroy vs native death effects at scale (11 props); missing fire/device-state sync; `power` mask client→host | LOG-PROVEN: 11 keyed props destroyed; power mask `0x00` client→host applied `ok=1`; the host's log has zero explosion/fire/damage events; every prop reached her only as a bare keyed destroy `eid=0`. PLAYER-REPORTED: the host saw nothing and later ate an MRE in the client's burn zone. NOT PROVABLE: the ignition moment (window derived); whether the mask visibly blacked out the host's base; what 2 of the keyed props were. |
| 10 | `bug_10_client_death_quits_to_menu_host_unjoinable.md` | 0.9.0n b150 | client death ~22:44:09 (derived, no direct line) → re-admit 22:48:28 | `death_revive` travel veto (host-only trace) + session teardown on gameplay→MENU; P2P listener lifecycle + stale lobby listing | LOG-PROVEN: `gameplay->MENU while a session is live`; the host's death armed, travel REFUSED, revive OK `hp=99.6`; the client's death left zero `death_revive` trace; both join attempts burned ~10 s timeouts with no trace in the host log; re-host at 22:48:04, client admitted 22:48:28. NOT PROVABLE: what killed the client; whether death skips the vetoed seam or arming just does not run client-side; why the lobby listing survived. |
| 11 | `bug_15_puppet_frozen_at_spawn_after_rejoin.md` | 0.9.0n b150 | **2026-09-05** 00:07:17–00:12:28 | player pose sync (`RemotePlayer` puppet / `pose-diag`) vs the held-item/prop ferry | LOG-PROVEN: the host's slot-1 pose pipeline applied a few poses in ~2 s then zero for 5 min 07 s; the 4.1 MB backlog drained 23 s after join and the link was healthy; the client sent ~60 pose packets/s; client-origin prop traffic applied byte-for-byte with exact throw velocity vectors. CONTROL: a join 4 minutes later was healthy from the first second. PLAYER-REPORTED (host's screen): a motionless puppet and "levitating" objects. NOT PROVABLE: the internal state of the pose consumer — no receive/drop/reject lines exist; `fresh=0/s` is the only witness. |

### 3.2 The cover notes, verbatim

Reproduced in the English half as supplied; the Russian half of each file is a translation of the
same text and is preserved in the archive copy. Report 3's note is Russian only and is given in
full.

**Report 1 — `report.txt`**

> Similar case: https://discordapp.com/channels/1508397685726646272/1545903206696222801
>
> On connect, a signal-catch event fires in the world feed without any player action: "PlayerNickname2 caught signal 'mars_bday'" — the client hadn't even finished loading (pawn announced 10 s later). It recurred identically on re-join ('whiteDwarf_0'). Later mid-game catches were intentional (player-confirmed) — the bug is specific to the connect/re-connect window.
>
> Log-proven (both peers): the false catch fires in the same second as the join-time baseline slew storm — all 24 dish mirrors slew at once while the client's dish tickers are parked; the host then applies "catch replay" and publishes the same feed message.
>
> Not provable from logs: the detector's internals. The trigger mechanism (the slew storm sweeping the active signal's tolerance window) is our hypothesis from timing, not a code reading.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 21:49:25 and 22:48:53–22:49:00. Full report with log excerpts — see attached file.

**Report 2 — `report.txt`**

> Similar issues: https://discordapp.com/channels/1508397685726646272/1545905185224663050/1545905185224663050
> https://discordapp.com/channels/1508397685726646272/1508398107753185363/1545852201010856066
>
> After joining, the client's E-presses on the radio telescope door do nothing: the client logs toggle requests under its key of the door ('pDcZUv8…'), but that key appears 0 times in the host's log and no "door: applied" ever arrives. Host-side toggles of the same door use a different key ('f2kqYIFF…') unknown to the client — the host walks through doors that stay visually closed on the client's side.
>
> Root cause is logged by the mod itself: the "compare host vs client for cross-peer Key stability" check fails — both peers rebuild the same 50 doors with different keys (4 distinct keysHash values across 2 world loads). Named base doors sync fine; the affected set is the 31 world-load-added doors.
>
> Worse variant (addendum): in session 2 a telescope door DID open on the client's E — but client-locally only, with zero sync — a silent door-state fork between peers.
>
> Not provable from logs: how door keys are derived (our hypothesis: non-stable runtime identity) and why the native use ran unsuppressed for one door but not the other.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 21:50:24–21:51:48 and 22:53:18. Full report with log excerpts — see attached file.

**Report 3 — `report.txt`** (Russian only, 136 bytes, the whole file)

> Мы очень часто сталкиваемся с той причиной, что дискету заживало в сервере

Rendered: *"We very often run into the problem that the floppy gets jammed/stuck in the server."*
This is the same report already archived as `PENDING-20260904-003`.

**Report 4 — `report.txt`**

> The client flips a light switch: three "light: sent ON/OFF" requests logged in 3 s, zero visible effect. The switch's key ('BqgD3X…') appears 0 times in the host's log, and the host's retry queue reports the message dying unapplied ("applied 0 deferred, dropped 1 expired").
>
> Same shape as [our door bug](https://discordapp.com/channels/1508397685726646272/1546182816767938710): the light and lightgroup indexes rebuild with different keysHash on each peer after world load (42 instances; 4 different hashes over 2 loads), so hash-keyed switches are unaddressable cross-peer. The join snapshot suffers too — right after connect the client dropped 27 expired light applies. Stable-named switches ('lightswitch_outside') work.
>
> Not provable from logs: the reverse direction (host→client) per-message for this switch; whether a louder WARN exists but is disabled.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 21:49:28–21:51:57. Full report with log excerpts — see attached file.

**Report 5 — `report.txt`**

> We recorded a signal to a disc (indicator green on both peers), then passed the disc around: taking it in hand turned the host's indicator red; after the host picked it up, the client's went red too; re-inserting it into the console left the console EMPTY on the client's screen while the host sees the disc inside.
>
> Log-proven chain: the payload applied to the disc's world actor (eid=3517) never follows the prop — every hand transition destroys the world-actor generation, and every put-down re-creates the disc as a new generation (eid drift 3517 → 5279 → 5280) with empty state. The final insert rebinds client-side (eid=5280) but is wire-destroyed the same second; the host's payload apply expires ("actor never resolved") and the disc's pose stream matches nothing ("no local match" ~13+/s).
>
> Not provable from logs: where the payload physically lives; whether per-generation binding is intentional.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:01:02–22:01:44. Full report with log excerpts — see attached file.

**Report 6 — `report.txt`** (English only)

> The client hammers a small container open: it spills 12 items client-side, and the host receives only a bare keyed destroy (eid=0) — the box vanishes in her world, but the loot never spawns there (no container_contents shipped, no PropSpawn; the mod's own log says "a client authors no PropSpawn").
>
> Addendum: the loot DOES reach the host later — but strictly one item per client pickup, ferried through the held-item broadcast (13 crossings in ~1 min, eids 42703–42715; each grab triggered a host-side OnSpawn, proving the items were host-unknown until touched). Untouched items never reach the host.
>
> Not provable from logs: the native loot-spill logic itself (only its client-side outcome — the re-seed count — is visible); whether this authority split is intended.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:07:36–22:08:55. Full report with log excerpts — see attached file.

**Report 7 — `Report.txt`** (note the capitalised filename)

> While handling items (e.g. ferrying container loot), the client's E-presses sometimes get hijacked: a trash clump appears in hand IN ADDITION to the held item ("out of nowhere", even on a light switch), the aimed-at object's native use is cancelled, and the next E throws the clump — it self-re-piles into a brand-new trash pile that persists in the world for both peers. Three new piles appeared in ~90 s; 33+ hijacks across the session (bursts: 5 presses in 5 s, 11 in 53 s).
>
> The mod's own log admits it: "[GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy (camera-ray cone) -> native use CANCELLED (no use_deny)" and "paired E-release CANCELLED (its press was intercepted)". The cone matched a pile proxy ~3.1 m from the player's hands while the cursor was on another object; dual carry (item + clump) is log-proven.
>
> Not provable from logs: the exact source of one switch-adjacent clump (the press was clean; nearest clump traffic 27–43 s later); any host-side hijack incident; whether the cone has a reach bound.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:09:04–22:11:43 and 23:24:53–23:44:56. Full report with log excerpts — see attached file.

**Report 8 — `report.txt`**

> The blackout event synced correctly (mask=0x00 + 11 lightgroup OFF applies), and the host's breaker restoration synced too — every mask bit applied ok=1, per-room lights came back on the client's screen, breaker states showed ON. But the client's main-room consoles never came back: off screens, interactions dead, the main computer won't boot (player-reported, consistent with logs — E-presses produce only bare use lines with no mod subsystem answering). The host's consoles work (player-reported). Only the processing panel works on the client.
>
> The mod's keyed-subsystem index has no console/computer subsystem at all — console power state is not synced, and the mask value lands client-side without the native restoration chain that boots consoles on the host. Our "state syncs, native event effects don't" class (see also [our container-loot report](…)).
>
> Not provable from logs: the native restoration chain itself (console power is unlogged on both sides); whether the client's consoles ever recovered.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:14:46–22:22:13. Full report with log excerpts — see attached file.

**Report 9 — `report.txt`**

> The client lit an explosive barrel at the TR1 depot: a chain reaction destroyed 11 keyed props (2 barrels, 4 gas cans, a box, 2 fire extinguishers, +2), knocked out the transformer (power mask 0x00 shipped client→host, applied ok=1) and set it on fire — all of this ONLY in the client's world. The host, 18 m from the depot, saw nothing: her log has zero explosion/fire/damage events, and she later ate an MRE lying in the client's burn zone.
>
> Every destroyed prop reached her only as a bare keyed destroy (eid=0), applied as a silent despawn — no explosion FX, no chain reaction, no loot spill ever ran on her side. Fire and transformer state have no replication path at all (the keyed subsystem index has no fire/device-state entry; the transformer exists to the mod only as a screen-UI class).
>
> Not provable from logs: the ignition moment (derived window); whether the mask apply visibly blacked out the host's empty base; what the 2 unknown keyed props were.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:34:52–22:36:09. Full report with log excerpts — see attached file.

**Report 10 — `report.txt`**

> The client's character died — and instead of the mod's intended "coop death keeps the world", his game traveled to the main menu and the live session was torn down as a quit-to-menu ("gameplay->MENU while a session is live" — log-proven). The host's native world kept running, but two findings aggravate it:
>
> 1. Asymmetric death handling: 2 s later the host also died — her death was armed, the level travel REFUSED, revive OK (hp=99.6, log-proven). The client's death produced zero death_revive trace: no arming, no refusal — the native death ran un-intercepted into the menu.
> 2. The orphaned host is unjoinable: the lobby stayed listed (stale), but the P2P listener died with the session. Both join attempts burned ~10 s timeouts without a single trace in the host's log. Only a quit-to-menu + re-host (s_Coop 3) rebuilt the listener; the next attempt connected within seconds.
>
> Not provable from logs: what killed the client; whether the death skips the vetoed seam entirely or death_revive arming simply doesn't run client-side; why the master-server lobby listing survived the listener's death.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-04, 22:44:09–22:48:28. Full report with log excerpts — see attached file.

The Russian half of Report 10 ends with a line that has no English counterpart:

> Вообще, кажется, видел сообщения о подобном баге, и даже где-то на гите видел, что он исправлен, но версия с исправлением ещё не выпущена. Возможно, баг не актуален

Rendered: *"Actually, I think I've seen reports of a similar bug, and I even saw somewhere on git
that it's fixed, but the version with the fix hasn't been released yet. Possibly the bug isn't
current."* **Recorded, not credited.** No such fix was looked for by this intake, and no network
access was used.

**Report 11 — `report.txt`**

> After rejoining the re-hosted session (right after the death-tear incident), the client's character model stood frozen at the spawn point on the host's screen for the entire 5-minute session — while he ran freely, and the host watched objects "levitate" and fly around the base with nobody near them. The logs show a clean split: the host's slot-1 pose pipeline applied a few poses in the first ~2 s, then zero for 5 min 07 s — while the transport fully recovered (the 4.1 MB backlog drained 23 s after join, then a perfectly healthy link), the client demonstrably sent ~60 pose packets/s, and client-origin prop traffic was applied byte-for-byte. His bucket throws crossed with exact velocity vectors — hence the "levitation": held items mirrored into the frozen puppet's view, throws applied by an invisible hand, and the save-position corrector snapping the props back.
>
> The control join 4 minutes later (same in-process re-host, same client) was healthy from the first second — a state/race specific to the first join after a host-death tear, not a general re-hosting property. The session was unplayable as co-op and ended in a full manual restart of both games.
>
> Not provable from logs: the internal state of the pose consumer — no receive/drop/reject lines exist; the fresh=0/s counter is the only witness.
>
> Setup: Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1. Logs 2026-09-05, 00:07:17–00:12:28. Full report with log excerpts — see attached file.

### 3.3 The sixteen screenshots, transcribed

Each was read as an image. Author, in-image timestamp and message order are preserved. Timestamps
are in the capturing client's own zone. **No capture states a build version.**

---

**`Screenshot 2026-09-04 110239.png`** → `PENDING-20260909-048` · Discord forum post, tagged only
with the default post icon, dated **4 September 2026**, marked `NEW`. Title: **"Cleaning Surfaces
with water is not synced"**.

- **5:42 AM · McJordan447 (OP)** — "When one of the players cleans a surface, even if it's the host, it doesn't sync up to other players."

Footer shows "React to Post" / "Follow"; the reply area reads "Start the conversation! Be the
first to share what you think!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-04 110249.png`** → `PENDING-20260909-049` · Discord forum post, tagged
`bug`, dated **4 September 2026**, marked `NEW`. Title: **"i always gets 2 or 9 fps max!"**.

- **6:40 AM · Felipe (OP)** — "i only get like a couple fps when im not the one hosting, for some reason, how do i fix it?"

Reaction row visible (a count of 2, then three badge reactions at 1 each). "Start the
conversation!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-04 110302.png`** → `PENDING-20260909-050` · Discord forum post, tagged
`bug`, dated **4 September 2026**, marked `NEW`. Title: **"Can't interact with bottom 2
servers"**. Author badge `COP9`.

- **10:36 AM · Bear Bear (OP)** — "I don't know what really is causing this but for some reason i can not interact/click on any server that's at the bottom of the ui"
- **10:52 AM · Bear Bear (OP)** *(edited)* — "Update i have seem to have found out why, the screen resolution was messing with it. I'm on a 2560x1600 monitor and for some reason the ui can't be reached from the resolution"
- **Bear Bear (OP)** — "it also seems 2560x1440 can not reach it as well"
- **Bear Bear (OP)** *(edited)* — "When I go to default resolution it seems to be fine, seems the mod ui can not handle anything above 1080p"

Not truncated; a further element begins below the visible crop.

---

**`Screenshot 2026-09-04 121754.png`** → `PENDING-20260909-051` · The **same thread** as
`-049`, captured later.

- **6:40 AM · Felipe (OP)** — "i only get like a couple fps when im not the one hosting, for some reason, how do i fix it?"
- **12:11 PM · actuallyafish** *(marked NEW)* — "apparently this is due to every single item on the map being duplicated for non-host players?"

The message composer is visible at the bottom; not truncated.

---

**`Screenshot 2026-09-04 121811.png`** → `PENDING-20260909-052` · Discord forum post, **4
September 2026**, marked `NEW`, `(edited)`. Title: **"Some bugs I've found"**. Author shown as
**Q** (OP).

- **11:20 AM · Q (OP)**, ten numbered items:
  1. "Client dragging around boxes causes the trash around the area to duplicate when the host cannot see this happening and isn't duping for the host"
  2. "Sometimes host or client cannot send the drone away"
  3. "Floppy Disk & Drive either disappear or get duplicated when ejected"
  4. "Sometimes when the client picks up a trash pile it gets stuck in their hands"
  5. "On occasion when going into a satellite room the terminal says \"Root\" on the top right just like the one at Alpha base but backing out of the terminal and back in seems to fix it sometimes."
  6. "When client is shoveling the trash to gather scraps, The host cannot see them doing this. and the scrap doesnt appear on the ground when they do this"
  7. "Scrap box can be de-synced sometimes."
  8. "After being in the game for a while with client the doors remain open and will not close unless host or client closes it."
  9. "Host must pick up the footlong sandwich for client to eat it, Same for fire extinguisher for the client to use it. And host and client cannot see either use the fire extinguisher."
  10. "When host saves then reloaded the lobby the client will have the same items as hosts and are shared and when client drops them they vanish."
- closing line — "As for mods, \"SpinnyChairs\", \"The Lad\", \"SlotMachine\", Are all de-synced between host and client"

Complete; the post ends with the reaction bar. Not truncated.

---

**`Screenshot 2026-09-05 063921.png`** → `PENDING-20260909-053` · A narrow chat crop, three lines
only, no thread title, no date divider.

- *(quoted message being replied to)* **@SeaTurtleMan** — "i think you change world settings by loading into the world in singleplayer and changing the settings how you normally would in vanilla"
- **6:37 AM · chap** *(VotV badge)* — "i tried this, the settings dont carry over it only applies to singleplayer mode"
- **chap** *(VotV badge)* — "is there anyway to edit the sav file that multivoid uses to change the settings"

Crop is tight; there may be surrounding conversation that is **not captured**.

---

**`Screenshot 2026-09-05 131410.png`** → `PENDING-20260909-054` · **GitHub issue capture.** Title
**"6 bugs (not sure if they are critical or not)"**, issue number **#14**, state **Open**, opened
by **BlueBerrySans365** "5h ago", "Last edited by BlueBerrySans365". The repository name is **not
visible in the crop**.

1. "Host sees other clients flashlights flickering"
2. "Sometimes clients see duplicated objects that only client see but not host"
3. "Clients somehow grabbing garbage and sometimes objects through the wall or when tries to interact with something on the base"
4. "When Client turns off the generator - it turns off the light, but host haven't seen the changes of this"
5. "Points somehow don't syncing up correctly"
6. "Clients can't call a drone, only host"

> "UPD: Looked into my notes"

7. "ATV is lagging when somebody rides it"

No comments are visible. Not truncated.

---

**`Screenshot 2026-09-05 181159.png`** → `PENDING-20260909-055` · Discord forum post, **5
September 2026**, marked `NEW`. Title: **"Data sync issue"**. Author **BitorBit#6268** (OP).

- **6:00 PM · BitorBit#6268 (OP)** — a heading and a bare list:
  - "Upgrades"
  - "cords"
  - "shop queue not reducing"
  - "picking up trash trough walls"
  - "beeing able to pick up multiple objects at once"
  - "any other data change that isnt object position like"
  - "opening a container and putting drives in it, including the drive rack"
  - "satelite doors cant be opened"
  - "some other doors in main facility cant be opened"
- **6:01 PM · BitorBit#6268 (OP)** — "i hope it gets fixed quickly so us 3 dont have those problems anymore"
- **BitorBit#6268 (OP)** — "basicly, only thing that works is positioning of objects, but not their data"

The list's punctuation is the author's; "any other data change that isnt object position like" is
transcribed exactly as written and reads as an incomplete sentence in the source, not a
transcription loss.

---

**`Screenshot 2026-09-06 010331.png`** → `PENDING-20260909-056` · Discord forum post, **6
September 2026**, marked `NEW`. Title: **"New bug when entering game"**.

- **12:41 AM · Nicoszoki (OP)** — "I tried entering my friend's game and my fps dropped to 2.5 and then started getting better, but then some crates on the basement duped and my friend said I was noclipping them and moving them effortlessly"

No replies visible. Not truncated.

---

**`Screenshot 2026-09-06 034445.png`** → `PENDING-20260909-057` · Discord forum post, tagged
`bug`, **6 September 2026**, marked `NEW`. Title: **"More on floppy/zip discs"**.

- **2:34 AM · chocolite (OP)** — "I was able to get the floppies to stop disappearing by spamming R to pick up the disks as soon as possible, but once you move the data to a zip disk, it turns into \"INVALID_DATA_BLOCK\" and the tasks are not able to be completed. I have also found that you can more reliably get floppies/zips back by turning the computer/zip drive on its side."

One reaction (count 1). "Start the conversation!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-06 034454-EXPLOSIONS.png .png`** → `PENDING-20260909-058` · Discord forum
post, tagged `bug`, **6 September 2026**, marked `NEW`. Title: **"Explosions close the session"**
(the title row is cut at the top edge of the crop but is fully legible).

- **3:27 AM · Eggy (OP)** *(VotV badge)* — "Explosions created by various sources like the Abandoned Village lightning strikes or explosive barrels cause a black screen and eventually boots everyone onto the main screen"

An **embedded video** follows with a play button. It is **not viewable by this intake**. Its
still frame shows a dark, foggy forest with a structure at the right, a hotbar of empty slots, an
inventory-weight readout, the text **"Saved..."** at centre, and a notification reading **"Saved
game to the main slot: s_Coop"**. Note the supplied filename carries a doubled extension:
`...EXPLOSIONS.png .png`.

---

**`Screenshot 2026-09-06 034600.png`** → `PENDING-20260909-059` · Discord forum post, tagged
`items`, **6 September 2026**, marked `NEW`. Title: **"inventory breaks after rejoining"**.

- **3:38 AM · Aiker228 (OP)** — "after rejoining the server everyone get host's items and those items may disappear after dropping them (even when host drops them)"

"Start the conversation!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-06 052818.png`** → `PENDING-20260909-060` · Discord forum post, tagged
`bug`, **6 September 2026**, marked `NEW`. Title: **"When someone joins it resets what you were
downloading (using the satellites)"**.

- **5:07 AM · Bear Bear (OP)** *(COP9 badge)* — "If someone joins the game when you are using the satellites or downloading something it resets it"

"Start the conversation!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-06 052830.png`** → `PENDING-20260909-061` · Discord forum post, tagged
`bug` and `item dupe`, **6 September 2026**, marked `NEW`. Title: **"Various Sync Issues"**.
Author **Clark The 1st** (OP).

- **5:15 AM · Clark The 1st (OP)**, twelve numbered items:
  1. "Other players inventories get replaced  by what the host has"
  2. "Host picking up duped items (shovel in this case) bricks the inventory upon using it. You cant open it with tab, or scroll through the hotbar"
  3. "Interacting with garbage piles duplicates it for other clients, and only the host can use the shovel on garbage. If a client shovels garbage it dissapears for everyone else and cant be interacted with."
  4. "Loot dropped by clients breaking supply boxes, disappears upon the save loading next time."
  5. "Some clients need to touch a keypad (not putting in a code) to open the door. Other doors in base worked fine."
  6. "Clients could not open transformer or satellite doors."
  7. "Frequent desync issues, where one player is invisible to others but visible to others."
  8. "Desynced players can be heard globally instead of proximally."
  9. "Only the host can send the drone away."
  10. "Even items being held in hand, not the inventory would be replaced."
  11. "Other clients copy the hosts hunger, stamina, and battery levels upon joining."
  12. "Desynced clients can use the atv, visually making it seem like its driving itself."
- closing line — "All of the above happened in a full four player lobby."

"Start the conversation!" — **no replies at capture time**. Not truncated.

---

**`Screenshot 2026-09-06 052842.png`** → `PENDING-20260909-062` · Discord forum post, tagged
`bug`, **6 September 2026**, marked `NEW`. Title: **"Remote player"**.

- **5:25 AM · Bear Bear (OP)** *(COP9 badge)*, *(edited)* — "every time someone leaves the lobby or fails to join a \"remote player\" it put in their place until someone takes that slot its getting weird. also it puts that player in the place of remote player and you can't see what the other person does anymore (movement) i think them grabbing things still works idk tho"

Transcribed exactly as written; the first sentence is garbled in the source. Not truncated.

---

**`Screenshot 2026-09-07 002454.png`** → `PENDING-20260909-063` · A narrow chat crop, two lines,
no thread title, no date divider.

- *(quoted message being replied to)* **Stonesword** *(GORP badge)*, *(edited)* — "Does anyone know any fixes for the insanely low FPS on client issue?"
- **8:50 AM · Stonesword** *(GORP badge)* — "Figured out this is because most physics objects are being duplicated on clients"

Crop is tight; surrounding conversation is **not captured**.

### 3.4 Videos — all UNVIEWED

This intake cannot decode or view video. Thirteen video files are recorded by name, size and
sha256 only, and **no claim in this ledger rests on any of them**.

| PENDING_ID | File | Size (bytes) | sha256 |
|---|---|---|---|
| PENDING-20260909-002 | `Report 1/bug_01_spurious_signal_catch_on_connect.mp4` | 12,345,598 | `24b8436fdcdacc56d0742feae251d5f9cf47dedd65f00e508f05b8a0fe8ec180` |
| PENDING-20260909-006 | `Report 10/bug_10_client_death_quits_to_menu_host_unjoinable_compressed.mp4` | 17,061,113 | `77bcbc03ca55bcb8ccfeea1b267c7fbc67de87032672a54410ceb420ab030a1c` |
| PENDING-20260909-010 | `Report 11/bug_15_..._client_compressed.mp4` | 17,238,000 | `b4b6ec760c5f1992900e440285b5a3227926be21077fb0d8aaf0ba057da3892c` |
| PENDING-20260909-011 | `Report 11/bug_15_..._host_compressed.mp4` | 17,570,844 | `d1aa3c33d21e7ce8eb6b01fe9e5d61e3e0d6631b72d0a76d1006e8df7b004b70` |
| PENDING-20260909-015 | `Report 2/bug_02_door_key_mismatch_client_cannot_open.mp4` | 5,532,235 | `159d9071781adadb234a0767e5c2738217445106a90fb5348512ef8c4f421245` |
| PENDING-20260909-022 | `Report 3/ну то самое.mov` | 6,460,549 | `0230fe2947ee50f425411e42fbf06d8b2c71f321012a08e18b1f7dd2f4e61afc` |
| PENDING-20260909-024 | `Report 4/bug_04_light_switch_client.mp4` | 4,456,129 | `08f00528c98afc8f34c4cd7e5bba5ebcc562e689302c090ca6147226834a21ac` |
| PENDING-20260909-028 | `Report 5/bug_05_disc_signal_lost_hand_transitions.mp4` | 18,761,626 | `1ea0cd8d66372ba7b929a5e818be7551325dfc6c1ac86da14b4419331fcff303` |
| PENDING-20260909-032 | `Report 6/bug_06_client_broken_container_loot_invisible_to_host.mp4` | 8,301,697 | `16a8d6b7d5eb2a8952990a2dd680db979440621549046f5869693366204c3d66` |
| PENDING-20260909-037 | `Report 7/bug_07_pile_grab_intent_..._trash_spawns.mp4` | 13,175,072 | `154c37ac1be89cb9f37bc1d0208e3845dc8658fc88da59b6916761701d3bb369` |
| PENDING-20260909-040 | `Report 8/bug_08_consoles_off_after_power_restore_client_compressed.mp4` | 18,550,624 | `3048013447ee924731004a108b1b7b1ff622ab2a51cc822e40793c39b18633a0` |
| PENDING-20260909-045 | `Report 9/bug_09_barrel_explosion_chain_tr1_client_only.mp4` | 15,170,417 | `0dc4d6720b0a2bf760accf6a6e096c7d547d3dd1c600ff94e0286895d7cca3c0` |
| PENDING-20260909-064 | `Votv 2026.09.05 - 19.11.18.14.DVR- EXPLOSIONS - Trim.mp4` (loose) | 9,549,709 | `dc282a253e589afd8c6dfe5de5ac000bfaec8ef46e0ea824a995058591c9c942` |

`PENDING-20260909-064` is associated with the "Explosions close the session" capture
(`-058`) **by filename token only**. Its filesystem modification time falls 14 seconds after that
capture's, which is consistent with the two having been saved together and is **not** proof of a
common source. Whether it is that post's embedded video or the submitter's own recording is
UNKNOWN and is not asserted.

The reporter's video windows (e.g. Bug 03 at `00:02:44`–`00:02:53`, Bug 10 at `00:55:46`
onward) are recorded from the md metadata tables and are **the reporter's own timings**, not
observations of the files.

---

## 4. Classification against the existing ledger

### 4.1 What each of the eleven structured reports does to the reviewed ledger

| Report | Category | Ledger relation | What is new |
|---|---|---|---|
| 1 · spurious signal catch on connect | `NEW_BUG_CANDIDATE` | Not in BUG-001..020. Nearest is `PENDING-20260904-002f` (a suspected desync when a signal is newly fixed) — **cross-referenced, NOT merged**: 002f is a queued signal diverging, this is a *fabricated* catch event at the connect edge. | The connect window as a distinct failure surface; a claimed both-peer temporal coincidence between the join-time slew of all 24 dishes and a published catch; two occurrences with a stated control (later mid-game catches were intentional). |
| 2 · door key mismatch | `NEW_FINDING_FOR_EXISTING_BUG` → **BUG-012** | BUG-012 ("confirmed client cannot open satellite doors") is symptom-only. This is the first mechanism. Also touches BUG-014 and `PENDING-20260903-003a`. | A **mechanism** (per-peer `keysHash` instability across world load) and a **scope** (the 31 world-load-added doors fail; the 19 named base doors sync at every join). That is a discriminator BUG-012 never had, and it yields a log-only falsifier that needs no gameplay: compare the two peers' door-index `keysHash` after world load. |
| 3 · floppy dissolves | `NEW_FINDING_FOR_EXISTING_BUG` → **BUG-018** | BUG-018 and four pending floppy corroborations (`PENDING-20260904-002a/003a/005a/005b`) are symptom-only. | A complete claimed causal chain, and a **generic, non-floppy** mechanism: keys are not instance-unique, destroy traffic carries `eid=0`, and a deferred destroy-by-key armed for the load window fires in steady state against a later spawn that reused the key. **This is the same shape as the programme's own incidental observation `OBS-R3Q-C01`** (host broadcasts DESTROY with `eid=0` and the box's logical key; the client resolves by key and destroys the just-placed mirror; the host has a guard, the client does not). Cross-referenced, **NOT merged** — asserting one mechanism needs source work this intake did not do. |
| 4 · light switch | `NEW_BUG_CANDIDATE` | Not in BUG-001..020 as a report. Same root class as Report 2 — **cross-referenced, NOT merged**, because it is the same instability shown independently on a second keyed index family. | The instability generalised beyond doors: 42 light/lightgroup instances, 4 hashes over 2 loads, plus a **join-snapshot** casualty count (27 expired light applies dropped right after connect). |
| 5 · disc signal lost | `NEW_FINDING_FOR_EXISTING_BUG` → **BUG-005** | BUG-005 records an intermittently invisible drive in the processor slot that "suddenly works again for an indiscriminate reason". The R2 audit's stated next step was to classify the failing object as native / remote mirror / hand mirror. | This answers that classification: the world actor is **destroyed and re-created as a new generation** at every hand transition, with the advertised eid drifting 3517 → 5279 → 5280 — which is exactly why the audit's eid-only `drive_sync` apply parks and then expires ("actor never resolved"). Also opens `-027a`: the recorded payload does not follow the item, cross-referenced to **MVF-0001** and **MVF-0004** and not merged. |
| 6 · client-broken container loot | `DUPLICATE_OR_CORROBORATION` → **BUG-016** | An exact match for BUG-016 ("Supply boxes don't spawn items for the host when client breaks them"), also matching `PENDING-20260904-006a`. | First named container class (`prop_container_sbox_C` `'wbox'`, key `'Z8bHmBthD7oLzVOS0c-WKw'`, eid 3924), first item count (12), first claim that **neither** `container_contents` **nor** `PropSpawn` is emitted, and a **partial recovery route**: the held-item broadcast ferries exactly one item per client pickup (13 crossings, eids 42703–42715). Against the R2 audit's six-part confirmation requirement for BUG-016, this supplies class, spill, admission and recoverability and leaves backing and expression open. |
| 7 · pile grab-intent hijack | `DUPLICATE_OR_CORROBORATION` → `PENDING-20260904-002h` / `-006d` | Those two record only that a guest sometimes grabs a nearby pile instead of the intended object. Also touches BUG-013 / BUG-015. | The named mechanism (a camera-ray cone matching a pile proxy), the mod's own cancellation line, a cone distance (~3.1 m), a frequency (33+), and a **consequence never recorded before**: the hijacked clump, once thrown, nativises into a **new world pile that persists for both peers**. That is world-state growth, not a lost input. |
| 8 · consoles dead after power restore | `NEW_BUG_CANDIDATE` | Not in BUG-001..020 as such. Cross-referenced to BUG-019 (network-room terminal not synced) and **NOT merged**: BUG-019 is command state, this is console **power expression** after a world power event. | A clean positive control inside one report: the mask and the lights **do** cross correctly, and only the native chain fails — which is the sharpest statement in the drop of the reporter's "state syncs, native event effects don't" class. |
| 9 · barrel explosion chain | `NEW_BUG_CANDIDATE` | Not in BUG-001..020. Same abstract class as Report 6 at larger scale. Distinct from `PENDING-BUG-022` (a client-placed barrel absent for the host), which is a **birth** failure; this is a **death-effects** failure — cross-referenced, NOT merged. | Scale (11 keyed props in one event), a second missing effect family (**fire**), an asserted structural absence (no fire or device-state entry in the keyed subsystem index; the transformer known to the mod only as a screen-UI class), and a mixed outcome — the power mask **did** cross client→host with `ok=1` while nothing else did. |
| 10 · client death tears the session | `NEW_BUG_CANDIDATE` | Not in BUG-001..020. | Two sub-candidates: `-005a` the teardown itself, and `-005b` the orphaned-but-listed host. The report contains its own **asymmetry control**: the host's death 2 s later was armed and vetoed correctly, so the veto mechanism exists and simply did not run on the client. |
| 11 · frozen puppet after rejoin | `NEW_BUG_CANDIDATE` | Not in BUG-001..020. Corroborates `PENDING-20260902-002b` (player models disappear). | A clean split — pose path dead, transport healthy, prop path applying byte-for-byte — plus a **control join** 4 minutes later that was healthy, which narrows the claim from "re-hosting is broken" to "the first join after a host-death tear is". Reporter numbering gap: this is his **Bug 15**; his Bugs 11–14 are absent from the drop (see §6). |

### 4.2 What the loose captures do

Full per-item dispositions are recorded in the `INDEX.jsonl` rows. The consequential ones:

- **`-050` UI unreachable above 1080p** — the only UI-layout defect anywhere in the ledger or the
  pending set, and the only report in this drop with a self-diagnosed, directly testable
  condition: 2560x1600 and 2560x1440 fail, default resolution works. "Anything above 1080p" is
  the reporter's extrapolation from two points, not a measured boundary.
- **`-057b` `INVALID_DATA_BLOCK`** — the only report in the whole drop that quotes a concrete
  in-game failure token and states a **progression block** ("the tasks are not able to be
  completed"). New candidate; cross-referenced to Report 5 and not merged.
- **`-061` item 4, supply-box loot lost across save load** — `NEW_FINDING_FOR_EXISTING_BUG` →
  **BUG-016**. Every prior BUG-016 record, Report 6 included, stops at the live session. The R2
  audit explicitly left BUG-016's persistence and recoverability unproven and **forbade**
  asserting permanent loss. This is REPORTED evidence toward that open question and **does not
  settle it**.
- **`-061` item 11, vitals copied from the host at join** — new candidate. **Repro warning:** this
  programme's own `vitals_keepalive` / `RestoreVitals` developer tool mutates hunger/sleep/health
  and broadcasts a restore to the connected peer. It must be inactive for the entire duration of
  any run testing this report, or the run measures the tool instead of the defect.
- **`-053` world settings** — `DUPLICATE_OR_CORROBORATION` → `PENDING-20260902-004a`, but it
  **contradicts** the workaround recorded there: `PENDING-20260902-004` has a second reporter
  confirming that the singleplayer-settings route works, while `chap` states it does not carry
  over. The contradiction is recorded, **not adjudicated**; neither claim was reproduced and the
  two reporters may have performed different sequences.
- **`-052` item 2 vs `-061` item 9, drone send** — a second contradiction: `-052` says "sometimes
  host *or* client cannot send the drone away", `-061` says "only the host can send the drone
  away". Recorded, not adjudicated.
- **`-054` item 1** — an independent second reporter for **BUG-009** (flashlight flicker), which
  was FIRSTHAND-only in the reviewed ledger. It drops BUG-009's stated precondition ("when client
  runs ahead of host"), so it neither confirms nor removes that precondition.
- **`-052` closing line, third-party mods** — `NON_BUG`. Third-party mod content is outside the
  audited surface; nothing in the Multivoid tree claims to replicate other mods' actors. Quoted so
  it is not silently dropped; not promoted.
- **`-052` item 7 ("Scrap box can be de-synced sometimes")** and **`-054` item 5 ("Points somehow
  don't syncing up correctly")** — `INSUFFICIENT` / lowest actionability. No peer, symptom,
  trigger or recovery is stated, and "points" is not disambiguated.
- **`FEEDBACK` rows**: `-055`'s "basicly, only thing that works is positioning of objects, but not
  their data" and "i hope it gets fixed quickly so us 3 dont have those problems anymore" (which
  is also a three-player topology datum); `-057`'s orientation workaround ("turning the
  computer/zip drive on its side").
- **`EXTERNAL_WORK_POINTER`**: **none in this drop.** The category is listed for completeness; the
  last such pointer was `PENDING-20260904-001`.

---

## 5. Theme map

Twelve cross-cutting themes. "Strongest evidence" names the single item that most constrains the
next investigation — in every case still `REPORTED`, never measured by this programme.

### T1 — Per-peer key instability for world-load-added keyed actors
Both peers rebuild the same keyed world actors under **different keys** after world load, so
keyed traffic is unaddressable across the peer boundary. Named-instance actors are unaffected;
world-load-**added** instances are.

- **Items:** `-014`/`-017` (Bug 02, doors), `-023`/`-026` (Bug 04, lights and lightgroups),
  `-052` item 8, `-055` (satellite + "some other doors in main facility"), `-061` items 5 and 6,
  `-054` item 4.
- **Existing ledger:** BUG-012, BUG-014, `PENDING-20260904-002b`, `PENDING-20260903-003a`,
  `PENDING-20260902-001b`.
- **Strongest evidence:** **`PENDING-20260909-014`** (`bug_02_door_key_mismatch_client_cannot_open.md`)
  — it names the failing check by the mod's own wording, gives the two divergent keys for one
  door, and splits the population (19 named base doors sync; 31 world-load-added doors do not),
  which is what makes it falsifiable from logs alone.

### T2 — Key/eid non-uniqueness with deferred destroy-by-key
Destroy traffic identified by key with `eid=0` is ambiguous across same-keyed live instances, and
a destroy deferred for the load window can fire in steady state against an unrelated later spawn.

- **Items:** `-018` (Bug 03), `-021`, `-020`, `-057` (the spam-R timing workaround), `-052` item 3.
- **Existing ledger:** BUG-018, `PENDING-20260904-002a/003a/005a/005b`; **and the programme's own
  `OBS-R3Q-C01`**, which is the same shape from the other direction.
- **Strongest evidence:** **`PENDING-20260909-018`**
  (`bug_03_floppy_dissolves_deferred_destroy_key_collision.md`) — the only item in the drop that
  claims a complete, quoted, both-peer causal chain plus contributing evidence that the collision
  is peer-symmetric.

### T3 — Client-authored native effects not replicated ("state syncs, effects don't")
A client action's *state delta* crosses the wire while the native chain that produces the visible
and material consequences never runs on the other peer.

- **Items:** `-031` (Bug 06, container loot), `-044` (Bug 09, explosion chain), `-039` (Bug 08,
  console restoration), `-052` item 6 (shovelling produces no scrap for the host), `-061` item 3,
  `-052` item 9 (extinguisher use unseen by both).
- **Existing ledger:** BUG-016, `PENDING-20260904-006a/006f`.
- **Strongest evidence:** **`PENDING-20260909-031`**
  (`bug_06_client_broken_container_loot_invisible_to_host.md`) — it is the only one with a named
  class and key, a count, an explicit "neither payload was emitted" claim, **and** a measured
  partial recovery route (13 ferried items with per-item eids) that separates "the items do not
  exist for the host" from "the items exist but are not expressed".

### T4 — Death and explosion → session teardown
A local event that should be survivable ends the co-op session, or the host survives but becomes
unreachable.

- **Items:** `-005` (Bug 10), `-058` (explosions boot everyone), `-064` (unviewed video),
  `-009` (Bug 15 is the *sequel* to a tear and depends on this theme's precondition).
- **Existing ledger:** nothing. Entirely new.
- **Strongest evidence:** **`PENDING-20260909-005`**
  (`bug_10_client_death_quits_to_menu_host_unjoinable.md`) — it carries its own asymmetry control
  (the host's death 2 s later was armed and vetoed correctly), which turns "death is broken" into
  the much narrower "the veto exists and did not arm on the client".

### T5 — Presence / pose pipeline death and slot leakage
The pose consumer for one peer stops while every other channel for that same peer stays healthy,
so the player acts on the world from a body nobody can see.

- **Items:** `-009` (Bug 15), `-062` (leaked "remote player" placeholder), `-061` items 7, 8 and
  12, `-054` item 2 in part.
- **Existing ledger:** `PENDING-20260902-002b`; BUG-010 only loosely (that is model corruption,
  not pose).
- **Strongest evidence:** **`PENDING-20260909-009`**
  (`bug_15_puppet_frozen_at_spawn_after_rejoin.md`) — the only item with a stated **control**
  (a later join, same re-host, same client, healthy from the first second) and an explicit
  ruling-out of the transport. `-062` is its best external corroboration because it independently
  reports the same split: movement invisible, grabbing still working.

### T6 — Inventory replaced by the host's after rejoin
After a rejoin or a host save/reload, non-host peers' inventories are replaced with the host's
contents, and the copied items are fragile on drop.

- **Items:** `-059` (Aiker228), `-052` item 10 (Q), `-061` items 1, 2 and 10 (Clark The 1st).
- **Existing ledger:** `PENDING-20260903-002e` (inventory wiped on rejoin), `PENDING-BUG-023`
  (cross-player inventory disappearance).
- **Strongest evidence:** **`PENDING-20260909-061`** (`Screenshot 2026-09-06 052830.png`) — it
  extends replacement to the item **held in hand**, which rules out a pure UI-projection
  explanation for that reporter's case, adds the bricked-host-inventory consequence, and states
  the topology (a full four-player lobby). Three independent reporters describing the same shape
  in one drop is itself the theme's weight; `PERMANENT_DATA_LOSS` remains `UNKNOWN`.

### T7 — Client-side object duplication and frame-rate collapse
Non-host peers see duplicated world objects, and several reporters attribute a severe client-side
frame-rate collapse to that duplication.

- **Items:** `-049` (2–9 fps when not hosting), `-051` (duplication hypothesis), `-056` (2.5 fps
  on join, then crates duped, plus noclipping), `-063` ("most physics objects are being
  duplicated on clients"), `-054` item 2, `-052` item 1, `-061` item 3.
- **Existing ledger:** `PENDING-20260902-002a` (performance), `PENDING-20260902-002c` (client
  physics mass), BUG-015 (phantom trash balls) as the narrow ancestor.
- **Strongest evidence:** **`PENDING-20260909-056`** (`Screenshot 2026-09-06 010331.png`) — the
  only item that reports the frame-rate collapse **and** an object duplication **in the same
  episode, in that order, on join**, together with an independent witness (the friend) describing
  the joiner noclipping and moving the duped crates effortlessly. The duplication-causes-FPS link
  itself remains two reporters' hypothesis (`-051`, `-063`); nothing here counts actors.

### T8 — Mod UI unreachable above 1080p
- **Items:** `-050` only.
- **Existing ledger:** nothing.
- **Strongest evidence:** **`PENDING-20260909-050`** (`Screenshot 2026-09-04 110302.png`) — a
  single-reporter theme, but it carries its own experiment: two failing resolutions, one working
  default, and a self-identified cause.

### T9 — Grab-intent capture of player input
The client's use input is intercepted by the trash-pile grab path, cancelling the aimed
interaction and producing carried clumps that become permanent world piles.

- **Items:** `-036`/`-035` (Bug 07), `-052` item 4 (stuck in hands), `-054` item 3 (through
  walls), `-055` ("picking up trash trough walls", "beeing able to pick up multiple objects at
  once"), `-061` item 3.
- **Existing ledger:** `PENDING-20260904-002h`, `-006d`, `-002i`; BUG-013, BUG-015.
- **Strongest evidence:** **`PENDING-20260909-036`**
  (`bug_07_pile_grab_intent_hijacks_item_interaction_trash_spawns.md`) — it quotes the mod
  cancelling the native use, gives the cone distance, counts occurrences, and identifies the
  world-growth consequence. `-055`'s "beeing able to pick up multiple objects at once" is an
  independent player-side description of the dual-carry state the report claims to log-prove, and
  `-054` item 3 / `-055` answer the occlusion half of the report's own open question about the
  cone's bounds.

### T10 — The join edge disturbing the signal / satellite pipeline
- **Items:** `-001` (Bug 01, a catch published at connect), `-060` (a join resets an in-progress
  download).
- **Existing ledger:** `PENDING-20260904-002f`.
- **Strongest evidence:** **`PENDING-20260909-001`**
  (`bug_01_spurious_signal_catch_on_connect.md`) — it names the concrete join-time behaviour (all
  24 dish mirrors slewing at once while the client's tickers are parked) that could plausibly
  disturb an in-progress acquisition. The two items are cross-referenced and **not merged**: one
  is a fabricated event, the other lost progress.

### T11 — Class-specific item data not carried with the actor
Transforms and identity replicate; the class-specific *data* attached to an item does not.

- **Items:** `-027` (Bug 05, recorded disc payload lost across hand transitions), `-055`
  (container + drives + drive rack; "only thing that works is positioning of objects, but not
  their data"), `-057b` (`INVALID_DATA_BLOCK` after a zip transfer), `-052` item 3.
- **Existing ledger:** BUG-017, BUG-005, BUG-018; findings **MVF-0001**, **MVF-0004**, and
  **MVF-0003** for the same shape on food progress.
- **Strongest evidence:** **`PENDING-20260909-027`**
  (`bug_05_disc_signal_lost_hand_transitions.md`) — it traces one identified payload through three
  actor generations by eid and reports the terminal expiry message, which is the most specific
  account of the loss point in the drop.

### T12 — Host-local world and player state applied to everyone, or to no one
Rules and vitals that should be per-peer or world-wide behave as host-local.

- **Items:** `-053` (world settings do not carry over), `-061` item 11 (clients copy the host's
  hunger, stamina and battery at join), `-048` (surface cleaning not synced even from the host).
- **Existing ledger:** `PENDING-20260902-001a` ("the server's rules only apply to the host"),
  `PENDING-20260902-004a`.
- **Strongest evidence:** **`PENDING-20260909-053`** (`Screenshot 2026-09-05 063921.png`) — the
  only item in the theme that engages a previously recorded workaround directly and contradicts
  it, which is what makes it worth resolving before anything is built on that workaround.

---

## 6. GAPS

1. **The reporter's Bugs 11, 12, 13 and 14 are not in this drop.** The eleven folders contain
   `bug_01` … `bug_10` and `bug_15`. Nothing in the drop names bug 11, 12 or 13 at all. **Bug 14
   is named once**, in Report 11's metadata table, as *"Host death tore down the session — the
   death_revive veto silently failed to arm, live client kicked"*, with a Discord permalink. Bug
   14 is therefore the direct precondition of Report 11 and the **host-side counterpart** to
   Report 10's client-side tear — and its report body is absent. **Request bugs 11–14 from the
   reporter before any work on themes T4 or T5.**
2. **Every video is UNVIEWED** (13 files, §3.4). This includes both peer views of Bug 15 and the
   loose EXPLOSIONS clip. No claim here rests on any of them; conversely, anything only a video
   could settle is unsettled.
3. **The log bundle was not opened.** The one shared 7.58 MB zip (11 identical copies) holds five
   logs totalling 70,877,190 bytes uncompressed, including the two role logs
   (`client_040920262246_multivoid.log`, `host_050920260342_multivoid.log`) that every one of the
   eleven reports quotes. **Not extracted, not censused, no quoted line verified.** Every
   "log-proven" claim in §3 and §4 is the reporter's assertion. A line-level verification pass
   over that bundle is the single highest-value follow-up in this drop, and it is cheap: the
   digests are recorded, the reports name exact timestamps and line numbers.
4. **No source verification was performed.** The reporters' root causes (per-peer key derivation,
   deferred destroy-by-key semantics, "the keyed subsystem index has no console entry", "the
   transformer exists to the mod only as a screen-UI class") are **hypotheses against source that
   this intake did not read**. In particular, the striking match between Report 3 and the
   programme's own `OBS-R3Q-C01` is **cross-referenced, not merged**.
5. **Re-submitted material.** `Report 3/multivoid.log` is byte-identical to the archived
   `PENDING-20260904-008`, and `Report 3/report.txt` repeats `PENDING-20260904-003`. Both were
   re-archived under this drop's ids so the drop's file record is complete; the delta audit should
   not count them as new evidence.
6. **`Report 9/New Text Document.txt` is empty** (0 bytes, empty-string digest). If the submitter
   intended content there, it did not survive.
7. **No screenshot states a build.** Only the eleven structured reports state `0.9.0n b150`. Every
   loose capture is therefore build-UNKNOWN, and none can support a claim about b150 specifically
   — the same limit already recorded for `PENDING-20260903-003`.
8. **Time zones are unresolved.** Filesystem times are local-clock values (recorded as
   `modified_filesystem_local` with an explicit note); in-image timestamps are in each capturing
   client's own zone. Nothing here is normalised to UTC except the archive directory name.
9. **Links were not followed.** Nine distinct Discord permalinks and one GitHub issue are quoted
   in the drop. No network access was used, so their targets — including the "similar case" and
   "similar issues" threads the reporter offers as corroboration, and the state of GitHub issue
   #14 since capture — are unread.
10. **GitHub issue #14 postdates the reviewed audit.** The R2 audit enumerated public issues up to
    #13. Issue #14 is recorded here only as a screen capture, not as a live read of the tracker.
11. **Two unresolved contradictions inside the drop**, both recorded and neither adjudicated:
    the world-settings workaround (`-053` vs `PENDING-20260902-004`), and whether the host is ever
    unable to send the drone (`-052` item 2 vs `-061` item 9).
12. **Topology coverage.** `-061` states a **four-player** lobby and several of its items (per-
    observer invisibility, global-instead-of-proximal audio, inventory replacement) are only fully
    expressible with three or more peers. The eleven structured reports are all host + one client.
    The repro-topology warning already recorded for `PENDING-BUG-023` applies to this whole theme
    family, and the local rig has four copies.
13. **`-061` item 11 cannot be tested with the vitals keepalive active.** Named here so the
    constraint is not rediscovered during a run.
14. **Cropped captures.** `-053` and `-063` are tight two- and three-line crops with surrounding
    conversation absent. `-058` contains an embedded video that cannot be played. No capture in
    this drop is truncated mid-sentence.

---

## 7. Session boundary

```
FILES_IN_INBOX_AT_CHECK=64
FILES_PROCESSED=64
FILES_ARCHIVED=64
ARCHIVE_HASHES_VERIFIED_AGAINST_SOURCE=64
HASH_MISMATCHES=0
INDEX_ROWS_APPENDED=64          (PENDING-20260909-001 .. -064)
SCREENSHOTS_TRANSCRIBED=16
REPORT_TEXT_FILES_TRANSCRIBED=12   (11 cover notes + 1 empty file recorded as empty)
VIDEOS_UNVIEWED=13
LOG_BUNDLES_UNEXTRACTED=11 copies of 1 bundle
NEW_PENDING_SUB_IDS=30
EXISTING_BUGS_TOUCHED=BUG-002, BUG-005, BUG-009, BUG-012, BUG-013, BUG-014, BUG-015, BUG-016,
                      BUG-017, BUG-018, BUG-019
EXISTING_PENDING_TOUCHED=PENDING-BUG-022, PENDING-BUG-023, PENDING-20260902-001a, -001b, -002a,
                      -002b, -002c, -004a, PENDING-20260903-002, -002d, -002e, -002f, -003a,
                      PENDING-20260904-002a, -002b, -002e, -002f, -002h, -002i, -002j, -003a,
                      -004a, -005a, -006a, -006f, -008
FINDINGS_TOUCHED=MVF-0001, MVF-0003, MVF-0004
PERMANENT_BUG_NUMBERS_ASSIGNED=0
PENDING_REPORTS_IMPLEMENTED=false
REPOSITORY_MODIFIED=false
INBOX_COPY_DELETED_AFTER_ARCHIVE=false   (the operator deletes after review)
```
