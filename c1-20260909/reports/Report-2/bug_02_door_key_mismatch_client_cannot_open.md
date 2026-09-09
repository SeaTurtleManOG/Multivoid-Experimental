# Bug 02: Client cannot open radio telescope door — cross-peer door key mismatch

## Metadata

| Field | Value |
|---|---|
| Video window | `00:02:01` (E-presses `00:02:02`–`00:02:04`, retried `00:03:00`–`00:03:01`); delta-door addendum: video `01:04:55` |
| Log window | `21:50:24`–`21:51:48`; delta-door addendum: `22:53:18` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | door keyed index (`keysHash`) + use-input hook suppression |
| Key evidence | door index `keysHash` differs between peers after world load (and across sessions): 4 distinct hashes over 2 world loads |
| Affected set | the 31 world-load-added doors (19 → 50 instances; radio-telescope doors among them); the 19 base doors incl. 5 hash-format keys sync OK at every join |

## Description

Right after joining, the client walked up to the radio telescope door and pressed E repeatedly — the door would not open (**player action, requests log-proven client-side**). The client then started hitting the door with a hammer; the host player (Mikuzavr) walked up to the door at 21:51:00 while the hammering was ongoing (**player action, host position log-proven**). The door eventually opened, but the player cannot tell whether it was opened by the host's toggle or broke open natively from the hammer hits (**unconfirmed** — VotV lets a sufficiently battered door break open). Host-side logs show the door state toggling ON/OFF seven times during this window (**log-proven**). At `21:51:23` **both players pressed E on the same door in the same second**: it opened in the host's world and she walked through, while the client's door stayed closed — the client saw the host's puppet phase through a visually closed door (**same-second cross-log proof**). *(Addendum, 2026-09-05: in the second session a telescope door — delta — did open on E, but client-locally only, with zero sync — see the Addendum below.)*

## Evidence

### 1. Root cause: keysHash mismatch

The mod itself logs `compare host vs client for cross-peer Key stability` on every index rebuild — that comparison fails:

```
host   1873: [21:48:52] [INFO ] door: index rebuilt -- 50 live keyed instance(s), keysHash=0xF1E83244393256B6
client 3824: [21:49:28] [INFO ] door: index rebuilt -- 50 live keyed instance(s), keysHash=0x4E76EA415DFF476B
```

Same door count (50), **different keys**. The pre-load menu-world index (19 instances, `0xB177A4B1F440E053`) matches on both peers — the mismatch is introduced by the world load (19 → 50 instances). The instability repeats on re-connect with fresh values on both sides:

```
host   38995: [22:48:08] door: index rebuilt -- 50 ... keysHash=0xEA9927171B688840
client, next session — client_050920260005_multivoid.log:3828: [22:48:58] door: index rebuilt -- 50 ... keysHash=0x68B6A2C34D85CFCA
```

→ 4 distinct hashes across 2 world loads; keys are unstable per-load **and** per-peer. Named doors (`basedoor_signalroom`, `basedoor_garage`, ...) are stable and work; the affected doors are the hash-keyed ones (`pDcZUv8cxnVTI-ey_0Jo0w`, `f2kqYIFFMbZ5pm8ZO-ZlUw`).

### 2. Client E-presses reference a client-local key, nothing ever applies

`client_040920262246_multivoid.log`:

```
15763: [21:50:25] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15765: [21:50:26] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15770: [21:50:26] [INFO ] door: use-input hook -> debounced repeat (press+release) key='pDcZUv8cxnVTI-ey_0Jo0w'
15772: [21:50:26] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15779: [21:50:27] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
16271: [21:51:23] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'   (retry wave — 6 more requests at 21:51:23–25)
16276: [21:51:23] [INFO ] pose-diag[slot 0]: fresh=59/s targetSpeed=400 target=(20006,239) puppet=(20005,268) trail=29cm   (host puppet moving at the doorway while client's door stays closed)
```

`pDcZUv8cxnVTI-ey_0Jo0w` appears **0 times** in the host log. No `door: applied` response ever arrives for these requests.

### 3. Host-side toggles use a different key — the same-second 00:03:01 pins both directions

Host-side toggles of the same physical door (Mikuzavr at the door — player-confirmed: she approached at 21:51:00 while the client was hammering) use a different key, unknown to the client (`host_050920260342_multivoid.log`):

```
5690: [21:50:57] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5731: [21:51:03] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5902: [21:51:23] [INFO ] grab_hook[InpActEvt.use]: self=000002C8553D7090 grabbing_actor(after)=0000000000000000   (Mikuzavr's own E-press — same second as the client's retry wave)
5903: [21:51:23] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5905: [21:51:23] [INFO ] pose-diag[slot 1]: fresh=58/s targetSpeed=322 target=(19999,-49) puppet=(19980,-39) trail=22cm   (walking through the open doorway)
5955: [21:51:28] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6083: [21:51:40] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6147: [21:51:46] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6172: [21:51:48] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
```

`f2kqYIFFMbZ5pm8ZO-ZlUw` appears **0 times** in the client log, and the client logs no `door: applied` lines in that window — the broadcast could not be mapped to the client's door instance. The host log contains no warning about unmatched door requests (silent drop).

## What the logs cannot show

- **Which path opened the session-1 door** (host toggle vs native hammer-break) — no log line distinguishes them.
- **Why the native use ran for the delta door but not the session-1 door** — the two are different instances at different sites; their classes/lock/state are not logged (see Addendum finding 1).
- **Where the door keys are derived from** — the logs show only that the mod's own `compare host vs client for cross-peer Key stability` check fails (4 distinct hashes over 2 loads); the derivation source is not logged, so the mechanism in Analysis is a hypothesis.
- **Whether a louder log level for unmatched keys exists but is disabled** — both sides drop unmatched requests/broadcasts silently (zero warnings in either log).
- **Whether the native hammer-break door path bypasses the keyed sync** — no log line distinguishes a native forced-open from a synced toggle; if it does bypass, hammer-breaking is an additional desync vector on top of the key mismatch.
- **Whether the use-input hook is supposed to suppress native use for every indexed door** — session 1 shows the state change suppressed, the delta addendum shows it running free; the rule is not derivable from logs.

## Analysis

Door identity keys are derived per-peer after the world loads, and the derivation is not deterministic across peers (nor across loads): each side rebuilds the same 50 doors with different keys. A client `DoorOpenRequest` therefore references a key that does not exist host-side and is silently dropped — the door never opens. Conversely, host-side state broadcasts reference keys that do not exist client-side, so host-initiated door movement never renders for the client. Net effect: every hash-keyed door is client-inoperable for the whole session (named `basedoor_*` keys still work). The `21:51:23` moment pins both halves together in one second: Mikuzavr's E-press opens the door host-side (`InpActEvt.use` → `sent ON`) while the client spams toggle requests for its own key of the same door with zero effect — since puppet positions sync independently of door state, a door-state desync manifests visibly as players phasing through closed doors. Hypothesis (not proven from logs): keys derived from non-stable identity (pointer value / iteration order / spawn index) instead of a deterministic per-instance identity. Note on the eventual opening: the door finally opened while the client was hammering it and the host stood nearby — this may have been the host's native toggle (works host-side regardless of the key mismatch) **or** (hypothesis) the game's native door-break mechanic (hammer damage → forced open), which would bypass the mod's keyed door sync entirely. Either way the failed E-presses are explained by the key mismatch; the final opening path is unconfirmed.

## Addendum (2026-09-05): the delta telescope door did open for the client — client-locally, with zero sync (session 2)

After the session-2 rejoin (host re-hosted `s_Coop 3` at 22:48:04, client admitted at 22:48:28), the client walked up to the **delta** radio telescope and pressed E at `22:53:18` — and this door **did open** for him. But the logs show the opening was **client-local only**:

```
client — client_050920260005_multivoid.log:17049: [22:53:18] reflected_offset: mainPlayer_C::releaseEToUse -> 0xE88 (resolved once via FindPropertyOffset)
client 17050: [22:53:18] reflected_offset: mainPlayer_C::actionIndex -> 0xA98 ...
client 17051: [22:53:18] grab_hook[InpActEvt.use]: self=000001D7D94C40C0 grabbing_actor(after)=0000000000000000
client 17054: [22:53:18] door: use-input hook -> toggle request key='I6J8HCQDcitfWptyjA4Jwg'
client 17058: [22:53:18] door: use-input hook -> debounced repeat (press+release) key='I6J8HCQDcitfWptyjA4Jwg'
client 17056: [22:53:18] pos diag: local actor=(9675,-20041,7735)   ← at the delta telescope (different site from the session-1 door near (20006,239), ~230 m away)
```

The key `I6J8HCQDcitfWptyjA4Jwg` appears **0 times** in the host log, no `door: applied` ever arrives, and the host puppet was ~190 m away at the time (`pose-diag[slot 0] ... target=(23110,-32888)`, client 17052) with zero door activity on her side. So the toggle request was lost exactly as in session 1 — but this time the client's **native door use was not suppressed**, and the door changed state in the client's world alone. That is a second, worse flavor of this bug: not "E does nothing" but a **silent cross-peer door-state fork** — the delta telescope stands open in the client's world while the host's copy keeps its previous state, and neither player sees any desync until they compare.

Two follow-up findings from the same investigation:

1. **The "first E after boot" explanation is ruled out.** A plausible story for the session-1/session-2 difference was that the reflected offsets (`releaseEToUse`, `actionIndex`) resolve lazily on the first door E-press, and that first press might fall through to native use before the hook is fully armed. Session 1 disproves it: there the offsets resolved at the first telescope-door press too — `15760–15761: [21:50:25] reflected_offset: ... releaseEToUse -> 0xE88` / `actionIndex -> 0xA98`, immediately followed by the `grab_hook` line and the toggle request `pDcZUv8cxnVTI-ey_0Jo0w` (15762–15763) — and that door stayed closed. Why the native use ran in one case and not the other is not determinable from the logs; the two doors are different instances at different sites, possibly different classes or lock/state.

2. **The affected set is narrower than "all hash-keyed doors".** Every join snapshot syncs the same 19 base doors with `door: applied ... ok=1 (from slot 0)` — including 5 hash-format keys (`vfHBOoFDLBu-MbDb1iRrNQ`, `2IVTljm57lxfJVNyipZsLA`, `opu4dmuubchlNraKht3k2w`, `xuHKjzEpmWoHshjk9wT77A`, `cINjJa7Yd_VZwzGaOAIm4Q`; session 1: client 3898–3934 [21:49:29], session 2: client 3934–3970 [22:49:00]). And the pre-load menu-world index (19 instances, keysHash `0xB177A4B1F440E053`) matches on both peers at every load. So a hash *format* key by itself does not break a door: the mismatch is confined to the **31 world-load-added doors** (19 → 50 instances) — the radio-telescope doors among them — consistent with keys derived from runtime identity at world load.

---

# Баг 02: Клиент не может открыть дверь радиотелескопа — рассинхрон ключей дверей между пирами

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `00:02:01` (нажатия E `00:02:02`–`00:02:04`, повтор `00:03:00`–`00:03:01`); аддендум про дверь delta: видео `01:04:55` |
| Окно лога | `21:50:24`–`21:51:48`; аддендум про дверь delta: `22:53:18` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, `client_050920260005_multivoid.log`; хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | keyed-индекс дверей (`keysHash`) + подавление в use-input-хуке |
| Ключевое доказательство | `keysHash` индекса дверей расходится между пирами после загрузки мира (и между сессиями): 4 различных хэша на 2 загрузки |
| Поражённый набор | 31 дверь, добавляемая загрузкой мира (19 → 50 инстансов; среди них двери радиотелескопов); 19 базовых дверей, вкл. 5 хэш-форматных ключей, синхронизируются при каждом входе |

## Описание

Сразу после подключения клиент подошёл к двери радиотелескопа и многократно нажал E — дверь не открылась (**действие игрока, запросы доказаны логом клиента**). Затем клиент начал бить дверь молотком; хост (Mikuzavr) подошла к двери в 21:51:00, пока удары продолжались (**действие игрока, позиция хоста доказана логом**). В итоге дверь открылась, но игрок не может сказать, открыл ли её хост или она проломилась сама от ударов молотком (**не подтверждено** — в VotV достаточно побитая дверь взламывается). В логах хоста состояние двери переключалось ON/OFF семь раз за этот промежуток (**доказано логом**). В `21:51:23` **оба игрока нажали E на одну и ту же дверь в одну и ту же секунду**: в мире хоста дверь открылась, и она прошла сквозь проём, а дверь клиента осталась закрытой — клиент видел, как аватар хоста проходит сквозь визуально закрытую дверь (**доказательство той же секунды по обоим логам**). *(Дополнение от 2026-09-05: во второй сессии дверь телескопа — delta — всё же открылась по E, но только локально у клиента, без синхронизации — см. Дополнение ниже.)*

## Доказательства

### 1. Первопричина: несовпадение keysHash

Мод сам логирует `compare host vs client for cross-peer Key stability` при каждой пересборке индекса — эта проверка проваливается:

```
host   1873: [21:48:52] [INFO ] door: index rebuilt -- 50 live keyed instance(s), keysHash=0xF1E83244393256B6
client 3824: [21:49:28] [INFO ] door: index rebuilt -- 50 live keyed instance(s), keysHash=0x4E76EA415DFF476B
```

Дверей поровну (50), **ключи разные**. До-загрузочный индекс мира меню (19 инстансов, `0xB177A4B1F440E053`) у обоих пиров совпадает — рассинхрон вносит именно загрузка мира (19 → 50 инстансов). Нестабильность повторяется при переподключении с новыми значениями по обе стороны:

```
host   38995: [22:48:08] door: index rebuilt -- 50 ... keysHash=0xEA9927171B688840
клиент, следующая сессия — client_050920260005_multivoid.log:3828: [22:48:58] door: index rebuilt -- 50 ... keysHash=0x68B6A2C34D85CFCA
```

→ 4 различных хэша на 2 загрузки мира; ключи нестабильны и между загрузками, **и** между пирами. Именованные двери (`basedoor_signalroom`, `basedoor_garage`, ...) стабильны и работают; поражены двери с хэш-ключами (`pDcZUv8cxnVTI-ey_0Jo0w`, `f2kqYIFFMbZ5pm8ZO-ZlUw`).

### 2. Нажатия E клиента ссылаются на клиентский локальный ключ — применения нет никогда

`client_040920262246_multivoid.log`:

```
15763: [21:50:25] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15765: [21:50:26] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15770: [21:50:26] [INFO ] door: use-input hook -> debounced repeat (press+release) key='pDcZUv8cxnVTI-ey_0Jo0w'
15772: [21:50:26] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
15779: [21:50:27] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'
16271: [21:51:23] [INFO ] door: use-input hook -> toggle request key='pDcZUv8cxnVTI-ey_0Jo0w'   (повторная волна — еще 6 запросов в 21:51:23–25)
16276: [21:51:23] [INFO ] pose-diag[slot 0]: fresh=59/s targetSpeed=400 target=(20006,239) puppet=(20005,268) trail=29cm   (аватар хоста движется у проема, дверь клиента остается закрытой)
```

`pDcZUv8cxnVTI-ey_0Jo0w` встречается в логе хоста **0 раз**. Ответ `door: applied` на эти запросы не приходит никогда.

### 3. Хостовые переключения используют другой ключ — та же секунда 00:03:01 склеивает оба направления

Хостовые переключения той же физической двери (Mikuzavr у двери — подтверждено игроком: подошла в 21:51:00, пока клиент бил дверь молотком) используют другой ключ, клиенту неизвестный (`host_050920260342_multivoid.log`):

```
5690: [21:50:57] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5731: [21:51:03] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5902: [21:51:23] [INFO ] grab_hook[InpActEvt.use]: self=000002C8553D7090 grabbing_actor(after)=0000000000000000   (собственное нажатие E Mikuzavr — та же секунда, что повторная волна клиента)
5903: [21:51:23] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
5905: [21:51:23] [INFO ] pose-diag[slot 1]: fresh=58/s targetSpeed=322 target=(19999,-49) puppet=(19980,-39) trail=22cm   (проходит сквозь открытый проем)
5955: [21:51:28] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6083: [21:51:40] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6147: [21:51:46] [INFO ] door: sent OFF key='f2kqYIFFMbZ5pm8ZO-ZlUw'
6172: [21:51:48] [INFO ] door: sent ON  key='f2kqYIFFMbZ5pm8ZO-ZlUw'
```

`f2kqYIFFMbZ5pm8ZO-ZlUw` встречается в логе клиента **0 раз**, и в этом окне у клиента нет ни одной строки `door: applied` — рассылку не удалось сопоставить с инстансом двери на клиенте. В логе хоста нет предупреждений о неопознанных запросах двери (молчаливая потеря).

## Чего логи показать не могут

- **Каким путём открылась дверь в сессии 1** (переключение хостом против нативного проломления молотком) — ни одна строка лога их не различает.
- **Почему нативное использование сработало для двери delta, но не для двери сессии 1** — это разные инстансы в разных местах; их классы/замок/состояние не логируются (см. вывод 1 в Дополнении).
- **Откуда вычисляются ключи дверей** — в логах видно лишь, что собственная проверка мода `compare host vs client for cross-peer Key stability` проваливается (4 разных хэша на 2 загрузки); источник вывода ключей не логируется, поэтому механизм в Анализе — гипотеза.
- **Существует ли более громкий уровень логирования для неопознанных ключей, но выключен** — обе стороны молча теряют несовпадающие запросы/рассылки (ни одного предупреждения в обоих логах).
- **Проходит ли нативное проломление двери молотком в обход ключевой синхронизации** — ни одна строка лога не отличает нативный взлом от синхронизированного toggle; если проходит, это дополнительный вектор рассинхрона поверх несовпадения ключей.
- **Должен ли use-input-хук подавлять нативное использование для каждой индексированной двери** — в сессии 1 смена состояния подавлена, в аддендуме про delta прошла свободно; правило из логов не выводится.

## Анализ

Ключи-идентификаторы дверей вычисляются каждым пиром независимо после загрузки мира, и это вычисление не детерминировано между пирами (и между загрузками): обе стороны пересобирают одни и те же 50 дверей с разными ключами. Поэтому `DoorOpenRequest` клиента ссылается на ключ, которого нет на хосте, и молча теряется — дверь не открывается. Обратный случай: рассылки хоста ссылаются на ключи, которых нет у клиента, поэтому движение двери, инициированное хостом, у клиента не отрисовывается. Итог: все двери с хэш-ключами неработоспособны для клиента всю сессию (именованные `basedoor_*` работают). Момент `21:51:23` склеивает обе половины рассинхрона в одну секунду: E-press МикУзавры открывает дверь на стороне хоста (`InpActEvt.use` → `sent ON`), а клиент в ту же секунду шлет toggle-запросы по своему ключу той же двери без всякого эффекта — поскольку позиции аватаров синхронизируются независимо от дверного состояния, рассинхрон дверей видимо проявляется как прохождение игроков сквозь закрытые двери. Гипотеза (логами не подтверждена): ключи выводятся из нестабильной идентичности (значение указателя / порядок итерации / индекс спавна) вместо детерминированной идентичности инстанса. Примечание о финальном открытии: дверь в итоге открылась, когда клиент бил её молотком, а хост стоял рядом — это мог быть нативный toggle хоста (на стороне хоста работает независимо от рассинхрона ключей) **или** (гипотеза) нативная механика проломления двери (урон молотком → взлом), которая идёт в обход ключевой синхронизации дверей мода. В любом случае неудачные нажатия E объясняются рассинхроном ключей; финальный путь открытия не подтвержден.

## Дополнение (2026-09-05): дверь телескопа delta всё же открылась у клиента — но только локально, без синхронизации (сессия 2)

После переподключения во второй сессии (хост перехостил `s_Coop 3` в 22:48:04, клиент допущен в 22:48:28) клиент подошёл к радиотелескопу **delta** и нажал E в `22:53:18` — и эта дверь у него **открылась**. Но в логах видно, что открытие было **только локальным, у клиента**:

```
клиент — client_050920260005_multivoid.log:17049: [22:53:18] reflected_offset: mainPlayer_C::releaseEToUse -> 0xE88 (resolved once via FindPropertyOffset)
клиент 17050: [22:53:18] reflected_offset: mainPlayer_C::actionIndex -> 0xA98 ...
клиент 17051: [22:53:18] grab_hook[InpActEvt.use]: self=000001D7D94C40C0 grabbing_actor(after)=0000000000000000
клиент 17054: [22:53:18] door: use-input hook -> toggle request key='I6J8HCQDcitfWptyjA4Jwg'
клиент 17058: [22:53:18] door: use-input hook -> debounced repeat (press+release) key='I6J8HCQDcitfWptyjA4Jwg'
клиент 17056: [22:53:18] pos diag: local actor=(9675,-20041,7735)   ← у телескопа delta (другое место, чем дверь первой сессии возле (20006,239), ~230 м)
```

Ключ `I6J8HCQDcitfWptyjA4Jwg` встречается в логе хоста **0 раз**, `door: applied` не приходит никогда, а аватар хоста был в тот момент в ~190 м (`pose-diag[slot 0] ... target=(23110,-32888)`, клиент 17052) без какой-либо дверной активности у себя. То есть toggle-запрос потерялся ровно как в первой сессии — но на этот раз **нативное использование двери у клиента не было подавлено**, и дверь сменила состояние только в мире клиента. Это второй, худший вариант бага: не «E ничего не делает», а **молчаливое расхождение состояния двери между пирами** — телескоп delta стоит открытый в мире клиента, копия хоста хранит прежнее состояние, и ни один игрок не видит рассинхрона, пока они не сравнят.

Два попутных вывода того же расследования:

1. **Объяснение «первое E после загрузки» опровергнуто.** Правдоподобная версия различия между сессиями 1 и 2 была такой: reflected-смещения (`releaseEToUse`, `actionIndex`) резолвятся лениво при первом нажатии E на дверь, и первое нажатие могло «провалиться» в нативное использование до полного вооружения хука. Сессия 1 это опровергает: там смещения резолвились тоже на первом нажатии на дверь телескопа — `15760–15761: [21:50:25] reflected_offset: ... releaseEToUse -> 0xE88` / `actionIndex -> 0xA98`, сразу за ними строки `grab_hook` и toggle-запрос `pDcZUv8cxnVTI-ey_0Jo0w` (15762–15763) — и та дверь осталась закрытой. Почему нативное использование сработало в одном случае и не сработало в другом, по логам определить нельзя; это разные инстансы в разных местах, возможно разных классов или с другим замком/состоянием.

2. **Поражённый набор уже, чем «все двери с хэш-ключами».** Каждый join-снапшот синхронизирует одни и те же 19 базовых дверей с `door: applied ... ok=1 (from slot 0)` — включая 5 ключей в хэш-формате (`vfHBOoFDLBu-MbDb1iRrNQ`, `2IVTljm57lxfJVNyipZsLA`, `opu4dmuubchlNraKht3k2w`, `xuHKjzEpmWoHshjk9wT77A`, `cINjJa7Yd_VZwzGaOAIm4Q`; сессия 1: клиент 3898–3934 [21:49:29], сессия 2: клиент 3934–3970 [22:49:00]). А до-загрузочный индекс мира меню (19 инстансов, keysHash `0xB177A4B1F440E053`) совпадает у обоих пиров при каждой загрузке. Значит, сам по себе хэш-формат ключа дверь не ломает: рассинхрон ограничен **31 дверью, добавляемой загрузкой мира** (19 → 50 инстансов) — среди них двери радиотелескопов — что согласуется с выводом ключей из рантайм-идентичности при загрузке мира.
