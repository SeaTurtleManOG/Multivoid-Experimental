# Bug 04: Client light switch does not work — cross-peer keysHash mismatch on lights/lightgroups

## Metadata

| Field | Value |
|---|---|
| Video window | `00:03:05`–`00:03:08` |
| Log window | `21:51:28`–`21:51:57` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | `light` / `lightgroup` keyed indexes + deferred-apply retry queue |
| Verdict | **Confirmed bug** — per-peer key derivation, same root cause as the door report [Bug 02 «Client cannot open radio telescope door — cross-peer door key mismatch»](https://discordapp.com/channels/1508397685726646272/1546182816767938710), proven for lights + lightgroups |
| Switch key | `'BqgD3X87kKv8T_DyLebx2g'` — 0 occurrences in the host log |

## Description

The client flipped a light switch — nothing happened (**player action, log-proven**: three `light: sent ON/OFF` lines in 3 seconds; **player-reported**: no visible effect). Investigation shows this is a real bug with the same root cause as the door report: the light (and lightgroup) keyed index diverges between peers after world load, so hash-keyed switch messages reference keys the other peer has never seen.

## Evidence

### 1. Light index keysHash mismatch after world load

Same shape as doors:

```
host   1874: [21:48:52] light: index rebuilt -- 42 live keyed instance(s), keysHash=0xEB28F0E05D23216A
host   1875: [21:48:52] lightgroup: index rebuilt -- 42 ... keysHash=0xC6B3EC5CEF12BC9E
client 3825: [21:49:28] light: index rebuilt -- 42 live keyed instance(s), keysHash=0x080690C6C8B01627
client 3826: [21:49:28] lightgroup: index rebuilt -- 42 ... keysHash=0x96517D838BD354E1
```

(Pre-load 15-instance menu index matches on both peers: `0x125425EF3DB15927` / `0x55CFFE39361F71BF`.)

### 2. The user's switch presses reference a key unknown to the host

`client_040920262246_multivoid.log`:

```
16358: [21:51:28] light: sent ON  key='BqgD3X87kKv8T_DyLebx2g'
16367: [21:51:28] light: sent OFF key='BqgD3X87kKv8T_DyLebx2g'
16447: [21:51:31] light: sent ON  key='BqgD3X87kKv8T_DyLebx2g'
```

`BqgD3X87kKv8T_DyLebx2g` appears **0 times** in the host log. The host's light retry queue then reports the message dying unapplied (`host_050920260342_multivoid.log`):

```
6261: [21:51:57] light: retry tick -- applied 0 deferred, dropped 1 expired, 0 still pending
```

### 3. Join-time light sync is broken the same way

The host's connect snapshot used host-side keys the client could not match:

```
client 15411: [21:49:55] light: retry tick -- applied 0 deferred, dropped 27 expired, 0 still pending
```

### 4. Stable-named switches work

Same split as `basedoor_*` doors vs hash-keyed doors:

```
client 20513: [21:58:58] light: sent ON key='lightswitch_outside'
```

## What the logs cannot show

- The host's drop of the client's messages is proven only via the retry-tick summary (`dropped 1 expired`) — no per-message WARN exists, so the exact drop moment is not pin-pointed.
- Whether the reverse direction (host→client state broadcasts for hash-keyed lights) failed for this specific switch is inferred from the 27-expired join-snapshot burst, not observed per-message.
- Whether a louder per-message WARN exists but is disabled — expiry is reported only by the quiet retry-tick summary line.

## Analysis

Identical to the door bug, one subsystem over: per-peer post-load key derivation is not deterministic, so hash-keyed entities (doors, lights, lightgroups) cannot be addressed cross-peer. Client→host switch messages defer and expire (`applied 0, dropped N expired`); host→client state broadcasts presumably suffer the same fate in reverse (hypothesis — not observed per-message for this switch). The 27-expired burst at 21:49:55 shows the connect snapshot itself is largely unappliable for hash-keyed lights. Impact: every hash-keyed light switch is client-inoperable, and initial light states may be wrong after join. This generalizes the door bug's root cause — a fix in the key derivation (stable per-instance identity) would plausibly cover doors, lights and lightgroups at once (player-side inference, not a code reading).

---

# Баг 04: Выключатель света у клиента не работает — рассинхрон keysHash света/lightgroup между пирами

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `00:03:05`–`00:03:08` |
| Окно лога | `21:51:28`–`21:51:57` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | keyed-индексы `light` / `lightgroup` + retry-очередь отложенного применения |
| Вердикт | **Подтверждённый баг** — по-пирная деривация ключей, та же первопричина, что в отчёте про двери [Баг 02 «Клиент не может открыть дверь радиотелескопа — рассинхрон ключей дверей между пирами»](https://discordapp.com/channels/1508397685726646272/1546182816767938710), доказано для света + lightgroup |
| Ключ выключателя | `'BqgD3X87kKv8T_DyLebx2g'` — 0 вхождений в логе хоста |

## Описание

Клиент щёлкнул выключателем света — ничего не произошло (**действие игрока, доказано логом**: три строки `light: sent ON/OFF` за 3 секунды; **со слов игрока**: видимого эффекта нет). Проверка показала: это реальный баг с той же первопричиной, что и в отчёте про двери, — индекс ключей света (и lightgroup) расходится между пирами после загрузки мира, поэтому сообщения хэш-ключевых выключателей ссылаются на ключи, которых другой пир никогда не видел.

## Доказательства

### 1. Несовпадение keysHash индекса света после загрузки мира

Та же картина, что у дверей:

```
host   1874: [21:48:52] light: index rebuilt -- 42 live keyed instance(s), keysHash=0xEB28F0E05D23216A
host   1875: [21:48:52] lightgroup: index rebuilt -- 42 ... keysHash=0xC6B3EC5CEF12BC9E
client 3825: [21:49:28] light: index rebuilt -- 42 live keyed instance(s), keysHash=0x080690C6C8B01627
client 3826: [21:49:28] lightgroup: index rebuilt -- 42 ... keysHash=0x96517D838BD354E1
```

(До-загрузочный индекс меню из 15 инстансов у обоих пиров совпадает: `0x125425EF3DB15927` / `0x55CFFE39361F71BF`.)

### 2. Нажатия выключателя игроком ссылаются на ключ, хосту неизвестный

`client_040920262246_multivoid.log`:

```
16358: [21:51:28] light: sent ON  key='BqgD3X87kKv8T_DyLebx2g'
16367: [21:51:28] light: sent OFF key='BqgD3X87kKv8T_DyLebx2g'
16447: [21:51:31] light: sent ON  key='BqgD3X87kKv8T_DyLebx2g'
```

`BqgD3X87kKv8T_DyLebx2g` встречается в логе хоста **0 раз**. Retry-очередь света хоста затем сообщает о смерти сообщения непримененным (`host_050920260342_multivoid.log`):

```
6261: [21:51:57] light: retry tick -- applied 0 deferred, dropped 1 expired, 0 still pending
```

### 3. Подключенческая синхронизация света сломана так же

Connect-snapshot хоста использовал хостовые ключи, которые клиент не смог сопоставить:

```
client 15411: [21:49:55] light: retry tick -- applied 0 deferred, dropped 27 expired, 0 still pending
```

### 4. Именованные выключатели работают

То же разделение, что `basedoor_*` двери против хэш-ключевых:

```
client 20513: [21:58:58] light: sent ON key='lightswitch_outside'
```

## Чего логи показать не могут

- Потерю сообщений клиента на хосте доказывает только сводка retry-тика (`dropped 1 expired`) — по-сообщений WARN нет, поэтому точный момент потери не указать.
- Отказал ли обратный канал (рассылки хост→клиент для хэш-ключевого света) именно для этого выключателя — вывод из всплеска «27 expired» на входе, а не наблюдение по-сообщений.
- Существует ли более громкий по-сообщений WARN, но выключен — протухание видно только по тихой сводной строке retry-тика.

## Анализ

Зеркально дверному багу, на одну подсистему выше: по-пирный вывод ключей после загрузки мира недетерминирован, поэтому хэш-ключевые сущности (двери, свет, lightgroup) не адресуются между пирами. Сообщения выключателей клиент→хост откладываются и протухают (`applied 0, dropped N expired`); рассылки хост→клиент, предположительно (гипотеза — для этого выключателя по-сообщений не наблюдалось), терпят ту же участь в обратную сторону. Всплеск «dropped 27 expired» в 21:49:55 показывает, что connect-snapshot для хэш-ключевого света массово неприменим. Последствие: все хэш-ключевые выключатели неработоспособны для клиента, а исходные состояния света после входа могут быть неверными. Это обобщение первопричины дверного бага — правка деривации ключей (стабильная per-instance идентичность), вероятно, закрыла бы двери, свет и lightgroup разом (вывод игроков, не чтение кода).

