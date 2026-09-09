# Bug 01: Spurious "caught signal" event fires on client connect

## Metadata

| Field | Value |
|---|---|
| Video window | `00:01:07` (feed visible `00:01:02`–`00:01:13`); recurrence at video `01:00:31` (22:48:54) |
| Log window | `21:49:25`–`21:49:36`; recurrence `22:48:53`–`22:49:00` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | `signal_catch` (client-side catch detector) × `dish_sync` (join baseline slew) + feed |
| Repro count | **×2** — initial join (21:49:25) and re-join (22:48:54); later mid-game catches confirmed intentional by player |

## Description

At the moment the client connected to the host's world, a signal-catch event fired in the world feed: `PlayerNickname2 caught signal 'mars_bday'`. The client had not even finished loading into the world (pawn appearance was announced 10 s later, at 21:49:35 — log-proven). **Player-reported:** neither player initiated a signal search. The event is log-proven on both peers; the absence of intent is player-confirmed.

## Evidence

### 1. First join: the detector fires mid world-load storm (client)

21:49:25, surrounded by `grab_hook[destroy-seam]` suppressions and `world spawn-refusal window` churn (`client_040920262246_multivoid.log`):

```
2842: [21:49:25] [INFO ] signal_catch: local catch detected ('mars_bday' at 6514,2025,0; slewValid=1) -- relayed
2843: [21:49:25] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'mars_bday
2844: [21:49:25] [INFO ] dish_sync: own-ping kill sweep -- 0 slew(s) killed, 0 skipped (host owns the theater)
2849: [21:49:25] [INFO ] dish_sync: client ticker_disher parked (timer cleared)
2867: [21:49:25] [INFO ] [dish] 0 '?' mirror slew START (yaw=246.7 roll=96.5)
      ... 24 × mirror slew START in the same second, all dish names unresolved ('?') client-side
```

### 2. Re-join repeats the pattern 1 s after detector install (client)

`client_050920260005_multivoid.log`:

```
 555: [22:48:53] [INFO ] signal_catch: installed (desk surface resolved; L4 tuple detector)
2830: [22:48:54] [INFO ] signal_catch: local catch detected ('whiteDwarf_0' at 4308,3003,0; slewValid=1) -- relayed
2831: [22:48:54] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'whiteDwar
4007: [22:49:00] [INFO ] signal_catch: catch identity applied ('whiteDwarf_0') -- host owns the theater
```

### 3. The host applies the catch and ships the connect seed

`host_050920260342_multivoid.log`:

```
4555: [21:49:25] [INFO ] signal_catch: catch replay applied ('mars_bday', 24 dish(es) slewing)
4556: [21:49:25] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'mars_bday
4637: [21:49:29] [INFO ] signal_catch: connect catch seed (kind=2) -> slot 1 ('mars_bday', slewValid=1)
42402: [22:49:01] [INFO ] signal_catch: connect catch seed (kind=2) -> slot 1 ('whiteDwarf_0', slewValid=1)
```

(24 host-side `mirror slew START` lines follow at 21:49:25, dishes properly named 'bravo', 'charlie', ...)

## What the logs cannot show

- The detector's internals — why the baseline slew transient crosses its tolerance window — are not logged; the mechanism below is a hypothesis, the trigger correlation is proven.
- Whether feed push is echo-suppressed between peers cannot be judged from a single client's log.
- Whether suppressing the detector until `connect catch seed` / `catch identity applied` arrives would prevent the false fire cannot be verified here — both recorded joins fired in the same second as the slew storm, before the seed arrived.

## Analysis

On every connect, the joiner's client broadcasts a "local catch" of whatever signal is currently catchable in the world, and both peers push the feed message `<joiner> caught signal '<name>'`. Timeline (initial join): client session starts 21:48:57 → world-load churn 21:49:24–25 → all 24 dish mirrors slew to baseline at once (client sees them as `'?')` → in the **same second** the client's `signal_catch` reports `local catch detected` → relayed to host → host applies `catch replay` and pushes the feed message → at 21:49:29 host `connect catch seed (kind=2)` ships the catch state to the joiner → joiner's pawn appears 21:49:35. On re-join (22:48) the same sequence repeats 1 s after `signal_catch: installed`. Hypothesis (not proven from logs): the join-time baseline slew storm sweeps dish bearings across the active signal's tolerance window, the client-side catch detector treats that transient as a genuine player catch, and the mod relays/broadcasts it. Client dish tickers are parked at that moment (`host owns the theater`), so the slew is sync-initialized, not player input.

Side note: two other catches in the session (`whiteDwarf_0` at 22:03:30, `neptune` at 23:58:08) share the same `local catch detected -- relayed` signature but were confirmed by the player as intentional dish usage — not part of this bug. The spurious fire is specific to the connect/re-connect window.

---

# Баг 01: Ложное событие поимки сигнала при подключении клиента

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `00:01:07` (фид виден `00:01:02`–`00:01:13`); рецидив на видео `01:00:31` (22:48:54) |
| Окно лога | `21:49:25`–`21:49:36`; рецидив `22:48:53`–`22:49:00` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | `signal_catch` (клиентский детектор поимки) × `dish_sync` (базовый slew при входе) + фид |
| Повторы | **×2** — первый вход (21:49:25) и повторный вход (22:48:54); поздние поимки посреди игры подтверждены игроком как намеренные |

## Описание

В момент подключения клиента к хосту в мире сработало событие поимки сигнала: `PlayerNickname2 caught signal 'mars_bday'`. Клиент даже не успел догрузиться в мир (появление аватара анонсировано на 10 секунд позже, в 21:49:35 — доказано логом). **Со слов игрока:** никто из игроков не инициировал поиск сигнала. Само событие доказано логами обоих пиров; отсутствие намерения подтверждено игроком.

## Доказательства

### 1. Первый вход: детектор срабатывает посреди шторма загрузки мира (клиент)

21:49:25, окружен подавлениями `grab_hook[destroy-seam]` и чурном `world spawn-refusal window` (`client_040920262246_multivoid.log`):

```
2842: [21:49:25] [INFO ] signal_catch: local catch detected ('mars_bday' at 6514,2025,0; slewValid=1) -- relayed
2843: [21:49:25] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'mars_bday
2844: [21:49:25] [INFO ] dish_sync: own-ping kill sweep -- 0 slew(s) killed, 0 skipped (host owns the theater)
2849: [21:49:25] [INFO ] dish_sync: client ticker_disher parked (timer cleared)
2867: [21:49:25] [INFO ] [dish] 0 '?' mirror slew START (yaw=246.7 roll=96.5)
      ... 24 × mirror slew START в ту же секунду, все имена тарелок на клиенте не разрешены ('?')
```

### 2. Повторный вход повторяет паттерн через 1 с после установки детектора (клиент)

`client_050920260005_multivoid.log`:

```
 555: [22:48:53] [INFO ] signal_catch: installed (desk surface resolved; L4 tuple detector)
2830: [22:48:54] [INFO ] signal_catch: local catch detected ('whiteDwarf_0' at 4308,3003,0; slewValid=1) -- relayed
2831: [22:48:54] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'whiteDwar
4007: [22:49:00] [INFO ] signal_catch: catch identity applied ('whiteDwarf_0') -- host owns the theater
```

### 3. Хост применяет поимку и шлёт connect-сид

`host_050920260342_multivoid.log`:

```
4555: [21:49:25] [INFO ] signal_catch: catch replay applied ('mars_bday', 24 dish(es) slewing)
4556: [21:49:25] [INFO ] feed: push via=action keep=history nickLen=15 lines=1 text="PlayerNickname2 caught signal 'mars_bday
4637: [21:49:29] [INFO ] signal_catch: connect catch seed (kind=2) -> slot 1 ('mars_bday', slewValid=1)
42402: [22:49:01] [INFO ] signal_catch: connect catch seed (kind=2) -> slot 1 ('whiteDwarf_0', slewValid=1)
```

(далее 24 хостовых строки `mirror slew START` в 21:49:25, тарелки корректно именованы: 'bravo', 'charlie', ...)

## Чего логи показать не могут

- Внутренности детектора — почему транзиент базового slew проходит его окно допуска — не логируются; механизм ниже — гипотеза, корреляция триггера доказана.
- Подавляется ли эхо feed-рассылки между пирами, по логу одного клиента судить нельзя.
- Проверить, устранило бы ложное срабатывание взведение детектора только после `connect catch seed` / `catch identity applied`, по этим логам нельзя — оба записанных входа сработали в ту же секунду, что и шторм slew, ещё до прихода сида.

## Анализ

При каждом подключении клиент присоединяющегося транслирует «локальную поимку» того сигнала, который в данный момент ловится в мире, и оба пира публикуют в фид сообщение `<joiner> caught signal '<имя>'`. Хронология (первый вход): клиент стартовал 21:48:57 → чурн загрузки мира 21:49:24–25 → все 24 зеркала тарелок разом уходят в базовый slew (у клиента они еще не именованы — `'?'`) → в **ту же секунду** `signal_catch` клиента репортит `local catch detected` → ретранслирует хосту → хост применяет `catch replay` и публикует сообщение в фид → в 21:49:29 хост шлет `connect catch seed (kind=2)` → аватар игрока появляется 21:49:35. При переподключении (22:48) та же последовательность повторяется через 1 секунду после `signal_catch: installed`. Гипотеза (логами не подтверждена): присоединительный «шторм» базовых slew при загрузке проводит ориентации тарелок через допуск активного сигнала, клиентский детектор принимает этот транзиент за настоящую поимку игроком, и мод транслирует её. Тикеры тарелок клиента в этот момент припаркованы (`host owns the theater`), т.е. slew — это синхронизационная инициализация, а не ввод игрока.

Попутное примечание: две другие поимки в сессии (`whiteDwarf_0` в 22:03:30, `neptune` в 23:58:08) имеют ту же сигнатуру `local catch detected -- relayed`, но подтверждены игроком как намеренное использование тарелок — к этому багу не относятся. Ложное срабатывание специфично для окна подключения/переподключения.

