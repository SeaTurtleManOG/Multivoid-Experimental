# Bug 08: Client consoles stay dead after host restores base power

## Metadata

| Field | Value |
|---|---|
| Video window | blackout `00:26:20` (synced OK); breakers `00:29:00` (synced OK); consoles noticed dead `00:31:42` |
| Log window | blackout `22:14:46`, breakers `22:17:24`–`22:17:26`, consoles `22:20:05` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | host (`Mikuzavr`) restores power; client (`PlayerNickname2`) observes dead consoles |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | `power` keyed mask sync vs native power-restoration event chain; no console/computer sync path exists |
| Power key | `power` panel key `'UrCgZUozHxXzTc5Ky5a9ZQ'` (1 live power panel) |

## Description

The story blackout event hit the base — it worked correctly on **both** peers, lights out everywhere (**player-visible; log-proven**: mask=0x00 applied + 11 `lightgroup: applied OFF ... ok=1`). The host then flipped the basement breakers — the switch states and per-room lights came back on for the client too, bit by bit, visibly correct (**log-proven**: every mask bit applied `ok=1`, named light switches applied ON). **But the main-room consoles never came back on for the client**: they show off screens, interacting with them gives no result, and the main computer won't boot either (**player-reported; consistent with the logs**: the client's E-presses in that window produce only bare `grab_hook[InpActEvt.use]` lines with no mod subsystem responding). On the host everything works (**player-reported**). Only the processing panel (панель обработки) is functional on the client (**player-reported**).

## Evidence

### 1. The blackout synced (this part works)

Host `21140: [22:14:47] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00`; client applied `mask=0x00 ok=1 (from slot 0)` (32216) + 11 `lightgroup: applied OFF ... ok=1` (32205–32215). Both peers went dark.

### 2. The breaker restoration synced too, bit by bit

```
host 22278: [22:17:24] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x08
host 22280: [22:17:25] power: sent ... mask=0x0A      host 22282: [22:17:25] mask=0x0B
host 22288: [22:17:26] power: sent ... mask=0x0F      host 22293: [22:17:26] mask=0x1F
client 33367–33382: [22:17:24–26] power: applied ... ok=1 (from slot 0)   ← every bit applied
```

Per-room lights followed (`light: applied ON key='lightswitch_staircase' ok=1` 22:17:42, `lightswitch_breakroom` 22:17:54, `lightswitch_sec1`/`sec2` 22:18:21, `lightswitch_security` 22:18:29, `lightswitch_garage` 22:18:34, `lightswitch_top` 22:22:13). The client confirms: breaker states displayed as ON.

### 3. The consoles did not follow

At 22:20:05 the client reports main-room consoles off (except the processing panel), interactions dead, main computer won't boot — while the host's consoles are up. The client's E-presses in that window produce only bare `grab_hook[InpActEvt.use]` lines (35344 at 22:20:08) with no mod subsystem responding — the presses reach the native game and produce nothing there, consistent with an off console being natively non-interactive.

### 4. No sync path exists for consoles

The mod's keyed-subsystem index (client log 615–628, 3824–3830): `door, light, lightgroup, container, garage, appliance, doorbox, keypad, power, atv, window, trash_pile` — **there is no console/computer subsystem at all**, and nothing in either log about console power state. The `console_state` messages that do exist are sky-row snapshots and sync fine (host broadcast gen 70–109 ↔ client applied gen 70–95, one-to-one) — unrelated to power.

## What the logs cannot show

- The native power-restoration chain itself (what exactly boots consoles and the main computer on the host) — console power state is not logged on either side; only the mask values and light applies are visible.
- Whether the client's consoles ever recovered later — no recovery trigger appears in either log; the reviewed window ends with the client's workstation row still dead.
- Whether the processing panel works because it is an appliance-class keyed entity — our inference from the appliance keysHash matching between peers; the panel's internal class is not logged.

## Analysis

The mod synced the power panel's mask *value* (and the switch states), and those applies succeeded (`ok=1`). But the native power-restoration chain — which on the host boots the consoles and the main computer (that host-side behavior is player-reported; the chain itself is not logged) — apparently never ran on the client (inference: the dead consoles are player-reported, the un-answered E-presses are log-proven): the client's world received switch **states** without the switch **events**, so its consoles remain in the pre-outage (off) state, and nothing reacts to E. This is the "state syncs, native event effects don't" class, same as [Bug 06 «Client breaks a container — host never sees the spilled loot»](https://discordapp.com/channels/1508397685726646272/1546191405670473778) (wire destroy skips native death effects). The processing panel working is consistent with it being an appliance-class keyed entity — `appliance` is one of the mod's keyed subsystems and notably the only world-load keysHash that **matches** between peers (`0x7FBD8FC70EF48EA4` on both, client 3828 / host 1877) — while consoles/computer have no sync path and no native trigger. Practical consequence: after any power event, the client's workstation row is dead until some unspecified action (none found in this session — it stayed dead through the reviewed window). Suggested directions: after applying a power mask change, re-emit (or simulate) the native power-restoration event on the client; or sync console/computer power as keyed state like appliances; or let the client "re-flip" already-on breakers natively to trigger its own boot chain.

---

# Баг 08: Консоли клиента остаются мёртвыми после восстановления питания хостом

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | отключение `00:26:20` (синхронизировалось ОК); рубильники `00:29:00` (синхронизировалось ОК); мёртвые консоли замечены `00:31:42` |
| Окно лога | отключение `22:14:46`, рубильники `22:17:24`–`22:17:26`, консоли `22:20:05` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | хост (`Mikuzavr`) восстанавливает питание; клиент (`PlayerNickname2`) наблюдает мёртвые консоли |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | keyed-синхронизация маски `power` против нативной цепочки восстановления питания; пути синхронизации консолей/компьютера не существует |
| Ключ питания | ключ панели `power` `'UrCgZUozHxXzTc5Ky5a9ZQ'` (1 живая панель питания) |

## Описание

Сюжетное событие отключения света ударило по базе — сработало корректно **у обоих** пиров, свет погас везде (**видно игрокам; доказано логом**: применён mask=0x00 + 11 `lightgroup: applied OFF ... ok=1`). Затем хост включила рубильники в подвале — состояния рубильников и свет по комнатам вернулись и у клиента, побитово, визуально корректно (**доказано логом**: каждый бит маски применён `ok=1`, именованные выключатели света применены ON). **Но консоли в главной комнате у клиента так и не ожили**: экраны выключены, взаимодействие не даёт результата, главный компьютер тоже не включается (**со слов игрока; согласуется с логами**: нажатия E клиента в этом окне дают только голые строки `grab_hook[InpActEvt.use]`, ни одна подсистема мода не отвечает). У хоста всё работает (**со слов игрока**). Работает только панель обработки (**со слов игрока**).

## Доказательства

### 1. Отключение синхронизировалось (эта часть работает)

Хост `21140: [22:14:47] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00`; клиент применил `mask=0x00 ok=1 (from slot 0)` (32216) + 11 × `lightgroup: applied OFF ... ok=1` (32205–32215). Оба пира погасли.

### 2. Восстановление рубильниками тоже синхронизировалось, побитово

```
host 22278: [22:17:24] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x08
host 22280: [22:17:25] power: sent ... mask=0x0A      host 22282: [22:17:25] mask=0x0B
host 22288: [22:17:26] power: sent ... mask=0x0F      host 22293: [22:17:26] mask=0x1F
client 33367–33382: [22:17:24–26] power: applied ... ok=1 (from slot 0)   ← применён каждый бит
```

Свет по комнатам последовал (`light: applied ON key='lightswitch_staircase' ok=1` 22:17:42, `lightswitch_breakroom` 22:17:54, `lightswitch_sec1`/`sec2` 22:18:21, `lightswitch_security` 22:18:29, `lightswitch_garage` 22:18:34, `lightswitch_top` 22:22:13). Клиент подтверждает: состояния рубильников отображаются как ON.

### 3. Консоли не последовали

В 22:20:05 клиент сообщает: консоли главной комнаты выключены (кроме панели обработки), взаимодействия мертвы, главный компьютер не загружается — тогда как консоли хоста работают. Нажатия E клиента в этом окне дают только голые строки `grab_hook[InpActEvt.use]` (35344 в 22:20:08), ни одна подсистема мода не отвечает — нажатия доходят до нативной игры и не дают результата, что согласуется с тем, что выключенная консоль нативно неинтерактивна.

### 4. Пути синхронизации для консолей не существует

Индекс ключевых подсистем мода (лог клиента 615–628, 3824–3830): `door, light, lightgroup, container, garage, appliance, doorbox, keypad, power, atv, window, trash_pile` — **подсистемы консолей/компьютера нет вовсе**, и в обоих логах нет ничего о состоянии питания консолей. Существующие сообщения `console_state` — снапшоты неба и синхронизируются нормально (хост broadcast gen 70–109 ↔ клиент applied gen 70–95, один-к-одному) — к питанию отношения не имеют.

## Чего логи показать не могут

- Саму нативную цепочку восстановления питания (что именно включает консоли и главный компьютер на хосте) — состояние питания консолей не логируется ни на одной из сторон; видны только значения маски и применения света.
- Ожили ли консоли клиента позже — триггера восстановления ни в одном логе нет; рассмотренное окно заканчивается при всё ещё мёртвом рабочем ряду клиента.
- Работает ли панель обработки потому, что она keyed-сущность класса appliance — наш вывод из совпадения appliance keysHash между пирами; внутренний класс панели не логируется.

## Анализ

Мод синхронизировал *значение* маски панели питания (и состояния рубильников), и все применения прошли успешно (`ok=1`). Но нативная цепочка восстановления питания — которая у хоста включает консоли и главный компьютер (работа консолей хоста — со слов игрока; сама цепочка не логируется) — у клиента, по-видимому, не выполнялась (вывод: мёртвые консоли — со слов игрока, неотвеченные нажатия E — доказаны логом): мир клиента получил **состояния** рубильников без **событий** их переключения, поэтому его консоли остаются в доаварийном (выключенном) состоянии и никак не реагируют на E. Это класс «состояние синхронизируется, нативные эффекты события — нет», тот же, что в [Баг 06 «Клиент ломает контейнер — хост не видит выпавшую добычу»](https://discordapp.com/channels/1508397685726646272/1546191405670473778) (сетевой destroy пропускает нативные эффекты смерти). Работающая панель обработки согласуется с тем, что она — keyed-сущность класса appliance: `appliance` — одна из ключевых подсистем мода и, заметим, единственная keysHash мировой загрузки, **совпадающая** между пирами (`0x7FBD8FC70EF48EA4` у обоих, клиент 3828 / хост 1877), тогда как у консолей/компьютера нет ни пути синхронизации, ни нативного триггера. Практическое следствие: после любого события питания рабочий ряд клиента мёртв до неустановленного действия (в рассмотренном окне сессии он так и не ожил). Направления: после применения изменения маски питания переиздавать (или эмулировать) на клиенте нативное событие восстановления питания; либо синхронизировать питание консолей/компьютера как keyed-состояние, как у appliance; либо позволить клиенту «перещёлкнуть» уже поднятые рубильники нативно, чтобы запустить свою цепочку загрузки.

