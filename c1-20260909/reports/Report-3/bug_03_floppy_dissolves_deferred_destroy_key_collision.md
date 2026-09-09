# Bug 03: Floppy extracted from server dissolves in mid-air — deferred destroy-by-key kills the freshly spawned prop

## Metadata

| Field | Value |
|---|---|
| Video window | `00:02:44`–`00:02:53` (extract `00:02:47`, drop + dissolve `00:02:53`) |
| Log window | `21:51:07`–`21:51:16` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | prop identity (`eid=0`, key-only) + deferred destroy-by-key (`[DESTROY-DEFER]` / destroy-seam) |
| Root cause | key/eid non-uniqueness + host-side deferred destroy applied "post-bind" to a later spawn reusing the same key |
| Colliding key | `'0zlKei78xf4Ek_bPrYWV0A'` — carried by ≥4 distinct client actors + 1 host actor around the incident |

## Description

The client inserted a floppy disk into a server rack, then extracted it. The floppy popped out of the drive, fell — and dissolved into thin air seconds later (**player-visible, fully log-proven**: the whole causal chain below is in the logs). The prop was destroyed on both peers by a *deferred* destroy message that had been armed ~5 s earlier against a *different* actor that happened to share the same key (`eid=0`, key-only identity). **Player-reported (other sessions):** the players have repeatedly observed floppies dissolving after *host-side* interactions as well — not captured on this recording (no host floppy interaction occurs in this session's recording); what the logs do show is the peer-symmetric key-collision traffic (see Evidence 4).

## Evidence

### 1. Step 1 — client extracts the floppy, broadcasts destroys of its world actor

`client_040920262246_multivoid.log`:

```
16099: [21:51:10] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868200 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
16100: [21:51:10] hand_item: local hand -> cls='prop_floppyDisc_Y_C' name='floppy_2' (announced, rel measured)
16116: [21:51:11] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
16117: [21:51:11] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
```

### 2. Step 2 — host has no actor with that key, arms a DEFERRED destroy time-bomb

`host_050920260342_multivoid.log`:

```
5797: [21:51:10] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 has no local actor YET -- DEFERRING to the quiescence drain-edge (destroy-before-load; the order owner applies it post-bind)
5798: [21:51:10] [DESTROY-DEFER] CLIENT armed deferred destroy key='0zlKei78xf4Ek_bPrYWV0A' eid=0 -- arrived before the target loaded; the drain-edge applies it post-bind at quiescence
5806: [21:51:11] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 has no local actor YET -- DEFERRING ...   (×2 more)
```

### 3. Step 3 — client drops the floppy; host spawns it; the deferred destroy kills it in the same tick

```
client 16190: [21:51:16] [PROP-DROP] CLIENT authored drop intent key='0zlKei78xf4Ek_bPrYWV0A' cls='prop_floppyDisc_Y_C' name='floppy_2' loc=(19828.6,-6.8,10446.1)
host   5848: [21:51:16] [PROP-DROP] HOST spawned client-placed prop key='0zlKei78xf4Ek_bPrYWV0A' cls='prop_floppyDisc_Y_C' slot=1 at (19828.6,-6.8,10446.1)
host   5849: [21:51:16] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C888D0D980 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
client 16191: [21:51:16] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 -> destroying local actor 000001877B7D0080
client 16192: [21:51:16] grab_hook[destroy-seam]: actor 000001877B7D0080 was wire-received destroy -- skip rebroadcast
```

The freshly dropped floppy exists for ~0 ticks: spawned, bound to key `0zlKei78…`, destroyed by the deferred destroy armed at step 2, broadcast destroyed, and removed client-side by key. Visually: the floppy falls out of the drive and dissolves.

### 4. Contributing evidence

- **Keys are not instance-unique.** Key `0zlKei78xf4Ek_bPrYWV0A` was carried by at least four distinct client actors around this time: `000001877D261580` (21:49:24, destroy suppressed during load), `0000018771868200` (21:51:10), `0000018771868C80` (21:51:11), `000001877B7D0080` (21:51:16) — plus host-side `000002C888D0D980`. Same pattern on other keys: `uZOFnEN6gtFjHsW8xdFm5Q` (client actors `000001877F24DD00`, `00000187853E2A80`, `0000018749133F80`; host `000002C878D15100`), `eBwy2V_TFGdYNy98Ks-FXg` (two actors destroyed 21:50:03–05).
- All destroy traffic in this incident carries **`eid=0`** (no unique instance id) — identity is key-only, so any destroy-by-key is ambiguous across same-keyed instances. Props spawned through the eid path (e.g. `eid=5209`, `eid=5215`) do carry unique ids; the client drop-intent path did not allocate one.
- The deferred-destroy mechanism itself logs its assumption — *"destroy-before-load order fix"* — i.e. it is designed for the load window, but here it fired ~5 s later in steady state against an unrelated spawn that reused the key.
- **The key identity problem is peer-symmetric.** At 22:11:03 the *host itself* broadcast `DESTROY actor=000002C888D01C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0` (during trash disposal) — same colliding key, same eid=0, host side (`host_050920260342_multivoid.log:17564`). The client answered `no local actor in STEADY state -- no-op drop` (`client_040920262246_multivoid.log:28969`). Destroy-by-key traffic flows in both directions with ambiguous identity.

## What the logs cannot show

- The host-side dissolving-floppy variant (floppy interactions by the host) is player-observed in other sessions and is not in these logs — only the symmetric key-collision traffic of 22:11:03 is captured here.
- Why the client drop-intent path allocates no eid while other paths do — a design-intent question, not visible in logs.
- Whether key sharing across live instances is intended design — the logs only show that multiple live actors simultaneously carry one key (`eid=0`), which makes destroy-by-key alone ambiguous.
- Whether `[DESTROY-DEFER]` entries have any expiry rule — here one survived into steady state and fired 5–6 s after arming in calm network conditions (net-diag: no backlog, inbox peak 5/6144).

## Analysis

Two mod subsystems interact fatally. (1) Prop identity: dropped/destroyed props are identified by a hash key that is *not unique per actor instance* (multiple live actors share one key, `eid=0`). (2) Destroy ordering: when a destroy-by-key arrives for an unknown actor, the host defers it and applies it "post-bind" later. Together: a destroy aimed at actor A (picked-up floppy) is deferred, and when a *new* actor B (the dropped floppy) reuses the key, the stale deferred destroy kills B in its first tick. The prop is unobtainable this way — any extraction that follows a same-key pickup is expected to dissolve too (extrapolation: only one full incident is captured in these logs). Suggested fixes: allocate unique eids on the client drop-intent path; scope deferred destroys to a single spawn generation (or expire them after a short window instead of holding them "until something binds"); make destroys match on (key, eid) with eid mandatory when available.

---

# Баг 03: Дискета, извлечённая из сервера, растворяется в воздухе — отложенный destroy по ключу убивает свежезаспавненный проп

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `00:02:44`–`00:02:53` (извлечение `00:02:47`, бросок + растворение `00:02:53`) |
| Окно лога | `21:51:07`–`21:51:16` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | идентичность пропов (`eid=0`, только ключ) + отложенный destroy по ключу (`[DESTROY-DEFER]` / destroy-seam) |
| Первопричина | неуникальность key/eid + отложенный destroy на стороне хоста, применённый «post-bind» к позднейшему спавну, переиспользовавшему тот же ключ |
| Конфликтующий ключ | `'0zlKei78xf4Ek_bPrYWV0A'` — носили ≥4 разных клиентских актора + 1 хостовый вокруг инцидента |

## Описание

Клиент вставил дискету в сервер, затем извлёк её. Дискета вылетела из дисковода, упала — и через несколько секунд растворилась в воздухе (**видно игроку, полностью доказано логом**: вся причинная цепочка ниже есть в логах). Проп был уничтожен на обоих пирах *отложенным* destroy-сообщением, которое было заряжено ~5 секундами ранее против *другого* актора, носившего тот же ключ (`eid=0`, идентичность только по ключу). **Со слов игрока (другие сессии):** игроки многократно наблюдали растворение дискет и при взаимодействии *хоста* — в эту запись не попало (в этой сессии хост с дискетой не взаимодействует); что логи всё же показывают, — симметричный по пирам трафик ключевых коллизий (см. Доказательства 4).

## Доказательства

### 1. Шаг 1 — клиент извлекает дискету, рассылает destroy её мирового актора

`client_040920262246_multivoid.log`:

```
16099: [21:51:10] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868200 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
16100: [21:51:10] hand_item: local hand -> cls='prop_floppyDisc_Y_C' name='floppy_2' (announced, rel measured)
16116: [21:51:11] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
16117: [21:51:11] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018771868C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
```

### 2. Шаг 2 — у хоста нет актора с таким ключом, взводится бомба отложенного DESTROY

`host_050920260342_multivoid.log`:

```
5797: [21:51:10] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 has no local actor YET -- DEFERRING to the quiescence drain-edge (destroy-before-load; the order owner applies it post-bind)
5798: [21:51:10] [DESTROY-DEFER] CLIENT armed deferred destroy key='0zlKei78xf4Ek_bPrYWV0A' eid=0 -- arrived before the target loaded; the drain-edge applies it post-bind at quiescence
5806: [21:51:11] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 has no local actor YET -- DEFERRING ...   (еще ×2)
```

### 3. Шаг 3 — клиент бросает дискету; хост спавнит её; отложенный destroy убивает её в тот же тик

```
client 16190: [21:51:16] [PROP-DROP] CLIENT authored drop intent key='0zlKei78xf4Ek_bPrYWV0A' cls='prop_floppyDisc_Y_C' name='floppy_2' loc=(19828.6,-6.8,10446.1)
host   5848: [21:51:16] [PROP-DROP] HOST spawned client-placed prop key='0zlKei78xf4Ek_bPrYWV0A' cls='prop_floppyDisc_Y_C' slot=1 at (19828.6,-6.8,10446.1)
host   5849: [21:51:16] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C888D0D980 key='0zlKei78xf4Ek_bPrYWV0A' eid=0
client 16191: [21:51:16] remote_prop::OnDestroy: key '0zlKei78xf4Ek_bPrYWV0A' eid=0 -> destroying local actor 000001877B7D0080
client 16192: [21:51:16] grab_hook[destroy-seam]: actor 000001877B7D0080 was wire-received destroy -- skip rebroadcast
```

Свежеброшенная дискета живёт ~0 тиков: заспавнена, забинжена на ключ `0zlKei78…`, уничтожена отложенным destroy со шага 2, уничтожение разослано, локально снята по ключу. Визуально: дискета выпадает из дисковода и растворяется.

### 4. Подтверждающие данные

- **Ключи не уникальны для инстанса.** Ключ `0zlKei78xf4Ek_bPrYWV0A` примерно в это время носили минимум четыре разных клиентских актора: `000001877D261580` (21:49:24, destroy подавлен при загрузке), `0000018771868200` (21:51:10), `0000018771868C80` (21:51:11), `000001877B7D0080` (21:51:16) — плюс хостовый `000002C888D0D980`. Тот же паттерн на других ключах: `uZOFnEN6gtFjHsW8xdFm5Q` (клиентские акторы `000001877F24DD00`, `00000187853E2A80`, `0000018749133F80`; хостовый `000002C878D15100`), `eBwy2V_TFGdYNy98Ks-FXg` (два актора уничтожены 21:50:03–05).
- Весь destroy-трафик инцидента несёт **`eid=0`** (нет уникального id инстанса) — идентичность только по ключу, поэтому любой destroy-by-key неоднозначен для одноимённых инстансов. Пропы, заспавненные через eid-путь (например `eid=5209`, `eid=5215`), уникальные id имеют; клиентский путь drop-intent свой не выделил.
- Механизм отложенного destroy сам логирует своё предположение — *"destroy-before-load order fix"*, — т.е. он рассчитан на окно загрузки, но здесь сработал ~5 с спустя в стационарном режиме против постороннего спавна, переиспользовавшего ключ.
- **Проблема ключевой идентичности симметрична по пирам.** В 22:11:03 *сам хост* разослал `DESTROY actor=000002C888D01C80 key='0zlKei78xf4Ek_bPrYWV0A' eid=0` (при выносе мусора) — тот же конфликтующий ключ, тот же eid=0, сторона хоста (`host_050920260342_multivoid.log:17564`). Клиент ответил `no local actor in STEADY state -- no-op drop` (`client_040920262246_multivoid.log:28969`). Destroy-by-key трафик ходит в обе стороны с неоднозначной идентичностью.

## Чего логи показать не могут

- Хостовый вариант «растворяющейся дискеты» (взаимодействия хоста с дискетами) — наблюдение игроков в других сессиях, в этих логах его нет — зафиксирован только симметричный трафик ключевой коллизии 22:11:03.
- Почему путь клиентского drop-intent не выделяет eid, тогда как другие пути выделяют — вопрос о замысле, из логов не видный.
- Ожидается ли совместное ношение одного ключа живыми инстансами — в логах видно лишь, что несколько живых акторов одновременно носят один ключ (`eid=0`), из-за чего destroy по одному ключу неоднозначен.
- Есть ли у записей `[DESTROY-DEFER]` правило протухания — здесь запись дожила до стационарного режима и сработала через 5–6 с после взвода в спокойной сети (net-diag: без бэклогов, inbox peak 5/6144).

## Анализ

Две подсистемы мода фатально взаимодействуют. (1) Идентичность пропов: брошенные/уничтожаемые пропы идентифицируются хэш-ключом, который *не уникален для инстанса* (несколько живых акторов носят один ключ, `eid=0`). (2) Порядок уничтожения: когда destroy-by-key приходит на неизвестный актор, хост откладывает его и применяет позже «post-bind». Вместе: destroy, адресованный актору A (поднятая дискета), откладывается, и когда *новый* актор B (брошенная дискета) переиспользует ключ, протухший отложенный destroy убивает B в первый же тик жизни. Проп становится неполучаемым этим путём — любое извлечение после поднятия дискеты с тем же ключом тоже должно «растворяться» (экстраполяция: в логах зафиксирован только один полный инцидент). Предложения: выдавать уникальные eid на пути клиентского drop-intent; ограничить отложенные destroy одним поколением спавна (или протухать по таймауту вместо хранения «пока что-то не забиндится»); матчить destroy по паре (key, eid) с обязательным eid, где он есть.

