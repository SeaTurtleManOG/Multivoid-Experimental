# Bug 10: Client death tears down the session; the in-game host becomes unjoinable

## Metadata

| Field | Value |
|---|---|
| Video window | client death `~00:55:46`; quit-to-menu + session tear `00:55:57–00:55:58`; failed joins `00:56:27` and `00:56:43` (timeouts `00:56:38`, `00:56:53`); client game restart `00:57:18–00:58:25`; host quit-to-menu + re-host `00:59:24–00:59:41`; successful rejoin `01:00:05` |
| Log window | client death `~22:44:09` (no direct line — derived, see Evidence 1) → client admitted `22:48:28` (2026-09-04) |
| Log files | client: `client_040920262246_multivoid.log` (disconnect + both timeouts) and `client_050920260005_multivoid.log` (client game restart + successful rejoin); host: `host_050920260342_multivoid.log` |
| Reporter | client (`PlayerNickname2`) dies and is kicked to the menu; host (`Mikuzavr`) also dies 2 s later (revived in place), keeps playing, then re-hosts so the client can return |
| Peers | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; host = slot 0, client = slot 1 |
| Subsystem | `death_revive` travel veto (host-only trace) + session teardown on gameplay→MENU; P2P listener lifecycle + stale lobby listing |
| Key moments | world→menu `22:44:20`, session stopped `22:44:21`; host death armed `22:44:23`, travel REFUSED + REVIVE OK `22:44:33`; join attempts `22:44:50→22:45:01` and `22:45:06→22:45:16`; host re-host listening `22:48:04`; client admitted `22:48:28` |

## Description

The client's character died (**not directly logged** — see Evidence 1; the aftermath is log-proven). Instead of keeping the world (the mod's intended "coop death keeps the world" behavior), the client's game traveled to the **main menu** — and the live session was torn down as a "quit-to-menu" (**log-proven**). The client was kicked out. The host's session dissolved with it (`all peers gone`), but the host's **native world kept running** — she stayed in gameplay and even got to play on.

Two aggravating findings:

1. **The death handling is asymmetric.** Two seconds after the client left, the *host also died* — and on the host the mod's `death_revive` subsystem worked exactly as designed: armed, refused the level travel, revived the player in place (**log-proven**). On the **client**, the same death flow produced **zero** `death_revive` trace: no arming, no refusal, no revive (**log-proven absence**). The native death ran un-intercepted and ended in the menu.
2. **The orphaned host is unjoinable.** The lobby `'Mikuzavr's game'` stayed visible in the server browser (stale listing), but the host's P2P listener was gone with the session (**log-proven**). Every join attempt burned a ~10 s timeout (`Timed out attempting to connect`) without leaving any trace in the host's log. The client only got back in after the host quit to the menu and re-hosted the save (`s_Coop 3`) — the listener was recreated, and the next attempt connected within seconds (**log-proven**).

## Evidence

### 1. Timing note: the client's death moment is derived (no direct line)

The client's death moment is **not directly logged** (no death lines, no ragdoll — see Evidence 2). It is derived: the native death flow runs ~10 s (the mod's own documentation in `death_revive: local death ARMED ... ~10 s: sound, black screen at +5 s`), and the client's travel-to-menu hit at `22:44:20` — putting death at `~22:44:09–10`, which is exactly the start of the user's reported window (`00:55:46` = `22:44:09`). The user's window `00:55:46–00:56:00` covers death → kick precisely.

Context (factual, cause not in the mod logs): 40 s before dying the client had ragdoll-knocked down and recovered (`22:43:27–30`); a FOG weather event was ramping (fog density 0.03→0.09 during `22:43:09–35`). The host died by an ~11 m fall (z 15626→14497 in the ragdoll telemetry) at `22:44:20–23` — ~15 s after the client's death, at a spot ~180 m away. Whether the two deaths share a cause is not visible in the logs; the client player can confirm what killed them.

### 2. Client: the death left no `death_revive` trace

In the whole client file, `death_revive` appears only at boot — the veto install and the revive verbs (client 82 [21:42:18], 319 [21:49:23]). At death time: nothing. The last ragdoll event is a knockdown-recovery 40 s earlier:

```
client 47157: [22:43:27] net: RagdollPose emit #960 -> pelvis(55979, 25800, 17371) ...
client 47184: [22:43:30] net: RagdollPose stream STOP (local player recovered)
```

### 3. Client: the game goes to the menu and the session dies

Menu UI rebuilt while the session was still live (`net stats: state=2` at 22:44:20):

```
client 47873: [22:44:20] world_identity: current world 00000187496C7120 -> 0000018790E55950 (gen=7, pc=...)
client 47891: [22:44:20] multiplayer_menu: menu restored for connect (opacity 1, hit-test on)
client 47899: [22:44:20] multiplayer_menu: MULTIPLAYER button injected into menu=000001878F09DAD0 ...
client 47900: [22:44:21] [WARN ] net: gameplay->MENU while a session is live (VOTV quit-to-menu?) -- ending the session + stopping the layer churn (RAM-balloon guard)
client 47901: [22:44:21] players::Registry: released Player Element eid=2277 for peerSlot=0
...
client 48746: [22:44:21] net: session stopped (sent=243080 recv=333590)
client 48750: [22:44:21] net: left gameplay to the menu (native quit) -- native menu travel already in flight; session stopped + held dormant (no second transition)
```

### 4. Client: both rejoin attempts time out while the host is still in gameplay

```
client 48770: [22:44:50] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client 48812: [22:45:03] server_browser_rows: row selected (6e8c52d9c63943bd)   ← the session's lobbyId, still listed
client 48798: [22:45:01] [WARN ] net: peer slot 0 closed (oldState=1 reason='Timed out attempting to connect')
client 48800: [22:45:01] [WARN ] join_progress: join FAILED (Timed out attempting to connect) -- aborting + reopening the browser

client 48817: [22:45:06] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client 48842: [22:45:16] [WARN ] net: peer slot 0 closed (oldState=1 reason='Timed out attempting to connect')
client 48844: [22:45:16] [WARN ] join_progress: join FAILED (Timed out attempting to connect) -- aborting + reopening the browser
```

### 5. Client: game restart; the third attempt — after the re-host — connects immediately

`client_050920260005_multivoid.log` starts with boot lines: `session_manager: configured` 22:46:38, `death_revive: travel veto published` 22:46:40, menu restored 22:46:48. The client waits in the browser, and:

```
client (file2) 127: [22:48:27] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client (file2) 163: [22:48:27] net: session started role=client topology=P2P sendHz=60
client (file2) 184: [22:48:28] player_handshake: slot 0 connect line shown ('Mikuzavr')
client (file2) 4326: [22:49:01] join_progress: BeginSnapshot -- receiving world (2785 objects)
```

### 6. Host: the departure arrives as a clean session stop; the native game continues

```
host 36588: [22:44:21] [WARN ] net: peer slot 1 closed (oldState=3 reason='session stop')
host 36589: [22:44:21] net: all peers gone -- session back to Disconnected
host 36634: [22:44:21] feed: push via=event keep=history nickLen=0 lines=1 text="PlayerNickname2 left the game"
host 36637: [22:44:21] net: peer slot 1 (#2) left -- puppet destroyed
```

### 7. Host: two seconds later the host dies — and the death handling works as designed

```
host 36565: [22:44:20] pos diag: local actor=(48960,10592,15626) ...   ← falling (z drops ~11 m by 22:44:24)
host 36645: [22:44:23] net: RagdollPose emit #2640 -> pelvis(49173, 8390, 14632) ... |linVel|=1373 cm/s
host 36659: [22:44:23] death_revive: local death ARMED -- the native death runs to completion (~10 s: sound, black screen at +5 s) and the level travel will be refused
host 36691: [22:44:33] death_revive: level travel REFUSED at UGameplayStatics::OpenLevel -- the world is kept; reviving on this pump task
host 36693: [22:44:33] death_revive: gameInstance.NewVar_1 cleared (the 'PQXYyeofZ8cr5rJD4YXLVw' death option a vetoed travel leaves armed) ok=1
host 36695: [22:44:33] teleport_client: applied (local=... path=teleportWObackrooms loc=(-37695,69978,6420) ...)
host 36696: [22:44:33] death_revive: REVIVE OK -- vitals=1 wake=1 tele=1 menu=1 deadClr=1 | ... ragdoll=0 dead=0 hp=99.6 distKPP=0 cm (tol 300)
host 36697: [22:44:33] net: RagdollPose stream STOP (local player recovered)
```

### 8. Host: nothing incoming during the client's attempts — the listener died with the session

While the client's two join attempts burn their timeouts (22:44:50–22:45:16), the host's log records NOTHING incoming — no `signaling` session, no `PENDING`, no admission/handshake trace between the teardown (22:44:21) and 22:47:47. The listener was gone with the session: `net: P2P host listening` exists only at host session starts — 21:48:46 (original, host 187) and then 22:48:04 (host 37172, the re-host); there is no such line in between.

### 9. Host: quit-to-menu + re-host restores joinability

```
host 37068: [22:47:47] world_identity: current world 000002C8D4A0A8B0 -> 0000000000000000 (gen=7 ...)
host 37097: [22:47:50] [WARN ] net: gameplay->MENU while a session is live (VOTV quit-to-menu?) -- ending the session ...
host 37112: [22:47:50] harness: host session ended -- returning to the main menu
host 37145: [22:48:01] host_session_settings: HOST accepted -- world=s_Coop 3 conn=0 listed=1 locked=1 name='Mikuzavr's game'
host 37154: [22:48:02] engine: LoadStorySave -- at preLoad/menu; (re)issuing 'open untitled_1' (save 's_Coop 3' registered)
host 37172: [22:48:04] net: P2P host listening as 'gen:8cc21b...' via signaling master.multivoid.dev:10000 (hListen=0x00020000 ...), capacity=3 clients
host 37173: [22:48:04] net: session started role=host topology=P2P sendHz=60
...
host 42035: [22:48:28] net: host accepted client into PENDING 0 (h=0xa499a6c1) -- no seat until admitted
host 42049: [22:48:28] net: PENDING 0 ADMITTED -> slot 1 (identity-bound, guid 7d99facc4a57a906380df1668e38bca5)
host 42092: [22:48:29] player_handshake: slot 1 connect line shown ('PlayerNickname2')
```

Note: the host's "restart" is **not a process restart** in the log — the log is continuous and the world-gen counter runs unbroken through the transition (gen=7 → 8 → 9 → 10 at 22:47:47–22:48:03). What restored joinability was quit-to-menu + re-host (`s_Coop 3`), i.e., a full host session re-creation with a world reload.

## What the logs cannot show

- What killed the client — no death line, no ragdoll at the death moment; a knockdown-recovery 40 s earlier and a ramping FOG event are context only (Evidence 1).
- Which of the two client-side failure mechanisms applies: whether the death skips the published OpenLevel veto entirely (a menu-bound travel path the veto never sees) or `death_revive` arming/detection simply doesn't run client-side — the logs show only the absence of any trace.
- Why the master-server lobby listing survived the listener's death (client-side stale cache vs server-side TTL) — only the effect (still listed, 10 s timeouts) is observable.
- Whether the ~10–11 s silent `Timed out attempting to connect` is the expected failure shape for a dead listener — the client side (signaling session created, then silence) is visible, but the master-server side is not logged.

## Analysis

Two stacked problems:

**A. A client death is a session death.** The mod's intended behavior ("a coop death now keeps the world", `death_revive: travel veto published`) demonstrably works on the host: her death at 22:44:23 was armed, the native travel refused at `UGameplayStatics::OpenLevel`, and the player revived in place (hp=99.6). On the client, the same native death flow (same ~10 s timing to the transition) produced no `death_revive` trace at all — no arming, no refusal — and the game traveled to the main menu. The mod's net layer then saw `gameplay->MENU while a session is live` and ended the session by design (RAM-balloon guard), kicking the client. Whether the client-side death (a) skips the vetoed seam entirely (a menu-bound path the published OpenLevel veto never sees — note the client's menu travel was "already in flight" when the mod noticed), or (b) `death_revive` arming/detection simply doesn't run client-side, cannot be distinguished from the logs. Either way: any client death currently ends the coop session and dumps the client to the menu.

**B. An in-game host whose session dissolved is unjoinable, but still listed.** After `all peers gone`, the host's mod session went to Disconnected while the native game kept running. The P2P listener and signaling registration died with the session (no `P2P host listening` re-creation, zero incoming-connection traces during the client's attempts), yet the master-server **lobby listing survived** — the browser still offered `6e8c52d9c63943bd` ('Mikuzavr's game'), so every join attempt looked promising client-side and then burned a 10–11 s timeout (`Timed out attempting to connect`). Only a full quit-to-menu + re-host rebuilt the listener (`P2P host listening ... capacity=3` at 22:48:04), after which the client connected on the first try (accepted into PENDING at 22:48:28). Practical consequence: after any client-side session tear, the host must abandon the world (quit-to-menu, re-host, world reload — losing the live session state) to become reachable again; there is no "re-open to peers" path from inside the game.

Suggested directions: intercept/cover the client's death→menu path with the same veto+revive choreography the host gets (or arm `death_revive` on clients); if a gameplay→MENU tear happens, keep the host listening (auto re-arm the P2P listener for the existing save) or withdraw/refresh the stale master-server listing so clients don't burn timeouts against a dead session; optionally offer in-game "re-open session" for the host.

## Addendum (2026-09-06): the same tear from the host side — [→ Bug 14 «Host death tore down the session — the death_revive veto silently failed to arm, live client kicked»]

The night produced a second teardown of this class, at 00:03:34, this time initiated by the **host's** death; the full chain is documented separately in the linked report. What it adds to this bug's picture:

1. **The `death_revive` veto is flaky, not host-reliable.** It vetoed this bug's host death (22:44:23) and a second one (22:59:32), then silently failed to arm on its third invocation (~00:03:24). The un-vetoed death travel reached the menu, the mod classified it as a manual quit, tore the session and retired the lobby.
2. **The surviving peer gets ejected too.** In this bug the client left voluntarily (after his own death); at 00:03:34 the live client was kicked mid-game with `HOST CLOSED OUR CONNECTION (reason: session stop)` — no action on his part. A host-side death tear is therefore strictly worse than this bug's client-side tear.
3. **Point B is reinforced from the clean side.** This time the host re-hosted (lobby `b238b592024d039b`, 00:06:44) *before* the client attempted to connect, and the join succeeded on the first try with zero timeouts (BeginConnect 00:07:16 → in-game 00:07:42) — consistent with B: reachability exists only after quit-to-menu + re-host, never from the in-game host.
4. **The asymmetry closes from both ends.** A native menu travel on the host tears the world for everyone; the same travel on the client only orphans the host (re-confirmed at 00:12:28: client quit → host stayed in-world solo, quit herself at 00:13:00). See the Bug 14 report for details.

---

# Баг 10: Смерть клиента разрывает сессию; хост в игре становится недоступной для подключения

## Метаданные

| Поле | Значение |
|---|---|
| Окно видео | смерть клиента `~00:55:46`; выход в меню + разрыв сессии `00:55:57–00:55:58`; неудачные подключения `00:56:27` и `00:56:43` (таймауты `00:56:38`, `00:56:53`); перезапуск игры клиента `00:57:18–00:58:25`; выход хоста в меню + перехост `00:59:24–00:59:41`; успешное переподключение `01:00:05` |
| Окно лога | смерть клиента `~22:44:09` (прямой строки нет — выведено, см. Доказательство 1) → клиент допущен `22:48:28` (2026-09-04) |
| Файлы логов | клиент: `client_040920262246_multivoid.log` (разрыв + оба таймаута) и `client_050920260005_multivoid.log` (перезапуск игры клиента + успешное переподключение); хост: `host_050920260342_multivoid.log` |
| Докладчик | клиент (`PlayerNickname2`) умирает и выкидывается в меню; хост (`Mikuzavr`) умирает через 2 с (оживлена на месте), продолжает играть, затем перехостится, чтобы клиент мог вернуться |
| Пиры | Multivoid 0.9.0n b150, VOTV Alpha 0.9.0n; хост = slot 0, клиент = slot 1 |
| Подсистема | travel-вето `death_revive` (след только на хосте) + разрыв сессии на gameplay→MENU; жизненный цикл P2P-слушателя + устаревший листинг лобби |
| Ключевые моменты | мир→меню `22:44:20`, сессия остановлена `22:44:21`; смерть хоста, вето вооружено `22:44:23`, переход ОТКЛОНЁН + ОЖИВЛЕНИЕ ОК `22:44:33`; попытки подключения `22:44:50→22:45:01` и `22:45:06→22:45:16`; хост слушает после перехоста `22:48:04`; клиент допущен `22:48:28` |

## Описание

Персонаж клиента умер (**в лог не попало напрямую** — см. Доказательство 1; последствия доказаны логом). Вместо того чтобы сохранить мир (задуманное поведение мода «смерть в коопе сохраняет мир»), игра клиента перешла в **главное меню** — а живая сессия была разорвана как «выход в меню» (**доказано логом**). Клиента выкинуло. Сессия хоста распалась вместе с ней (`all peers gone`), но **нативный мир хоста продолжал работать** — она осталась в геймплее и даже продолжала играть.

Два отягчающих обстоятельства:

1. **Обработка смерти асимметрична.** Через две секунды после ухода клиента умерла и *хоста* — и у неё подсистема `death_revive` мода сработала ровно как задумано: вооружилась, отклонила переход между уровнями, оживила игрока на месте (**доказано логом**). У **клиента** тот же поток смерти не оставил **никакого** следа `death_revive`: ни вооружения, ни отказа, ни оживления (**доказанное логом отсутствие**). Нативная смерть прошла без перехвата и закончилась в меню.
2. **Хост-сирота недоступна для подключения.** Лобби `'Mikuzavr's game'` оставалось видимым в браузере серверов (устаревший листинг), но P2P-слушатель хоста исчез вместе с сессией (**доказано логом**). Каждая попытка подключения сжигала ~10-секундный таймаут (`Timed out attempting to connect`), не оставляя никакого следа в логе хоста. Клиент смог вернуться только после того, как хост вышла в меню и перехостилась на сохранение (`s_Coop 3`) — слушатель был пересоздан, и следующая попытка подключилась за секунды (**доказано логом**).

## Доказательства

### 1. Замечание о тайминге: момент смерти клиента выведен (прямой строки нет)

Момент смерти клиента **в лог не попал напрямую** (нет строк смерти, нет рэгдолла — см. Доказательство 2). Он выведен: нативный поток смерти занимает ~10 с (собственная документация мода в `death_revive: local death ARMED ... ~10 s: sound, black screen at +5 s`), а переход клиента в меню зафиксирован в `22:44:20` — что даёт смерть на `~22:44:09–10`, ровно начало заявленного пользователем окна (`00:55:46` = `22:44:09`). Окно пользователя `00:55:46–00:56:00` покрывает смерть → выкидывание точно.

Контекст (факты; причина в логах мода не видна): за 40 с до смерти клиент был сбит с ног рэгдолл-ударом и поднялся (`22:43:27–30`); набирал силу погодное событие FOG (плотность тумана 0.03→0.09 за `22:43:09–35`). Хоста умерла от падения с ~11 м (z 15626→14497 в телеметрии рэгдолла) в `22:44:20–23` — через ~15 с после смерти клиента, в ~180 м от того места. Делят ли две смерти общую причину — по логам не видно; клиентский игрок может подтвердить, что его убило.

### 2. Клиент: смерть не оставила следа `death_revive`

Во всём файле клиента `death_revive` встречается только на загрузке — установка вето и revive-глаголы (клиент 82 [21:42:18], 319 [21:49:23]). В момент смерти: ничего. Последнее событие рэгдолла — сбит с ног и поднялся за 40 с до того:

```
client 47157: [22:43:27] net: RagdollPose emit #960 -> pelvis(55979, 25800, 17371) ...
client 47184: [22:43:30] net: RagdollPose stream STOP (local player recovered)
```

### 3. Клиент: игра уходит в меню, сессия умирает

UI меню пересобран, пока сессия ещё жива (`net stats: state=2` в 22:44:20):

```
client 47873: [22:44:20] world_identity: current world 00000187496C7120 -> 0000018790E55950 (gen=7, pc=...)
client 47891: [22:44:20] multiplayer_menu: menu restored for connect (opacity 1, hit-test on)
client 47899: [22:44:20] multiplayer_menu: MULTIPLAYER button injected into menu=000001878F09DAD0 ...
client 47900: [22:44:21] [WARN ] net: gameplay->MENU while a session is live (VOTV quit-to-menu?) -- ending the session + stopping the layer churn (RAM-balloon guard)
client 47901: [22:44:21] players::Registry: released Player Element eid=2277 for peerSlot=0
...
client 48746: [22:44:21] net: session stopped (sent=243080 recv=333590)
client 48750: [22:44:21] net: left gameplay to the menu (native quit) -- native menu travel already in flight; session stopped + held dormant (no second transition)
```

### 4. Клиент: обе попытки переподключения сгорают в таймаутах, пока хост ещё в геймплее

```
client 48770: [22:44:50] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client 48812: [22:45:03] server_browser_rows: row selected (6e8c52d9c63943bd)   ← lobbyId сессии, всё ещё в списке
client 48798: [22:45:01] [WARN ] net: peer slot 0 closed (oldState=1 reason='Timed out attempting to connect')
client 48800: [22:45:01] [WARN ] join_progress: join FAILED (Timed out attempting to connect) -- aborting + reopening the browser

client 48817: [22:45:06] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client 48842: [22:45:16] [WARN ] net: peer slot 0 closed (oldState=1 reason='Timed out attempting to connect')
client 48844: [22:45:16] [WARN ] join_progress: join FAILED (Timed out attempting to connect) -- aborting + reopening the browser
```

### 5. Клиент: перезапуск игры; третья попытка — уже после перехоста — подключается сразу

`client_050920260005_multivoid.log` начинается со строк загрузки: `session_manager: configured` 22:46:38, `death_revive: travel veto published` 22:46:40, меню восстановлено 22:46:48. Клиент ждёт в браузере, и:

```
client (file2) 127: [22:48:27] join_progress: BeginConnect -- loading screen up (connecting to 'Mikuzavr's game')
client (file2) 163: [22:48:27] net: session started role=client topology=P2P sendHz=60
client (file2) 184: [22:48:28] player_handshake: slot 0 connect line shown ('Mikuzavr')
client (file2) 4326: [22:49:01] join_progress: BeginSnapshot -- receiving world (2785 objects)
```

### 6. Хост: уход приходит как чистая остановка сессии; нативная игра продолжается

```
host 36588: [22:44:21] [WARN ] net: peer slot 1 closed (oldState=3 reason='session stop')
host 36589: [22:44:21] net: all peers gone -- session back to Disconnected
host 36634: [22:44:21] feed: push via=event keep=history nickLen=0 lines=1 text="PlayerNickname2 left the game"
host 36637: [22:44:21] net: peer slot 1 (#2) left -- puppet destroyed
```

### 7. Хост: двумя секундами позже умирает хоста — и обработка смерти работает как задумано

```
host 36565: [22:44:20] pos diag: local actor=(48960,10592,15626) ...   ← падение (z падает ~11 м к 22:44:24)
host 36645: [22:44:23] net: RagdollPose emit #2640 -> pelvis(49173, 8390, 14632) ... |linVel|=1373 cm/s
host 36659: [22:44:23] death_revive: local death ARMED -- the native death runs to completion (~10 s: sound, black screen at +5 s) and the level travel will be refused
host 36691: [22:44:33] death_revive: level travel REFUSED at UGameplayStatics::OpenLevel -- the world is kept; reviving on this pump task
host 36693: [22:44:33] death_revive: gameInstance.NewVar_1 cleared (the 'PQXYyeofZ8cr5rJD4YXLVw' death option a vetoed travel leaves armed) ok=1
host 36695: [22:44:33] teleport_client: applied (local=... path=teleportWObackrooms loc=(-37695,69978,6420) ...)
host 36696: [22:44:33] death_revive: REVIVE OK -- vitals=1 wake=1 tele=1 menu=1 deadClr=1 | ... ragdoll=0 dead=0 hp=99.6 distKPP=0 cm (tol 300)
host 36697: [22:44:33] net: RagdollPose stream STOP (local player recovered)
```

### 8. Хост: во время попыток клиента ничего входящего — слушатель умер вместе с сессией

Пока две попытки подключения клиента сжигают свои таймауты (22:44:50–22:45:16), в логе хоста нет НИЧЕГО входящего — ни `signaling`-сессии, ни `PENDING`, ни следа допуска/рукопожатия между разрывом (22:44:21) и 22:47:47. Слушатель исчез вместе с сессией: `net: P2P host listening` существует только на стартах сессий хоста — 21:48:46 (исходная, хост 187) и затем 22:48:04 (хост 37172, перехост); между ними такой строки нет.

### 9. Хост: выход в меню + перехост возвращает доступность

```
host 37068: [22:47:47] world_identity: current world 000002C8D4A0A8B0 -> 0000000000000000 (gen=7 ...)
host 37097: [22:47:50] [WARN ] net: gameplay->MENU while a session is live (VOTV quit-to-menu?) -- ending the session ...
host 37112: [22:47:50] harness: host session ended -- returning to the main menu
host 37145: [22:48:01] host_session_settings: HOST accepted -- world=s_Coop 3 conn=0 listed=1 locked=1 name='Mikuzavr's game'
host 37154: [22:48:02] engine: LoadStorySave -- at preLoad/menu; (re)issuing 'open untitled_1' (save 's_Coop 3' registered)
host 37172: [22:48:04] net: P2P host listening as 'gen:8cc21b...' via signaling master.multivoid.dev:10000 (hListen=0x00020000 ...), capacity=3 clients
host 37173: [22:48:04] net: session started role=host topology=P2P sendHz=60
...
host 42035: [22:48:28] net: host accepted client into PENDING 0 (h=0xa499a6c1) -- no seat until admitted
host 42049: [22:48:28] net: PENDING 0 ADMITTED -> slot 1 (identity-bound, guid 7d99facc4a57a906380df1668e38bca5)
host 42092: [22:48:29] player_handshake: slot 1 connect line shown ('PlayerNickname2')
```

Примечание: «перезапуск» хоста — **не перезапуск процесса** в логе — лог непрерывен, и счётчик генераций мира идёт без разрыва через переход (gen=7 → 8 → 9 → 10 в 22:47:47–22:48:03). Доступность вернул выход в меню + перехост (`s_Coop 3`), т.е. полное пересоздание сессии хоста с перезагрузкой мира.

## Чего логи показать не могут

- Что убило клиента — ни строки смерти, ни рэгдолла в момент смерти; сбивание с ног за 40 с до и набор силы FOG — только контекст (Доказательство 1).
- Какой из двух клиентских механизмов сбоев работает: переходит ли смерть в обход опубликованного вето OpenLevel (путь в меню, которого вето вообще не видит) или же вооружение/детекция `death_revive` просто не выполняется на клиенте — логи показывают только отсутствие какого-либо следа.
- Почему листинг лобби мастер-сервера пережил смерть слушателя (устаревший кэш на клиенте vs TTL на сервере) — наблюдаем только эффект (всё ещё в списке, 10-секундные таймауты).
- Является ли ~10–11-секундный тихий `Timed out attempting to connect` ожидаемой формой отказа при мёртвом слушателе — клиентская сторона (signaling-сессия создана, затем тишина) видна, но сторона мастер-сервера не логируется.

## Анализ

Две наложенные проблемы:

**A. Смерть клиента — это смерть сессии.** Задуманное поведение мода («смерть в коопе теперь сохраняет мир», `death_revive: travel veto published`) доказуемо работает на хосте: её смерть в 22:44:23 была вооружена, нативный переход отклонён на `UGameplayStatics::OpenLevel`, игрок оживлён на месте (hp=99.6). У клиента тот же нативный поток смерти (тот же тайминг ~10 с до перехода) не оставил вообще никакого следа `death_revive` — ни вооружения, ни отказа — и игра перешла в главное меню. Сетевой слой мода затем увидел `gameplay->MENU while a session is live` и разорвал сессию по дизайну (RAM-balloon guard), выкинув клиента. Различить, (а) переходит ли смерть клиента в обход ветоированного шва целиком (путь в меню, которого опубликованное вето OpenLevel вообще не видит — заметим, переход клиента в меню был «уже в полёте», когда мод это заметил), или (б) вооружение/детекция `death_revive` просто не выполняется на клиенте, по логам нельзя. В любом случае: любая смерть клиента сейчас завершает кооп-сессию и выкидывает клиента в меню.

**B. Хост в игре, чья сессия распалась, недоступна, но всё ещё в списке.** После `all peers gone` сессия мода хоста ушла в Disconnected, а нативная игра продолжала работать. P2P-слушатель и signaling-регистрация умерли вместе с сессией (пересоздания `P2P host listening` нет, ноль следов входящих подключений во время попыток клиента), однако **листинг лобби мастер-сервера пережил** — браузер всё ещё предлагал `6e8c52d9c63943bd` ('Mikuzavr's game'), поэтому каждая попытка подключения выглядела многообещающе на клиенте, а затем сжигала 10–11-секундный таймаут (`Timed out attempting to connect`). Только полный выход в меню + перехост пересобрали слушатель (`P2P host listening ... capacity=3` в 22:48:04), после чего клиент подключился с первой попытки (принят в PENDING в 22:48:28). Практическое следствие: после любого клиентского разрыва сессии хост должна покинуть мир (выход в меню, перехост, перезагрузка мира — с потерей живого состояния сессии), чтобы снова стать достижимой; пути «заново открыть для пиров» изнутри игры нет.

Направления: перехватить/покрыть путь смерть→меню клиента той же хореографией вето+оживление, которую получает хост (или вооружать `death_revive` на клиентах); если происходит разрыв gameplay→MENU, оставлять хост слушающей (автоматически перевооружать P2P-слушатель для существующего сохранения) или отзывать/обновлять устаревший листинг мастер-сервера, чтобы клиенты не сжигали таймауты о мёртвую сессию; опционально предложить хосту внутриигровое «заново открыть сессию».

## Аддендум (2026-09-06): тот же разрыв со стороны хоста — [→ Баг 14 «Смерть хоста разорвала сессию — вето death_revive молча не сработало, живого клиента выкинуло»]

За ночь случился второй разрыв этого класса, в 00:03:34, на этот раз запущенный смертью **хоста**; полная цепочка задокументирована отдельно в связанном отчёте. Что это добавляет к картине данного бага:

1. **Вето `death_revive` нестабильно, а не «надёжно на хосте».** Оно ответировало смерть хоста из этого бага (22:44:23) и вторую (22:59:32), затем молча не сработало на третьем вызове (~00:03:24). Неотменённый переход смерти дошёл до меню, мод классифицировал его как ручной выход, разорвал сессию и закрыл лобби.
2. **Выживший пир тоже выбрасывается.** В этом баге клиент вышел добровольно (после собственной смерти); в 00:03:34 живого клиента выкинуло посреди игры с `HOST CLOSED OUR CONNECTION (reason: session stop)` — без каких-либо действий с его стороны. Разрыв от смерти хоста, таким образом, строго хуже клиентского из этого бага.
3. **Тезис B подтверждается с чистой стороны.** В этот раз хост перехостилась (лобби `b238b592024d039b`, 00:06:44) *до* попытки клиента, и подключение прошло с первой попытки без таймаутов (BeginConnect 00:07:16 → в игре 00:07:42) — согласуется с B: доступность появляется только после выхода в меню + перехоста, изнутри игры — никогда.
4. **Асимметрия закрывается с обоих концов.** Нативный переход в меню на хосте рвёт мир для всех; тот же переход на клиенте лишь оставляет хоста-сироту (подтверждено повторно в 00:12:28: выход клиента → хост осталась в мире соло, сама вышла в 00:13:00). Подробности — в отчёте Баг 14.
