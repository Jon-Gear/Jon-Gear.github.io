# IgKnighted — Blueprint Showcase Plan

Planning document for the Blueprint images that support the **Year 1: Block D — IgKnighted** case study.

The goal is to place Blueprints only where a Blueprint is the honest layer for the point being made:

- data defaults and per-instance configuration;
- presentation hooks exposed by C++;
- animation state and animation notifies;
- UI that binds to authoritative counters;
- level actors that listen to gameplay delegates.

Do not use Blueprints to re-implement rules that live in C++. The page's argument is that the rules are C++ and the Blueprint layer is deliberately thin.

All API names below come from `_code_samples/igknighted/*.h`. If a figure needs something that is not exposed there, the plan says so.

---

## 0. Current state and mismatches to fix first

The page currently has five Blueprint placeholders:

| File line | Placeholder | Status |
|---|---|---|
| `_projects/year-1-block-d.markdown:261` | `BP_IgKnightedGameMode` — `BeginPlay` flow | **Wrong layer.** Rewrite. |
| `_projects/year-1-block-d.markdown:677` | `BP_IgKnightedPlayerCharacter` — component list | Keep as a Components-panel screenshot, not a graph. |
| `_projects/year-1-block-d.markdown:1157` | `ABP_IgKnightedPlayer` — death state and notify calling `FinishDeath` | **Not buildable as a BP notify** unless `FinishDeath` is exposed. Rewrite. |
| `_projects/year-1-block-d.markdown:1286` | `BP_PowerUpPickup` — `BuffKind` and duration exposed | Keep, but make it a Details + cosmetics figure. |
| `_projects/year-1-block-d.markdown:1664` | `ABP_Crawler` — attack montage with melee notify | Keep, but split into AnimGraph + notify graph. |

### 0.1 `BP_IgKnightedGameMode` — the `BeginPlay` flow is C++

`AIgKnightedGameMode::BeginPlay` reads the Game Instance's pending run settings, initialises the Game State, and binds `OnLethalDamage`. The functions involved are not `BlueprintCallable`:

- `HandlePlayerEliminated` — plain C++.
- `FinishMatch` — plain C++.
- `AIgKnightedGameState::InitializeMatch` — plain C++.

A Blueprint child of the GameMode cannot reproduce that flow. Its legitimate seams are the two `BlueprintImplementableEvent`s:

- `BP_OnMatchFinished(EMatchResult Result)`
- `BP_OnPlayerRespawned(AIgKnightedPlayerController* Controller)`

Replace the placeholder caption with those two events.

### 0.2 `ABP_IgKnightedPlayer` — `FinishDeath` is not Blueprint-callable

`AIgKnightedPlayerCharacter::FinishDeath()` is public C++ but has no `UFUNCTION`, so a **Blueprint AnimNotify cannot call it**. A C++ `UAnimNotify` can. The sampled code contains no player-death notify class; the comment in `StartDeathPresentation` says one exists in the project.

Two honest options:

1. **Recommended for the page:** show the ABP event graph and AnimGraph only. Explain that `IsDeadState` drives the death state, and that the C++ notify calls `FinishDeath` at the contact frame (not shown).
2. **If the BP notify must be shown:** add `UFUNCTION(BlueprintCallable, Category = "Death")` to `FinishDeath`, rebuild, then create `AN_PlayerDeath` and show `Received Notify -> Get Owner -> Cast To IgKnightedPlayerCharacter -> Finish Death`.

Do not caption a Blueprint notify as calling `FinishDeath` until option 2 is done.

### 0.3 Fire montage Blueprint — `BP_PlayFireMontage` is unreachable

`UIgKnightedCombatComponent::BP_PlayFireMontage` is a `BlueprintImplementableEvent`, but the player constructs a **native** `UIgKnightedCombatComponent` with `CreateDefaultSubobject`. There is no `TSubclassOf<UIgKnightedCombatComponent>` property and no BP component template, so no Blueprint subclass can implement the event in the sampled code.

Consequences:

- Do not plan a "BP fire montage" figure.
- Show the C++ path instead: `UAnimNotify_IgFireProjectile` finds the combat component and calls the BlueprintCallable `SpawnProjectileFromMuzzle()`.
- If a BP version is wanted, create a BP AnimNotify that calls `SpawnProjectileFromMuzzle()` on the owner's combat component. That works without changing C++.
- The page's limitation bullet that says `BP_PlayFireMontage` "plays the base montage" is inaccurate under the sampled code. Either correct it or add the missing BP component subclass.

### 0.4 HUD widget creation is not in the samples

`UIgKnightedHUDWidget` exposes `BindToPlayerState` and `BindToGameState`, but nothing in the sampled C++ creates the widget or calls those functions. The figure can show the widget binding itself in `Construct`, but the page should say where the widget is created (BP PlayerController/GameMode, or a C++ HUD).

### 0.5 Other BP-surface limitations

- `AIgKnightedPowerUpPickup::CanBeCollectedBy`, `Collect` and `SetPickupAvailable` are not `UFUNCTION`s. The pickup Blueprint is defaults + cosmetics, not collection logic.
- `UIgKnightedEnemyDefinition` properties are `EditDefaultsOnly` but not `BlueprintReadOnly`, so other Blueprints should not read them. The owning enemy Blueprint can read its own `Definition` reference, but the plan avoids relying on that.
- `AIgKnightedGameState`'s counters are private `VisibleAnywhere` values with `BlueprintPure` getters. Use the getters.
- `AIgKnightedEnemyCharacter::IsDead` is a `BlueprintNativeEvent`/`BlueprintCallable` interface function, so an Anim BP can read it through the interface.

---

## 1. Figure map

| ID | Asset | Graph type | Page point | Purpose |
|---|---|---|---|---|
| F1 | `WBP_MainMenu` | UMG Event Graph | 01 / Challenge 1 | Writes `FIgRunSettings` to the Game Instance |
| F2 | `BP_IgKnightedGameMode` | Actor Event Graph | 01 / Challenge 1 | `BP_OnMatchFinished` / `BP_OnPlayerRespawned` presentation |
| F3 | `WBP_HUD` | UMG Event Graph | 02 / Challenge 4 | Binds score, shared lives and dash charges |
| F4a | `BP_IgKnightedPlayerCharacter` | Components panel | 02 / Challenge 3 | Native component hierarchy and `WeaponPivot` |
| F4b | `BP_IgKnightedPlayerCharacter` | Actor Event Graph | 02 / Challenge 6 | `OnFired` muzzle feedback; `BP_OnDeathStarted` |
| F5 | `ABP_IgKnightedPlayer` | Anim Event Graph + AnimGraph | 02 / Challenge 8 | Reads `IsDeadState`, `IsFiring`, `IsDashing`, `GetAimYaw` |
| F6 | `BP_PowerUpPickup` | Actor Event Graph + Details | 03 / Challenge 1 | One class, four data rows; `BuffKind` selects visuals |
| F7 | `WBP_ShieldIndicator` | UMG Event Graph | 03 / Challenge 2 | Binds to `OnShieldChanged` |
| F8 | `ABP_Crawler` + `AN_CrawlerMelee` | Anim Event Graph + AnimNotify | 04 / Challenge 1 | One swing, one hit at the contact frame |
| F9 | `BP_EnemyPresentation` | Actor Event Graph | 04 / Challenges 1–3 | Shared `BP_OnDefeated` presentation seam |
| F10 | `AN_ScorpionShot` / `AN_MolluskBurst` | AnimNotify | 04 / Challenges 2–3 | The same notify pattern, different attack verbs |
| F11 | `BP_Barrier` | Actor Event Graph | 05 / Challenge 1 | `BP_OnOpenChanged` visual; collision stays C++ |
| F12 | `BP_ZoneAnnouncer` | Actor Event Graph | 05 / Challenge 2 | Listens to `OnZoneStarted` / `OnZoneCleared` |
| F13 | `AN_HazardCycle` | AnimNotify | 04 Ch3 / 05 | Calls `BeginNewCycle` / `EndCycle` on the hazard |

Optional figures: F7, F10, F12, F13. They add value but are not required for the core argument.

---

## 2. Detailed figures

### F1. `WBP_MainMenu` — write the run once

**Placement:** §01 Challenge 1, after the Game Instance setter/read-once code.

**Asset:** Widget Blueprint, parent `UserWidget`.

**Widgets**

- `Button_Play`
- `SpinBox_Players` (1–2)
- `ComboBox_Difficulty` (Easy / Medium / Hard)
- `CheckBox_Tutorial`
- `CheckBox_Dynamic`

**Variables (UMG only, not gameplay state)**

| Name | Type | Default |
|---|---|---|
| `PlayerCount` | int | 1 |
| `Difficulty` | `EIgDifficulty` | Medium |
| `bTutorial` | bool | true |
| `bDynamic` | bool | false |

The widget controls set these four variables with simple `OnValueChanged` / `OnCheckStateChanged` events. The interesting graph is the Play handler.

**Graph: Play**

```
On Clicked (Button_Play)
  -> Make FIgRunSettings
       PlayerCount          = PlayerCount
       Difficulty           = Difficulty
       bTutorialEnabled     = bTutorial
       bDynamicDifficulty   = bDynamic
  -> Get Game Instance
  -> Cast To IgKnightedGameInstance
  -> Set Pending Run Settings (New Settings = struct)
  -> Open Level (by Name) (Level Name = L_Arena)
```

**C++ contract**

- `UIgKnightedGameInstance::SetPendingRunSettings(const FIgRunSettings&)` is `BlueprintCallable`.
- `GetPendingRunSettings()` is `BlueprintPure`.
- `FIgRunSettings` is a `BlueprintType` struct with `BlueprintReadWrite` fields.
- The GameMode reads the struct once in `BeginPlay`; nothing survives in the widget.

**Blueprintue description**

> The menu does not start the run. It writes one `FIgRunSettings` into the Game Instance and travels. The GameMode reads it exactly once at `BeginPlay`. No widget state and no GameMode state survives the level load; the Game Instance is the only object that does.

**Takeaway:** front-end configuration is data written to the persistent object, not state copied through level travel.

---

### F2. `BP_IgKnightedGameMode` — C++ decides, BP presents

**Placement:** §01 Challenge 1, replacing the incorrect `BeginPlay` placeholder at line 261.

**Asset:** Blueprint child of `AIgKnightedGameMode`, Event Graph.

**Variables (BP-only presentation assets)**

| Name | Type | Default |
|---|---|---|
| `VictorySting` | `SoundBase` | victory cue |
| `DefeatSting` | `SoundBase` | defeat cue |
| `RespawnBurst` | `NiagaraSystem` | respawn effect |
| `RespawnSound` | `SoundBase` | respawn cue |

**Event `BP_OnMatchFinished(Result)`**

```
BP_OnMatchFinished (Result)
  -> Switch on EMatchResult (Result)
       Victory:
         Play Sound 2D (VictorySting)
         Set Global Time Dilation (0.25)
         Set Timer by Event (Time = 1.0, Looping = false)
           -> FinishPresentation
       Defeat:
         Play Sound 2D (DefeatSting)
         Set Global Time Dilation (0.25)
         Set Timer by Event (Time = 1.0, Looping = false)
           -> FinishPresentation
       None:
         (nothing)
```

`FinishPresentation`:

```
Set Game Paused (true)
```

If a results screen exists later, `FinishPresentation` is where it is created. Do not invent one in the figure; the page currently lists the results screen as future work.

**Event `BP_OnPlayerRespawned(Controller)`**

```
BP_OnPlayerRespawned (Controller)
  -> Get Controlled Pawn (Controller)
  -> Get Actor Location
  -> Spawn System at Location (RespawnBurst, Location)
  -> Play Sound at Location (RespawnSound, Location)
```

**Blueprintue description**

> By the time these events run, the rule has already been applied: a shared life was spent, the match was ended, or the player was respawned. The Blueprint layer only adds game feel. It cannot spend a life or end the match; those functions are not Blueprint-callable.

**Also capture:** Class Defaults for `MaxSharedLives`, `RespawnDelaySeconds` and `DifficultyAsset` as a Details-panel screenshot. This shows the BP subclass is configuration plus presentation, not a second rules implementation.

**Do not show:** a BP `BeginPlay` that reads run settings, initialises the Game State or binds `OnLethalDamage`. That is C++.

---

### F3. `WBP_HUD` — a view over authoritative counters

**Placement:** §02 Challenge 4 (Player State), after the PlayerState code. This figure also carries the Game State's shared-lives counter from §01.

**Asset:** Widget Blueprint, parent `UIgKnightedHUDWidget`.

**Widgets**

- `Text_Score`
- `Text_Lives`
- `Image_Dash1`, `Image_Dash2` (two pips; use one per possible charge)

**Variables**

None that store counters. Only cached widget references (`Bind Widget`). This is the point of the figure.

**Event `Construct`**

```
Construct
  -> Get Owning Player
  -> Get Player State
  -> Cast To IgKnightedPlayerState
  -> Bind To Player State (self, PlayerState)

Construct
  -> Get Game State
  -> Cast To IgKnightedGameState
  -> Bind To Game State (self, GameState)
```

`BindToPlayerState` also binds the dash component's `OnDashChargesChanged` if the pawn exists. If the HUD is created before the pawn, bind again on possession or respawn.

**Event `BP_OnMatchScoreChanged(NewScore)`**

```
BP_OnMatchScoreChanged (NewScore)
  -> Set Text (Text_Score, As Text (NewScore))
```

**Event `BP_OnSharedLivesChanged(NewLives, Delta)`**

```
BP_OnSharedLivesChanged (NewLives, Delta)
  -> Set Text (Text_Lives, As Text (NewLives))
  -> Branch (Delta < 0)
       True -> Play Animation (DamagePulse)
       False -> (nothing)
```

**Event `BP_OnDashChargesChanged(CurrentCharges, MaxCharges)`**

```
BP_OnDashChargesChanged (CurrentCharges, MaxCharges)
  -> Set Visibility (Image_Dash1, CurrentCharges >= 1)
  -> Set Visibility (Image_Dash2, CurrentCharges >= 2)
```

For a bar instead of pips, use `Set Percent` with `CurrentCharges / MaxCharges` and guard `MaxCharges > 0`.

**Blueprintue description**

> The HUD never accumulates. C++ pushes a new value and the widget renders it. `BindToPlayerState` unbinds before binding, so a recreated HUD cannot observe the same counter twice. There is no `CurrentScore` variable in the widget; the Player State is the only score owner. The same view pattern covers shared lives from the Game State and dash charges from the dash component.

**Note:** `Get Game State` is the `UGameplayStatics` pure node. `Get Player State` comes from the owning Player Controller.

---

### F4a. `BP_IgKnightedPlayerCharacter` — component hierarchy (screenshot)

**Placement:** §02 Challenge 3, replacing the current placeholder at line 677.

**Capture:** Components panel, not a Blueprint graph.

```
BP_IgKnightedPlayerCharacter (parent AIgKnightedPlayerCharacter)
  CapsuleComponent
  Mesh
    WeaponPivot
  Combat   (UIgKnightedCombatComponent)
  Dash     (UIgKnightedDashComponent)
  Status   (UIgKnightedStatusComponent)
  CharacterMovement
```

**Caption:** The body is composition. `WeaponPivot` is attached to the mesh and follows aim; the capsule and mesh follow movement. The three components own combat, dash and buffs. The Blueprint subclass changes appearance; it does not add gameplay logic.

**Note:** the components are native (`CreateDefaultSubobject`), so they appear in the BP hierarchy but cannot be replaced by BP component subclasses without changing C++.

---

### F4b. `BP_IgKnightedPlayerCharacter` — feedback at the C++ seams

**Placement:** §02 Challenge 6 (Shoot) and §02 Challenge 8 (Die).

**Asset:** same BP, Event Graph.

**Variables (BP-only presentation assets)**

| Name | Type | Default |
|---|---|---|
| `MuzzleFX` | `NiagaraSystem` | muzzle flash |
| `FireSound` | `SoundBase` | shot cue |
| `DeathBurst` | `NiagaraSystem` | death effect |
| `DeathSound` | `SoundBase` | death cue |

**Graph**

```
BeginPlay
  -> Get Combat Component
  -> Bind Event to On Fired
       -> HandleFired (MuzzleLocation)

HandleFired (MuzzleLocation)
  -> Spawn System at Location (MuzzleFX, MuzzleLocation)
  -> Play Sound at Location (FireSound, MuzzleLocation)

BP_OnDeathStarted
  -> Spawn System at Location (DeathBurst, GetActorLocation)
  -> Play Sound at Location (DeathSound, GetActorLocation)
```

**C++ contract**

- `GetCombatComponent()` is `BlueprintPure`.
- `OnFired` is a `BlueprintAssignable` dynamic multicast (`FOnFired(FVector MuzzleLocation)`).
- `BP_OnDeathStarted` is `BlueprintImplementableEvent` on the Character.

**Blueprintue description**

> This graph writes no gameplay state. C++ has already decided the shot or the death. The Character Blueprint only adds presentation at two hooks: `OnFired` for muzzle feedback and `BP_OnDeathStarted` for the death cue. The projectile, the cadence and the life are all C++.

**Do not include:** `BP_PlayFireMontage`. It is declared on the native combat component but no BP subclass can implement it in the sampled code.

---

### F5. `ABP_IgKnightedPlayer` — animation reads, never writes

**Placement:** §02 Challenge 8, replacing the placeholder at line 1157. Update the caption so it no longer claims a BP notify calls `FinishDeath`.

**Asset:** Anim Blueprint, parent `UAnimInstance`.

**Anim variables**

| Name | Type | Default |
|---|---|---|
| `bDead` | bool | false |
| `bFiring` | bool | false |
| `bDashing` | bool | false |
| `AimYaw` | float | 0 |

**Event Graph**

```
Blueprint Update Animation (DeltaTimeX)
  -> Try Get Pawn Owner
  -> Cast To AIgKnightedPlayerCharacter
       -> Is Dead State  -> Set bDead
       -> Is Firing      -> Set bFiring
       -> Is Dashing     -> Set bDashing
       -> Get Aim Yaw    -> Set AimYaw
```

Use an `Is Valid` branch after the cast and leave the defaults when the pawn is null.

**AnimGraph screenshot**

- Locomotion state machine.
- Transition to `Death` when `bDead`.
- `Death` plays the death montage.
- Aim Offset uses `AimYaw`.
- Fire additive uses `bFiring`.
- Dash pose uses `bDashing`.

**C++ contract**

- `IsFiring()`, `IsDashing()`, `IsDeadState()` and `GetAimYaw()` are `BlueprintPure`.
- The Anim BP never calls `ApplyGameplayDamage`, `TryStartDash` or `FinishDeath`.

**Blueprintue description**

> The Anim BP is a pure view. It reads `IsDeadState`, `IsFiring`, `IsDashing` and `GetAimYaw` every frame and never calls gameplay. The death state plays the death montage; the C++ notify destroys the actor at the contact frame. `FinishDeath` is not Blueprint-callable in the sampled code, so the notify is C++, not a Blueprint notify.

**If the project already has a C++ death notify:** show its montage track as a screenshot and keep the caption above.

---

### F6. `BP_PowerUpPickup` — one class, four data rows

**Placement:** §03 Challenge 1, replacing the placeholder at line 1286.

**Asset:** Blueprint child of `AIgKnightedPowerUpPickup`.

**Variables (BP-only materials)**

| Name | Type | Default |
|---|---|---|
| `MI_Shield` | `MaterialInterface` | shield material |
| `MI_PowerShot` | `MaterialInterface` | power-shot material |
| `MI_EnhancedDash` | `MaterialInterface` | dash material |
| `MI_Matchstick` | `MaterialInterface` | matchstick material |

Alternatively use one `Map<EIgBuffKind, MaterialInterface>`.

**Graph**

```
BeginPlay
  -> Switch on EIgBuffKind (BuffKind)
       Shield        -> Set Material (Mesh, 0, MI_Shield)
       PowerShot     -> Set Material (Mesh, 0, MI_PowerShot)
       EnhancedDash  -> Set Material (Mesh, 0, MI_EnhancedDash)
       Matchstick    -> Set Material (Mesh, 0, MI_Matchstick)
```

**Details screenshot**

Four placed instances of the same class:

| Instance | BuffKind | DurationSeconds | ScoreValue | RespawnDelaySeconds |
|---|---|---|---|---|
| Pickup_Shield | Shield | 10 | 25 | 12 |
| Pickup_PowerShot | PowerShot | 10 | 25 | 12 |
| Pickup_EnhancedDash | EnhancedDash | 10 | 25 | 12 |
| Pickup_Matchstick | Matchstick | 10 | 25 | 12 |

The point is that there are no `BP_ShieldPickup`, `BP_PowerShotPickup`, `BP_DashPickup` or `BP_MatchstickPickup` children. One class, four rows.

**C++ contract**

- `BuffKind`, `DurationSeconds`, `ScoreValue`, `RespawnDelaySeconds` are `EditDefaultsOnly`.
- `CanBeCollectedBy`, `Collect` and `SetPickupAvailable` are plain C++.
- `OverlapSphere` and `Mesh` are `BlueprintReadOnly`.

**Blueprintue description**

> The rules live in C++: whether a pickup may be collected, what happens on collection, and when it respawns. This Blueprint is a data row plus cosmetics. `BuffKind` selects the material; the status component decides whether the buff is a duplicate. Four placed instances replace four nearly identical Blueprints.

**Note:** there is no Blueprint event for "collected". Do not invent one in the figure. If a collection effect is wanted, add a `BP_OnCollected` event to C++ later.

---

### F7. `WBP_ShieldIndicator` — bind to the buff owner, not the pickup

**Placement:** §03 Challenge 2 (Shield), optional but strongly supportive.

**Asset:** Widget Blueprint, parent `UserWidget`.

**Widgets**

- `Image_Shield`
- optional `Text_Charges`

**Event `Construct`**

```
Construct
  -> Get Owning Player
  -> Get Pawn
  -> Cast To AIgKnightedPlayerCharacter
  -> Get Status Component
  -> Bind Event to On Shield Changed
       -> HandleShieldChanged (ShieldCharges)
```

**Event `HandleShieldChanged(ShieldCharges)`**

```
HandleShieldChanged (ShieldCharges)
  -> Set Visibility (Image_Shield, ShieldCharges > 0)
  -> Set Text (Text_Charges, As Text (ShieldCharges))
```

**C++ contract**

- `GetStatusComponent()` is `BlueprintPure`.
- `OnShieldChanged` is `BlueprintAssignable` with `FOnShieldChanged(int32 ShieldCharges)`.
- The pickup never talks to the HUD; the status component broadcasts.

**Blueprintue description**

> The pickup never talks to the HUD. The status component broadcasts `OnShieldChanged`; the widget binds. This is the same view pattern as score and lives, applied to a consumable buff. The shield is consumed on the hit that would have been lethal; the widget only displays the remaining charges.

**Note:** bind again after possession/respawn if the pawn did not exist on `Construct`. This is an optional figure; the HUD in F3 does not bind shield.

---

### F8. `ABP_Crawler` + `AN_CrawlerMelee` — one swing, one hit

**Placement:** §04 Challenge 1, replacing the placeholder at line 1664.

This figure has three captures: AnimGraph, AnimNotify graph, and montage notify track.

**ABP Event Graph**

```
Blueprint Update Animation (DeltaTimeX)
  -> Try Get Pawn Owner
  -> Cast To AIgKnightedEnemyCharacter
       -> Get Velocity
       -> Vector Length XY
       -> Set Speed

       -> Is Dead (interface call)
       -> Set bDead
```

The interface call is available because `IIgKnightedDamageable::IsDead` is `BlueprintNativeEvent` and `BlueprintCallable`.

**Anim Variables**

| Name | Type | Default |
|---|---|---|
| `Speed` | float | 0 |
| `bDead` | bool | false |

**AnimGraph screenshot**

- Idle <-> Locomotion when `Speed > 10`.
- Locomotion -> Attack when the Behaviour Tree requests the attack.
- Attack plays `AM_CrawlerAttack`.
- Any state -> Death when `bDead`.

**`AN_CrawlerMelee` — Blueprint AnimNotify**

```
Received Notify (MeshComp, Animation, EventReference)
  -> Get Owner (MeshComp)
  -> Cast To AIgKnightedEnemyCharacter
  -> Perform Melee Attack
```

**Montage screenshot:** `AM_CrawlerAttack` with `AN_CrawlerMelee` on the contact frame. No per-frame overlap; no capsule damage volume.

**C++ contract**

- `PerformMeleeAttack()` is `BlueprintCallable`.
- It performs a single sphere overlap at the current actor location plus forward offset, using the definition's `MeleeRadius`.
- The notify is the only caller.

**Blueprintue description**

> `PerformMeleeAttack` is BlueprintCallable and does one sphere overlap. The animation notify is the only caller, so one swing is one hit. The Anim BP never damages anything; it signals the contact frame. The C++ method has no tick and no per-frame overlap.

**Note:** if the project uses a C++ melee notify instead, show that class and keep the caption. The sampled code only contains the fire notify (`UAnimNotify_IgFireProjectile`).

---

### F9. `BP_EnemyPresentation` — death is one event

**Placement:** §04 Challenge 1, with a note that it covers all three archetypes.

**Asset:** Blueprint child of `AIgKnightedEnemyCharacter`. If the project already has `BP_Crawler`, `BP_Scorpion` and `BP_Mollusk`, implement the event in a shared `BP_EnemyBase` parent and inherit. Do not duplicate the graph three times.

**Variables (BP-only presentation assets)**

| Name | Type | Default |
|---|---|---|
| `DeathSound` | `SoundBase` | death cue |
| `DeathBurst` | `NiagaraSystem` | death effect |
| `MI_Dissolve` | `MaterialInterface` | optional dissolve material |

**Graph**

```
BP_OnDefeated
  -> Play Sound at Location (DeathSound, GetActorLocation)
  -> Spawn System at Location (DeathBurst, GetActorLocation, GetActorRotation)
  -> Set Material (Mesh, 0, MI_Dissolve) [optional]
```

**Do not set collision or lifespan in BP.** C++ already does both.

**C++ contract**

`HandleDefeated` does, in order:

1. sets `bDefeated`;
2. stops combat;
3. disables targeting tick;
4. disables movement;
5. disables capsule collision;
6. notifies the Zone Manager;
7. awards score and multiplier to the killer;
8. calls `BP_OnDefeated()`;
9. sets the corpse lifespan.

**Blueprintue description**

> C++ has already stopped combat, disabled collision, notified the zone, awarded score and set the corpse lifespan. `BP_OnDefeated` is the one presentation seam shared by all three archetypes. The Blueprint adds a sound and an effect; it cannot double-award score or keep a dead enemy in the encounter count.

**Note:** because `BP_OnDefeated` runs before `SetLifeSpan`, the effect spawns at the body location while the corpse still exists.

---

### F10. `AN_ScorpionShot` / `AN_MolluskBurst` — the same pattern, different verbs

**Placement:** §04 Challenge 2 (Scorpion) and Challenge 3 (Mollusk), optional.

**Scorpion**

```
Received Notify
  -> Get Owner (MeshComp)
  -> Cast To AIgKnightedEnemyCharacter
  -> Get Targeting Component
  -> Get Current Target
  -> Fire At Target (Target)
```

**Mollusk**

```
Received Notify
  -> Get Owner (MeshComp)
  -> Cast To AIgKnightedEnemyCharacter
  -> Fire Radial Burst
```

**C++ contract**

- `FireAtTarget(AActor* Target)` is `BlueprintCallable`.
- `FireRadialBurst()` is `BlueprintCallable`.
- `GetTargetingComponent()` and `GetCurrentTarget()` are `BlueprintPure`.
- The archetype's numbers come from its `UIgKnightedEnemyDefinition` asset.

**Blueprintue description**

> The archetype differs in data, not in the damage pipeline. The notify decides when the attack leaves the body; `FireAtTarget` and `FireRadialBurst` are BlueprintCallable. The Scorpion needs the targeting component's current target; the Mollusk does not need a target at all because its ring is radial.

**Note:** `GetDefinition()` returns the asset, but its fields are not `BlueprintReadOnly`. Do not read `ProjectileSpeed` or `RadialShotCount` in these graphs.

**Alternative:** one shared `AN_EnemyAttack` notify with an `EAttackMode` variable and a `Switch`. This is fewer assets but adds a branch. Two small notifies are easier to explain.

---

### F11. `BP_Barrier` — collision is C++, the Blueprint is the light

**Placement:** §05 Challenge 1 (The Three Parts) or Challenge 2, after the barrier code.

**Asset:** Blueprint child of `AIgKnightedBarrier`.

**Variables (BP-only presentation assets)**

| Name | Type | Default |
|---|---|---|
| `OpenSound` | `SoundBase` | barrier opening cue |
| `CloseSound` | `SoundBase` | barrier closing cue |

**Graph**

```
BP_OnOpenChanged (bOpen)
  -> Branch (bOpen)
       True:
         Play Sound at Location (OpenSound, GetActorLocation)
         Set Visibility (Mesh, false)
       False:
         Set Visibility (Mesh, true)
         Play Sound at Location (CloseSound, GetActorLocation)
```

For a dissolve instead of a pop, add a Timeline (0 -> 1) and drive a dynamic material scalar parameter on `Mesh`.

**C++ contract**

`SetOpen` does:

1. stores `bOpen`;
2. sets `BlockingVolume` collision to `NoCollision` when open, `QueryAndPhysics` when closed;
3. toggles overlap generation;
4. calls `BP_OnOpenChanged(bOpen)`.

**Blueprintue description**

> `SetOpen` already made collision authoritative: `BlockingVolume` is `NoCollision` when open and `QueryAndPhysics` when closed. The Blueprint event only animates the mesh, so the visual cannot disagree with the block. The collision change is not exposed to Blueprint, which is the point.

**Note:** `Mesh` and `BlockingVolume` are `BlueprintReadOnly`. Do not set collision in BP.

---

### F12. `BP_ZoneAnnouncer` — listen to the encounter

**Placement:** §05 Challenge 2 (Enemy Spawning), optional.

**Asset:** Actor Blueprint placed in the level.

**Variables**

| Name | Type | Default |
|---|---|---|
| `ZoneManager` | `AIgKnightedZoneManager` | instance reference |
| `ZoneBannerClass` | `WBP_ZoneBanner` class | banner widget |
| `ZoneStartSound` | `SoundBase` | zone start cue |
| `ZoneClearSound` | `SoundBase` | zone clear cue |

**Event `BeginPlay`**

```
BeginPlay
  -> Get Actor of Class (AIgKnightedZoneManager)
  -> Bind Event to On Zone Started
       -> HandleZoneStarted (ZoneIndex)
  -> Bind Event to On Zone Cleared
       -> HandleZoneCleared (ZoneIndex)
```

If the `ZoneManager` reference is set in the Details panel, use it directly instead of `Get Actor of Class`.

**Event `HandleZoneStarted(ZoneIndex)`**

```
HandleZoneStarted (ZoneIndex)
  -> Create Widget (ZoneBannerClass)
  -> Set Text (Text_Zone, As Text (ZoneIndex))
  -> Add to Viewport
  -> Set Timer by Event (3.0, false)
       -> RemoveBanner
```

**Event `HandleZoneCleared(ZoneIndex)`**

```
HandleZoneCleared (ZoneIndex)
  -> Play Sound 2D (ZoneClearSound)
  -> Set Text (Text_Zone, "Cleared")
```

**C++ contract**

- `OnZoneStarted` and `OnZoneCleared` are `BlueprintAssignable` on `AIgKnightedZoneManager`.
- `Get` is a static C++ helper; BP uses `Get Actor of Class`.
- `GetAliveEnemyCount()` and `GetPendingSpawnCount()` are `BlueprintPure` if a debug counter is wanted.

**Blueprintue description**

> The Zone Manager broadcasts; the announcer listens. No tick, no polling of `AliveEnemies`. The same BlueprintAssignable delegates feed UI and audio. The encounter's completion rule stays in C++; the Blueprint only reacts to the result.

**Ordering caveat:** if the spawner has `bAutoStart = true`, its `BeginPlay` can broadcast before the announcer binds. Set `bAutoStart = false` and start the encounter after binding, or bind from C++. Mention this in the caption; it is a real lesson about Blueprint delegate timing.

---

### F13. `AN_HazardCycle` — the beam's animation is the clock

**Placement:** §04 Challenge 3 (Mollusk) or §05 Challenge 1, optional.

**Asset:** Blueprint AnimNotify.

**Graph**

```
Received Notify
  -> Get Owner (MeshComp)
  -> Cast To AIgKnightedHazardVolume
  -> Begin New Cycle
```

A second notify `AN_HazardOff` calls `End Cycle`.

**C++ contract**

- `BeginNewCycle()` and `EndCycle()` are `BlueprintCallable`.
- `SourceId` is `EditInstanceOnly`.
- The volume latches the first lethal hit of each cycle.

**Blueprintue description**

> The hazard's damage is C++. The notify only tells the volume when a new beam cycle starts. The C++ latch makes the first lethal hit of a cycle count once; a dash or a shield does not latch, so the same beam can still kill later in the cycle.

**Note:** this fits the Water Mollusk's beam. If the beam's on/off is driven by a Level Sequence or a timer instead, do not use this figure.

---

## 3. Non-blueprintue captures

These are screenshots with captions, not graph text. They support points where a graph would be misleading or where the asset itself is the point.

### 3.1 Player components panel

- Asset: `BP_IgKnightedPlayerCharacter`.
- Shows Capsule, Mesh, `WeaponPivot`, Combat, Dash, Status.
- Caption: composition; body follows movement, pivot follows aim.

### 3.2 Enhanced Input assets

- `IMC_Gameplay`
- `IA_Move`, `IA_Aim`, `IA_MouseAim`, `IA_Fire`, `IA_Dash`
- Caption: the C++ controller owns the handlers; the BP-visible assets are the mapping context and actions. No Blueprint graph is involved in `OnMoveTriggered` etc.; those handlers are C++.

### 3.3 Montage notify tracks

- `AM_CrawlerAttack` — `AN_CrawlerMelee` at the contact frame.
- `AM_PlayerDeath` — the C++ notify that calls `FinishDeath` (if it exists in the project).
- Fire montage — the C++ `UAnimNotify_IgFireProjectile`.
- Caption: the montage is the timing authority; C++ or the BP notify is the effect.

### 3.4 Behaviour Tree and Blackboard

- `BT_Enemy`
- `BB_Enemy` (`TargetActor`, `LastKnownLocation`, `HasTarget`)
- `BTTask_IgFireProjectile` Details (`TargetActorKey`, `bFailWithoutTarget`)
- Caption: the controller writes the target to the blackboard; the task asks the combat component to fire. The task does not build a projectile, choose a socket or play an animation.

### 3.5 Data assets

- `DA_Weapon_Base` vs `DA_Weapon_PowerShot`:
  - `BonusProjectiles = 2`
  - `SpreadDegrees` symmetric around the aim axis
  - same `ProjectileClass`, different numbers
- `DA_Enemy_Crawler`, `DA_Enemy_Scorpion`, `DA_Enemy_Mollusk`:
  - Crawler `PreferredRange = 0` (melee)
  - ranged archetypes hold a range
  - `RadialShotCount` for the Mollusk ring
- `DA_Difficulty`:
  - Easy / Medium / Hard rows of `FIgDifficultyTuning`
  - Caption: adding a difficulty is a new asset, not a new branch. Be honest that only `EnemyCountScale` is consumed by the sampled spawner today.

### 3.6 Spawner Details

- `BP_EnemySpawner`:
  - `ZoneIndex`
  - `EnemyClass`
  - `BaseEnemyCount`
  - `SpawnInterval`
  - `bAutoStart`
  - `SpawnPoints`
- Caption: the spawner owns the pending count; the Zone Manager owns the completion rule.

### 3.7 GameMode Class Defaults

- `MaxSharedLives`
- `RespawnDelaySeconds`
- `DifficultyAsset`
- Caption: the BP subclass configures the C++ rules; it does not reimplement them.

---

## 4. Page placement summary

| Page point | Action |
|---|---|
| 01 Ch1 after GI code | Insert F1 |
| `:261` GameMode placeholder | Replace with F2 + Class Defaults screenshot |
| 02 Ch1 | Insert Enhanced Input screenshots |
| 02 Ch3 `:677` | Keep as F4a Components screenshot |
| 02 Ch4 after PlayerState | Insert F3 |
| 02 Ch6 | Insert F4b (OnFired feedback) |
| `:1157` ABP player | Replace with F5; fix the `FinishDeath` caption |
| 03 Ch1 `:1286` | Replace with F6 + four-instance Details screenshot |
| 03 Ch2 | Insert F7 (optional) |
| 04 Ch1 `:1664` | Replace with F8 + montage screenshot |
| 04 Ch1–3 | Insert F9 |
| 04 Ch2–3 | Insert F10 (optional) |
| 04 Ch4 | Insert BT/BB/BTTask screenshots |
| 05 Ch1 | Insert F11 |
| 05 Ch2 | Insert F12 (optional) + Spawner Details |
| 05 Ch3 | Insert `DA_Difficulty` screenshot |

---

## 5. Blueprintue description template

For each graph, paste this structure into the blueprintue description field:

```
Asset: <name>
Parent: <C++ class or UserWidget/AnimInstance>
Graph: <Event Graph / AnimGraph / AnimNotify>
Variables: <name : type = default> (or "none that store gameplay state")
Flow:
  <Event> -> <Node> -> <Node>
  ...
C++ contract:
  <which C++ function or delegate this binds/calls>
Takeaway:
  <one sentence the reader should remember>
```

Guidance:

- Name the C++ function or delegate each node touches. That is what makes the image evidence for the architecture claim.
- State explicitly when a graph stores no gameplay state. That is the recurring theme.
- Keep the graph small. One idea per figure. If a figure needs two events, they should share one idea (e.g., `BP_OnMatchFinished` + `BP_OnPlayerRespawned` share "presentation at the rules' seams").
- Do not include nodes that are only there to make the graph look busy.
- If blueprintue cannot render UMG or AnimGraph text, use a screenshot and paste the description as the caption. The description is still the teaching material.

---

## 6. Execution order

1. Fix the four mismatches in §0 (caption changes; optionally expose `FinishDeath`).
2. Build/capture the core figures: F1, F2, F3, F4a/F4b, F5, F6, F8, F9, F11.
3. Add the optional figures if time allows: F7, F10, F12, F13.
4. Capture the non-BP assets in §3.
5. Insert into the page at the locations in §4 and replace the five existing placeholders.
6. Rebuild the site and verify:
   - placeholder count returns to the intended number;
   - images load;
   - no overflow at 390 px;
   - code blocks remain faithful to `_code_samples/igknighted/`.

---

## 7. Open decisions

- **`FinishDeath`:** keep the notify in C++ and show the ABP state only, or expose `FinishDeath` as `BlueprintCallable` and show a BP notify? Recommended: keep C++ and fix the caption.
- **HUD ownership:** where is `WBP_HUD` created? The sampled C++ does not create it. The figure assumes a BP PlayerController or GameMode creates it; confirm the project's actual owner before claiming it.
- **Enemy Blueprint organisation:** one shared `BP_EnemyPresentation` parent, or the existing `BP_Crawler`/`BP_Scorpion`/`BP_Mollusk` each implementing `BP_OnDefeated`? The shared parent is better if the project does not already have one.
- **Blueprintue support:** confirm whether the site accepts UMG and AnimGraph text exports. If not, use screenshots for those and keep the descriptions as captions.
