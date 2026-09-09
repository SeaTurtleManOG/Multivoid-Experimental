# Bug 07: E-press hijacked by pile grab-intent — phantom clumps spawn world piles

## Metadata

| Field | Value |
|---|---|
| Video window | `~00:21:50` (dual-carry grab at `00:21:51`); switch incident `00:29:23`; observed throughout the session (evening bursts 23:24, 23:41) |
| Log window | `22:09:04`–`22:11:43` (2026-09-04), primary episode `22:10:13`–`22:10:14`; recurrence `23:24:53`–`23:44:56` |
| Log files | client: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`); host (`Mikuzavr`) world receives the side effects |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | pile grab-intent (camera-ray cone) vs native use; throw-intent `release(E)`; clump→pile NATIVIZE |
| Frequency | 10 hijack-grabs in 22:09:04–22:11:43 + 23 more in the evening (`client_050920260005_multivoid.log`, 22:46–00:05) |

## Description

While the client was handling items (ferrying the loot from a box he had broken — [Bug 06 «Client breaks a container — host never sees the spilled loot»](https://discordapp.com/channels/1508397685726646272/1546191405670473778)), presses of E sometimes produced a trash clump **in addition to the item already in hand** (**player-reported**; the dual carry itself is **log-proven** — Evidence 1). Player report: the clump appears in the hands exactly like a normal E-grab, **out of nowhere, with no visible logic** — it has appeared while interacting with a light switch, and while carrying items; in every case the player pressed E with the cursor aimed at some interactive object, and a clump appeared in the hands. A clump thrown to the ground **persists in the world for both peers** (**log-proven** — NATIVIZED with `drift=0.00cm`, LAND COMMIT broadcast). The mechanism is **log-proven**: the E-press is matched against *pile proxies* by a **camera-ray cone**, the native use of the intended target is cancelled, and a pile→clump grab is requested from the host. The next E-press then acts as `release(E)` on the carried clump — it is **thrown**, flies, self-re-piles, and materializes as a **brand-new trash pile** wherever it lands. Three such piles appeared within ~90 seconds in the reported window; the loop then repeats (see the 23:24 specimen below).

## Evidence

### 1. The dual-carry moment — 22:10:14: item in hand + clump grabbed

```
client 28260: [22:10:14] net: NEW held actor 000001876B5CD980 cls='prop_C' key='7Mxy4eSbIrfsHwUAeInXnA' eid=42712 -> carry-only(trash/clump)      ← loot item (d_wood_3) in hands
client 28272: [22:10:14] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=4896 (camera-ray cone) -> native use CANCELLED (no use_deny) + requesting grab from host
client 28277: [22:10:14] [PILE] CLIENT recv convert GRAB(pile->clump) eid=4896 ctx=1 -> PROXY re-skinned IN PLACE to CLUMP chipType=3 [SYNC-MIRROR OK -- no spawn-fresh, no dup]
client 28278: [22:10:14] [GRAB-INTENT] CLIENT carry CONFIRMED eid=4896 (inbound ToClump matched our request)
client 28284: [22:10:15] [TRASH-CARRY] CLIENT APPLY eid=4896 ctx=1 -> target(-115.9,640.9,6324.9) (per-eid carry drive)   ← clump carried alongside the item
```

The item stayed in hand through the whole clump carry: its `net: NEW held actor ... eid=42712` re-broadcasts continue every ~1.5 s during the walk (client 28545–29108) **and after** the clump landed at 22:10:30 — both objects held simultaneously.

### 2. Same hijack twice more while ferrying loot

`27669 [22:09:04] E-PRESS aimed at pile proxy eid=4899`, `28165 [22:10:05] ... eid=5062` — the client was moving box loot at the time (item grabs 42712–42715 at 22:08:44–55).

### 3. The loop in its purest form — 23:24:53–55 (`client_050920260005_multivoid.log`, 4 seconds, grab→throw→pile→re-grab)

```
52502: [23:24:53] prop_sound: use click at (-777, 210, 6563)
52504: [23:24:53] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=10418 (camera-ray cone) -> native use CANCELLED (no use_deny) + requesting grab from host
52508: [23:24:53] [PILE] CLIENT recv convert GRAB(pile->clump) eid=10418 ctx=1 -> PROXY re-skinned IN PLACE to CLUMP ... [SYNC-MIRROR OK -- no spawn-fresh, no dup]
52511: [23:24:53] [USE-RELEASE] paired E-release CANCELLED (its press was intercepted -- no use_deny on release)
52515: [23:24:54] [THROW-INTENT] CLIENT E-PRESS while carrying eid=10418 -> requesting release(E-drop) from host (native use CANCELLED -- no use_deny)
52521: [23:24:54] [PILE] CLIENT recv convert LAND(clump->pile) eid=10418 ... -> NATIVIZED native=000001D7CFB66000 at (-1070.1,231.2,6563.5) host=(-1070.1,231.2,6563.5) drift=0.00cm
52529: [23:24:54] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=10416 (camera-ray cone) -> native use CANCELLED ...   ← next press, next clump
```

Note the distances: the drive target for the grabbed clump (the player's hands) was at (-1085.7, 226.3, 6628.7), while the matched pile proxy sat at (-777, 210, 6563) — **~3.1 m away**. The cone matched a pile far from the crosshair target, and the mod's own line admits the interception (`paired E-release CANCELLED (its press was intercepted)`). The thrown clump's pile (NATIVIZED, `drift=0.00cm`, LAND COMMIT broadcast) stays in the world for both peers — matching the player report.

### 4. Frequency

10 hijack-grabs in 22:09:04–22:11:43 (`client_040920262246_multivoid.log`), 23 more in `client_050920260005_multivoid.log` — including bursts of 5 presses in 5 s (23:24:53–57) and 11 presses in 53 s (23:41:02–54), where the player was, by his account, trying to interact with non-pile objects and every press landed on a clump instead.

### 5. E = `release(E)` throw → clump self-re-piles → NEW world pile (three in ~90 s in the reported window)

```
host 16441: [22:09:06] [THROW-INTENT] SUCCESS eid=4899 slot=1 mode=release(E) ... -- puppet released + physics thrown vel=(5,-292,-103); clump flies + self-re-piles (thunk -> ToPile)
client 27702: [22:09:06] [PILE] CLIENT ToPile LAND eid=4899 ctx=2 -> NATIVIZED native=00000187C079A600 at (-844.2,406.7,6232.3) host=(-844.2,406.7,6232.3) drift=0.00cm

host 16932: [22:10:06] [THROW-INTENT] SUCCESS eid=5062 slot=1 mode=release(E) ... vel=(19,-6,-59); clump flies + self-re-piles
client 28192: [22:10:06] [PILE] CLIENT ToPile LAND eid=5062 ctx=2 -> NATIVIZED native=00000187BECF0D80 at (63.2,542.8,6232.3) ...

client 28516: [22:10:30] [PILE] CLIENT ToPile LAND eid=4896 ctx=2 -> NATIVIZED native=0000018767DD7E00 at (-990.9,962.9,6232.3) ...
client 28511: [22:10:30] synth-key: minted 'cs_67dd7e00_5' for actor 0000018767DD7E00 class='actorChipPile_C'
client 28514: [22:10:30] [PILE] native_pile_mirror: MATERIALIZED eid=4896 native=0000018767DD7E00 class='actorChipPile_C' chipType=3 ...
```

Each landing mints a **new** pile actor (`synth-key: minted ...`, `MATERIALIZED`) — i.e., a new trash pile object appears in the world as a side effect of item handling.

### 6. Interaction churn on the held item

After the clump landed, every E-press cycled the held item through release/re-grab: `[REL-EDGE] eid=42712 -> FIRE(release)` + `net: held -> released` (client 28551 at 22:10:33 and on; 29 such releases for this item in the file), each landing on the host as a GRAB-IN/RELEASE pair ~1/s for 40+ s (`host_050920260342_multivoid.log:17217+`).

### 7. The "clump from a light switch" report (player-reported moment ≈ 22:17:46)

The switch press itself was clean — `light: sent ON key='lightswitch_signalroom'` + lightgroup applied, 22:17:50, no hijack at that second. The nearest log evidence of clumps-appearing-on-E right after: the **22:18:17–29 burst** — 12 grab-intents in 12 s against piles 4877, 4339, 4875, 4902, 4905 (pile 4905 pressed ×4 within one second, client 33931–33957), and the **22:19:45–55 series** during trash cleanup while carrying a garbage bin — which also exposes a second hijack signature: `[GRAB-INTENT] CLIENT E-PRESS on BOUND native pile eid=5000 at(-2019.8,2168.7,6263.7) chipType=2 (lookAtActor, occlusion-correct) -> native use CANCELLED (no grab, no use_deny) + requesting grab from host` (client 35222). Either the switch-adjacent clump came from these bursts a few seconds later than remembered, or a switch-adjacent path exists without a GRAB-INTENT line — not decidable from these logs (see below).

## What the logs cannot show

- The exact source of the switch-adjacent clump (player-reported moment ≈ 22:17:46): the switch press itself was clean, and the nearest clump traffic is 27–43 s later (22:18:17–29, 22:19:45–55 bursts). Either the player's timing is off by tens of seconds, or a hijack path without a GRAB-INTENT line exists — not decidable from these logs.
- A host-side hijack incident: symmetric machinery exists (the host executes `THROW-INTENT ... mode=release(E)` and LAND COMMIT on the client's behalf), but no host E-press hijack occurs in this session's logs — only client-side interceptions are captured.
- Whether the cone match is bounded by any reach distance — the 23:24:53 grab matched a pile proxy ~3.1 m from the hands; no bounds are visible in the logs.
- Whether dual carry (held item + hovering clump) is intended — the player reports native gameplay allows one carried object at a time; not derivable from logs.

## Analysis

The mod re-implements trash-pile grabbing for the client as an E-press "grab intent" matched by a **camera-ray cone** against pile proxies, cancelling the native use (`native use CANCELLED (no use_deny)`). To the player this is invisible — the clump just appears in the hands as if the aimed-at object had been grabbed — which matches the report of "no visible logic" (a light switch, carried items, anything). The cone ignores the player's actual target, the hand state, and effectively the distance: at 23:24:53 it matched a pile proxy **~3.1 m from the hands** while the cursor was on another interactive object. While the client held a loot item, hijacked presses grabbed a trash clump **in addition** to the held item (dual carry — the item remains held and re-broadcast during the entire clump carry). The next E is consumed as `release(E)` for the clump: thrown, self-re-piled, NATIVIZED as a brand-new world pile that persists for both peers — so merely handling items near trash littered the area with piles (three in 90 seconds in the reported window; more in every later burst). The mod's own logging admits the interception (`paired E-release CANCELLED (its press was intercepted)`) — the press never reaches the intended target at all. The hijack recurred 33+ times across the session; symmetric host-side machinery exists (`THROW-INTENT ... mode=release(E)`, `LAND COMMIT`), so the host is presumably exposed the same way (hypothesis — no host-side incident is captured in these logs). Suggested directions: make the cone lose to a closer ray-hit prop or to any non-pile target; bound the cone by reach distance and require line-of-sight; refuse pile grabs while a hand slot is occupied (or require an explicit mode); decouple `release(E)` from item interactions; consider merging landed clumps into an existing pile instead of minting a new pile actor per landing.

---

# Баг 07: Нажатие E перехватывается grab-intent куч — фантомные комки спавнят кучи мира

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `~00:21:50` (двойное взятие в `00:21:51`); инцидент с выключателем `00:29:23`; наблюдается по всей сессии (вечерние всплески 23:24, 23:41) |
| Окно лога | `22:09:04`–`22:11:43` (2026-09-04), основной эпизод `22:10:13`–`22:10:14`; рецидив `23:24:53`–`23:44:56` |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`); мир хоста (`Mikuzavr`) получает побочные эффекты |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | pile grab-intent (камерный конус) против нативного use; throw-intent `release(E)`; NATIVIZE комок→куча |
| Частота | 10 перехваченных взятий в 22:09:04–22:11:43 + еще 23 вечером (`client_050920260005_multivoid.log`, 22:46–00:05) |

## Описание

Пока клиент взаимодействовал с предметами (разносил добычу из сломанного им ящика — [Баг 06 «Клиент ломает контейнер — хост не видит выпавшую добычу»](https://discordapp.com/channels/1508397685726646272/1546191405670473778)), нажатия E иногда дополнительно давали **комок мусора поверх предмета в руке** (**со слов игрока**; сама двойная переноска **доказана логом** — Доказательство 1). Со слов игрока: комок появляется в руках ровно как обычный grab по E, **из ниоткуда, без видимой логики** — случалось и при взаимодействии с выключателем, и при переносе предметов; во всех случаях игрок нажимал E с курсором на каком-то интерактивном объекте, и у него в руках появлялся комок. Брошенный на землю комок **остаётся в мире и у хоста** (**доказано логом** — NATIVIZED с `drift=0.00cm`, рассылка LAND COMMIT). Механизм **доказан логом**: нажатие E матчится с *pile-прокси* по **конусу камеры (camera-ray cone)**, нативное использование намеченной цели отменяется, и хосту запрашивается grab «куча→комок». Следующее нажатие E затем работает как `release(E)` для переносимого комка — он **бросается**, летит, сам пересобирается в кучу и материализуется как **совершенно новая куча мусора** в точке падения. В отчётном окне за ~90 секунд так появилось три кучи; затем цикл повторяется (см. экземпляр 23:24 ниже).

## Доказательства

### 1. Момент двойной переноски — 22:10:14: предмет в руке + схвачен комок

```
client 28260: [22:10:14] net: NEW held actor 000001876B5CD980 cls='prop_C' key='7Mxy4eSbIrfsHwUAeInXnA' eid=42712 -> carry-only(trash/clump)      ← предмет добычи (d_wood_3) в руках
client 28272: [22:10:14] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=4896 (camera-ray cone) -> native use CANCELLED (no use_deny) + requesting grab from host
client 28277: [22:10:14] [PILE] CLIENT recv convert GRAB(pile->clump) eid=4896 ctx=1 -> PROXY re-skinned IN PLACE to CLUMP chipType=3 [SYNC-MIRROR OK -- no spawn-fresh, no dup]
client 28278: [22:10:14] [GRAB-INTENT] CLIENT carry CONFIRMED eid=4896 (inbound ToClump matched our request)
client 28284: [22:10:15] [TRASH-CARRY] CLIENT APPLY eid=4896 ctx=1 -> target(-115.9,640.9,6324.9) (per-eid carry drive)   ← комок переносится вместе с предметом
```

Предмет оставался в руке всю перевозку комка: его ребродкасты `net: NEW held actor ... eid=42712` продолжаются каждые ~1.5 с во время ходьбы (клиент 28545–29108) **и после** приземления комка в 22:10:30 — оба объекта удерживаются одновременно.

### 2. Тот же перехват еще дважды при переносе добычи

`27669 [22:09:04] E-PRESS aimed at pile proxy eid=4899`, `28165 [22:10:05] ... eid=5062` — клиент в это время разносил добычу из ящика (взятия предметов 42712–42715 в 22:08:44–55).

### 3. Цикл в чистейшем виде — 23:24:53–55 (`client_050920260005_multivoid.log`, 4 секунды, взятие→бросок→куча→повторное взятие)

```
52502: [23:24:53] prop_sound: use click at (-777, 210, 6563)
52504: [23:24:53] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=10418 (camera-ray cone) -> native use CANCELLED (no use_deny) + requesting grab from host
52508: [23:24:53] [PILE] CLIENT recv convert GRAB(pile->clump) eid=10418 ctx=1 -> PROXY re-skinned IN PLACE to CLUMP ... [SYNC-MIRROR OK -- no spawn-fresh, no dup]
52511: [23:24:53] [USE-RELEASE] paired E-release CANCELLED (its press was intercepted -- no use_deny on release)
52515: [23:24:54] [THROW-INTENT] CLIENT E-PRESS while carrying eid=10418 -> requesting release(E-drop) from host (native use CANCELLED -- no use_deny)
52521: [23:24:54] [PILE] CLIENT recv convert LAND(clump->pile) eid=10418 ... -> NATIVIZED native=000001D7CFB66000 at (-1070.1,231.2,6563.5) host=(-1070.1,231.2,6563.5) drift=0.00cm
52529: [23:24:54] [GRAB-INTENT] CLIENT E-PRESS aimed at pile proxy eid=10416 (camera-ray cone) -> native use CANCELLED ...   ← следующее нажатие, следующий комок
```

Обратите внимание на дистанции: drive-цель для схваченного комка (руки игрока) была в (-1085.7, 226.3, 6628.7), тогда как сматченный pile-прокси сидел в (-777, 210, 6563) — **~3,1 м**. Конус сматчил кучу далеко от цели прицела, и собственная строка мода признаёт перехват (`paired E-release CANCELLED (its press was intercepted)`). Куча от брошенного комка (NATIVIZED, `drift=0.00cm`, рассылка LAND COMMIT) остаётся в мире у обоих пиров — совпадает с рассказом игрока.

### 4. Частота

10 перехваченных взятий в 22:09:04–22:11:43 (`client_040920262246_multivoid.log`), еще 23 в `client_050920260005_multivoid.log` — включая всплески 5 нажатий за 5 с (23:24:53–57) и 11 нажатий за 53 с (23:41:02–54), где игрок, по его словам, пытался взаимодействовать с непилевыми объектами, и каждое нажатие приземлялось на комок.

### 5. E = `release(E)` бросок → комок самопересобирается → НОВАЯ куча мира (три за ~90 с в отчётном окне)

```
host 16441: [22:09:06] [THROW-INTENT] SUCCESS eid=4899 slot=1 mode=release(E) ... -- puppet released + physics thrown vel=(5,-292,-103); clump flies + self-re-piles (thunk -> ToPile)
client 27702: [22:09:06] [PILE] CLIENT ToPile LAND eid=4899 ctx=2 -> NATIVIZED native=00000187C079A600 at (-844.2,406.7,6232.3) host=(-844.2,406.7,6232.3) drift=0.00cm

host 16932: [22:10:06] [THROW-INTENT] SUCCESS eid=5062 slot=1 mode=release(E) ... vel=(19,-6,-59); clump flies + self-re-piles
client 28192: [22:10:06] [PILE] CLIENT ToPile LAND eid=5062 ctx=2 -> NATIVIZED native=00000187BECF0D80 at (63.2,542.8,6232.3) ...

client 28516: [22:10:30] [PILE] CLIENT ToPile LAND eid=4896 ctx=2 -> NATIVIZED native=0000018767DD7E00 at (-990.9,962.9,6232.3) ...
client 28511: [22:10:30] synth-key: minted 'cs_67dd7e00_5' for actor 0000018767DD7E00 class='actorChipPile_C'
client 28514: [22:10:30] [PILE] native_pile_mirror: MATERIALIZED eid=4896 native=0000018767DD7E00 class='actorChipPile_C' chipType=3 ...
```

Каждое приземление минтит **нового** актора кучи (`synth-key: minted ...`, `MATERIALIZED`) — т.е. новый объект кучи мусора появляется в мире как побочный эффект взаимодействия с предметами.

### 6. Чурн взаимодействий на удерживаемом предмете

После приземления комка каждое нажатие E прогоняло предмет через release/re-grab: `[REL-EDGE] eid=42712 -> FIRE(release)` + `net: held -> released` (клиент 28551 в 22:10:33 и далее; 29 таких release для этого предмета в файле), каждое приземляется на хосте парой GRAB-IN/RELEASE ~1/с на протяжении 40+ с (`host_050920260342_multivoid.log:17217+`).

### 7. Отчёт «комок от выключателя» (момент со слов игрока ≈ 22:17:46)

Само нажатие выключателя было чистым — `light: sent ON key='lightswitch_signalroom'` + применённый lightgroup, 22:17:50, перехвата в ту секунду нет. Ближайшие свидетельства в логе появления комков по E вскоре после: **всплеск 22:18:17–29** — 12 grab-intent'ов за 12 с против куч 4877, 4339, 4875, 4902, 4905 (куча 4905 нажата ×4 в одну секунду, клиент 33931–33957), и **серия 22:19:45–55** при уборке мусора с переноской мусорного бака — которая заодно вскрывает вторую сигнатуру перехвата: `[GRAB-INTENT] CLIENT E-PRESS on BOUND native pile eid=5000 at(-2019.8,2168.7,6263.7) chipType=2 (lookAtActor, occlusion-correct) -> native use CANCELLED (no grab, no use_deny) + requesting grab from host` (клиент 35222). Либо комок у выключателя пришёл из этих всплесков на несколько секунд позже, чем запомнилось, либо существует путь «у выключателя» без строки GRAB-INTENT — из этих логов неразличимо (см. ниже).

## Чего логи показать не могут

- Точный источник комка «у выключателя» (момент со слов игрока ≈ 22:17:46): само нажатие выключателя было чистым, а ближайший комковый трафик — на 27–43 с позже (всплески 22:18:17–29, 22:19:45–55). Либо тайминг игрока расходится на десятки секунд, либо существует путь перехвата без строки GRAB-INTENT — из этих логов неразличимо.
- Инцидент хостового перехвата: симметричная механика существует (хост исполняет `THROW-INTENT ... mode=release(E)` и LAND COMMIT за клиента), но хостового перехвата E-нажатия в логах этой сессии нет — зафиксированы только клиентские перехваты.
- Ограничен ли матч конуса дистанцией досягаемости — взятие в 23:24:53 сматчило pile-прокси в ~3,1 м от рук; границ в логах не видно.
- Задумана ли двойная переноска (предмет в руке + висящий комок) — со слов игрока, нативный геймплей допускает один переносимый объект; из логов не выводится.

## Анализ

Мод переимплементирует взятие куч мусора для клиента как «grab-intent» по нажатию E, матчащийся **конусом камеры** с pile-прокси, с отменой нативного использования (`native use CANCELLED (no use_deny)`). Для игрока это невидимо — комок просто появляется в руках, будто был схвачен прицеленный объект, — что совпадает с наблюдением «без видимой логики» (выключатель, переносимые предметы, что угодно). Конус игнорирует реальную цель прицела, состояние рук и, фактически, дистанцию: в 23:24:53 он сматчил pile-прокси в **~3,1 м от рук**, при курсоре на другом интерактивном объекте. Пока клиент держал предмет добычи, перехваченные нажатия хватали комок мусора **помимо** удерживаемого предмета (двойная переноска — предмет остаётся в руке и ретранслируется всю перевозку комка). Следующее E расходуется как `release(E)` для комка: бросок, самосборка, NATIVIZATION в совершенно новую кучу мира, остающуюся у обоих пиров, — простое взаимодействие возле мусора засоряет окрестности кучами (три за 90 секунд в отчётном окне; больше в каждом позднем всплеске). Лог мода сам признаёт перехват (`paired E-release CANCELLED (its press was intercepted)`) — нажатие до намеченной цели не доходит вовсе. Перехват повторился 33+ раз за сессию; симметричная хостовая механика существует (`THROW-INTENT ... mode=release(E)`, `LAND COMMIT`), так что хосту, предположительно (гипотеза — хостовых инцидентов в этих логах нет), грозит то же самое. Направления: конус не должен перебивать более близкий проп под прицелом или любую непилевую цель; ограничить конус дистанцией досягаемости и требовать line-of-sight; отказывать во взятии кучи при занятой руке (или требовать явный режим); развязать `release(E)` от взаимодействия с предметами; рассмотреть слияние упавших комков с существующей кучей вместо минта нового актора кучи на каждое падение.

