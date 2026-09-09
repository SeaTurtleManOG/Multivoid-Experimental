# Bug 06: Client breaks a container — host sees the container vanish but never sees the spilled loot

## Metadata

| Field | Value |
|---|---|
| Video window | `~00:19:10` (destroy at `00:19:13`) |
| Log window | `22:07:36`–`22:07:37` (2026-09-04); ferry evidence `22:07:47`–`22:08:55` |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) breaks the container; host (`Mikuzavr`) observes |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | wire destroy vs native container death effects; re-seed authority model + held-broadcast ferry |
| Container | `prop_container_sbox_C` name=`'wbox'`, key `'Z8bHmBthD7oLzVOS0c-WKw'`, eid=3924, loc `(-90.2, 1095.8, 6330.1)` |
| Loot | 12 spilled items; recovered host-side only via 13 client pickups (eids 42703–42715) |

## Description

The client destroyed a small container box (hammer break — **player action**; the destroy and its replication are **log-proven**). The box's contents spilled out on the client's screen (12 items — **log-proven** via the re-seed count). The host saw the box disappear — and none of the spilled items appeared in her world at the moment of the break (**player-reported; consistent with the logs**: no spawn traffic reaches the host). Follow-up (**user observation, confirmed in logs** — see Addendum): the items do reach the host later, but **one by one, only as the client picks them up**.

## Evidence

### 1. The container's identity (bound at join on both peers)

```
client 9000: [21:49:30] remote_prop::OnSpawn: cls='prop_container_sbox_C' key='Z8bHmBthD7oLzVOS0c-WKw' name='wbox' loc=(-90.2, 1095.8, 6330.1)
host   2657: [21:48:52] snapshot: incremental PropSpawn for runtime-adopted prop 000002C979564980 (eid=3924, key='Z8bHmBthD7oLzVOS0c-WKw')
```

### 2. The client-native destroy, replicated by key

```
client 26835: [22:07:36] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188B269A4C0 key='Z8bHmBthD7oLzVOS0c-WKw' eid=0
host   15416: [22:07:36] remote_prop::OnDestroy: key 'Z8bHmBthD7oLzVOS0c-WKw' eid=0 -> destroying local actor 000002C979564980
```

→ the box disappears for the host (destroy applied). Note: the host's copy dies **by wire** — the native container-death logic (loot spill) never runs on the host.

### 3. The loot exists only on the client — and cannot be propagated

```
client 26871: [22:07:37] net_pump: steady-world re-seed adopted 12 NEW runtime-spawned keyed prop(s) -- tracked locally; a client authors no PropSpawn
client 26872: [22:07:37] reseed: queue drained (n=2068 new=12 rejects=0 ticks=2 full=1)
```

### 4. The host receives nothing

(22:07:33–45): no `container_contents` records shipped, no PropSpawn, and its own re-seed queue drains with `new=0`. The 12 items simply do not exist in the host's world.

## Addendum (2026-09-05): loot reaches the host only after the client handles it

The spilled items are not lost forever — but the only recovery path is the client's **hand**. While an item lies untouched it exists only on the client (re-seed: "tracked locally; a client authors no PropSpawn"). The moment the client grabs it, the held-item path broadcasts a full spawn descriptor **with a properly allocated eid**, and the host materializes the item into the puppet's hands. Example — a potato at 22:08:01:

```
client 27079: [22:08:01] net: NEW held actor 00000188667EA030 cls='prop_food_potato_C' key='Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -> BROADCAST
host   15754: [22:08:01] [WARN] remote_prop: slot 1 incoming PropPose key 'Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -- no local match (key or eid)
host   15755: [22:08:01] remote_prop::OnSpawn: cls='prop_food_potato_C' key='Y6PMidfLAhXi2Y6TR1NqYg' name='potato' loc=(-59.4, 1042.4, 6237.3) rot=(-0.0, 41.0, -180.0) physFlags=0x01
host   15760: [22:08:01] sync::CreateOrAdoptPropMirror: eid=42707 bound to actor=000002C841BBD260 key='Y6PMidfLAhXi2Y6TR1NqYg' cls='prop_food_potato_C' ownerSlot=1
host   15762: [22:08:01] remote_prop: slot 1 GRAB-IN key='Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -> local actor=000002C841BBD260 mesh=000002C90A19DAD0 (Aprop physics-off)
```

On release the copy persists in the host's world (`client 27240: [22:08:20] remote_prop: RELEASE wire 'Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 ctx=0 linVel=(5.3, 10.7, -11.5) ...`). In total, **13 held-broadcast crossings** happened in 22:07:47–22:08:55 (eids 42703–42715: wood/electronic scrap, MRE, potatoes, coal — the box's loot profile), matching one-to-one: client `net: NEW held actor ... -> BROADCAST` ↔ host `remote_prop::OnSpawn` + `sync::CreateOrAdoptPropMirror ... ownerSlot=1`. That every grab triggered a host-side OnSpawn also proves each item was **host-unknown until touched** (a grab of a shared world prop needs no spawn).

Two side observations:

- The held-item path allocates real eids (42703+) — unlike the drop-intent and destroy paths, which ship `eid=0` [Bug 03 «Floppy dissolves in mid-air»](https://discordapp.com/channels/1508397685726646272/1545093214191624262).
- At 22:07:57 the host re-seeded 4 already-crossed items **back** to the client (`snapshot: incremental PropSpawn for runtime-adopted prop ... (eid=0, key='...')` ×4, host 15694–15698), and the client dropped all four echoes: `event_feed: PropSpawn elementId=0x00000000 out of allowed host(any) range (senderPeerSlot=0) -- dropping` (client 27019–27022). Harmless here (the client already has them), but the host's echo-back carries `eid=0` and fails client-side range validation — the same eid=0 disease in reverse.

## What the logs cannot show

- The native loot-spill logic itself (what should have spawned on a non-wire death) — only its client-side outcome (12 re-seeded items) is visible.
- Whether untouched loot is recoverable later by any means other than grab-ferrying — nothing in the logs shows another bridge.
- Whether the client-native death authority split (client broadcasts a bare destroy, host skips death effects) is intended — not derivable from logs.
- Whether client-tracked runtime spawns (re-seed `new=12`) have any designed reconciliation path to the host — empirically the held-item broadcast was the only bridge (13 crossings, eids 42703–42715).
- Whether host echo-back PropSpawns carrying `eid=0` (4 items at 22:07:57, dropped by the client's range validation) are intended — no further log evidence either way.

## Analysis

The container's death is processed natively on the client (hammer → native destroy + native loot spill of 12 items) and replicated to the host as a bare keyed destroy (`eid=0`). The host applies the destroy — box gone — but the loot-spawn logic never executes host-side (wire destroy bypasses native death effects), and the mod's authority model explicitly forbids the client from authoring PropSpawns (`a client authors no PropSpawn`). No `container_contents` records are shipped as a fallback either. Result: the host's world loses the container without gaining its contents; the loot exists only on the client and migrates to the host **only through manual ferrying** — one item per client pickup via the held-item broadcast path (see Addendum). Until/unless the client grabs each item, it is absent from the host's world; items never picked up never reach the host (extrapolation — in this session every item was eventually ferried). This is the wire-destroy-skips-death-effects class (same family as [Bug 03 «Floppy dissolves in mid-air»](https://discordapp.com/channels/1508397685726646272/1545093214191624262) — bare eid=0 destroys — and [Bug 05 «Recorded signal lost when disc leaves console»](https://discordapp.com/channels/1508397685726646272/1546189631677145170) — actor-generation churn). Suggested directions: when a container dies by wire on the host, reconstruct the spill from replicated container contents; or route container damage through host authority like other interactions (so the death — and its effects — happen where the loot can spawn); or ship container contents on wire death.

Side note: seconds earlier (22:07:28–29) the same area hosted a different interaction — the **host** draining a `trashBitsPile_C` (key `'uEiOTgvK2y5Sf1sNz_R5gA'`, items collected into the host's hands, `local collect`), which then depleted and was destroyed. That part behaved consistently for both peers; it is included only to disambiguate the busy scene in the recording.

---

# Баг 06: Клиент ломает контейнер — хост видит, что контейнер исчез, но выпавшие предметы не видит

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | `~00:19:10` (разрушение в `00:19:13`) |
| Окно лога | `22:07:36`–`22:07:37` (2026-09-04); доказательства переправки `22:07:47`–`22:08:55` |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) ломает контейнер; хост (`Mikuzavr`) наблюдает |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | сетевой destroy против нативных эффектов смерти контейнера; модель полномочий re-seed + held-broadcast переправка |
| Контейнер | `prop_container_sbox_C` name=`'wbox'`, ключ `'Z8bHmBthD7oLzVOS0c-WKw'`, eid=3924, локация `(-90.2, 1095.8, 6330.1)` |
| Добыча | 12 выпавших предметов; доехала до хоста только через 13 клиентских взятий (eid 42703–42715) |

## Описание

Клиент уничтожил небольшой контейнер-ящик (проломил молотком — **действие игрока**; destroy и его репликация **доказаны логом**). Содержимое высыпалось на экране клиента (12 предметов — **доказано логом** через счётчик re-seed). Хост увидела, что ящик исчез, — и ни один из выпавших предметов не появился в её мире в момент разрушения (**со слов игрока; согласуется с логами**: до хоста не доходит ни одного спавна). Дополнение (**наблюдение пользователя, подтверждено логами** — см. Дополнение): предметы всё же доехали до хоста позже, но **по одному, только когда клиент их поднимал**.

## Доказательства

### 1. Идентичность контейнера (забинжена при входе у обоих пиров)

```
client 9000: [21:49:30] remote_prop::OnSpawn: cls='prop_container_sbox_C' key='Z8bHmBthD7oLzVOS0c-WKw' name='wbox' loc=(-90.2, 1095.8, 6330.1)
host   2657: [21:48:52] snapshot: incremental PropSpawn for runtime-adopted prop 000002C979564980 (eid=3924, key='Z8bHmBthD7oLzVOS0c-WKw')
```

### 2. Нативный клиентский destroy, реплицированный по ключу

```
client 26835: [22:07:36] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188B269A4C0 key='Z8bHmBthD7oLzVOS0c-WKw' eid=0
host   15416: [22:07:36] remote_prop::OnDestroy: key 'Z8bHmBthD7oLzVOS0c-WKw' eid=0 -> destroying local actor 000002C979564980
```

→ ящик исчезает у хоста (destroy применён). Внимание: копия хоста умирает **по сети** — нативная логика смерти контейнера (высыпание добычи) на хосте не выполняется никогда.

### 3. Добыча существует только у клиента — и не может быть передана

```
client 26871: [22:07:37] net_pump: steady-world re-seed adopted 12 NEW runtime-spawned keyed prop(s) -- tracked locally; a client authors no PropSpawn
client 26872: [22:07:37] reseed: queue drained (n=2068 new=12 rejects=0 ticks=2 full=1)
```

### 4. Хост не получает ничего

(22:07:33–45): записей `container_contents` не доставлено, PropSpawn нет, собственная re-seed очередь хоста опустошается с `new=0`. 12 предметов в мире хоста просто не существуют.

## Дополнение (2026-09-05): добыча попадает к хосту только после того, как клиент её потрогает

Высыпавшиеся предметы не теряются навсегда — но единственный путь восстановления — **руки клиента**. Пока предмет лежит нетронутым, он существует только у клиента (re-seed: «tracked locally; a client authors no PropSpawn»). В момент взятия клиентом путь held-предметов рассылает полный дескриптор спавна **с корректно выделенным eid**, и хост материализует предмет в руки куклы. Пример — картошка в 22:08:01:

```
client 27079: [22:08:01] net: NEW held actor 00000188667EA030 cls='prop_food_potato_C' key='Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -> BROADCAST
host   15754: [22:08:01] [WARN] remote_prop: slot 1 incoming PropPose key 'Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -- no local match (key or eid)
host   15755: [22:08:01] remote_prop::OnSpawn: cls='prop_food_potato_C' key='Y6PMidfLAhXi2Y6TR1NqYg' name='potato' loc=(-59.4, 1042.4, 6237.3) rot=(-0.0, 41.0, -180.0) physFlags=0x01
host   15760: [22:08:01] sync::CreateOrAdoptPropMirror: eid=42707 bound to actor=000002C841BBD260 key='Y6PMidfLAhXi2Y6TR1NqYg' cls='prop_food_potato_C' ownerSlot=1
host   15762: [22:08:01] remote_prop: slot 1 GRAB-IN key='Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 -> local actor=000002C841BBD260 mesh=000002C90A19DAD0 (Aprop physics-off)
```

При отпускании копия остаётся в мире хоста (`client 27240: [22:08:20] remote_prop: RELEASE wire 'Y6PMidfLAhXi2Y6TR1NqYg' eid=42707 ctx=0 linVel=(5.3, 10.7, -11.5) ...`). Всего в 22:07:47–22:08:55 прошло **13 held-пересылок** (eid 42703–42715: дерево/электронный лом, MRE, картошка, уголь — профиль добычи из ящика), соответствие один-к-одному: клиентский `net: NEW held actor ... -> BROADCAST` ↔ хостовые `remote_prop::OnSpawn` + `sync::CreateOrAdoptPropMirror ... ownerSlot=1`. То, что каждое взятие порождало хостовый OnSpawn, заодно доказывает: до прикосновения предметы были **хосту неизвестны** (взятие общего мирового пропа спавна не требует).

Два попутных наблюдения:

- Путь held-предметов выделяет настоящие eid (42703+) — в отличие от путей drop-intent и destroy, шлющих `eid=0` [Баг 03 «Дискета растворяется в воздухе»](https://discordapp.com/channels/1508397685726646272/1545093214191624262).
- В 22:07:57 хост ре-сидировал 4 уже переехавшие предметы **обратно** клиенту (`snapshot: incremental PropSpawn for runtime-adopted prop ... (eid=0, key='...')` ×4, хост 15694–15698), и клиент отбросил все четыре эха: `event_feed: PropSpawn elementId=0x00000000 out of allowed host(any) range (senderPeerSlot=0) -- dropping` (клиент 27019–27022). Здесь это безвредно (у клиента они уже есть), но ответная рассылка хоста несёт `eid=0` и не проходит клиентскую валидацию диапазона — та же болезнь eid=0 в обратную сторону.

## Чего логи показать не могут

- Саму нативную логику высыпания добычи (что должно было заспавниться при не-сетевой смерти) — виден только её клиентский итог (12 re-seed предметов).
- Восстанавливаются ли нетронутые предметы позже иначе, чем переправкой руками — другого моста в логах не видно.
- Намеренное ли разделение полномочий (клиент шлёт «голый» destroy, хост пропускает эффекты смерти) — из логов не выводится.
- Есть ли у клиентских runtime-спавнов (re-seed `new=12`) задуманный путь реконсиляции на хосте — эмпирически единственный мост — held-broadcast (13 пересылок, eid 42703–42715).
- Намеренно ли ответные PropSpawn хоста несут `eid=0` (4 предмета в 22:07:57, отброшены клиентской валидацией диапазона) — иных следов в логах нет.

## Анализ

Смерть контейнера обрабатывается нативно у клиента (молоток → нативное уничтожение + нативное высыпание 12 предметов) и реплицируется на хост «голым» destroy по ключу (`eid=0`). Хост применяет destroy — ящик исчезает, — но логика спавна добычи на хосте не выполняется (сетевой destroy проходит в обход нативных эффектов смерти), а модель полномочий мода прямо запрещает клиенту рассылать PropSpawn (`a client authors no PropSpawn`). Fallback через `container_contents` тоже не срабатывает — записи не доставляются. Итог: мир хоста теряет контейнер, не получив его содержимое; добыча существует только у клиента и попадает к хосту **только через ручную перевозку** — по одному предмету за каждое взятие, через путь held-broadcast (см. Дополнение). Пока клиент не взял предмет, его в мире хоста нет; невзятые предметы до хоста не доедут никогда (экстраполяция — в этой сессии все предметы в итоге переправили). Это класс «сетевой destroy пропускает эффекты смерти» (та же семья, что [Баг 03 «Дискета растворяется в воздухе»](https://discordapp.com/channels/1508397685726646272/1545093214191624262) — голые eid=0 destroy — и [Баг 05 «Записанный сигнал теряется при изъятии диска»](https://discordapp.com/channels/1508397685726646272/1546189631677145170) — дрейф поколений акторов). Направления: при wire-смерти контейнера на хосте реконструировать высыпание из реплицированного содержимого; либо проводить урон контейнеров через полномочия хоста, как другие взаимодействия (тогда смерть и её эффекты происходят там, где может заспавниться добыча); либо доставлять содержимое контейнера при сетевой смерти.

Попутное примечание: секундами ранее (22:07:28–29) в той же зоне происходило другое взаимодействие — **хост** разбирала `trashBitsPile_C` (ключ `'uEiOTgvK2y5Sf1sNz_R5gA'`, предметы собраны в руки хоста, `local collect`), после чего куча опустела и была уничтожена. Эта часть вела себя консистентно у обоих пиров; включено только для разграничения насыщенной сцены в записи.

