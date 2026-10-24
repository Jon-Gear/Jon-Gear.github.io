---
layout: project
title: "IgKnighted — Local Multiplayer Arena Shooter in Unreal Engine"
role: "Gameplay Programmer"

version: short
short_url: /projects/year-1-block-d-short
long_url: /projects/year-1-block-d-long

date: 2024-06-28
categories: [featured_projects]
image: /assets/images/Y1D/main.gif
description: "A local 1–2 player co-op bullet-hell arena shooter built in Unreal Engine 5, and a quick tour of the systems behind it: the gameplay framework, player and abilities, power-ups, three enemy archetypes, and the three-part level with dynamic difficulty."
contributions: "Gameplay Framework & Core Architecture | Player: Input, Controller, Pawn, State & Abilities | Collectables & Power-Ups | Enemy AI: Crawler, Scorpion, Mollusk | Level, Spawning & Dynamic Difficulty"
tools: "Unreal Engine 5, C++, Blueprints, Gameplay Framework, Enhanced Input, Gameplay Tags, Behavior Trees, UMG, Animation Blueprints, Niagara"
team_size: "4 Programmers | 3 Designers | 5 Artists"
platforms: "Windows"
duration: "8 Weeks (May 2024 - Jun 2024)"
---

# 💼 IgKnighted — A Stylized Local Multiplayer Arena Shooter

![IgKnighted — the front end where a run is configured](../../assets/images/Y1D/main.jpg)

[▶️ Play IgKnighted on itch.io](https://buas.itch.io/igknighted)

IgKnighted is a couch co-op bullet-hell arena shooter built in Unreal Engine 5 by a team of twelve over eight weeks. One or two players descend through a three-part flooded temple, fight three enemy archetypes, collect power-ups, and share a single pool of five lives.

I was one of **four programmers**. This is the short version: each system I owned, the main problem it solved, and what that solution cost.

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted" target="_blank" rel="noopener">⚙️ See Code Examples</a>

---

## 👤 Zhangir Before the Project

I came in knowing Unity and self-taught programming, not Unreal. I had never used the Gameplay Framework, Enhanced Input, Behaviour Trees, Gameplay Tags, UMG, Animation Blueprints or Niagara. My previous work was single-player prototypes where state could live in the one Blueprint that used it and "it works when I press play" counted as done.

The goal was to stop building prototypes and start building systems another person could safely change — under eight weeks, four programmers, designers who needed to tune without a compile, artists who needed hooks, and a public itch.io release.

---

## 🔨 My Contributions

## 📋 01. Gameplay Framework & Core Architecture

![IgKnighted — the front end where a run is configured](../../assets/images/Y1D/gameplay_framework.png)

### 🎯 What was the Goal?

Match every piece of game state to the object whose lifetime it actually needs. `GameInstance` is the **session**, `GameMode` is the **rules**, `GameState` is the **numbers**.

### 💡 What I built

- **Session — `GameInstance`.** Run settings (player count, difficulty, tutorial, dynamic difficulty) live here and survive level travel. The `GameMode` reads them **once** at `BeginPlay`, so a menu widget cannot change the run halfway through.
- **Rules — `GameMode`.** `FinishMatch` is the only way to end a match. It refuses a second ending and clears every timer it owns, so a respawn cannot fire on the results screen. If the last life and the final kill share a frame, **defeat wins**.
- **Numbers — `GameState`.** Lives, match phase and result live here, with events for the HUD and zone systems. Only the `GameMode` writes them; the `GameState` stores, clamps and broadcasts. It holds no Pawn or enemy pointers.

### 💥 Impact

Difficulty now holds across menu → tutorial → arena, retries and results screens. The match has one owner and one ending. The HUD can read state without depending on the class that decides it.

### 📉 Main tradeoffs

`GameInstance` is global mutable state; reading settings once means no live difficulty change; the `GameState` is not replicated because the game is local co-op.

---

## 📋 02. Player

![IgKnighted — Enhanced Input actions and per-player mapping contexts](../../assets/images/Y1D/4.png)

### 🎯 What was the Goal?

Build a player that behaves the same with one player, two players, a gamepad, a mouse, after a death and after a retry — without special cases in four systems.

| Object | Owns | Survives |
|---|---|---|
| `PlayerController` | input devices, aim intent | Pawn death, level travel |
| `APawn` | movement, aim direction, components | nothing |
| `PlayerState` | identity, score, multiplier, colour | Pawn death |
| Components | one ability each | the Pawn's lifetime |
| `GameMode` | the rules that use all of the above | the level |

### 💡 What I built

- **Input.** Bind actions once in `SetupInputComponent`; install mapping contexts in `OnPossess`, resolved from that controller's own `LocalPlayer`. Input then survives respawn and travel, and two players get separate context stacks.
- **Aiming.** The controller collects intent; stick aim is built from camera yaw, mouse aim from the cursor. Both write one `AimDirection`. A shared `IIgKnightedAimProvider` lets the same combat component fire for the player and the Scorpion.
- **The Pawn.** The level owns the fixed top-down camera. The body orients to movement, a weapon pivot orients to aim. Combat, Dash and Status are separate components, not one growing character class.
- **Player state.** Score, multiplier and colour live on `PlayerState`, so they survive the Pawn. The HUD binds once, unbinds before rebinding and reads values instead of computing them.
- **Move.** Normalise once, after the deadzone and sprint multiplier, in the camera's yaw plane — diagonal movement is no longer 1.41× faster.
- **Shoot.** Cadence is weapon data, not animation length or tap speed. Spread is symmetric around the aim axis, so three shots are `-half`, `0`, `+half`.
- **Dash.** Invulnerability is enabled **before** the first frame of movement, so a hazard resolved that frame is dodged. Each enemy is killed once per dash.
- **Die.** The Pawn latches `bIsDead` and stops acting; the `GameMode` spends exactly one shared life and schedules the respawn. Presentation never decides the rule.

### 💥 Impact

Two players, two devices and two HUD panels work through the same code path. A dead player cannot move, shoot, dash or be damaged again, and a respawn cannot leave the next body with old state.

### 📉 Main tradeoffs

Mouse aim needs a visibility hit; there is no aim assist. The smoothed weapon pivot visually lags the true aim. The HUD has both durable state (`PlayerState`) and temporary body state (dash charges). Dash traversal uses a per-frame sphere overlap rather than a sweep.

---

## 📋 03. Powerups

![IgKnighted — collectable pickups, the shield and the Power Shot buff](../../assets/images/Y1D/6.png)

### 🎯 What was the Goal?

A pickup must be **readable**, **refusable**, **temporary** where designed, and **fair in co-op**. A buff is a state, not an event: the damage resolver, the HUD and the pickup all need to see it without owning it.

| Pickup | Effect | Duration | Stacking rule |
|---|---|---|---|
| **Shield** | Absorbs the next lethal hit | until spent | cannot pick up a second |
| **Power Shot** | Stronger weapon | 10 s | cannot refresh while active |
| **Enhanced Dash** | Two dash charges | 10 s | cannot refresh while active |
| **Matchstick** | Instant score | — | always collectable |

### 💡 What I built

- **One pickup class, four data configurations.** The sphere is the single overlap authority; the mesh is decoration. The pickup asks the status component `CanAcceptBuff`, so a shielded player cannot waste a shield pickup.
- **Shield.** A charge owned by the status component, consumed once. The damage resolver checks **dash i-frames first**, then the shield — dashing is free and must not spend it.
- **Power Shot.** Weapon swaps go through one function that remembers whether the player was firing, stops, swaps and restarts at the new cadence. A second pickup is refused instead of starting a second expiry timer.
- **Enhanced Dash.** One entry point changes the maximum and clamps `CurrentCharges` on both activation and expiry, so the HUD never shows `2/1`. Respawn clears buff timers before the next body spawns.
- **Damage resolver.** A stateless world subsystem with a documented order: dash → shield → lethal. Water, projectiles and melee all produce the same behaviour without knowing about each other.

### 💥 Impact

Adding a buff is a new enum value and a branch in `CanAcceptBuff`, not a new Blueprint, timer and HUD binding. One touch collects once; one shield absorbs once; one buff expiry restores the previous state.

### 📉 Main tradeoffs

The pickup still knows about `EIgBuffKind`. Respawn is unconditional, so a refused pickup comes back. Enhanced Dash activation deliberately refills charges — a design choice that looks like a bug. There is no shield-piercing damage type and no buff remaining-time API.

---

## 📋 04. Enemies

![IgKnighted — the three enemy archetypes and the shared behaviour tree](../../assets/images/Y1D/5.png)

### 🎯 What was the Goal?

Three archetypes that ask three different questions, but **not three codebases**. They share a body, damage pipeline, death path, score award and brain; only numbers and a Behaviour Tree differ.

| Archetype | Role | Attack | The question it asks |
|---|---|---|---|
| **Crawler** | Rusher | Melee swing, short telegraph | "Can you keep space?" |
| **Scorpion** | Skirmisher | Single aimed projectile | "Can you dodge a shot aimed at you?" |
| **Mollusk** | Artillery | Radial ring of projectiles | "Can you read a pattern and commit?" |

### 💡 What I built

- **Crawler.** Removed contact damage entirely. Damage comes only from the attack animation's notify, via one sphere query tagged `Ig.Player` and delivered through the damage interface. One swing is one hit.
- **Scorpion.** The blackboard is for decisions and memory; the shot asks the Pawn for the live target at fire time and spawns at the muzzle. Blackboard location is no longer a stale aim point.
- **Mollusk.** The radial burst is a full `360 / count` ring, offset so one projectile points at the player and every shot spawns at the muzzle. The gap moves with the target, but the pattern stays readable.
- **AI Controller.** Every blackboard write goes through one function, including the explicit "no target" case. It binds `OnTargetChanged` with `AddUniqueDynamic` on possession and unbinds on unpossession, so enemies stop chasing and shooting at a corpse.

### 💥 Impact

Every enemy attack is one deliberate event with one origin and one target. A bug fixed in the Crawler's death path is already fixed for the other two. Adding a fourth archetype is a definition, a tree and an animation — not a new damage pipeline.

### 📉 Main tradeoffs

Melee damage is tied to the animation notify; an interrupted montage deals no damage. The Scorpion does not lead its target. The Mollusk's ring is centred on the shooter, so point-blank targets are inside its origin. The controller is one class for three archetypes.

---

## 📋 05. Level

![IgKnighted — zone progression, barriers and encounter flow](../../assets/images/Y1D/3.png)

### 🎯 What was the Goal?

One arena, three parts, one camera, no loading screens, no checkpoints. Each part has to teach, escalate, gate progress on clearing its encounter, and survive a retry.

```text
PART 1   tutorial-shaped · Crawlers · first power-up
PART 2   open pool · hazards · Scorpion introduction
PART 3   tight corridors · Mollusk + Scorpion mix · final wave
```

### 💡 What I built

- **Encounter state machine.** The zone manager owns one encounter at a time and tracks both pending spawns and the alive set. A generation counter makes stale completion checks harmless.
- **Barriers open on cleared, not on spawned.** Completion is deferred by one tick so the frame's damage resolves first — that is what makes defeat win the same-frame race against victory.
- **Spawn registration before `FinishSpawning`.** An enemy cannot be damaged and killed while the manager still thinks it is a pending spawn. The spawner also spawns the first enemy immediately, so a part never starts with an empty arena.
- **Dynamic difficulty.** Normalised life pressure and kills per second feed a bounded `0.85–1.15` scale, damped over several seconds. It nudges the enemy count inside the chosen difficulty; it does not reclassify the game.
- **Authored difficulty stays authored.** Easy, Medium and Hard are rows of numbers in a data asset. The director only perturbs one column of the selected row.

### 💥 Impact

Barriers cannot open with enemies still alive, parts always complete, spawns cannot vanish before registration, and retries rebuild the same arena from the same actors. That repeatability is what made playtesting difficulty possible.

### 📉 Main tradeoffs

"One tick" is a scheduling assumption. The alive set is a linear array, fine for tens of enemies. Only `EnemyCountScale` is consumed from the difficulty tuning today. Barriers, spawners and part indices are parallel arrays that a designer must keep aligned.

---

## 💥 Overall Impact

**Before:** difficulty reset at level boundaries, two systems could end the match, input died after transitions, gamepad aim was world-relative, HUD counters doubled after respawn, diagonal movement was 41% faster, fire rate depended on tapping, spread was asymmetric, the dash protected the frame *after* it started, a dead player could still shoot, one Crawler could erase the life pool, enemies attacked corpses, and the adaptive difficulty oscillated.

**After:** every seam has a contract. Configuration survives travel, rules have one owner, abilities own one resource each, the HUD reads durable identity, one pickup class and one damage pipeline serve every buff, and the three archetypes share one brain. The run is repeatable enough to playtest.

The lesson underneath all of it: **a feature is not finished when it works — it is finished when the next feature cannot break it.**

---

## 📚 What I Learned

- **Architecture is a statement about lifetimes.** Every framework bug in §01 was state living in an object that died too early.
- **Rules belong in C++, content belongs in data.** A content edit must not be able to break ownership, damage order or match termination; tuning should not need a compile.
- **Most bugs are ordering bugs.** Shield before dash, victory before defeat, spawn registration after spawn, mapping contexts before possession. The lines were correct; the sequence was not.
- **One writer per fact.** Two owners of score, target or lives eventually disagree. Make the second write impossible instead of relying on care.
- **Feel bugs are arithmetic bugs.** 1.41× diagonal speed, asymmetric spread, a one-frame-late i-frame. Test the number, not just the behaviour.
- **Testing is a matrix, not a happy path.** The bugs that shipped were interactions: two deaths in one frame, a retry after the tutorial, a buff expiring during death.

---

## 🔄 Future Improvements

- Turn the damage resolver into a priority-ordered rule list with a pre-damage hook, so new defences are data rather than hard-coded order.
- Finish the difficulty tuning: route speed, attack interval, hazard uptime and collectable interval through the same data asset that already provides the enemy count.
- Give each archetype its own Behaviour Tree while keeping one controller class; add projectile pooling and a capsule sweep for dash traversal.
- Replace the zone manager's parallel arrays with a real part struct, add a leave-volume latch to pickups, and build the results screen on an append-only score ledger.

---

## 👤 Zhangir After the Project

I left able to open an empty Unreal project and build the systems a co-op game needs without a tutorial: the framework, input and possession flow, a data-driven enemy, a damage order, a spawn protocol and an encounter state machine. What changed most is that I stopped thinking of a feature as code I write and started thinking of it as a **contract I publish** — to the designer who tunes it, the artist who animates it, and the next programmer who has to change it without breaking the run.

*IgKnighted is playable on [itch.io](https://buas.itch.io/igknighted). The full technical write-up is in the [long version](https://github.com/Jon-Gear/Jon-Gear.github.io/blob/main/_projects/year-1-block-d-long.markdown); code excerpts are collected in [`_code_samples/igknighted`](https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted).*

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted" target="_blank" rel="noopener">⚙️ See Code Examples</a>
