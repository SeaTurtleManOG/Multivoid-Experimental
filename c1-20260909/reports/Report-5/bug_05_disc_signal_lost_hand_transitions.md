# Bug 05: Recorded signal lost when disc leaves console; re-inserted disc invisible to client

## Metadata

| Field | Value |
|---|---|
| Video window | `00:12:39`–`00:13:15` (+ aftereffects to `00:13:21`) |
| Log window | `22:01:02`–`22:01:44` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`); host (`Mikuzavr`) participated in the experiment |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | `drive_sync` (recorded payload) vs `hand_item`/`remote_prop` actor generations + re-insert fuzzy match |
| Disc identity | `prop_drive_C`, wire key `'2OekYEdv0NU9ziyN64IVBQ'`, eid drift 3517 → 5279 → 5280 |
| Payload | `eid=3517 (name='планета [1]' size=1)` |

## Description

Experiment with recording a signal to a disc. The client inserted an empty disc (red indicator) into the recording console and recorded the loaded signal — the indicator turned green **on both peers** (indicator colors as seen on screens; every underlying step is log-proven below). The client then took the disc in hand — the indicator turned **red for the host** (signal gone). The client put the disc on a table; the host picked it up — now the indicator turned **red for the client** too. Finally the host inserted the disc into the console — **the client sees no disc in the console at all**. The logs confirm every step and expose the mechanism: the disc's recorded state is attached to its *world-actor generation*, and each hand transition destroys that generation.

## Evidence

### 1. T0 — 22:01:06: recording succeeded, green on both peers

```
host 11317: [22:01:06] drive_sync: payload applied eid=3517 (name='планета [1]' size=1) from slot 1
```

### 2. T1 — 22:01:09: client takes disc in hand → host's indicator red

```
client 22406: [22:01:09] hand_item: local hand -> cls='prop_drive_C' name='drive' (announced, rel measured)
```

No drive payload event follows the disc out of the console — the payload stays with the actor generation it was applied to (`eid=3517`), which the hand transition kills. The held disc is only a view-anchored mirror (`hand_item: ... display-only, view-anchored`) with no state.

### 3. T2 — 22:01:16: client puts disc on the table

```
client 22450: [22:01:16] hand_item: local hand -> EMPTY (announced)
client 22457: [22:01:16] remote_prop::OnSpawn: key '2OekYEdv0NU9ziyN64IVBQ' resolves to live actor 000001878B392740 -- diverged (d=2.1cm), converging transform to host
client 22458: [22:01:16] sync::CreateOrAdoptPropMirror: eid=5279 bound to actor=000001878B392740 ...
```

The disc's world identity is now generation `eid=5279` — a *new* actor binding, carrying **no drive payload** (red for anyone who looks, including the host on pickup).

### 4. T3 — 22:01:21–27: host picks the disc up → client's indicator red

```
client 22485: [22:01:21] remote_prop: slot 0 GRAB-IN key='2OekYEdv0NU9ziyN64IVBQ' eid=5279 -> local actor=000001878B392740 (Aprop physics-off)
client 22527: [22:01:27] hand_item: slot 0 mirror SPAWNED cls='prop_drive_C' name='drive' (display-only, view-anchored)
client 22525: [22:01:27] remote_prop::OnDestroy: key '2OekYEdv0NU9ziyN64IVBQ' eid=5279 -> destroying local actor 000001878B392740
```

### 5. T4 — 22:01:30: host inserts the disc into the console → client cannot see it

```
client 22567: [22:01:30] hand_item: slot 0 mirror destroyed (hand now empty)
client ———: [22:01:30] remote_prop::OnDestroy: key '2OekYEdv0NU9ziyN64IVBQ' eid=0 no local actor in STEADY state -- no-op drop   (×2 — eid-less destroys, unappliable)
client 22561: [22:01:30] remote_prop::OnSpawn: cls='prop_drive_C' key='2OekYEdv0NU9ziyN64IVBQ' name='drive' loc=(216.7, 1936.8, 6369.8)
client 22562: [22:01:30] remote_prop::OnSpawn: Gap-I-1 FUZZY MATCH 'prop_drive_C' -> existing actor 0000018750BC53E0 within 30.0 cm -- de-duping, converging transform + rekeying
client 22564: [22:01:30] sync::CreateOrAdoptPropMirror: eid=5280 bound to actor=0000018750BC53E0 key='2OekYEdv0NU9ziyN64IVBQ'
client 22566: [22:01:30] grab_hook[destroy-seam]: actor 0000018750BC53E0 was wire-received destroy -- skip rebroadcast
```

The freshly bound disc actor (`eid=5280`) is destroyed **by wire in the same second** it was bound.

### 6. T5 — 22:01:41–44: the disc is now unrepresentable on the client

```
client 22623: [22:01:41] [WARN] drive_sync: pending apply kind=0 expired (actor never resolved)
client 22637+: [22:01:44] [WARN] remote_prop: slot 0 incoming PropPose key '2OekYEdv0NU9ziyN64IVBQ' eid=5280 -- no local match (key or eid)   (flooding ~13+/s)
```

The host's drive payload for the re-inserted disc expires unapplied, and the host's pose stream for the disc-in-console matches nothing on the client — **the console appears empty to the client** while the host sees the disc inside.

## What the logs cannot show

- Where the payload physically lives (actor property vs a side table keyed by generation) — the generation binding is inferred from the eid churn + expiry behavior.
- Why the OnSpawn fuzzy match picked an actor that was wire-doomed the same second — the matching internals are not logged.
- Whether per-generation payload binding is intentional — not derivable from logs.
- Whether the two eid-less `OnDestroy eid=0` drops at 22:01:30 travel the same eid-less destroy path seen in the floppy bug [Bug 03 «Floppy extracted from server dissolves in mid-air»](https://discordapp.com/channels/1508397685726646272/1545093214191624262) — same signature, but no log line ties them together.

## Analysis

The recorded state (drive payload) is bound to the disc's *world-actor generation* (the binding itself is our inference from the eid churn + expiry behavior; the event chain is log-proven), but the mod's held-item model destroys the world actor whenever a player picks the prop up (held items are `display-only, view-anchored` mirrors). Every pickup therefore orphans the state; every put-down re-creates the prop as a new generation (`eid` churn 3517 → 5279 → 5280) with empty state — exactly the red-indicator flips both players saw. The final insert re-spawned the disc client-side, but the rebind raced a wire destroy (bound and destroyed the same second), after which neither the drive payload (`pending apply ... actor never resolved`) nor the pose stream (`no local match (key or eid)`) could attach — the disc is invisible to the client from then on. Suggested direction: attach drive payload state to the logical item identity (wire key / a persistent item id) rather than the actor generation, and preserve the payload across hand transitions (re-emit it on release/spawn), plus resolve the spawn-vs-wire-destroy race on the re-insert path.

Side note: the `no local match` pose flood at 22:01:44+ is the same signature as the massive background `remote_prop ... no local match` spam present throughout the session logs — prolonged desyncs of this class are its likely source (hypothesis).

---

# Баг 05: Записанный сигнал теряется при изъятии диска из консоли; вставленный обратно диск для клиента невидим

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `00:12:39`–`00:13:15` (+ последствия до `00:13:21`) |
| Окно лога | `22:01:02`–`22:01:44` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`); хост (`Mikuzavr`) участвовала в эксперименте |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | `drive_sync` (записанный payload) против поколений акторов `hand_item`/`remote_prop` + fuzzy-матч повторной вставки |
| Идентичность диска | `prop_drive_C`, wire-ключ `'2OekYEdv0NU9ziyN64IVBQ'`, дрейф eid 3517 → 5279 → 5280 |
| Payload | `eid=3517 (name='планета [1]' size=1)` |

## Описание

Эксперимент с записью сигнала на диск. Клиент вставил пустой диск (красный индикатор) в консоль записи и записал загруженный сигнал — индикатор стал зелёным **у обоих** (цвета индикаторов — с экранов; каждый нижележащий шаг доказан логом ниже). Клиент взял диск в руку — индикатор стал **красным у хоста** (сигнал пропал). Клиент положил диск на стол; хост взяла его — теперь индикатор стал **красным и у клиента**. Наконец хост вставила диск в консоль — **клиент вообще не видит диска в консоли**. Логи подтверждают каждый шаг и вскрывают механизм: записанное состояние диска привязано к *поколению его мирового актора*, а каждое взятие в руку это поколение уничтожает.

## Доказательства

### 1. T0 — 22:01:06: запись удалась, зелёный у обоих

```
host 11317: [22:01:06] drive_sync: payload applied eid=3517 (name='планета [1]' size=1) from slot 1
```

### 2. T1 — 22:01:09: клиент берёт диск в руку → индикатор хоста красный

```
client 22406: [22:01:09] hand_item: local hand -> cls='prop_drive_C' name='drive' (announced, rel measured)
```

За диском из консоли не следует никакого drive payload события — payload остаётся с поколением актора, к которому был применён (`eid=3517`), а взятие в руку это поколение убивает. Держимый диск — лишь view-anchored зеркало (`hand_item: ... display-only, view-anchored`) без состояния.

### 3. T2 — 22:01:16: клиент кладёт диск на стол

```
client 22450: [22:01:16] hand_item: local hand -> EMPTY (announced)
client 22457: [22:01:16] remote_prop::OnSpawn: key '2OekYEdv0NU9ziyN64IVBQ' resolves to live actor 000001878B392740 -- diverged (d=2.1cm), converging transform to host
client 22458: [22:01:16] sync::CreateOrAdoptPropMirror: eid=5279 bound to actor=000001878B392740 ...
```

Мировая идентичность диска теперь — поколение `eid=5279`, *новая* привязка актора, **без drive payload** (красный для любого смотрящего, включая хоста при взятии).

### 4. T3 — 22:01:21–27: хост берёт диск → индикатор клиента красный

```
client 22485: [22:01:21] remote_prop: slot 0 GRAB-IN key='2OekYEdv0NU9ziyN64IVBQ' eid=5279 -> local actor=000001878B392740 (Aprop physics-off)
client 22527: [22:01:27] hand_item: slot 0 mirror SPAWNED cls='prop_drive_C' name='drive' (display-only, view-anchored)
client 22525: [22:01:27] remote_prop::OnDestroy: key '2OekYEdv0NU9ziyN64IVBQ' eid=5279 -> destroying local actor 000001878B392740
```

### 5. T4 — 22:01:30: хост вставляет диск в консоль → клиент его не видит

```
client 22567: [22:01:30] hand_item: slot 0 mirror destroyed (hand now empty)
client ———: [22:01:30] remote_prop::OnDestroy: key '2OekYEdv0NU9ziyN64IVBQ' eid=0 no local actor in STEADY state -- no-op drop   (×2 — destroy без eid, неприменимые)
client 22561: [22:01:30] remote_prop::OnSpawn: cls='prop_drive_C' key='2OekYEdv0NU9ziyN64IVBQ' name='drive' loc=(216.7, 1936.8, 6369.8)
client 22562: [22:01:30] remote_prop::OnSpawn: Gap-I-1 FUZZY MATCH 'prop_drive_C' -> existing actor 0000018750BC53E0 within 30.0 cm -- de-duping, converging transform + rekeying
client 22564: [22:01:30] sync::CreateOrAdoptPropMirror: eid=5280 bound to actor=0000018750BC53E0 key='2OekYEdv0NU9ziyN64IVBQ'
client 22566: [22:01:30] grab_hook[destroy-seam]: actor 0000018750BC53E0 was wire-received destroy -- skip rebroadcast
```

Свежезабинженный актор диска (`eid=5280`) уничтожен **по сети в ту же секунду**, что и забинжен.

### 6. T5 — 22:01:41–44: диск теперь непредставим на клиенте

```
client 22623: [22:01:41] [WARN] drive_sync: pending apply kind=0 expired (actor never resolved)
client 22637+: [22:01:44] [WARN] remote_prop: slot 0 incoming PropPose key '2OekYEdv0NU9ziyN64IVBQ' eid=5280 -- no local match (key or eid)   (флуд ~13+/с)
```

Drive payload хоста для повторно вставленного диска истекает непримененным, а поток поз хоста для диска-в-консоли не матчится ни с чем на клиенте — **консоль для клиента выглядит пустой**, тогда как хост видит диск внутри.

## Чего логи показать не могут

- Где физически хранится payload (свойство актора против боковой таблицы по поколению) — привязка к поколению выведена из дрейфа eid и поведения протухания.
- Почему fuzzy-матч OnSpawn выбрал актора, обречённого сетевым destroy в ту же секунду — внутренности матчинга не логируются.
- Намеренная ли привязка payload к поколению — из логов не выводится.
- Ходят ли два сообщения `OnDestroy eid=0` из 22:01:30 тем же путём destroy без eid, что и в баге с дискетой [Баг 03 «Дискета, извлечённая из сервера, растворяется в воздухе»](https://discordapp.com/channels/1508397685726646272/1545093214191624262), — сигнатура та же, но ни одна строка лога их не связывает.

## Анализ

Записанное состояние (drive payload) привязано к *поколению мирового актора* диска (само привязывание — наш вывод из дрейфа eid и поведения протухания; цепочка событий доказана логом), но модель держимых предметов мода уничтожает мировой актор при каждом взятии пропа в руку (держимый предмет — только `display-only, view-anchored` зеркало). Каждое взятие сиротит состояние; каждое опускание пересоздаёт проп новым поколением (`eid` дрейфует 3517 → 5279 → 5280) с пустым состоянием — ровно те переключения индикатора на красный, что видели оба игрока. Финальная вставка пересоздала диск на клиенте, но перепривязка проиграла гонке сетевому destroy (бинд и destroy в одну секунду), после чего ни drive payload (`pending apply ... actor never resolved`), ни поток поз (`no local match (key or eid)`) не смогли привязаться — диск для клиента невидим. Направление исправления: привязывать состояние drive payload к логической идентичности предмета (wire key / персистентный id), а не к поколению актора; сохранять payload через переходы рука↔мир (переиздавать при release/spawn); устранить гонку спавна и сетевого destroy на пути повторной вставки.

Попутное примечание: флуд `no local match` с 22:01:44 — та же сигнатура, что у массового фонового спама `remote_prop ... no local match`, присутствующего в логах всей сессии; затяжные рассинхроны этого класса — его вероятный источник (гипотеза).

