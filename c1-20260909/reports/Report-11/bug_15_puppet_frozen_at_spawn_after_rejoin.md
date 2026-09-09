# Bug 15: Client's puppet frozen at spawn on the host's screen all session — host pose pipeline died in the congested join

## Metadata

| Field | Value |
|---|---|
| Video window (reported) | ~02:19:00→02:24:00 (session C: client joined 02:18:54, quit 02:24:05) |
| Log window | 00:07:17→00:12:28 (`log = video − 02:11:37`) |
| Reporter | client `PlayerNickname2` — his own free movement plus what the **host's screen** showed (host `Mikuzavr` saw a motionless puppet and "levitating" objects) |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | player pose sync (`RemotePlayer` puppet / `pose-diag`) vs the held-item/prop ferry — the pose path died, everything else lived |
| Key state | session C = lobby #3 `b238b592024d039b`, world gen=13 (in-process re-host after [Bug 14 «Host death tore down the session — the death_revive veto silently failed to arm, live client kicked»](https://discordapp.com/channels/1508397685726646272/1546200747803418654/1546209922776309971)); control session D = lobby #4 `98e8b8ef19e80793`, gen=16 |

## Description

Directly after that death-tear incident the host re-hosted (00:06:44) and the client rejoined (accepted 00:07:17, in-game ~00:07:42). From the first seconds of that session until its end, the client's character model on the host's screen stood at the spawn spot and never moved — while the client himself ran freely around the world and interacted with objects, periodically scaring the host with objects that "levitated" or flew across the base with nobody near them. The session was unplayable as co-op; both players quit manually at 00:12:28 to restart (the second disconnect described in the death-tear report's §6).

The logs show a clean split inside the mod: **the pose pipeline for slot 1 applied a handful of poses in the first ~2 seconds and then zero for the remaining 5 minutes**, even though the transport recovered, the client was demonstrably sending poses, and client-origin prop/item traffic kept arriving and being applied the whole time. The next join (session D, 00:14:14) into an identical in-process re-hosted world was healthy immediately — so this is a state/race in the join-recovery path, not a deterministic property of re-hosting.

## Evidence

### 1. The puppet spawned once and froze — the pose timeline (host log, `host_050920260342_multivoid.log`)

- l.101641 [00:07:17] `host accepted client into PENDING 0 (h=0xb5e2d692)`
- l.101667 [00:07:18] `net: first remote pose on slot 1 -> auto-spawning puppet` — **exactly one** arming of the pose feed all session
- l.101700 [00:07:18] `RemotePlayer::Spawn: puppet=000002C8D7F8A060 at (-231,2004,6320) yaw=-149 nick='PlayerNickname2'`
- l.101763 [00:07:20] first `pose-diag[slot 1]: fresh=0/s targetSpeed=400 target=(-1096,1782) puppet=(-1096,1782) trail=0cm` — the target had already been nudged once (a few poses arrived after the spawn, moving it from (-231,2004) to (-1096,1782)), and `fresh=0/s` from then on
- l.107558 [00:12:27] the last pose-diag before the client quit — **still** `fresh=0/s target=(-1096,1782) puppet=(-1096,1782) trail=0cm`

Every one of the ~307 pose-diag lines in between (one per second, 00:07:20→00:12:27) shows the same frozen point: `target == puppet == (-1096,1782)`, `trail=0cm`, and a fossilized `targetSpeed=400` that never decays — a stream that stopped updating entirely. The puppet never received another applied pose for **5 min 07 s**.

### 2. The decisive contradiction: the transport recovered, the poses didn't

The join window was genuinely congested — the join snapshot flood (2600 objects) jammed the host's outbound channel:

- l.101767–101768 [00:07:20] `net-diag[slot 1]: ping=49ms qual=-100/-100% in=0 out=0 pkt/s … pendRel=4141170B` + `[WARN] net-diag[slot 1]: SEND BACKLOG pendRel=4141170B … (> 65536)`
- l.101790 [00:07:22] → l.102100 [00:07:27]: `in=18–19 out=704–888 pkt/s`, `pendRel` oscillating 4.14–4.18 MB, SEND BACKLOG WARNs repeating every second
- l.103820 [00:07:40] `pendRel=0B … qual=100/100%` — **the backlog fully drained 23 s after the join**
- l.105460 [00:08:00] → l.107556 [00:12:27]: steady state `in=58–59 out=61 pkt/s`, `pendRel=0B`, `qual=100/100%`, `ping=47–49 ms` — a perfectly healthy link

But `pose-diag[slot 1] fresh=0/s` spans the congestion window **and** the healthy steady state alike. The client's pose packets were demonstrably arriving in the healthy phase (see §3), and client-origin prop traffic in the very same minutes was being received and applied (see §4) — so the dead thing is not the link, it is the host-side slot-1 pose consumer. It died in the congestion window and was never re-armed; the `first remote pose -> auto-spawning puppet` trigger of l.101667 fired exactly once and never again.

### 3. The client was moving and sending — his side was healthy in both directions

Client log (`client_050920260012_multivoid.log`):

- `pos diag: local actor=…` shows him walking around the base the whole time: (-446,1877) at 00:07:44 (l.2902) → (-436,1886) at 00:07:50 (l.4322) → (-353,1888) at 00:07:54 (l.13930), and onward.
- His outbound pose stream stalled during the snapshot flood — `net stats: sent=26` at 00:07:44 (l.2901, i.e. ~26 packets in the first 27 s after connect), `sent=35` at 00:07:54 (l.13929) — then took off at 00:07:56: `sent=115` (l.13988) → `sent=243` (l.14021) → `sent=480→676` at 00:08:00–02 (l.14059–14125), i.e. a steady ~60 pose packets/s from then on.
- The host's inbound counters match that takeoff: `in=28` at 00:08:00 (l.105460) → `in=58–59 pkt/s` steady from 00:08:02 (l.105493 ff.). The poses were arriving at the host; `pose-diag[slot 1]` stayed `fresh=0/s` anyway.
- The reverse direction was fine: the client received the host's poses — `pose-diag[slot 0]: fresh=1/s` at 00:07:49 (l.3926, still eating the snapshot) → `fresh=37/s` at 00:07:51 (l.11220) → `fresh=53–59/s` stable from 00:07:53 (l.13907 ff.) — and spawned the host's puppet normally: l.3876 [00:07:49] `net: first remote pose on slot 0 -> auto-spawning puppet`, l.3923 `RemotePlayer::Spawn … at (-220,2013,6320) yaw=-149`. On the client's screen the host moved normally (player-reported, and consistent with the pose rates).

So both halves of "the client moved freely, the host's puppet didn't" are instrumented: the client's world view and outbound stream were alive from ~00:07:56; the host's picture of him froze at 00:07:20.

### 4. The prop/item channel worked — the mechanics of the "levitation"

While the pose pipe was dead, everything item-shaped crossed the wire and was applied on the host. The bucket `prop_bucket_C` key `'4sV2hWFkhdc3xAHLXYdXMA'` eid=17877 is the documented example:

- Client l.14840 [00:08:51] `trash_collect: BROADCAST held untracked item cls='prop_bucket_C' key='4sV2hWFkhdc3xAHLXYdXMA' loc=(-770.0,1831.0,6247.2) -- held-pose stream now mirrors it into the collector's hands`; l.14841 `net: NEW held actor … eid=17877 -> BROADCAST`
- Client l.14860 [00:08:52] `net: held -> released (vel.ok=1 linVel=(-1278.5, -821.2, -23.6) |v|=1519.7 cm/s angVel=(-3.3, -193.9, 234.8))` — he threw it
- Host l.106014 [00:08:53] `remote_prop: RELEASE wire '4sV2hWFkhdc3xAHLXYdXMA' eid=17877 … linVel=(-1278.5, -821.2, -23.6) |v|=1519.7 cm/s` — **the velocity vectors match byte-for-byte**: the client's throw was applied to the host's world copy at full strength
- Same bucket re-grabbed and re-thrown at 00:08:57–59 (client l.14887–14888, host l.106063, |v|=1499.4 cm/s); a second prop, eid=16157 (runtime-adopted at world load, host l.97861 [00:06:48]), poked three times at 00:09:24/30/36 (host l.106233/106270/106312, |v|=14.3→52.9→357.0 cm/s)

On the host's screen this looked like exactly what she reported: objects flying around the base with her teammate's model standing motionless at the spawn. Three more log families complete the picture:

- l.105442 [00:07:58] `hand_item: slot 1 mirror SPAWNED cls='prop_drive_C' … (display-only, view-anchored)`; destroyed 00:08:01 (l.105479); again `cls='prop_food_C' name='mrefood_2'` at 00:08:41 (l.105894), destroyed 00:08:43 (l.105915) — the client's held items were mirrored into the host's world **anchored to the frozen puppet's view**, i.e. items materializing and vanishing in mid-air at the spawn point.
- l.105481–483/105524–527/105559–562 [00:08:02–07] `[PILE-B3] HOST slot 1 keyed pos-correction eid=17877 save=(-2169.2,1764.3,6326.3) -> current=(-816.3,1920.1,6334.7) drift=1361.9cm … (late-arm in-window move -> deliver the authoritative position)` — the save-authoritative position corrector kept snapping props back while something kept dragging them away: the two subsystems (ferry in, save-authority out) fought over the same props during the join tail.
- l.105486 [00:08:02] `[ROCK-DROP] EnsureHeldItemBroadcast DECLINE (tracker-known): … -- 'pose stream suffices' assumes an active stream; false if the prop is no longer held` — the mod's own log admits the failure mode this bug demonstrates: several of its paths assume the pose stream is alive, and nothing re-checks that assumption.

### 5. The pose-trust ledger never ran; the night's join ledger

- `movement_ledger[slot 1]` — the pose-trust statistics that in healthy sessions log every ~10 s (e.g. l.5074 ff. in session A) — has **zero lines** in the entire 00:07–00:12 window (grepped). With no poses applied, the ledger had nothing to score; consistent with §1, and it means the session ran 5 minutes with the mod's trust/discipline machinery blind for slot 1.
- Join ledger of the night: join 1 (21:48, session A) healthy (`pose-diag fresh=57–61/s` from the first lines, l.5841 ff.); join 2 (22:48, session B) healthy (fresh=55–59/s, l.96202 ff.); **join 3 (00:07, session C) broken**; join 4 (00:14, session D) healthy (§6). The one broken join is the first join after a **host-death session tear** — the dirtiest teardown of the night (998 deferred destroys dropped at the tear, host-side grab counter still at call #19410 from the old session while the re-hosted world was fresh).

### 6. Control: session D — the same recovery path, healthy immediately

After the manual restart the host re-hosted in-process again (gen=14→15→16, l.107838/107916/107917 — no process restart), lobby #4 `98e8b8ef19e80793` at 00:13:11, client accepted 00:13:41 (l.112301), in-game 00:14:14 (l.113063), spawn at (145,1688,6475) at 00:14:19 (l.113172/113213). From the very first diagnostic line: l.113231 [00:14:20] `pose-diag[slot 1]: fresh=41/s`, and from 00:14:22 onward `fresh=56–59/s` with the target running around the base (`targetSpeed` up to 400, `trail` up to 31 cm, l.113283–113471). Same in-process re-host, same client, same night — a clean join. The defect is therefore a state/race specific to session C's join (first-join-after-death-tear + the congested window), not a general property of re-hosting.

### 7. What the logs cannot show

- The internal state of the host's slot-1 pose consumer: there is no log line for pose receive/drop/reject (the `fresh=0/s` counter is the only witness). Whether the consumer hit a sequence/ack desync during the stall, timed out and disarmed, or kept silently discarding — not distinguishable.
- Whether the host's `in=58–59 pkt/s` in the healthy phase still contained pose packets specifically (the counter is total per-peer; the applied prop releases in the same window prove the channel processed client traffic, but per-type counts are not logged).
- What the client experienced visually of his own puppet — he never sees his own puppet; the "stuck model" is host-screen, player-reported, and matches the frozen `puppet=(-1096,1782)` line precisely.

## Analysis

1. **Proven:** the host's client-puppet pipeline for slot 1 applied a few poses in the first ~2 s of session C (00:07:18–20) and then zero for the remaining 5 min 07 s, while (a) the transport recovered to a fully healthy steady state by ~00:08:00, (b) the client's outbound pose stream ran at ~60 pkt/s, (c) client-origin prop/held traffic was received and applied with exact fidelity in the same minutes, and (d) the control join 4 minutes later was healthy from the first second. This is a host-side join-recovery defect in the pose path specifically — not network, not client, not re-hosting in general.
2. The broken join is the first join after a host-death session tear, and its join window had the worst congestion of the night (4.1–4.2 MB stuck outbound backlog for ~20 s). A plausible (unproven) mechanism: the pose consumer disarmed or desynced during the stall and its only re-arming trigger ("first remote pose") had already been spent at 00:07:18. A sequence-poison fits the same signature: if remote poses are sequence-stamped and dropped on gap (hypothesis — not logged either way), the stall inside the congested window would make every later pose arrive "stale" and be rejected.
3. **The gameplay consequence was worse than a cosmetic statue:** the held-item ferry mirrors items into view anchored to the puppet ("display-only, view-anchored") and applies client throws to world props (the byte-identical velocity vectors), so the host experienced items teleporting, levitating and being flung around the base by an invisible hand — while every one of those props' positions was simultaneously being "corrected" back to save positions by PILE-B3. Two of the mod's own log lines (`'pose stream suffices' assumes an active stream; false if the prop is no longer held`; the PILE-B3 "late-arm … hole closed" phrasing) show the mod already knows these streams are fragile; nothing re-validates them mid-session.
4. Cost of the bug: the session was unplayable as co-op and ended in a manual full restart of both games + lobby #4 — the second teardown of the night that a re-sync could have avoided.
5. Relation to other bugs: the dead-pose/alive-items split is the mirror image of the "state syncs, native event effects don't" family ([Bug 08 «Client consoles stay dead after host restores base power»](https://discordapp.com/channels/1508397685726646272/1546197518474543207)) — here the *event-ish* channels (prop poses, held ferry, releases) worked while the highest-level continuous channel (player pose) died. It is also the first bug in the set whose damage is bounded by the join path rather than by identity/sync divergence.

---

# Баг 15: Кукла клиента замерла на точке спавна на экране хоста всю сессию — хостовый конвейер поз умер в перегруженном окне входа

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео (заявлено) | ~02:19:00→02:24:00 (сессия C: клиент зашёл 02:18:54, вышел 02:24:05) |
| Окно лога | 00:07:17→00:12:28 (`лог = видео − 02:11:37`) |
| Докладчик | клиент `PlayerNickname2` — своё свободное перемещение плюс то, что было **на экране хоста** (хост `Mikuzavr` видела неподвижную куклу и «левитирующие» предметы) |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | синхронизация поз игрока (`RemotePlayer`-кукла / `pose-diag`) против ферри предметов/пропов — путь поз умер, всё остальное жило |
| Ключевое состояние | сессия C = лобби №3 `b238b592024d039b`, мир gen=13 (внутри-процессный перехост после [Баг 14 «Смерть хоста разорвала сессию — вето death_revive молча не сработало, живого клиента выкинуло»](https://discordapp.com/channels/1508397685726646272/1546200747803418654/1546209922776309971)); контрольная сессия D = лобби №4 `98e8b8ef19e80793`, gen=16 |

## Описание

Сразу после того инцидента разрыва от смерти хост перехостилась (00:06:44), клиент переподключился (допущен 00:07:17, в игре ~00:07:42). С первых секунд той сессии и до её конца модель персонажа клиента на экране хоста стояла на месте спавна и не двигалась — при этом сам клиент свободно бегал по миру и взаимодействовал с предметами, периодически пугая хоста объектами, которые «левитировали» или летали по базе в отсутствие кого-либо рядом. Сессия как кооп была непроиграбельна; оба вышли вручную в 00:12:28, чтобы перезапуститься (второй разрыв, описанный в §6 отчёта о разрыве от смерти).

Логи показывают чёткий раскол внутри мода: **конвейер поз slot 1 применил несколько поз в первые ~2 секунды и потом ноль за оставшиеся 5 минут**, хотя транспорт восстановился, клиент доказуемо слал позы, а клиентский трафик предметов/пропов продолжал приходить и применяться всё это время. Следующий вход (сессия D, 00:14:14) в такой же внутри-процессный перехост был здоров сразу — значит, это состояние/гонка в пути восстановления входа, а не детерминированное свойство перехоста.

## Доказательства

### 1. Кукла заспавнилась один раз и замерла — хронология поз (лог хоста, `host_050920260342_multivoid.log`)

- l.101641 [00:07:17] `host accepted client into PENDING 0 (h=0xb5e2d692)`
- l.101667 [00:07:18] `net: first remote pose on slot 1 -> auto-spawning puppet` — **единственное** за сессию вооружение потока поз
- l.101700 [00:07:18] `RemotePlayer::Spawn: puppet=000002C8D7F8A060 at (-231,2004,6320) yaw=-149 nick='PlayerNickname2'`
- l.101763 [00:07:20] первая `pose-diag[slot 1]: fresh=0/s targetSpeed=400 target=(-1096,1782) puppet=(-1096,1782) trail=0cm` — цель уже дёрнулась (после спавна пришло ещё несколько поз, сдвинувших её с (-231,2004) на (-1096,1782)), а дальше `fresh=0/s`
- l.107558 [00:12:27] последняя pose-diag перед выходом клиента — по-прежнему `fresh=0/s target=(-1096,1782) puppet=(-1096,1782) trail=0cm`

Все ~307 строк pose-diag между ними (по одной в секунду, 00:07:20→00:12:27) показывают одну и ту же замороженную точку: `target == puppet == (-1096,1782)`, `trail=0cm`, и окаменевший `targetSpeed=400`, который никогда не затухает — поток, переставший обновляться целиком. Кукла не получила ни одной применённой позы **5 мин 07 с**.

### 2. Решающее противоречие: транспорт восстановился — позы нет

Окно входа было по-настоящему перегружено — флуд снапшота входа (2600 объектов) заклинил исходящий канал хоста:

- l.101767–101768 [00:07:20] `net-diag[slot 1]: ping=49ms qual=-100/-100% in=0 out=0 pkt/s … pendRel=4141170B` + `[WARN] net-diag[slot 1]: SEND BACKLOG pendRel=4141170B … (> 65536)`
- l.101790 [00:07:22] → l.102100 [00:07:27]: `in=18–19 out=704–888 pkt/s`, `pendRel` колеблется 4,14–4,18 МБ, SEND BACKLOG-предупреждения каждую секунду
- l.103820 [00:07:40] `pendRel=0B … qual=100/100%` — **бэклог полностью слился через 23 с после входа**
- l.105460 [00:08:00] → l.107556 [00:12:27]: установившийся режим `in=58–59 out=61 pkt/s`, `pendRel=0B`, `qual=100/100%`, `ping=47–49 мс` — абсолютно здоровый линк

Но `pose-diag[slot 1] fresh=0/s` накрывает и окно затора, и здоровый установившийся режим одинаково. Позы клиента в здоровой фазе доказуемо приходили (см. §3), и клиентский трафик пропов в те же минуты принимался и применялся (см. §4) — значит, мёртв не линк, а клиентский (host-side) потребитель поз slot 1. Он умер в окне затора и не был перезапущен; триггер `first remote pose -> auto-spawning puppet` из l.101667 сработал ровно один раз и больше никогда.

### 3. Клиент двигался и слал — его сторона была здорова в обе стороны

Лог клиента (`client_050920260012_multivoid.log`):

- `pos diag: local actor=…` показывает его ходящим вокруг базы всё время: (-446,1877) в 00:07:44 (l.2902) → (-436,1886) в 00:07:50 (l.4322) → (-353,1888) в 00:07:54 (l.13930) и дальше.
- Его исходящий поток поз застрял во время флуда снапшота — `net stats: sent=26` в 00:07:44 (l.2901, т.е. ~26 пакетов за первые 27 с после коннекта), `sent=35` в 00:07:54 (l.13929) — потом рванул в 00:07:56: `sent=115` (l.13988) → `sent=243` (l.14021) → `sent=480→676` в 00:08:00–02 (l.14059–14125), т.е. стабильные ~60 поз-пакетов/с дальше.
- Входящие счётчики хоста совпадают с этим разгоном: `in=28` в 00:08:00 (l.105460) → `in=58–59 pkt/s` стабильно с 00:08:02 (l.105493 и далее). Позы приходили на хост; `pose-diag[slot 1]` всё равно стоял на `fresh=0/s`.
- Обратное направление в порядке: клиент получал позы хоста — `pose-diag[slot 0]: fresh=1/s` в 00:07:49 (l.3926, ещё доедает снапшот) → `fresh=37/s` в 00:07:51 (l.11220) → `fresh=53–59/s` стабильно с 00:07:53 (l.13907 и далее) — и нормально заспавнил куклу хоста: l.3876 [00:07:49] `net: first remote pose on slot 0 -> auto-spawning puppet`, l.3923 `RemotePlayer::Spawn … at (-220,2013,6320) yaw=-149`. На экране клиента хост двигалась нормально (со слов игрока, и согласуется с частотами поз).

Обе половины «клиент двигался свободно, кукла у хоста — нет» покрыты инструментарием: мировая картинка клиента и его исходящий поток ожили с ~00:07:56; картинка хоста о нём замерла в 00:07:20.

### 4. Канал предметов/пропов работал — механика «левитации»

Пока труба поз была мертва, всё предметное пересекало провод и применялось на хосте. Ведро `prop_bucket_C` key `'4sV2hWFkhdc3xAHLXYdXMA'` eid=17877 — документированный пример:

- Клиент l.14840 [00:08:51] `trash_collect: BROADCAST held untracked item cls='prop_bucket_C' key='4sV2hWFkhdc3xAHLXYdXMA' loc=(-770.0,1831.0,6247.2) -- held-pose stream now mirrors it into the collector's hands`; l.14841 `net: NEW held actor … eid=17877 -> BROADCAST`
- Клиент l.14860 [00:08:52] `net: held -> released (vel.ok=1 linVel=(-1278.5, -821.2, -23.6) |v|=1519.7 см/с angVel=(-3.3, -193.9, 234.8))` — бросил
- Хост l.106014 [00:08:53] `remote_prop: RELEASE wire '4sV2hWFkhdc3xAHLXYdXMA' eid=17877 … linVel=(-1278.5, -821.2, -23.6) |v|=1519.7 см/с` — **векторы скоростей совпадают байт в байт**: бросок клиента применён к мировой копии хоста в полную силу
- То же ведро снова взято-брошено в 00:08:57–59 (клиент l.14887–14888, хост l.106063, |v|=1499,4 см/с); второй проп eid=16157 (рутимо-усыновлён при загрузке мира, хост l.97861 [00:06:48]) потроган трижды в 00:09:24/30/36 (хост l.106233/106270/106312, |v|=14,3→52,9→357,0 см/с)

На экране хоста это выглядело ровно как то, что она рассказала: предметы летают по базе, а модель напарника стоит неподвижно на спавне. Три ещё семейства строк достраивают картину:

- l.105442 [00:07:58] `hand_item: slot 1 mirror SPAWNED cls='prop_drive_C' … (display-only, view-anchored)`; уничтожено в 00:08:01 (l.105479); снова `cls='prop_food_C' name='mrefood_2'` в 00:08:41 (l.105894), уничтожено 00:08:43 (l.105915) — предметы в руках клиента зеркалились в мир хоста **привязанными к обзору замороженной куклы**, т.е. предметы материализовались и исчезали в воздухе на точке спавна.
- l.105481–483/105524–527/105559–562 [00:08:02–07] `[PILE-B3] HOST slot 1 keyed pos-correction eid=17877 save=(-2169.2,1764.3,6326.3) -> current=(-816.3,1920.1,6334.7) drift=1361.9cm … (late-arm in-window move -> deliver the authoritative position)` — save-авторитетный корректор позиций продолжал прищёлкивать пропы назад, пока что-то продолжало их утаскивать: две подсистемы (ферри внутрь, save-авторитет наружу) дрались за одни и те же пропы весь хвост входа.
- l.105486 [00:08:02] `[ROCK-DROP] EnsureHeldItemBroadcast DECLINE (tracker-known): … -- 'pose stream suffices' assumes an active stream; false if the prop is no longer held` — собственный лог мода признаёт режим отказа, который демонстрирует этот баг: несколько его путей исходят из того, что поток поз жив, и ничто не перепроверяет это допущение.

### 5. Реестр доверия поз не запускался вообще; реестр входов за ночь

- `movement_ledger[slot 1]` — статистика доверия поз, которая в здоровых сессиях пишется каждые ~10 с (например, l.5074 и далее в сессии A), — имеет **ноль строк** за всё окно 00:07–00:12 (проверено грепом). Без применяемых поз реестру нечего было оценивать; согласуется с §1 и означает, что сессия прошла 5 минут с ослепшей на slot 1 машиной доверия/дисциплины мода.
- Реестр входов за ночь: вход 1 (21:48, сессия A) здоровый (`pose-diag fresh=57–61/s` с первых строк, l.5841 и далее); вход 2 (22:48, сессия B) здоровый (fresh=55–59/s, l.96202 и далее); **вход 3 (00:07, сессия C) сломан**; вход 4 (00:14, сессия D) здоровый (§6). Единственный сломанный вход — первый вход после **разрыва сессии смертью хоста** — самого грязного разбора той ночи (998 отложенных destroy сброшено при разрыве, счётчик grab хоста всё ещё на call #19410 из старой сессии, пока перехостенный мир был свежим).

### 6. Контроль: сессия D — тот же путь восстановления, здорова сразу

После ручного перезапуска хост снова перехостилась внутри процесса (gen=14→15→16, l.107838/107916/107917 — без перезапуска процесса), лобби №4 `98e8b8ef19e80793` в 00:13:11, клиент допущен 00:13:41 (l.112301), в игре 00:14:14 (l.113063), спавн на (145,1688,6475) в 00:14:19 (l.113172/113213). С самой первой диагностической строки: l.113231 [00:14:20] `pose-diag[slot 1]: fresh=41/s`, а с 00:14:22 — `fresh=56–59/s` с бегающей по базе целью (`targetSpeed` до 400, `trail` до 31 см, l.113283–113471). Тот же внутри-процессный перехост, тот же клиент, та же ночь — чистый вход. Значит, дефект — состояние/гонка, специфичные именно для входа в сессию C (первый вход после разрыва смертью + перегруженное окно), а не общее свойство перехоста.

### 7. Чего логи показать не могут

- Внутреннее состояние потребителя поз slot 1 на хосте: строки приёма/сброса/отказа поз нет (свидетель один — счётчик `fresh=0/s`). Попал ли потребитель в рассинхрон последовательности/ack во время затора, за таймаутился и разоружился или молча отбрасывал — неразличимо.
- Содержал ли `in=58–59 pkt/s` хоста в здоровой фазе именно позы (счётчик суммарный по пиру; применённые релизы пропов в том же окне доказывают, что канал клиентский трафик обрабатывал, но по-типовых счётчиков нет).
- Что клиент видел визуально у собственной куклы — своей куклы он не видит; «застывшая модель» — это экран хоста, со слов игрока, и точно совпадает с замороженной строкой `puppet=(-1096,1782)`.

## Анализ

1. **Доказано:** конвейер куклы клиента slot 1 на хосте применил несколько поз в первые ~2 с сессии C (00:07:18–20) и потом ноль за оставшиеся 5 мин 07 с, при том что (а) транспорт восстановился до полностью здорового установившегося режима к ~00:08:00, (б) исходящий поток поз клиента шёл на ~60 pkt/с, (в) клиентский трафик пропов/предметов в те же минуты принимался и применялся с точностью до байта, и (г) контрольный вход 4 минутами позже был здоров с первой секунды. Это host-side дефект пути восстановления входа именно в трубе поз — не сеть, не клиент, не перехост вообще.
2. Сломанный вход — первый вход после разрыва сессии смертью хоста, и его окно входа имело худший затор той ночи (4,1–4,2 МБ застрявшего исходящего бэклога ~20 с). Правдоподобный (недоказанный) механизм: потребитель поз разоружился или рассинхронизировался во время затора, а его единственный триггер перевооружения («first remote pose») уже был потрачен в 00:07:18. Под ту же картину подходит и «отравление последовательности»: если удалённые позы штампуются номерами и отбрасываются при разрыве (гипотеза — в логах этого нет ни так, ни эдак), затор в перегруженном окне сделал бы каждую позднюю позу «протухшей» и отбрасываемой.
3. **Игровое последствие хуже косметической статуи:** ферри предметов зеркалит их в обзор, привязанный к кукле («display-only, view-anchored»), и применяет броски клиента к мировым пропам (байт-в-байт совпадающие векторы скоростей), так что хост видела предметы, телепортирующиеся, левитирующие и разлетающиеся по базе невидимой рукой — пока позиции тех же пропов одновременно «исправлялись» назад к сохранённым силами PILE-B3. Две собственные строки лога мода (`'pose stream suffices' assumes an active stream; false if the prop is no longer held`; формулировка PILE-B3 про «late-arm … hole closed») показывают, что мод и сам знает, насколько хрупки эти потоки; но ничего не перепроверяет их посреди сессии.
4. Цена бага: сессия стала непроиграбельной как кооп и кончилась полным ручным перезапуском обеих игр + лобби №4 — вторым разбором той ночи, которого могла бы избежать перепроверка/перевооружение потока.
5. Связь с другими багами: раскол «позы мертвы / предметы живы» — зеркальный к семейству «состояние синхронизируется, нативные эффекты события — нет» ([Баг 08 «Консоли клиента остаются мёртвыми после восстановления питания хостом»](https://discordapp.com/channels/1508397685726646272/1546197518474543207)): здесь *событийные* каналы (позы пропов, ферри удержания, релизы) работали, а высший непрерывный канал (позы игрока) умер. Это и первый баг набора, чья поломка ограничена путём входа, а не расхождением идентичности/синхронизации.
