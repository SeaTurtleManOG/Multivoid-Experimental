# Bug 09: Client-only barrel explosion chain — the host receives bare destroys and nothing else

## Metadata

| Field | Value |
|---|---|
| Video window | chain `00:46:29`–`00:47:46`; user's timestamp `00:46:40` = the `22:35:03` gas-can explosion; ignition ≈ `00:46:09`–`00:46:22` (log-derived, see Evidence 1) |
| Log window | `22:34:52`–`22:36:09` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log`, host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) lit the barrel and observed; host (`Mikuzavr`) reports nothing happening on her side |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | wire destroy vs native death effects at scale (11 props); missing fire/device-state sync paths; `power` mask client→host |
| Site | TR1 depot (transformer station), props around loc `(39.1k–39.9k, 19.7k–20.9k, z≈16.9k)`; the base is ~440 m away |

## Description

The client lit an explosive barrel (`prop_bombBarrel_C` `'rad_b2'`) with the lighter at the TR1 depot, and a chain reaction followed (**player-visible; every step log-proven** below): the first explosion knocked out the transformer (the base power dropped to mask `0x00` — the mask apply is log-proven; the explosion→transformer link is from the client's screen, not logged), further explosions tore through the depot (the second barrel, four gas cans, a small container box, two fire extinguishers, two more keyed props — all log-proven as bare keyed destroys), and finally the transformer itself caught fire (**player-visible on the client's screen**; the fire itself has no log trace). On the host's side nothing was happening at that moment — no explosions, no fire, no transformer damage (**player-reported; consistent with the logs**: zero explosion/fire/damage/transformer events in the host's log for the whole window). The logs show the split precisely: every destroyed prop reached the host only as a bare keyed destroy (the props silently vanish, no death effects run), while the explosion effects, the fire and the transformer's state have no replication path at all. The only part of the catastrophe that synced is the abstract power-panel value (`power: mask=0x00`, client→host, applied `ok=1`).

## Evidence

### 1. Timing: the ignition has no dedicated log line (window derived)

The client was at the depot with the lighter in hand until ~22:34:43 (`hand_item` 43257 at 22:34:39; `pos diag` 43194/43249/43276), so the lighting fits ~22:34:32–22:34:45. The user's timestamp corresponds to log `22:35:03` and lands exactly on the fourth explosion of the chain (gas can `NWfDBqc…`, client 43395 / host 31883).

### 2. Client: the depot props (identity bound at join)

```
client 7583: [21:49:30] remote_prop::OnSpawn: cls='prop_container_sbox_C' key='W9jK5Sw_Qb4s4rSNg9JAEA' name='wbox' loc=(39829.2, 20254.5, 16854.2)
client 7679: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='eJoturOaXtiH09RUvIAwXA' name='gascan' loc=(39749.2, 19723.7, 16854.2)
client 7682: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='9Vhg4v17ghnCLKwjR_ST9g' name='gascan' loc=(39676.8, 19721.4, 16860.4)
client 7712: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='zPQnuupMQUEZuFJPaZeLYg' name='gascan' loc=(39756.3, 19702.6, 16854.2)
client 7709: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='NWfDBqc3Gut-X9w6D26oKQ' name='gascan' loc=(39886.2, 19743.3, 16860.4)
client 8379: [21:49:30] remote_prop::OnSpawn: cls='prop_bombBarrel_C' key='C4KJm6LG3YGOcNzPBCqXIA' name='rad_b2' loc=(39625.0, 19745.0, 16854.2)   ← bomb barrel #1 (eid=3717, bind at 8381)
client 8376: [21:49:30] remote_prop::OnSpawn: cls='prop_bombBarrel_C' key='JZM72ub2Xs40069iJjgWqw' name='rad_b2' loc=(39890.0, 20264.8, 16950.8)   ← bomb barrel #2 (eid=3716, bind at 8378)
client 8196: [21:49:30] remote_prop::OnSpawn: cls='prop_fireExt_C' key='i68K2jVyv_lV14Ig16w0sw' name='fireExt' loc=(39115.6, 19739.3, 16861.3)
client 7907: [21:49:30] remote_prop::OnSpawn: cls='prop_food_mre_C' key='vQVJVw7xJCXEs2_SU4LVaw' name='mre' loc=(39886.9, 19762.7, 16999.7)   ← the MRE the host later eats
```

### 3. Client: the ignition window — lighter in hand at the depot, then a fast exit

```
client 43257: [22:34:39] hand_item: local hand -> cls='prop_lighter_C' name='lighter' (announced, rel measured)
client 43249: [22:34:39] pos diag: local actor=(39084,20271,16941)   ← at the depot
client 43276: [22:34:43] pos diag: local actor=(39232,20080,16941)   ← leaving
client 43337: [22:34:55] pos diag: local actor=(41155,20724,16929)   ← ~21 m from the depot as the chain blows
```

### 4. Client: the native destruction chain — every death broadcast as a bare keyed destroy, `eid=0`

```
client 43323: [22:34:52] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=000001886784C8E0 key='9Vhg4v17ghnCLKwjR_ST9g' eid=0   ← gas can 1
client 43366: [22:34:57] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018837845F00 key='C4KJm6LG3YGOcNzPBCqXIA' eid=0   ← bomb barrel #1
client 43367: [22:34:58] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=000001886784D0C0 key='eJoturOaXtiH09RUvIAwXA' eid=0   ← gas can 2
client 43368: [22:34:58] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867937A20 key='zPQnuupMQUEZuFJPaZeLYg' eid=0   ← gas can 3
client 43395: [22:35:03] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867848200 key='NWfDBqc3Gut-X9w6D26oKQ' eid=0   ← gas can 4 (the user's 00:46:40 moment)
client 43441: [22:35:09] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018837844300 key='JZM72ub2Xs40069iJjgWqw' eid=0   ← bomb barrel #2
client 43564: [22:35:23] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188B25F3BD0 key='W9jK5Sw_Qb4s4rSNg9JAEA' eid=0   ← wbox
client 43602: [22:35:29] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000187744AF200 key='SzPMwpI9WldO0n2FQ7qyYg' eid=0   ← keyed prop (no OnSpawn record; class unconfirmed)
client 43603: [22:35:29] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000187BC5DE400 key='0BIr-TLMpNb8hmUEZrHtjQ' eid=0   ← keyed prop (class unconfirmed)
client 43666: [22:35:41] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188675856B0 key='jzF4zUWoinT41HWj00ZXlg' eid=0   ← fire extinguisher #2 (burned)
client 43816: [22:35:49] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867584ED0 key='i68K2jVyv_lV14Ig16w0sw' eid=0   ← fire extinguisher #1 (burned; dup actor destroyed again at 43989 [22:36:05])
```

### 5. Client: the grid outage — the only part that synced — plus client-only side effects

```
client 43369: [22:34:58] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00   ← the transformer died → base power off
client 43508: [22:35:19] net_pump: steady-world re-seed adopted 13 NEW runtime-spawned keyed prop(s) -- tracked locally; a client authors no PropSpawn   ← explosion debris/loot, client-only
client 44018: [22:36:09] device_screen: device class 'transformerMGPanel_C' resolved at edge (lazy, no walk)   ← the client opens the transformer's panel UI
client 44023: [22:36:09] input_owner: gameOwnsText -> YES (owner=uiwindow_transformerScreens_C, activeInterface=1 scan=0 via -)
```

The transformer is present to the mod **only as a screen-UI class** — it has no keyed prop, no OnSpawn/OnDestroy, and no state sync; the fire has no messages anywhere (greps for fire/burn/explode return only unrelated subsystem-install lines).

### 6. Host: every bare destroy applied silently — props vanish with no death effects

```
host 31788: [22:34:53] remote_prop::OnDestroy: key '9Vhg4v17ghnCLKwjR_ST9g' eid=0 -> destroying local actor 000002C97814A180
host 31837: [22:34:58] remote_prop::OnDestroy: key 'C4KJm6LG3YGOcNzPBCqXIA' eid=0 -> destroying local actor 000002C882417E80   ← bomb barrel #1: despawns, no explosion
host 31842: [22:34:58] remote_prop::OnDestroy: key 'eJoturOaXtiH09RUvIAwXA' eid=0 -> destroying local actor 000002C9781499A0
host 31844: [22:34:58] remote_prop::OnDestroy: key 'zPQnuupMQUEZuFJPaZeLYg' eid=0 -> destroying local actor 000002C97814F040
host 31883: [22:35:03] remote_prop::OnDestroy: key 'NWfDBqc3Gut-X9w6D26oKQ' eid=0 -> destroying local actor 000002C97814E860
host 31932: [22:35:09] remote_prop::OnDestroy: key 'JZM72ub2Xs40069iJjgWqw' eid=0 -> destroying local actor 000002C88241F900   ← bomb barrel #2: despawns, no explosion
host 32029: [22:35:24] remote_prop::OnDestroy: key 'W9jK5Sw_Qb4s4rSNg9JAEA' eid=0 -> destroying local actor 000002C9795EB270
host 32070: [22:35:29] remote_prop::OnDestroy: key 'SzPMwpI9WldO0n2FQ7qyYg' eid=0 has no local actor YET -- DEFERRING to the quiescence drain-edge ...   ← neither unknown prop was loaded in the host's world when its destroy arrived (locations unknown); deferred (32070–32073)
```

### 7. Host: the power mask applies too

```
host 31851: [22:34:58] power: applied key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00 ok=1 (from slot 1)
```

### 8. Host: and that is all — zero explosion/fire/damage/transformer events

In the whole window the host's log contains zero explosion/fire/damage/transformer events. Her world carries on as if nothing had happened — seconds before the chain she was handling the depot's fuse (host-native, synced correctly with a real eid), and minutes after it she eats an MRE that lies in the client's burn zone:

```
host 31691: [22:34:40] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C866512E00 key='6l784OraV6hMy3ihithBuA' eid=3769   ← prop_fuse_C 'fuse' consumed by the host pre-chain (client applied at 43259)
host 32205: [22:35:44] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C8E9041580 key='vQVJVw7xJCXEs2_SU4LVaw' eid=3560   ← host grabs & eats the MRE (client: GRAB-IN 43669 [22:35:41], OnDestroy 43688 [22:35:43])
host 32261: [22:35:47] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C8790A6280 key='fYxJxeimK9HxR1NxA36pww' eid=5453   ← and its food portions (client: spawns 43690–43707, applies 43758/43782/43785)
```

Host position at the first explosion: `host 31848: [22:34:58] pos diag: local actor=(41290,20614,16929)` — only ~18 m from the depot, having just left it together with the client. At that range a real explosion would have been unmissable — she saw nothing because in her world nothing was happening.

## What the logs cannot show

- The ignition moment itself — no dedicated log line exists; the window is derived from `hand_item` + `pos diag` telemetry (Evidence 1).
- Whether the mask=0x00 apply produced any visible blackout on the host's base — nobody was there, and no `lightgroup` messages accompanied the apply (cf. [Bug 08 «Client consoles stay dead after host restores base power»](https://discordapp.com/channels/1508397685726646272/1546197518474543207) — "value lands, nothing consumes it"); the mask stayed `0x00` until the 22:44:21 disconnect with no restore logged.
- Whether fire/device-state sync exists but is disabled, or was never implemented — the keyed subsystem index simply has no such entry; not derivable from logs.
- What the two unknown keyed props (`SzPMwpI9…`, `0BIr-TLM…`) were — no OnSpawn record, class unconfirmed, and their locations are not in the logs (the host's copies weren't even loaded when their destroys arrived).

## Analysis

The whole catastrophe is client-native: ignition → chain explosion → fire → transformer death. The mod's keyed-prop replication carries only the **outcomes** — each consumed prop ships as a bare keyed destroy (`eid=0`) and the host applies it as a silent despawn (`destroying local actor`). This is the "wire destroy skips native death effects" class from [Bug 06 «Client breaks a container — host never sees the spilled loot»](https://discordapp.com/channels/1508397685726646272/1546191405670473778), at visual scale: no explosion FX, no damage, no chain reaction, no loot spill ever run on the host — 11 keyed props simply evaporate across 22:34:52–22:36:05. The fire itself and the transformer's broken/burning state have **no replication path at all**: the mod's keyed subsystem index (`door, light, lightgroup, container, garage, appliance, doorbox, keypad, power, atv, window, trash_pile`) has no fire and no device-state subsystem, and the transformer surfaces to the mod only as a `device_screen` UI class (`transformerMGPanel_C` / `uiwindow_transformerScreens_C`, resolved at 22:36:09 when the client opened its panel) — not a keyed prop.

Ironically, the one abstract value that did cross is the fact the user described as "the transformer broke": the grid outage shipped as `power: sent mask=0x00` (client 43369) and was applied host-side (`ok=1`, host 31851). Per [Bug 08 «Client consoles stay dead after host restores base power»](https://discordapp.com/channels/1508397685726646272/1546197518474543207)'s "state syncs, native event effects don't" pattern, the mask value lands but nothing natively consumes it — no `lightgroup` messages accompanied it, and nobody was at the base to observe whether anything visibly turned off; the mask stayed `0x00` until the 22:44:21 disconnect with no restore logged.

Both players had stepped away from the depot before the blows (client at `(41155,20724)` by 22:34:55, host local at `(41290,20614)` at 22:34:58 — just ~18 m from the site), but distance is not the explanation: at that range real explosions would be unmissable. The host player saw "nothing happening" because in her world nothing *was* happening there — the death effects have no replication path. The durable desync is what each world keeps: the client's depot is a burning wreck with a dead transformer and client-only debris (re-seed `new=13` at 22:35:19, unshippable by design), while the host's depot is intact-but-gutted — 11 props silently missing with no cause, transformer whole, no scorch, no fire; she can walk the site and eat an MRE from the middle of the client's burn zone. Suggested directions: on wire-destroy of props with native death effects (explosives, containers, devices), run the native death logic host-side (explosion FX/damage/chain, loot spill — the container-loot class above) or route destructive interactions through host authority; introduce keyed state for fire and native device state (appliance-style), so a burning transformer is a fact in both worlds.

Side notes: (1) The depot's `prop_fuse_C` (`'fuse'`, eid=3769) was handled by the *host* seconds before the chain (grab 22:34:37 → consumed 22:34:40, shipped with its real eid and applied client-side at 43259) — the held/inventory path working as designed (real eids); later dup-actor churn of the same key produced `eid=0` echo destroys (host 32120–32122, client no-op drops 43641–43643) — the same eid=0 echo family as [Bug 03 «Floppy dissolves in mid-air»](https://discordapp.com/channels/1508397685726646272/1545093214191624262). (2) The lighter key `5fBZsGPS…` kept emitting DESTROY broadcasts from multiple duplicate actors through the window (client 42827–43428 range) with the host deferring/destroying in lockstep — same churn family, ignition context only.

---

# Баг 09: Клиентская цепочка взрывов бочек — хост получает только голые destroy и ничего больше

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | цепочка `00:46:29`–`00:47:46`; таймкод пользователя `00:46:40` = взрыв канистры в `22:35:03`; поджиг ≈ `00:46:09`–`00:46:22` (выведено из лога, см. Доказательства 1) |
| Окно лога | `22:34:52`–`22:36:09` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log`, хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) поджёг бочку и наблюдал; хост (`Mikuzavr`) сообщает, что у неё ничего не происходило |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | сетевой destroy против нативных эффектов смерти в масштабе (11 пропов); отсутствие путей синхронизации пожара/состояния устройств; маска `power` клиент→хост |
| Место | точка TR1 (трансформаторная), пропы вокруг локации `(39.1k–39.9k, 19.7k–20.9k, z≈16.9k)`; база в ~440 м |

## Описание

Клиент зажигалкой поджёг взрывную бочку (`prop_bombBarrel_C` `'rad_b2'`) на точке TR1, после чего пошла цепная реакция (**видно игроку; каждый шаг доказан логом** ниже): первый взрыв вывел из строя трансформатор (питание базы упало в маску `0x00` — применение маски доказано логом; связь «взрыв→трансформатор» — с экрана клиента, в логе её нет), дальнейшие взрывы прошлись по точке (вторая бочка, четыре канистры, маленький ящик-контейнер, два огнетушителя, ещё два keyed-пропа — все доказаны логом как «голые» destroy по ключу), и наконец на трансформаторе начался пожар (**видно на экране клиента**; сам пожар следа в логе не имеет). У хоста в этот момент не происходило ничего — ни взрывов, ни пожара, ни повреждения трансформатора (**со слов игрока; согласуется с логами**: ноль событий взрыв/пожар/урон/трансформатор в логе хоста за всё окно). Логи показывают расщепление точно: каждый уничтоженный проп доехал до хоста только как «голый» destroy по ключу (пропы молча исчезают, эффекты смерти не выполняются), а сами эффекты взрывов, пожар и состояние трансформатора не имеют пути репликации вовсе. Единственная часть катастрофы, которая синхронизировалась, — абстрактное значение панели питания (`power: mask=0x00`, клиент→хост, применено с `ok=1`).

## Доказательства

### 1. Тайминг: у поджига нет отдельной строки в логе (окно выведено)

Клиент был на точке с зажигалкой в руке до ~22:34:43 (`hand_item` 43257 в 22:34:39; `pos diag` 43194/43249/43276), так что поджиг укладывается в ~22:34:32–22:34:45. Таймкод пользователя соответствует логу `22:35:03` и попадает ровно в четвёртый взрыв цепочки (канистра `NWfDBqc…`, клиент 43395 / хост 31883).

### 2. Клиент: пропы точки (идентичность забинжена на входе)

```
client 7583: [21:49:30] remote_prop::OnSpawn: cls='prop_container_sbox_C' key='W9jK5Sw_Qb4s4rSNg9JAEA' name='wbox' loc=(39829.2, 20254.5, 16854.2)
client 7679: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='eJoturOaXtiH09RUvIAwXA' name='gascan' loc=(39749.2, 19723.7, 16854.2)
client 7682: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='9Vhg4v17ghnCLKwjR_ST9g' name='gascan' loc=(39676.8, 19721.4, 16860.4)
client 7712: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='zPQnuupMQUEZuFJPaZeLYg' name='gascan' loc=(39756.3, 19702.6, 16854.2)
client 7709: [21:49:30] remote_prop::OnSpawn: cls='prop_gascan_C' key='NWfDBqc3Gut-X9w6D26oKQ' name='gascan' loc=(39886.2, 19743.3, 16860.4)
client 8379: [21:49:30] remote_prop::OnSpawn: cls='prop_bombBarrel_C' key='C4KJm6LG3YGOcNzPBCqXIA' name='rad_b2' loc=(39625.0, 19745.0, 16854.2)   ← взрывная бочка №1 (eid=3717, бинд в 8381)
client 8376: [21:49:30] remote_prop::OnSpawn: cls='prop_bombBarrel_C' key='JZM72ub2Xs40069iJjgWqw' name='rad_b2' loc=(39890.0, 20264.8, 16950.8)   ← взрывная бочка №2 (eid=3716, бинд в 8378)
client 8196: [21:49:30] remote_prop::OnSpawn: cls='prop_fireExt_C' key='i68K2jVyv_lV14Ig16w0sw' name='fireExt' loc=(39115.6, 19739.3, 16861.3)
client 7907: [21:49:30] remote_prop::OnSpawn: cls='prop_food_mre_C' key='vQVJVw7xJCXEs2_SU4LVaw' name='mre' loc=(39886.9, 19762.7, 16999.7)   ← MRE, которое хост потом съест
```

### 3. Клиент: окно поджига — зажигалка в руке на точке, затем быстрый уход

```
client 43257: [22:34:39] hand_item: local hand -> cls='prop_lighter_C' name='lighter' (announced, rel measured)
client 43249: [22:34:39] pos diag: local actor=(39084,20271,16941)   ← на точке
client 43276: [22:34:43] pos diag: local actor=(39232,20080,16941)   ← уходит
client 43337: [22:34:55] pos diag: local actor=(41155,20724,16929)   ← ~21 м от точки, когда цепочка рвётся
```

### 4. Клиент: цепочка нативных разрушений — каждая смерть уходит «голым» destroy по ключу, `eid=0`

```
client 43323: [22:34:52] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=000001886784C8E0 key='9Vhg4v17ghnCLKwjR_ST9g' eid=0   ← канистра 1
client 43366: [22:34:57] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018837845F00 key='C4KJm6LG3YGOcNzPBCqXIA' eid=0   ← взрывная бочка №1
client 43367: [22:34:58] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=000001886784D0C0 key='eJoturOaXtiH09RUvIAwXA' eid=0   ← канистра 2
client 43368: [22:34:58] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867937A20 key='zPQnuupMQUEZuFJPaZeLYg' eid=0   ← канистра 3
client 43395: [22:35:03] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867848200 key='NWfDBqc3Gut-X9w6D26oKQ' eid=0   ← канистра 4 (момент 00:46:40 у пользователя)
client 43441: [22:35:09] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018837844300 key='JZM72ub2Xs40069iJjgWqw' eid=0   ← взрывная бочка №2
client 43564: [22:35:23] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188B25F3BD0 key='W9jK5Sw_Qb4s4rSNg9JAEA' eid=0   ← wbox
client 43602: [22:35:29] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000187744AF200 key='SzPMwpI9WldO0n2FQ7qyYg' eid=0   ← keyed-проп (записи OnSpawn нет; класс не подтверждён)
client 43603: [22:35:29] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000187BC5DE400 key='0BIr-TLMpNb8hmUEZrHtjQ' eid=0   ← keyed-проп (класс не подтверждён)
client 43666: [22:35:41] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=00000188675856B0 key='jzF4zUWoinT41HWj00ZXlg' eid=0   ← огнетушитель №2 (сгорел)
client 43816: [22:35:49] grab_hook[destroy-seam]: CLIENT broadcasting DESTROY actor=0000018867584ED0 key='i68K2jVyv_lV14Ig16w0sw' eid=0   ← огнетушитель №1 (сгорел; дубль-актор уничтожен снова в 43989 [22:36:05])
```

### 5. Клиент: отключение питания — единственная синхронизировавшаяся часть — плюс клиентские побочные эффекты

```
client 43369: [22:34:58] power: sent key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00   ← трансформатор умер → питание базы выключено
client 43508: [22:35:19] net_pump: steady-world re-seed adopted 13 NEW runtime-spawned keyed prop(s) -- tracked locally; a client authors no PropSpawn   ← обломки/добыча взрывов, только у клиента
client 44018: [22:36:09] device_screen: device class 'transformerMGPanel_C' resolved at edge (lazy, no walk)   ← клиент открывает панель трансформатора
client 44023: [22:36:09] input_owner: gameOwnsText -> YES (owner=uiwindow_transformerScreens_C, activeInterface=1 scan=0 via -)
```

Трансформатор существует для мода **только как класс экранного UI** — у него нет keyed-пропа, ни OnSpawn/OnDestroy, ни синхронизации состояния; пожара в логах нет нигде (поиск fire/burn/explode даёт только строки установки посторонних подсистем).

### 6. Хост: каждый «голый» destroy применяется молча — пропы исчезают без эффектов смерти

```
host 31788: [22:34:53] remote_prop::OnDestroy: key '9Vhg4v17ghnCLKwjR_ST9g' eid=0 -> destroying local actor 000002C97814A180
host 31837: [22:34:58] remote_prop::OnDestroy: key 'C4KJm6LG3YGOcNzPBCqXIA' eid=0 -> destroying local actor 000002C882417E80   ← взрывная бочка №1: исчезает, без взрыва
host 31842: [22:34:58] remote_prop::OnDestroy: key 'eJoturOaXtiH09RUvIAwXA' eid=0 -> destroying local actor 000002C9781499A0
host 31844: [22:34:58] remote_prop::OnDestroy: key 'zPQnuupMQUEZuFJPaZeLYg' eid=0 -> destroying local actor 000002C97814F040
host 31883: [22:35:03] remote_prop::OnDestroy: key 'NWfDBqc3Gut-X9w6D26oKQ' eid=0 -> destroying local actor 000002C97814E860
host 31932: [22:35:09] remote_prop::OnDestroy: key 'JZM72ub2Xs40069iJjgWqw' eid=0 -> destroying local actor 000002C88241F900   ← взрывная бочка №2: исчезает, без взрыва
host 32029: [22:35:24] remote_prop::OnDestroy: key 'W9jK5Sw_Qb4s4rSNg9JAEA' eid=0 -> destroying local actor 000002C9795EB270
host 32070: [22:35:29] remote_prop::OnDestroy: key 'SzPMwpI9WldO0n2FQ7qyYg' eid=0 has no local actor YET -- DEFERRING to the quiescence drain-edge ...   ← ни один из неопознанных пропов не был загружен в мир хоста на момент прихода его destroy (местоположения неизвестны); отложены (32070–32073)
```

### 7. Хост: маска питания тоже применена

```
host 31851: [22:34:58] power: applied key='UrCgZUozHxXzTc5Ky5a9ZQ' mask=0x00 ok=1 (from slot 1)
```

### 8. Хост: и это всё — ноль событий взрыв/пожар/урон/трансформатор

Во всём окне в логе хоста ноль событий взрывов/пожара/урона/трансформатора. Её мир живёт как ни в чём не бывало — за секунды до цепочки она держала предохранитель с точки (нативно у хоста, синхронизировано корректно, с настоящим eid), а через минуту после неё она ест MRE, который у клиента лежит в зоне пожара:

```
host 31691: [22:34:40] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C866512E00 key='6l784OraV6hMy3ihithBuA' eid=3769   ← prop_fuse_C 'fuse' потреблён хостом до цепочки (клиент применил в 43259)
host 32205: [22:35:44] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C8E9041580 key='vQVJVw7xJCXEs2_SU4LVaw' eid=3560   ← хост берёт и съедает MRE (клиент: GRAB-IN 43669 [22:35:41], OnDestroy 43688 [22:35:43])
host 32261: [22:35:47] grab_hook[destroy-seam]: HOST broadcasting DESTROY actor=000002C8790A6280 key='fYxJxeimK9HxR1NxA36pww' eid=5453   ← и его порции еды (клиент: спавны 43690–43707, применения 43758/43782/43785)
```

Позиция хоста в момент первого взрыва: `host 31848: [22:34:58] pos diag: local actor=(41290,20614,16929)` — всего ~18 м от точки, только что покинутой вместе с клиентом. С такого расстояния настоящий взрыв было бы невозможно не заметить — а она не увидела ничего, потому что в её мире ничего и не происходило.

## Чего логи показать не могут

- Сам момент поджига — отдельной строки в логе нет; окно выведено из телеметрии `hand_item` + `pos diag` (Доказательства 1).
- Дала ли маска 0x00 видимый блэкаут на базе хоста — никого там не было, и сопутствующих `lightgroup`-сообщений не было (ср. [Баг 08 «Консоли клиента остаются мёртвыми после восстановления питания хостом»](https://discordapp.com/channels/1508397685726646272/1546197518474543207) — «значение приходит, ничего не потребляет»); маска простояла `0x00` до дисконнекта в 22:44:21, восстановления в логе нет.
- Существует ли путь синхронизации пожара/состояния устройств, но выключен, или его никогда не было — в индексе keyed-подсистем просто нет такой записи; из логов не выводится.
- Что это за два неопознанных keyed-пропа (`SzPMwpI9…`, `0BIr-TLM…`) — записи OnSpawn нет, класс не подтверждён, местоположения в логах нет (копии хоста даже не были загружены, когда пришли их destroy).

## Анализ

Вся катастрофа — нативная у клиента: поджиг → цепная детонация → пожар → смерть трансформатора. Keyed-репликация пропов мода переносит только **итоги**: каждый потреблённый проп уезжает «голым» destroy по ключу (`eid=0`), и хост применяет его как тихое исчезновение (`destroying local actor`). Это класс «сетевой destroy пропускает нативные эффекты смерти» из [Баг 06 «Клиент ломает контейнер — хост не видит выпавшую добычу»](https://discordapp.com/channels/1508397685726646272/1546191405670473778), но в визуальном масштабе: ни эффектов взрыва, ни урона, ни цепной реакции, ни высыпания добычи у хоста не выполняется никогда — 11 keyed-пропов просто испаряются за 22:34:52–22:36:05. Сам пожар и состояние трансформатора «вышел из строя / горит» **не имеют пути репликации вовсе**: в индексе keyed-подсистем мода (`door, light, lightgroup, container, garage, appliance, doorbox, keypad, power, atv, window, trash_pile`) нет ни пожара, ни состояния устройств, а трансформатор виден моду только как класс экранного UI (`transformerMGPanel_C` / `uiwindow_transformerScreens_C`, резолвится в 22:36:09, когда клиент открыл его панель) — не как keyed-проп.

Иронично, но единственное абстрактное значение, которое доехало, — это как раз факт «трансформатор вышел из строя», который описал пользователь: отключение сети уехало как `power: sent mask=0x00` (клиент 43369) и применено на хосте (`ok=1`, хост 31851). По паттерну [Баг 08 «Консоли клиента остаются мёртвыми после восстановления питания хостом»](https://discordapp.com/channels/1508397685726646272/1546197518474543207) «состояние синхронизируется, нативные эффекты события — нет» значение маски приходит, но нативно его ничего не потребляет — сопутствующих `lightgroup`-сообщений не было, а у базы никого не было, чтобы увидеть, выключилось ли что-то видимо; маска оставалась `0x00` до дисконнекта в 22:44:21, восстановления в логе нет.

Оба игрока отошли от точки до взрывов (клиент в `(41155,20724)` к 22:34:55, хост локально в `(41290,20614)` в 22:34:58 — всего ~18 м от места), но дело не в дистанции: с такого расстояния настоящие взрывы было бы невозможно не заметить. Хост увидела «ничего не происходило», потому что в её мире там действительно ничего не происходило — эффекты смерти не имеют пути репликации. Стойкий рассинхрон — в том, какой мир остаётся у каждого: у клиента точка — горящие обломки с мёртвым трансформатором и клиентскими обломками (re-seed `new=13` в 22:35:19, отправить нельзя by design), у хоста — нетронутая, но ощипанная точка: 11 пропов молча отсутствуют без причины, трансформатор цел, ни гари, ни огня; она может ходить по месту и есть MRE из середины клиентской зоны пожара. Направления: при сетевой смерти пропов с нативными эффектами смерти (взрывное, канистры, контейнеры, устройства) исполнять нативную логику смерти на хосте (эффекты/урон/цепочка, высыпание добычи — класс контейнерной добычи выше) либо проводить разрушающие взаимодействия через полномочия хоста; ввести keyed-состояние для пожара и нативных устройств (в стиле appliance), чтобы горящий трансформатор был фактом в обоих мирах.

Попутные заметки: (1) `prop_fuse_C` (`'fuse'`, eid=3769) с точки хост держала в руках за секунды до цепочки (взятие 22:34:37 → потреблён 22:34:40, уехал с настоящим eid, клиент применил в 43259) — путь held/инвентаря работает как задумано (настоящие eid); позже дубли-акторы того же ключа надавали `eid=0`-эхов (хост 32120–32122, клиент отбросил как no-op 43641–43643) — то же семейство eid=0-эхов, что [Баг 03 «Дискета растворяется в воздухе»](https://discordapp.com/channels/1508397685726646272/1545093214191624262). (2) Ключ зажигалки `5fBZsGPS…` всю сцену генерировал DESTROY-рассылки с нескольких дублей-акторов (клиент 42827–43428), хост синхронно откладывал/уничтожал — то же семейство, тут только контекст поджига.

