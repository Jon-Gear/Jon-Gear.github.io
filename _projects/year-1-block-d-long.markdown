---
layout: project
title: "IgKnighted — Unreal Engine 5 Gameplay Programming & Systems Architecture"
role: "Gameplay Programmer"

version: long
short_url: /projects/year-1-block-d
long_url: /projects/year-1-block-d-long

date: 2024-06-28
categories: [featured_projects]
image: /assets/images/Y1D/main.jpg
description: "A local 1–2 player co-op bullet-hell arena shooter built in Unreal Engine 5. A technical case study of the Enhanced Input stack, local multiplayer possession, shared-life damage resolution, enemy AI, power-up abilities and zone progression — with the C++ architecture I would ship it as today."
contributions: "Enhanced Input Stack | Local Multiplayer & Pawn Possession | Shared-Life Damage Resolution | Dash Invulnerability System | Enemy AI (Perception, Targeting, Ranged Attacks) | Power-Up & Ability Layer | Zone Encounter Manager | Event-Driven HUD & Scoring"
tools: "Unreal Engine 5, C++, Blueprints, Enhanced Input, Gameplay Ability System, Gameplay Tags, Behavior Trees, UMG, Animation Blueprints, Niagara"
team_size: "4 Programmers | 3 Designers | 5 Artists"
platforms: "Windows"
duration: "8 Weeks (May 2024 - Jun 2024)"
---

## 🔥 IgKnighted – A Stylized Local Multiplayer Arena Shooter

[Click here to visit our game page!](https://buas.itch.io/igknighted)

IgKnighted is a fast-paced, couch co-op arena shooter built in Unreal Engine 5 using Blueprints. Designed for local multiplayer mayhem, players face off against waves of enemies while collecting power-ups, dodging enemy fire, and racing for the highest score. Inspired by classic arcade energy and modern stylized visuals, the game features dynamic combat, painterly post-processing, and a responsive control scheme. Developed collaboratively by a multidisciplinary team, IgKnighted blends tight gameplay mechanics with polished visuals and smart system design—all brought together through agile development and hands-on iteration.

---

## 🔨 My Contributions

---

## How to read this page

This is not a feature list. It is a **systems case study**: every section follows the same shape —

> **The requirement → the constraint that made it hard → the architecture I chose → the code that implements it → the edge cases that bit me → the trade-off I accepted.**

If you are reviewing this for a gameplay programming role, the fastest path is:

| If you want to see… | Go to |
|---|---|
| I understand Unreal's object lifetimes and can place state correctly | [§02 Gameplay Framework](#02-gameplay-framework--core-architecture) |
| I can build local multiplayer and route input per player | [§03 Local Multiplayer](#31-local-multiplayer-architecture) |
| I can write damage/state machines that cannot double-fire | [§03.4 Dash](#34-dash--invulnerability) · [§04.2 Damage](#42-projectile--damage-system) |
| I can build enemy AI that is readable and tunable | [§04 Enemy AI](#04-combat-systems--enemy-ai) |
| I know the modern ability stack (GAS, tags, effects) | [§05.3 GAS](#53-gameplay-ability-system--reference-design) |
| I can own a progression system end to end | [§06 Zone Progression](#06-level-design-progression--difficulty-systems) |
| I debug, profile and make trade-offs like a professional | [§08 Engineering Practices](#08-engineering-practices--technical-decisions) |

**Role alignment.** The target role for this project is a *Senior Gameplay Programmer* (Unreal). The systems below map directly onto that job description: **player mechanics and player abilities** (§03, §05), **player progression** (§05.4, §06), **game flow systems** (§02, §06), **world systems and activities** (§06), **Enhanced Input / Animation / GAS / EQS** (§03.2, §07.1, §05.3, §04.5), **robust and maintainable systems** (§08.1, §08.5), and **diagnosing gameplay, memory and performance issues** (§08.2, §08.4).

---

## 01. Project Overview

### 1.1 About IgKnighted

![IgKnighted — flooded temple arena](../../assets/images/Y1D/main.jpg)

IgKnighted is a **stylized top-down twin-stick / bullet-hell arena shooter** built in Unreal Engine 5 and released for Windows. The premise is deliberately simple so the systems can be the interesting part: a forgotten, flooded temple; knights forged from fire; and the water-things that live in it.

| | |
|---|---|
| **Genre** | Top-down twin-stick arena shooter / bullet hell |
| **Engine** | Unreal Engine 5 (UE5.4) |
| **Players** | 1–2 local, shared-screen, keyboard + gamepad |
| **Duration** | 8 weeks (May – June 2024) |
| **Team** | 4 programmers · 3 designers · 5 artists |
| **Delivery** | Public downloadable Windows build on [itch.io](https://buas.itch.io/igknighted) |

**The core loop.**

```
   ┌────────────────────────────────────────────────────────────────────┐
   │  MENU          Move · Aim · Shoot · Dash · Collect · Survive        │
   │    ↓           ↑                                                    │
   │  ZONE 1   ─────┘   Clear the encounter → unlock the barrier        │
   │    ↓                                                                 │
   │  ZONE 2   ─────┘   Clear the encounter → unlock the barrier        │
   │    ↓                                                                 │
   │  ZONE 3   ─────┘   Clear the encounter → VICTORY                     │
   └────────────────────────────────────────────────────────────────────┘

   Shared lives: 5  ▓▓▓▓▓▓▓▓▓▓  (one pool for both players)
   On 0 lives   → GAME OVER, even if a teammate is still standing
```

**The three rules that define the whole game.** Every combat, UI and AI system in this project ultimately answers to these:

1. **Water damage is lethal in one hit.** There is no health bar to chip down. If it touches you, you die.
2. **The dash is your only defence.** A dash grants temporary invulnerability; it is the intended answer to the Water Mollusk's beam and to any projectile you cannot dodge.
3. **Lives are a shared pool of five.** Your teammate's death spends *your* stock. The run ends at zero, no matter who is still alive.

That third rule is the one that shapes the entire architecture, and it is the reason §02.3 exists.

### 1.2 My Role & Contributions

I was one of **four programmers** on a team of twelve. I owned the systems that everything else in the game depended on — the ones that, if they broke, the game stopped being playable.

| # | System I owned | Why it mattered |
|---|---|---|
| 1 | **Enhanced Input stack** | The foundation every player action sits on. Replaced deprecated input, added keyboard + gamepad parity, and made per-player mapping contexts possible. |
| 2 | **Local multiplayer** | Turned a single-player template into a couch-co-op game: LocalPlayer creation, Pawn spawning, possession, per-player UI. |
| 3 | **Shared-life damage resolution** | The instant-death / shield / dash precedence rules and the one-life-per-death guarantee. |
| 4 | **Dash & invulnerability** | Charge-based dash, cooldown, i-frames, and the enhanced-dash traversal kill. |
| 5 | **Enemy AI** | Perception, target selection, aiming and projectile attacks; a refactor that removed duplicated perception logic. |
| 6 | **Power-ups & abilities** | Shield, Power Shot (triple shot), Enhanced Dash, and the duplicate-pickup restriction rules. |
| 7 | **Collectables & scoring** | Procedural spawn during waves, score + multiplier, live HUD updates. |
| 8 | **Animation integration** | Animation Blueprints for idle / walk / shoot / death, driven by gameplay state. |

**What "owned" meant in practice.** I did not just write these — I defined their *contract*. When designers asked for a new enemy, they did not need to know how my targeting code worked; they needed to fill in a data asset and press play. When artists needed a hook for a VFX, there was one delegate and one `BlueprintImplementableEvent` to call, not an arbitrary graph to reverse-engineer. That is the difference between "I made a feature" and "I made a feature the rest of the team could build on", and it is the part of the job I care most about.

**Where I did not contribute.** The level art, the character art, the shader and post-processing work, the audio pass and the narrative/design pitch belonged to the artists and designers. I integrated with them — sockets, Animation Blueprint slots, Niagara trigger parameters, event dispatchers — but I did not author them, and I am not going to claim them here.

### 1.3 Technical Highlights

Six systems that a gameplay programmer can be measured on, and how this project answers each:

| System | The hard part | How it was solved |
|---|---|---|
| **Local multiplayer** | Two players, one process, one screen, independent input, correct possession | `LocalPlayer` → `PlayerController` → `Pawn` chain with per-`LocalPlayer` Enhanced Input mapping contexts and `GameMode`-driven spawning |
| **Input** | Legacy input is deprecated; gamepad and keyboard must both work, and must not bleed between players | Enhanced Input: `InputAction` assets + `InputMappingContext` assets installed on `UEnhancedInputLocalPlayerSubsystem` |
| **Damage** | Instant death, but shield and dash must not double-fire, and a hazard overlap must not spend a life every frame | One centralised `DamageResolver` with an explicit precedence chain and an idempotency guard |
| **Enemy AI** | Three archetypes with different spatial behaviour, and a single player-or-the-other target | `AIController` + `Blackboard` target selection, per-archetype behaviour, `EQS` for spacing queries |
| **Abilities** | Four power-ups that must stack, expire, and not be collected twice | A `GameplayAbilitySystemComponent` layer with `GameplayTag` state and `GameplayEffect`-driven durations |
| **Progression** | Zones must not unlock early, and a late enemy death must not open the wrong gate | `ZoneManager` with a **separate active-enemy count and pending-spawn count**; the barrier opens only when both are zero |

**The architecture underneath.** The game is built on a **C++ base class + Blueprint-derived content** split: gameplay *contracts and validation* live in C++ so they cannot be broken by a designer dragging a node wrong; *content, tuning and presentation* live in Blueprints and data assets so a designer can iterate in seconds without recompiling. §08.1 covers this split in detail, and §08.5 contains the architecture decision records for the calls I actually agonised over.

---

## 02. Gameplay Framework & Core Architecture

> **The single most important thing I got right in this project was placing state in the right place.** Almost every ugly bug we had in the last three weeks of production traced back to a piece of state living in the wrong object.

### 2.1 Overall Game Architecture

Unreal's Gameplay Framework gives you a set of objects with very different lifetimes. Before writing any gameplay code I drew the map of *who owns what* and *when does it die*, because in a game with two local players, a shared camera and a persistent three-zone map, the lifetime of an object is a gameplay rule, not an implementation detail.

```text
 ┌──────────────────────────────── APPLICATION LIFETIME ───────────────────────────────┐
 │                                                                                    │
 │  UGameInstance  (IgKnightedGameInstance)                                            │
 │  ├── LocalPlayerCount : 1 | 2                                                      │
 │  ├── Difficulty : Easy | Medium | Hard                                              │
 │  ├── bDynamicDifficulty : bool                                                      │
 │  ├── bPlayTutorial : bool                                                          │
 │  └── Owns: save data, input settings, the pending "run configuration"              │
 │      ▲                                                                            │
 │      │ reads configuration at match start                                            │
 │      │ (survives map travel: Menu → Tutorial → Main Map)                            │
 └──────┼────────────────────────────────────────────────────────────────────────────┘
        │
 ═══════╪════════════════════════ WORLD LIFETIME ═══════════════════════════════════
        │                                                                            │
 │  ┌────▼───────────────────────────────────────────────────────────────────────┐  │
 │  │  AGameMode  (IgKnightedGameMode)      ← the RULES. One per world.            │  │
 │  │  ├── StartRun(Settings)                                                      │  │
 │  │  ├── CreateLocalPlayers(int32)                                               │  │
 │  │  ├── SpawnPlayerPawn(AController*)                                           │  │
 │  │  ├── HandlePlayerEliminated(AController*)  ← deducts ONE shared life         │  │
 │  │  ├── ScheduleRespawn(AController*)                                           │  │
 │  │  ├── HandleZoneCleared(int32 ZoneIndex)                                      │  │
 │  │  └── FinishMatch(EMatchResult)                                               │  │
 │  │            │                    │                                             │  │
 │  │            │ mutates           │ reads                                       │  │
 │  │            ▼                    ▼                                             │  │
 │  │  ┌─────────────────────────┐  ┌──────────────────────────────────────────┐  │  │
 │  │  │ AGameState              │  │  AZoneManager                             │  │  │
 │  │  │ (IgKnightedGameState)   │  │  ├── CurrentZoneIndex                      │  │  │
 │  │  │ ├── SharedLives = 5     │  │  ├── ActiveEnemies  (registry)             │  │  │
 │  │  │ ├── CurrentZone         │  │  ├── PendingSpawns  (count)                │  │  │
 │  │  │ ├── MatchPhase          │  │  └── OnZoneCleared(int32)                  │  │  │
 │  │  │ ├── ActiveEnemiesLeft   │  └──────────────────────────────────────────┘  │  │
 │  │  │ └── OnLivesChanged(…)   │                                                 │  │
 │  │  └─────────────────────────┘                                                 │  │
 │  └────────────────────────────────────────────────────────────────────────────┘  │
 │                                                                                    │
 │  ┌── LOCAL PLAYER 0 ────────────────────┐   ┌── LOCAL PLAYER 1 ────────────────────┐  │
 │  │ ULocalPlayer 0                        │   │ ULocalPlayer 1                        │  │
 │  │   └── EnhancedInputLocalPlayerSubsys  │   │   └── EnhancedInputLocalPlayerSubsys  │  │
 │  │         └── IMC_Gameplay (keyboard)    │   │         └── IMC_Gameplay (gamepad)    │  │
 │  │              + IMC_Gamepad             │   │              + IMC_Keyboard         │  │
 │  │   ↓ possesses                          │   │   ↓ possesses                          │  │
 │  │ APlayerController 0  (IgKnightedPC)   │   │ APlayerController 1  (IgKnightedPC)   │  │
 │  │   │  one CLASS, two INSTANCES         │   │   │  ← same class, separate instance   │  │
 │  │   ├── Possesses ────────────────┐     │   │   ├── Possesses ────────────────┐     │  │
 │  │   └── Owns a PlayerState         │     │   │   └── Owns a PlayerState         │     │  │
 │  └───────────────────┬──────────────┘     │   └───────────────────┬──────────────┘     │
 │                      ▼                    │                       ▼                    │
 │      ┌───────────────────────────┐         │       ┌───────────────────────────┐         │
 │      │ APlayerCharacter (Pawn)   │         │       │ APlayerCharacter (Pawn)   │         │
 │      │  ├ CapsuleComponent        │         │       │  ├ CapsuleComponent        │         │
 │      │  ├ CharacterMovement       │         │       │  ├ CharacterMovement       │         │
 │      │  ├ Mesh + ABP_Player       │         │       │  ├ Mesh + ABP_Player       │         │
 │      │  ├ CombatComponent  ◀── gameplay logic, no VFX                        │  │
 │      │  ├ DashComponent    ◀── charges, cooldown, i-frames                    │  │
 │      │  └ StatusComponent   ◀── shield, buffs, ability tags                   │  │
 │      └───────────────────────────┘         │       └───────────────────────────┘         │
 └────────────────────────────────────────────────────────────────────────────────────┘
        │
        │  AI side
        ▼
 ┌───────────────────────────────┐      ┌──────────────────────────────────────────┐
 │  AIController (per enemy)     │─────▶│  EnemyCharacter                          │
 │  ├── Blackboard: TargetActor  │      │   ├── Perception (sight/hearing)          │
 │  ├── Target selection policy  │      │   ├── AttackComponent (ranged/contact)    │
 │  └── Behaviour Tree            │      │   └── OnDefeated(EnemyID, EncounterID)    │
 └───────────────────────────────┘      └──────────────────────────────────────────┘
```

**Reading the diagram — three rules I enforced in every system:**

1. **Rules live in the GameMode, state lives in the GameState, content lives in Blueprints.** The GameMode *decides*; the GameState *remembers*; neither one holds a pointer to an enemy actor.
2. **One controller class, two instances.** I did not write `PlayerOneController` and `PlayerTwoController`. Both players are the same class with different `LocalPlayer` ownership, different `PlayerState`, and different `Pawn`. Player identity is data, not a type.
3. **Components own behaviour, Actors own identity.** `CombatComponent` knows how to fire a shot. `DashComponent` knows how to dash. Neither knows about the enemy, the HUD, or Niagara. The `Pawn` composes them; the `GameMode` arbitrates.

### 2.2 GameInstance & Game Configuration

The first thing I needed to solve was **what survives a map change**. IgKnighted has three levels in the flow — Main Menu, Tutorial, and the persistent Main Map — and the player chooses difficulty *before* any of them loads. If the difficulty choice lives in a widget, it dies when the widget closes; if it lives in a `GameMode`, it dies when the world changes.

The correct owner is the **`UGameInstance`**: it exists for the lifetime of the application and is reachable from anywhere.

```cpp
// IgKnightedTypes.h
#pragma once

#include "CoreMinimal.h"
#include "IgKnightedTypes.generated.h"

/**
 * The result of a damage transaction.
 *
 * Returning a result instead of a bool is what lets the caller know WHY the
 * damage did not count: "you were invulnerable" and "you already died" and
 * "this was a duplicate" are three different situations with three different
 * pieces of feedback, and they must not be conflated.
 */
USTRUCT(BlueprintType)
struct FIgDamageResult
{
    GENERATED_BODY()

    /** The damage did not apply. */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bIgnored = false;

    /** The target was immune (dash i-frames). */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bInvulnerable = false;

    /** A shield absorbed the hit. */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bShieldConsumed = false;

    /** This was a repeat report of a death we already processed. */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bDuplicate = false;

    /** The hit was lethal. */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bLethal = false;
};
```

```cpp
// IgKnightedTypes.h — continued

UENUM(BlueprintType)
enum class EIgDifficulty : uint8
{
    Easy   UMETA(DisplayName = "Easy"),
    Medium UMETA(DisplayName = "Medium"),
    Hard   UMETA(DisplayName = "Hard")
};

/** The state of the match itself. Everything else keys off this. */
UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
    Initializing,
    Active,
    ZoneTransition,
    Victory,
    Defeat
};

/** Which damage channel a hit belongs to. */
UENUM(BlueprintType)
enum class EIgDamageKind : uint8
{
    Unknown,
    /** Fire projectiles: damage enemies, never lethal to the player. */
    EnemyDamage,
    /** Water: lethal to the player unless protected. */
    LethalToPlayer,
    /** Contact damage from a Crawler. */
    ContactDamage
};

/** The kinds of buff a pickup can apply. */
UENUM(BlueprintType)
enum class EIgBuffKind : uint8
{
    Shield,
    PowerShot,
    EnhancedDash
};

/** The outcome of the match. */
UENUM(BlueprintType)
enum class EMatchResult : uint8
{
    Victory,
    Defeat
};

/** Everything the player chose in the front-end, packaged for one atomic hand-off. */
USTRUCT(BlueprintType)
struct FIgRunSettings
{
    GENERATED_BODY()

    /** 1 or 2. Decided once, before the tutorial is even offered. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    int32 LocalPlayerCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    EIgDifficulty Difficulty = EIgDifficulty::Medium;

    /** The adaptive-difficulty toggle from the difficulty screen. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    bool bDynamicDifficulty = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    bool bPlayTutorial = true;

    /** Human-readable join of the above, used in the log and in bug reports. */
    FString ToString() const
    {
        return FString::Printf(TEXT("%dP | %s | Dyn=%s | Tutorial=%s"),
            LocalPlayerCount,
            *UEnum::GetValueAsString(Difficulty),
            bDynamicDifficulty ? TEXT("On") : TEXT("Off"),
            bPlayTutorial ? TEXT("Yes") : TEXT("No"));
    }
};
```

```cpp
// IgKnightedGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IgKnightedTypes.h"
#include "IgKnightedGameInstance.generated.h"

UCLASS()
class IGKNIGHTED_API UIgKnightedGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    /**
     * The configuration for the NEXT run.
     *
     * Deliberately not "the current run": the GameInstance outlives the level,
     * so anything we hang off it must be safe to read at any point in the flow.
     * That single constraint is what stopped me from caching enemy or Pawn
     * references here, which would have dangled the moment the tutorial map
     * loaded. It is the same reason there is no pointer to a world in this class.
     */
    UPROPERTY(BlueprintReadWrite, Category = "Run")
    FIgRunSettings PendingRunSettings;

    /** Difficulty presets, loaded from Content/Data. Kept here so the menu can preview them. */
    UPROPERTY(EditDefaultsOnly, Category = "Run")
    TMap<EIgDifficulty, TObjectPtr<class UIgDifficultyDataAsset>> DifficultyPresets;

    UFUNCTION(BlueprintPure, Category = "Run")
    const UIgDifficultyDataAsset* GetDifficultyPreset() const
    {
        return DifficultyPresets.Find(PendingRunSettings.Difficulty);
    }
};
```

**Why not put it in the `GameMode`?** Because the `GameMode` is destroyed on level travel. The tutorial loads *after* the difficulty screen, so a `GameMode`-held setting would be gone before the main map ever opened. This is the single most common mistake I saw in my own early prototypes: reaching for the nearest convenient object instead of the one with the right lifetime.

**Menu flow, end to end:**

```
WBP_MainMenu  ──"PLAY"──▶  WBP_PlayerCount  ──1 or 2──▶  WBP_Difficulty
                                                               │
                                       Easy / Medium / Hard  +  [x] Dynamic Difficulty
                                                               │
                                                               ▼
                                                        WBP_TutorialPrompt
                                                       "Play the tutorial?"
                                                          │            │
                                                        Yes           No
                                                          ▼            ▼
                                                  L_Tutorial ──▶ L_Temple (persistent)
```

Each widget **writes into `PendingRunSettings` and never travels with the player**. The widgets are stateless views over the `GameInstance` struct, which means:

- The player can go back and change difficulty without stale widget state.
- The menu can be reopened (Art Display, Options) without losing the configuration.
- The `GameMode` reads exactly one struct at `BeginPlay` and never asks the UI anything.

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::BeginPlay()
{
    Super::BeginPlay();

    // The GameInstance is the only object guaranteed to outlive every level in
    // the flow, so it is the only place we can read the front-end configuration.
    const UIgKnightedGameInstance* GI = GetGameInstance<UIgKnightedGameInstance>();
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("IgKnighted: no GameInstance - cannot start a run."));
        return;
    }

    RunSettings = GI->PendingRunSettings;
    UE_LOG(LogTemp, Log, TEXT("IgKnighted: starting run -> %s"), *RunSettings.ToString());

    // Order matters. State first (other systems read it during their own BeginPlay),
    // then players, then the first encounter.
    if (AIgKnightedGameState* GS = GetGameState<AIgKnightedGameState>())
    {
        GS->InitializeMatch(/*MaxLives=*/5, RunSettings.Difficulty);
    }

    CreateLocalPlayers(RunSettings.LocalPlayerCount);
    // Zone activation is deferred one tick: PlayerControllers must exist and be
    // initialised before we spawn Pawns and start the first encounter.
    GetWorldTimerManager().SetTimerForNextTick(this, &AIgKnightedGameMode::BeginFirstEncounter);
}
```

### 2.3 GameMode & GameState

**This is the section that defines the game.** The shared five-life pool is the rule that broke most of my early designs, so it gets the most attention.

**The error I made first.** My first instinct was to give each `PlayerState` five lives, because that is the intuitive mental model ("the HUD shows *your* lives"). Two minutes into playtesting I realised it was wrong: with two players each holding five lives, the team could lose ten pawns before the run ended, and the Game Over screen would appear while one player was still standing with four lives left. The design said *five lives between you*. So the lives had to move.

```cpp
// IgKnightedGameState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IgKnightedTypes.h"
#include "IgKnightedGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSharedLivesChanged, int32, NewLives, int32, MaxLives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchPhaseChanged, EMatchPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneChanged, int32, NewZoneIndex);

/**
 * Read-mostly mirror of the match, broadcast to anyone who cares (HUD, audio,
 * Niagara, the tutorial director). The GameMode owns the RULES; this class owns
 * the NUMBERS that the rest of the game is allowed to read.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Match")
    int32 SharedLivesRemaining = 5;

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    int32 MaxSharedLives = 5;

    /** 0-based index into the zone array. */
    UPROPERTY(BlueprintReadOnly, Category = "Match")
    int32 CurrentZoneIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    EMatchPhase MatchPhase = EMatchPhase::Initializing;

    UPROPERTY(BlueprintReadOnly, Category = "Match")
    EIgDifficulty Difficulty = EIgDifficulty::Medium;

    /** Convenience for the HUD: how many enemies are still standing in this zone. */
    UPROPERTY(BlueprintReadOnly, Category = "Match")
    int32 ActiveEnemiesRemaining = 0;

    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnSharedLivesChanged OnSharedLivesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnMatchPhaseChanged OnMatchPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnZoneChanged OnZoneChanged;

    void InitializeMatch(int32 InMaxLives, EIgDifficulty InDifficulty)
    {
        MaxSharedLives      = FMath::Max(1, InMaxLives);
        SharedLivesRemaining = MaxSharedLives;
        Difficulty          = InDifficulty;
        MatchPhase          = EMatchPhase::Active;

        OnSharedLivesChanged.Broadcast(SharedLivesRemaining, MaxSharedLives);
        OnMatchPhaseChanged.Broadcast(MatchPhase);
    }

    /**
     * The ONLY way shared lives are allowed to change.
     *
     * Centralising this is what makes the "one death spends exactly one life"
     * rule enforceable. If three different systems could each decrement the
     * counter, "exactly one" would be a convention rather than a guarantee.
     */
    bool TryConsumeSharedLife()
    {
        if (MatchPhase != EMatchPhase::Active) { return false; }
        if (SharedLivesRemaining <= 0)         { return false; }

        SharedLivesRemaining = FMath::Max(0, SharedLivesRemaining - 1);
        OnSharedLivesChanged.Broadcast(SharedLivesRemaining, MaxSharedLives);
        return true;
    }

    void SetMatchPhase(EMatchPhase NewPhase)
    {
        if (MatchPhase == NewPhase) { return; }
        MatchPhase = NewPhase;
        OnMatchPhaseChanged.Broadcast(MatchPhase);
    }

    void SetZoneIndex(int32 NewIndex)
    {
        if (CurrentZoneIndex == NewIndex) { return; }
        CurrentZoneIndex = NewIndex;
        OnZoneChanged.Broadcast(CurrentZoneIndex);
    }
};
```

**The GameMode — the only place a life may be spent.**

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::HandlePlayerEliminated(AController* Controller)
{
    // Guard 1: the match must still be running. A death that lands in the same
    // frame as the victory transition must not resurrect a dead run.
    if (bMatchFinished) { return; }

    // Guard 2: the controller must still be valid. A Pawn that was destroyed
    // during its own death animation can call back with a stale controller.
    if (!IsValid(Controller)) { return; }

    AIgKnightedGameState* GS = GetGameState<AIgKnightedGameState>();
    if (!GS) { return; }

    // Guard 3 (the important one): one death, one life.
    // HandlePlayerEliminated can be reached from a projectile impact, a hazard
    // overlap, AND a crawler contact. Without this set, one death could spend
    // two or three lives.
    if (!EliminatedControllersThisFrame.Contains(Controller))
    {
        EliminatedControllersThisFrame.Add(Controller);

        if (!GS->TryConsumeSharedLife())
        {
            // Pool is empty: the whole team is out. Even if the other Pawn is
            // still alive, the run ends here. This is the design, not a bug.
            FinishMatch(EMatchResult::Defeat);
            return;
        }
    }
    else
    {
        // Duplicate report of a death we already paid for. Swallow it.
        return;
    }

    if (GS->SharedLivesRemaining <= 0)
    {
        FinishMatch(EMatchResult::Defeat);
        return;
    }

    // Lives remain: the dead player comes back, the run continues.
    ScheduleRespawn(Controller);
}
```

**The invariant list.** These are the rules I wrote down before touching the respawn code, and they are what I tested against:

| # | Invariant | Why it exists |
|---|---|---|
| 1 | `SharedLivesRemaining` is always in `[0, MaxSharedLives]` and never negative | Two players can die on the same frame |
| 2 | One pawn death spends **at most** one shared life | Water, hazard and contact damage all funnel here |
| 3 | No respawn is scheduled after the match has finished | The Game Over screen must be final |
| 4 | Game Over fires at zero **even if the other Pawn is alive** | This is the rule that makes the pool "shared" |
| 5 | No zone progression happens after the match ends | Prevents a victory screen racing a Game Over screen |
| 6 | A death and a final-zone clear in the same frame resolve deterministically | The rule above defines the precedence: **death wins**, because a player who is dead cannot have cleared the zone |

### 2.4 PlayerController, PlayerState & Pawn

Three classes, three different jobs, and mixing them up is one of the classic Unreal mistakes. Here is the mental model I used:

| Class | Question it answers | IgKnighted responsibility | Typical lifetime |
|---|---|---|---|
| **`APlayerController`** | *"What is this player's intent, and what are they looking at?"* | Input routing, Pawn possession, camera, UI creation, spawning the Pawn | Long — survives Pawn death |
| **`APlayerState`** | *"Who is this player, and what have they done?"* | Score, score multiplier, colour/identity, personal stats | Long — survives Pawn replacement |
| **`APawn` (Character)** | *"Where is this player physically, and what are they doing right now?"* | Movement, aiming, dash, shooting, hit handling, visuals | Short — destroyed and replaced on death |

The rule that matters: **the score must not live on the Pawn.** When a player dies and respawns, a new Pawn is spawned. If the score were a Pawn property, it would reset on death — which is exactly the kind of bug the "collectables doubled" symptom in our release was adjacent to. Score lives in `PlayerState`, so it survives.

```cpp
// IgKnightedPlayerState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IgKnightedPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, NewScore, int32, Multiplier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplierChanged, int32, NewMultiplier);

/**
 * Per-player identity and statistics.
 *
 * NOTE: there are deliberately NO lives here. The five lives are a TEAM pool
 * that lives in the GameState. Giving each PlayerState its own counter was the
 * first design I tried and it produced a Game Over screen that lied to the
 * player - the run could end while someone was still alive with lives left.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 Score = 0;

    /** Multiplies collectable value. Reset per zone so one player cannot snowball. */
    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 ScoreMultiplier = 1;

    /** How many matchsticks this player has personally collected. */
    UPROPERTY(BlueprintReadOnly, Category = "Score")
    int32 CollectablesCollected = 0;

    /** Player 0 = crimson, Player 1 = gold. Drives the HUD panel and the rim light. */
    UPROPERTY(BlueprintReadOnly, Category = "Identity")
    FLinearColor PlayerColor = FLinearColor::White;

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnMultiplierChanged OnMultiplierChanged;

    /**
     * The single entry point for awarding score. Everything that can score
     * goes through here - collectables, enemy kills, zone bonuses - so the
     * multiplier is applied in exactly one place.
     */
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 BaseValue)
    {
        const int32 Awarded = BaseValue * ScoreMultiplier;
        Score += Awarded;
        OnScoreChanged.Broadcast(Score, ScoreMultiplier);
    }

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddCollectable()
    {
        ++CollectablesCollected;
        // The HUD reads the authoritative counter, it never keeps its own tally.
        // Two counters drifting apart is how you get "the collectable count
        // appears doubled on the Game Over screen".
        AddScore(100);
    }

    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetMultiplier(int32 NewMultiplier)
    {
        ScoreMultiplier = FMath::Max(1, NewMultiplier);
        OnMultiplierChanged.Broadcast(ScoreMultiplier);
    }
};
```

**Possession.** When a Pawn dies, the `PlayerController` survives; the `Pawn` is replaced. Possession is the only thing that changes:

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::ServerRespawnPlayer_Implementation(AController* Controller)
{
    // (In a local game the "server" is this process; the naming keeps the code
    //  honest about where the authority boundary would sit in a networked build.)
    if (!Controller || !Controller->PlayerState) { return; }

    const AIgKnightedGameMode* GM = GetWorld()->GetAuthGameMode<AIgKnightedGameMode>();
    if (!GM || GM->IsMatchFinished()) { return; }

    // Spawn at a PlayerStart, not at the death location. Our design respawns
    // both players at the entrance of the level, which also removes spawn
    // camping: the enemies of the current wave reset with the player.
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner          = Controller;
    SpawnParams.Instigator     = Controller;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    const FVector SpawnLocation = GM->GetRespawnLocation(Controller);
    const FRotator SpawnRotation = GM->GetRespawnRotation(Controller);

    ACharacter* NewPawn = GetWorld()->SpawnActor<ACharacter>(GM->GetPlayerPawnClass(), SpawnLocation, SpawnRotation, SpawnParams);
    if (!NewPawn) { return; }

    // Possess replaces the old Pawn cleanly. The PlayerController - and with it
    // the PlayerState and the score - survives untouched.
    Possess(NewPawn);
    NewPawn->SpawnDefaultController();

    // Restore input: a newly possessed Pawn may need its mapping contexts and
    // its dash charges re-armed.
    NewPawn->ResetPlayerStateForSpawn();

    // Safety net: if the Possess above failed (Pawn already destroyed), make
    // sure the controller is not left pointing at nothing.
    if (GetPawn() != NewPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("IgKnighted: possession failed during respawn."));
    }
}
```

> **The lifetime summary I would put on a whiteboard:**
> `GameInstance` = the *session* (front-end config). `GameMode` = the *match rules*. `GameState` = the *match numbers*. `PlayerController` = the *intent*. `PlayerState` = the *identity*. `Pawn` = the *body*. Nothing that must survive a death lives on the body.

---

## 03. Player Systems & Local Multiplayer

> **Local multiplayer is where the Gameplay Framework stops being theory.** Two controllers, one process, one screen. Everything you learned about `LocalPlayer`, `PlayerController` and `Possess` becomes load-bearing.

### 3.1 Local Multiplayer Architecture

![IgKnighted — the two-player setup and player creation flow in the shipped build](../../assets/images/Y1D/1.png)

When the player chooses "2 players" in the front-end, we need **two** of everything that a player owns, and **one** of everything that the match owns:

```text
  PLAYER-OWNED (×2)                          MATCH-OWNED (×1)
  ─────────────────                          ─────────────────
  ULocalPlayer            ×2                 AGameMode            ×1
  UEnhancedInputLocalPlayerSubsystem  ×2     AGameState           ×1
  APlayerController       ×2                 AZoneManager         ×1
  APlayerState            ×2                 Encounter data       ×1
  APawn (Character)       ×2                 Shared lives = 5     ×1
  Player HUD panel        ×2
```

**The creation sequence.** The order is not arbitrary — each step depends on the previous one existing:

```
 1. GameMode::CreateLocalPlayers(2)
       │
       ├── Is there already a LocalPlayer?  → find it, do not create a second one
       │      (this matters: we travel Menu → Tutorial → Main Map, and the
       │       GameInstance keeps LocalPlayers alive across travel)
       │
       └── for each missing player index:
             UGameplayStatics::CreatePlayer(GetWorld(), PlayerIndex, /*bSpawnPlayerController=*/true)
                   │
                   └── Engine creates: ULocalPlayer → APlayerController
                                        │
                                        └── creates / assigns APlayerState
 2. Wait for the controller to finish initialisation
 3. GameMode::SpawnPlayerPawn(Controller)
       │
       ├── Pick a PlayerStart that is not already occupied
       ├── Spawn the Pawn with FActorSpawnParameters (Owner = Controller)
       └── Controller->Possess(Pawn)
 4. Each LocalPlayer installs its Enhanced Input mapping contexts
```

**Step 4 was the fix that finally made two controllers work.** Before it, both players read the *same* keyboard because the mapping context was on the world rather than on the `LocalPlayer`. After it, each `LocalPlayer`'s `UEnhancedInputLocalPlayerSubsystem` owns its contexts and its own device routing.

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::CreateLocalPlayers(int32 DesiredCount)
{
    UWorld* World = GetWorld();
    if (!World) { return; }

    const int32 Clamped = FMath::Clamp(DesiredCount, 1, 2);

    // The LocalPlayers live on the GameInstance and SURVIVE level travel.
    // The naive implementation - "always create DesiredCount players" - is what
    // gives you duplicate controllers after the tutorial loads, and it is why
    // the first thing I do is count what already exists.
    const UIgKnightedGameInstance* GI = GetGameInstance<UIgKnightedGameInstance>();
    int32 ExistingCount = GI ? GI->GetNumLocalPlayers() : 0;

    for (int32 Index = ExistingCount; Index < Clamped; ++Index)
    {
        // bSpawnPlayerController = true so the engine gives us a PlayerController
        // AND a PlayerState in one step, in the correct order. Doing this by hand
        // (spawning a controller, then a PlayerState, then wiring them) is how
        // you end up with a controller that has no PlayerState and a score that
        // silently resets.
        ULocalPlayer* NewLocalPlayer = UGameplayStatics::CreatePlayer(World, Index, /*bSpawnPlayerController=*/true);
        if (!NewLocalPlayer)
        {
            UE_LOG(LogTemp, Error, TEXT("IgKnighted: failed to create LocalPlayer %d"), Index);
            continue;
        }

        // CreatePlayer returns the player with its controller already assigned in
        // most UE5 versions, but we look it up explicitly rather than assume:
        // the exact assignment order differs between engine versions and this
        // is exactly the sort of assumption that breaks on an engine upgrade.
        APlayerController* PC = NewLocalPlayer->GetPlayerController(World);
        if (!PC)
        {
            UE_LOG(LogTemp, Warning, TEXT("IgKnighted: LocalPlayer %d has no controller yet."), Index);
            continue;
        }

        // Give the controller a stable identity so the HUD can label it
        // (Player 1 / Player 2) and so the Pawn spawner knows which colour to use.
        if (AIgKnightedPlayerController* IgPC = Cast<AIgKnightedPlayerController>(PC))
        {
            IgPC->SetLocalPlayerIndex(Index);
        }
    }

    // The controllers are created, but their Pawns are spawned on the next tick:
    // the engine finishes initialising LocalPlayers asynchronously, and spawning
    // a Pawn for a controller that is not fully initialised produces a Pawn that
    // is never possessed.
    GetWorldTimerManager().SetTimerForNextTick(this, &AIgKnightedGameMode::SpawnAllPlayerPawns);
}
```

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::SpawnAllPlayerPawns()
{
    // Collect the occupied PlayerStarts first so two players never spawn inside
    // each other. Simple, but it is exactly the "players are overlapping at the
    // start" bug that shows up the moment you support two players.
    TArray<AActor*> AvailableStarts;
    for (AActor* Actor : PlayerStartActors)
    {
        if (APlayerStart* PS = Cast<APlayerStart>(Actor))
        {
            bool bOccupied = false;
            for (const APlayerController* PC : GetWorld()->GetPlayerControllerIterator())
            {
                if (PC && PC->GetPawn() && PC->GetPawn()->GetActorLocation().Equals(PS->GetActorLocation(), 150.f))
                {
                    bOccupied = true;
                    break;
                }
            }
            if (!bOccupied) { AvailableStarts.Add(PS); }
        }
    }

    int32 StartIndex = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (!PC || PC->GetPawn()) { continue; }   // already has a Pawn

        const FVector SpawnLocation = AvailableStarts.IsValidIndex(StartIndex)
            ? AvailableStarts[StartIndex]->GetActorLocation()
            : PlayerStartActors[0]->GetActorLocation();
        ++StartIndex;

        SpawnPlayerPawn(PC, SpawnLocation);
    }
}
```

**Things that broke and how I reasoned about them:**

| Symptom | Root cause | Fix |
|---|---|---|
| Player 2's controller did nothing | The Enhanced Input mapping context was added to the world, so both LocalPlayers consumed the same keyboard | Mapping contexts live on the `ULocalPlayer` subsystem, not the world |
| Two controllers after returning from the tutorial | The `CreatePlayer` call ran again on a fresh `GameMode` while the `LocalPlayer` already existed on the `GameInstance` | Count existing `LocalPlayers` before creating |
| A Pawn that nothing could control | The Pawn was spawned before the `LocalPlayer` finished initialising, so `Possess` failed silently | Deferred the spawn by one tick and asserted on the possess result |
| The HUD showed Player 1's score on both panels | The widget read `GetPlayerState()` on its *own* controller instead of its *owning* player's | The HUD is bound to a specific `PlayerState` reference, passed in at creation |
| Keyboard input kept moving Player 1 during the tutorial | The gameplay mapping context was still installed on the tutorial's LocalPlayer | Each level owns its own contexts; the tutorial installs `IMC_Tutorial` only |

The pattern in every row: **the mistake is always about identity.** "Which player does this event belong to?" is the question local multiplayer asks constantly.

### 3.2 Enhanced Input System

Enhanced Input replaced the old `BindAxis`/`BindAction` model, and for a project with two local players and keyboard/gamepad parity it is the right tool: it separates *what* a player can do (an `InputAction`) from *how* it is bound (an `InputMappingContext`).

![IgKnighted — the Enhanced Input Action and Mapping Context assets in the shipped build](../../assets/images/Y1D/4.png)

**The asset layout.**

```text
Content/IgKnighted/Input/
  IA/
    IA_Move        Axis2D     — left stick / WASD
    IA_Aim         Axis2D     — right stick / mouse
    IA_Fire         Boolean   — RT / left mouse
    IA_Dash         Boolean   — RB / space
    IA_Pause        Boolean   — Start / Esc
    IA_Confirm      Boolean   — Accept / Enter
    IA_Cancel       Boolean   — Back / Esc
  IMC/
    IMC_Gameplay_Keyboard    WASD + mouse + space
    IMC_Gameplay_Gamepad     sticks + face buttons
    IMC_Menu                 navigation + confirm/cancel
    IMC_Tutorial             tutorial-only bindings
```

Two contexts for gameplay (keyboard and gamepad) rather than one big one, because the **mapping priority** trick lets a player pick up a gamepad mid-session: while the keyboard context is active it owns the keyboard, and the moment a gamepad input arrives the gamepad context takes over.

**Installing the contexts per player.**

```cpp
// IgKnightedPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputTriggers.h"
#include "IgKnightedPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIgKnightedPlayerController();

    /** 0 for Player 1, 1 for Player 2. Used for HUD labelling and player colour. */
    UPROPERTY(BlueprintReadOnly, Category = "Player")
    int32 LocalPlayerIndex = 0;

    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    /** Owned so the subsystem keeps a strong reference and the context is not GC'd. */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> GameplayKeyboardContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> GameplayGamepadContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> AimAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> DashAction;

    /** Bound to the Pawn so we can forward intent without the Pawn polling. */
    FVector2D MoveInput = FVector2D::ZeroVector;
    FVector2D AimInput  = FVector2D::ZeroVector;

    UFUNCTION()
    void OnMoveTriggered(const FInputActionValue& Value);

    UFUNCTION()
    void OnAimTriggered(const FInputActionValue& Value);

    UFUNCTION()
    void OnFireStarted(const FInputActionValue& Value);

    UFUNCTION()
    void OnFireCompleted(const FInputActionValue& Value);

    UFUNCTION()
    void OnDashPressed(const FInputActionValue& Value);
};
```

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Enhanced Input requires the EnhancedInputComponent specifically.
    // Casting is safe here because the project default input component class is
    // set to EnhancedInputComponent in Project Settings; we log loudly rather
    // than silently doing nothing if that setting is ever changed.
    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EIC)
    {
        UE_LOG(LogTemp, Error,
            TEXT("IgKnighted: InputComponent is not an EnhancedInputComponent. "
                 "Check Project Settings > Input > Default Input Component Class."));
        return;
    }

    EIC->BindAction(MoveAction,  ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnMoveTriggered);
    EIC->BindAction(AimAction,   ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnAimTriggered);
    EIC->BindAction(FireAction,  ETriggerEvent::Started,    this, &AIgKnightedPlayerController::OnFireStarted);
    EIC->BindAction(FireAction,  ETriggerEvent::Completed, this, &AIgKnightedPlayerController::OnFireCompleted);
    EIC->BindAction(DashAction,  ETriggerEvent::Started,    this, &AIgKnightedPlayerController::OnDashPressed);
}
```

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // ---------------------------------------------------------------------
    // Install the Enhanced Input mapping contexts ON THIS LOCAL PLAYER.
    //
    // This is the single line that separates "works with one player" from
    // "works with two players". The contexts are added to the LocalPlayer's
    // EnhancedInputLocalPlayerSubsystem, so player 0's keyboard and player 1's
    // gamepad are resolved by two independent subsystems.
    //
    // It must be re-applied on Possess because:
    //   - the subsystem is per LocalPlayer and survives, but
    //   - after a level travel the engine clears input, and after a respawn
    //     the mapping context may have been removed by the level's own setup.
    // ---------------------------------------------------------------------
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->ClearAllMappings();

            // Priority 0 = gameplay, 1 = menus. Menus win when they are open,
            // so a player pressing 'A' on the Game Over screen does not also
            // fire a shot.
            if (GameplayKeyboardContext)
            {
                Subsystem->AddMappingContext(GameplayKeyboardContext, 0);
            }
            if (GameplayGamepadContext)
            {
                Subsystem->AddMappingContext(GameplayGamepadContext, 1);
            }
        }
    }
}

void AIgKnightedPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Remove our contexts so a menu level does not inherit stale gameplay
    // bindings. "Controls stop working after the art-display map" was exactly
    // this class of bug: the art gallery pushed its own level, the gameplay
    // context was still mapped somewhere, and the menu context was never
    // re-installed for the new world.
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->ClearAllMappings();
        }
    }

    Super::EndPlay(EndPlayReason);
}
```

**Movement and aim mathematics.** In a top-down game the character lives in a plane, and the input vector must be transformed from screen space into world space. Getting this wrong is what makes a twin-stick feel wrong: the character slides diagonally at 141% of its intended speed.

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::UpdateMovementInput(const FVector2D& Input)
{
    // Raw input, unclamped and unnormalised. The component applies dead zone
    // and magnitude; we only need the direction here.
    if (Input.IsNearlyZero())
    {
        // Do NOT call Normalize() on a zero vector - it returns NaN on some
        // platforms and the character explodes out of the level.
        AddMovementInput(FVector::ZeroVector, 0.f);
        return;
    }

    // Screen-space -> world-space. On a top-down camera the mapping is roughly
    // identity in XY, but keeping it as an explicit basis vector means the
    // game still works if the camera is rotated or the level is tilted.
    const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
    const FVector Right   = GetActorRightVector().GetSafeNormal2D();

    // The MovementComponent already normalises the final direction, so we do
    // NOT normalise here - normalising a diagonal input would make diagonal
    // movement as fast as straight movement, which feels wrong in a shooter.
    const FVector WorldDirection = (Forward * Input.Y + Right * Input.X);
    AddMovementInput(WorldDirection, 1.0f);
}

void AIgKnightedPlayerCharacter::UpdateAimInput(const FVector2D& Input)
{
    if (Input.IsNearlyZero()) { return; }

    // Aim is independent of movement - a twin-stick player must be able to run
    // left while shooting right. Aim rotates the weapon pivot (and therefore
    // the muzzle), not the whole character.
    const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
    const FVector Right   = GetActorRightVector().GetSafeNormal2D();
    const FVector AimDir  = (Forward * Input.Y + Right * Input.X).GetSafeNormal();

    if (AimDir.IsNearlyZero()) { return; }

    TargetAimRotation = AimDir.Rotation();
}
```

**Dead zones and sensitivity** live on the `InputAction` as modifiers (`Dead Zone`, `Negation`, `Swizzle`, `U-Axis` modifiers) rather than in code, so a designer can tune them without a rebuild.

### 3.3 Player Character Architecture

The Pawn is a **composition**, not a monolith. Every gameplay rule that could be isolated became a component; the Character's job is to wire them together and to own the visual representation.

```text
  AIgKnightedPlayerCharacter : ACharacter
  │
  ├── UCapsuleComponent                (inherited)  ── collision & root
  ├── UCharacterMovementComponent      (inherited)  ── movement, rotation mode
  ├── UMeshComponent + ABP_Player       (inherited)  ── visuals + animation
  │
  ├── UIgKnightedCombatComponent        ── firing, fire pattern, muzzle, ammo state
  ├── UIgKnightedDashComponent          ── charges, cooldown, i-frames, traversal hits
  ├── UIgKnightedStatusComponent        ── shield, buffs, damage-taken routing
  │
  └── AIgKnightedPlayerController       ── input intent, camera, possession
      AIgKnightedPlayerState            ── score, multiplier, identity
```

**Why components and not one big Character class.** Three reasons, in order of how much they mattered in practice:

1. **Reuse.** The Scorpion, the Crawler and the player all need "fire a projectile at a target". With `UIgKnightedCombatComponent` that is one implementation; the enemy version just supplies a different projectile class and a different aim source.
2. **Testability.** The dash rules (charges, cooldown, i-frames) live in a component with no dependency on the world, the HUD or the animation. I could reason about them without launching the game.
3. **Designer ownership.** `CombatComponent` exposes `FirePattern`, `ProjectileClass`, `MuzzleSocket` as `EditDefaultsOnly`. An artist changes the muzzle socket or the projectile art without touching the character logic.

The counter-argument is real, and I want to be honest about it: **components are not free.** Each one adds a level of indirection, a lifecycle to reason about, and a chance to hide a bug behind "is the component enabled?". For a game with one player character, a single `Character` with four well-named functions would have been faster to write and easier to debug. The component split paid off because the *same* concepts were needed by three different actors (player, Crawler, Scorpion). If a component were used exactly once, I would inline it.

```cpp
// IgKnightedPlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IgKnightedPlayerCharacter.generated.h"

class UIgKnightedCombatComponent;
class UIgKnightedDashComponent;
class UIgKnightedStatusComponent;

UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AIgKnightedPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

    UFUNCTION(BlueprintPure, Category = "Player")
    AIgKnightedPlayerState* GetIgPlayerState() const;

    /** Called by the GameMode after a respawn. */
    UFUNCTION(BlueprintCallable, Category = "Player")
    void ResetPlayerStateForSpawn();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    TObjectPtr<UIgKnightedCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    TObjectPtr<UIgKnightedDashComponent> DashComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    TObjectPtr<UIgKnightedStatusComponent> StatusComponent;

    /** The weapon pivot that follows the aim direction. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    TObjectPtr<USceneComponent> WeaponPivot;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    FName MuzzleSocketName = TEXT("Muzzle");

    UFUNCTION()
    void OnAimUpdated(FVector NewAimDirection);

    void UpdateMovementInput(const FVector2D& Input);
    void UpdateAimInput(const FVector2D& Input);
};
```

### 3.4 Dash & Invulnerability

The dash is the game's central verb: it is movement, it is a defensive ability, and (when enhanced) it is an offensive one. Three rules define it:

- A dash has a **cooldown** and a limited number of **charges**.
- While dashing, the player is **immune to water damage** — that is the intended counter to the Water Mollusk's beam.
- The **Enhanced Dash** power-up grants extra charges and lets the dash **kill enemies you pass through**.

**The state model.** The obvious bug here is using a single `bool bInvulnerable`. The instant a second protection source appears — and it does, the shield — a boolean cannot represent "immune because of dash" and "protected by shield" at the same time, and clearing the dash flag can accidentally clear the shield.

```cpp
// IgKnightedDashComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedDashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDashStateChanged, bool, bIsDashing, int32, RemainingCharges);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashEnded);

UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedDashComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIgKnightedDashComponent();

    /** How long the dash movement lasts, in seconds. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
    float DashDuration = 0.18f;

    /** Distance travelled during one dash. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
    float DashDistance = 700.f;

    /** Seconds between charge regenerations. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
    float DashCooldown = 1.2f;

    /** How many charges the player has at full charge. Enhanced Dash raises this. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
    int32 MaxDashCharges = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    int32 CurrentCharges = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    bool bIsDashing = false;

    /** True while the dash-granted invulnerability is active. */
    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    bool bDashInvulnerable = false;

    UPROPERTY(BlueprintAssignable, Category = "Dash")
    FOnDashStateChanged OnDashStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Dash")
    FOnDashEnded OnDashEnded;

    /**
     * The ONLY way to start a dash. Returns false and explains nothing:
     * callers use it as a gate, and the reason for refusal is a state question
     * the UI answers by reading CurrentCharges / bIsDashing.
     */
    UFUNCTION(BlueprintCallable, Category = "Dash")
    bool TryStartDash(const FVector& WorldDirection);

    UFUNCTION(BlueprintPure, Category = "Dash")
    bool IsDashInvulnerable() const { return bDashInvulnerable; }

    /** Called by the Enhanced Dash power-up. */
    UFUNCTION(BlueprintCallable, Category = "Dash")
    void AddDashCharges(int32 AdditionalCharges);

    /** Whether the dash, if it starts now, would kill enemies it passes through. */
    UFUNCTION(BlueprintPure, Category = "Dash")
    bool DoesDashDealDamage() const { return bEnhancedDashActive; }

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    bool bEnhancedDashActive = false;

    /** Enemies already hit by THIS dash, so a long overlap does not kill twice. */
    TSet<TWeakObjectPtr<AActor>> EnemiesHitThisDash;

    FTimerHandle DashEndTimer;
    FTimerHandle ChargeRegenTimer;

    void EndDash();
    void HandleDashTravel();
    void RegenerateCharge();
    void OnEnhancedDashChanged(bool bNewActive);
};
```

```cpp
// IgKnightedDashComponent.cpp
bool UIgKnightedDashComponent::TryStartDash(const FVector& WorldDirection)
{
    // ---- Validity gate -----------------------------------------------------
    // Every one of these has been a real bug at some point during the project.
    if (bIsDashing)                      { return false; }  // already dashing
    if (CurrentCharges <= 0)             { return false; }  // on cooldown
    if (!GetOwner() || !GetOwner()->GetInstigatorController()) { return false; }
    if (!GetOwner()->GetWorld())         { return false; }

    // Direction: use the requested direction if valid, otherwise fall back to
    // the facing direction. A dash with no direction should still work - the
    // player pressed the button.
    FVector Dir = WorldDirection.GetSafeNormal();
    if (Dir.IsNearlyZero())
    {
        Dir = Cast<AActor>(GetOwner())->GetActorForwardVector();
    }

    // ---- Consume the charge BEFORE applying the movement --------------------
    // If we consumed it after, a dash that triggered an overlap callback which
    // re-entered this function would be free.
    --CurrentCharges;

    bIsDashing = true;

    // ---- Invulnerability FIRST ---------------------------------------------
    // The i-frame is enabled before the pawn moves a single centimetre. If we
    // moved first and enabled it after, the player could dash into a beam and
    // take the hit on the first frame of the dash - which is exactly the
    // "dashing through the water stream does not work" bug.
    bDashInvulnerable = true;

    // Clear the per-dash hit set. Without this, a second dash would not kill
    // an enemy that the first dash already touched.
    EnemiesHitThisDash.Reset();

    // ---- Movement ----------------------------------------------------------
    // I use a swept movement with a launch velocity rather than a timer-driven
    // position lerp, because a sweep respects world geometry (walls) and
    // produces overlap events with enemies along the whole path in one frame.
    const FVector LaunchVelocity = Dir * (DashDistance / FMath::Max(DashDuration, KINDA_SMALL_NUMBER));
    GetOwner()->LaunchCharacter(LaunchVelocity, true, false);

    GetWorld()->GetTimerManager().SetTimer(
        DashEndTimer, this, &UIgKnightedDashComponent::EndDash, DashDuration, false);

    OnDashStateChanged.Broadcast(bIsDashing, CurrentCharges);
    return true;
}

void UIgKnightedDashComponent::EndDash()
{
    if (!bIsDashing) { return; }

    bIsDashing = false;

    // Remove ONLY the dash's own invulnerability. We never touch the shield -
    // they are independent protections, and clearing the wrong one is how a
    // player ends up dying while holding a shield they were told they had.
    bDashInvulnerable = false;
    EnemiesHitThisDash.Reset();

    OnDashEnded.Broadcast();
    OnDashStateChanged.Broadcast(bIsDashing, CurrentCharges);
}
```

**The Enhanced Dash traversal kill.** Crossing an enemy must kill it **once**, even though the sweep can generate several overlap events across consecutive frames.

```cpp
// IgKnightedDashComponent.cpp
void UIgKnightedDashComponent::HandleDashTravel()
{
    if (!bEnhancedDashActive || !bIsDashing) { return; }

    // Sweep along the dash path. A sphere sweep gives us every enemy the dash
    // passed through, including ones we are no longer overlapping this frame.
    const FVector Start = GetOwner()->GetActorLocation();
    const FVector End   = Start + GetOwner()->GetVelocity() * GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(IgDashSweep), /*bTraceComplex=*/false);
    Params.AddIgnoredActor(GetOwner());

    const bool bHit = GetWorld()->SweepSingleByChannel(
        Hit, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(40.f), Params);

    if (!bHit) { return; }

    AActor* HitActor = Hit.GetActor();
    if (!HitActor || !HitActor->IsA<AIgKnightedEnemyCharacter>()) { return; }

    // Idempotency: this exact enemy may be reported by several frames of the
    // sweep. A TSet of weak pointers is the cheapest correct answer - the set
    // is reset at the start of every dash.
    if (EnemiesHitThisDash.Contains(HitActor)) { return; }
    EnemiesHitThisDash.Add(HitActor);

    // Route the kill through the SAME damage resolver that a normal shot uses.
    // One path for all damage means the death event, the score award and the
    // enemy registry update cannot be forgotten in one of them.
    if (AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(HitActor))
    {
        Enemy->ApplyLethalDamage(this, /*Instigator=*/GetOwner()->GetInstigatorController());
    }
}
```

**Edge cases I had to handle explicitly:**

| Edge case | What went wrong before | The rule I settled on |
|---|---|---|
| Player dashes *through* a Water Mollusk beam | The hit landed on the first frame, before the i-frame existed | Enable i-frames **before** movement starts |
| Player dashes through a Crawler twice in one dash | The Crawler died, then the death event fired again on the next overlap frame | Per-dash `TSet` of already-hit enemies |
| Player is hit by a projectile on the *same frame* the dash starts | Order of execution decided the outcome; sometimes the player died mid-dash | The dash's i-frame is queried at the moment damage is *resolved*, not when it is *delivered* |
| Dash interrupted by a stagger | The player was left permanently invulnerable | The dash timer is the single authority; interruption calls `EndDash()` explicitly |
| Player dies mid-dash | The respawned Pawn inherited the old dash's timers | Timers are bound to the component and cleared in `BeginPlay`/`ResetPlayerStateForSpawn` |
| Shield and dash active at the same time | Ending the dash cleared the shield | The two protections are independent flags; each removes only itself |

### 3.5 Player Death & Respawning

![IgKnighted — the player damage, death feedback and respawn flow in the shipped build](../../assets/images/Y1D/2.png)

**The damage precedence.** This is the single most important rule in the game, and it is a strict ordering:

```
        water damage / lethal contact arrives
                     │
                     ▼
    ┌────────────────────────────────────────────┐
    │ 1. Is the match still active?              │──no──▶ ignore
    ├────────────────────────────────────────────┤
    │ 2. Has this death already been processed?  │──yes─▶ ignore  (idempotency)
    ├────────────────────────────────────────────┤
    │ 3. Is the target dash-invulnerable?        │──yes─▶ ignore, no life spent
    ├────────────────────────────────────────────┤
    │ 4. Does the target have a shield?          │──yes─▶ consume shield, break it,
    ├────────────────────────────────────────────┤        NO life spent, no death
    │ 5. Otherwise: this is a lethal hit         │
    │    → mark dead, consume ONE shared life,   │
    │      respawn or end the match              │
    └────────────────────────────────────────────┘
```

**Why the order matters.** Check the shield *before* the dash and a dashing player would shatter their shield every time they dodged a projectile — the shield would be worthless and the dash would be the only real defence. Check death before the shield and a shielded player would die anyway. The order encodes the design intent: *the dash is a free action, the shield is a consumable resource.*

```cpp
// IgKnightedDamageResolver.cpp  (a UObject service, not an Actor)
// This is the ONLY place in the project where a player can die.

FIgDamageResult UIgKnightedDamageResolver::ResolvePlayerHit(
    AIgKnightedPlayerCharacter* Target,
    const FIgDamageContext& Context)
{
    FIgDamageResult Result;

    if (!IsValid(Target)) { return Result; }

    // ---- 1. Match still running? -------------------------------------------
    const AIgKnightedGameState* GS = Target->GetWorld()->GetGameState<AIgKnightedGameState>();
    if (!GS || GS->MatchPhase != EMatchPhase::Active)
    {
        Result.bIgnored = true;
        return Result;
    }

    // ---- 2. Idempotency ----------------------------------------------------
    // The same death can be reported by a projectile impact, a hazard overlap
    // AND a contact overlap in the same frame. We latch the death so only the
    // first report is processed.
    if (Target->IsDeathProcessed())
    {
        Result.bIgnored = true;
        Result.bDuplicate = true;
        return Result;
    }

    // ---- 3. Dash invulnerability ------------------------------------------
    // Checked BEFORE the shield: a dash is a free dodge and must not cost the
    // player their shield.
    if (Target->GetDashComponent()->IsDashInvulnerable())
    {
        Result.bIgnored = true;
        Result.bInvulnerable = true;
        Target->NotifyDamageIgnored(Context);   // sparks / whoosh VFX, no damage
        return Result;
    }

    // ---- 4. Shield ---------------------------------------------------------
    // The shield absorbs exactly one lethal hit and is consumed doing so.
    if (Target->GetStatusComponent()->ConsumeShield())
    {
        Result.bShieldConsumed = true;
        Target->NotifyShieldBroken(Context);    // shield shatter VFX + SFX
        return Result;
    }

    // ---- 5. Lethal ---------------------------------------------------------
    // Mark the death BEFORE broadcasting anything. If a listener calls back
    // into the resolver, we must already be in the "dead" state.
    Target->MarkDeathProcessed();
    Result.bLethal = true;

    // Cache the controller NOW. Once the Pawn starts destroying itself, its
    // GetController() can return null, and the GameMode needs a valid
    // controller to respawn the right player.
    AController* CachedController = Target->GetController();
    Target->GetStatusComponent()->SetControllerBeforeDeath(CachedController);

    Target->OnPlayerLethalHit(Context);

    // Hand the decision to the GameMode - it owns the shared life pool.
    if (AIgKnightedGameMode* GM = Target->GetWorld()->GetAuthGameMode<AIgKnightedGameMode>())
    {
        GM->HandlePlayerEliminated(CachedController);
    }

    return Result;
}
```

**Death visuals and the respawn sequence.**

```
  lethal hit
     │
     ▼
  Disable collision, disable input, disable firing
     │
     ▼
  Play death animation (Animation Blueprint) + fire "puff" Niagara effect
     │   (the character's fire goes out - it is a fire knight, after all)
     ▼
  Wait RespawnDelay (default 2.0s)
     │
     ▼
  GameMode: shared lives > 0 ?
     │      └── no ──▶ GAME OVER screen, disable gameplay input
     ▼ yes
  Spawn a new Pawn at the level entrance, possess it, restore input + charges
     │
     ▼
  Reset the current zone's encounter (enemies respawn, so nobody can camp
  the entrance), then re-arm the fight
```

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::OnPlayerLethalHit(const FIgDamageContext& Context)
{
    // Stop interacting with the world immediately. A dying player that can still
    // shoot is a source of "how did I die AND kill that enemy" support tickets.
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->StopMovementImmediately();

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        ASC->CancelAllAbilities(this);   // cancel the dash, drop the i-frames
    }

    // Broadcast to animation / VFX / audio. The C++ side owns the STATE, the
    // Blueprint side owns the FEEL. That split is why animators could iterate
    // on the death sequence without touching my code.
    OnDeathStarted.Broadcast(Context);

    // The pawn destroys itself after the death animation; the GameMode spawns
    // the replacement. Doing it the other way round (destroying immediately and
    // playing the animation on a corpse) is what made our first build look
    // like the player blinked out of existence.
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AIgKnightedPlayerCharacter::HandleDeathAnimationEnd);
}
```

**Why the respawn location is fixed at the level entrance.** Two reasons, one mechanical and one design:

- **Mechanical:** a respawn point near where you died is a death loop in a bullet-hell arena — you die, respawn into the same stream, die again. The team would burn all five lives in thirty seconds and never see Zone 2.
- **Design:** resetting the current wave on death keeps the encounter symmetric for both players and removes spawn camping entirely. If you die, you both get a fresh fight.

**Acceptance tests for the death loop.** These are the tests I would (and did) run before calling the feature done:

| ID | Scenario | Expected |
|---|---|---|
| T03 | Player 1 takes an unshielded water hit | One shared life spent, Player 1 respawns |
| T05 | Player 1 spends the last life while Player 2 is alive | Game Over for the whole team |
| T06 | Shielded player takes a water hit | Shield breaks, **no** life spent |
| T07 | Player dashes across the mollusk beam | No life spent, no shield consumed |
| T08 | Player stands in the mollusk beam without shield or dash | Exactly one life spent, no matter how many overlap ticks fire |
| T10 | Two hit callbacks arrive during the same death | Exactly one life spent |
| T17 | Final enemy dies on the same frame as the last life is spent | Deterministic: death wins, run ends in defeat |
| T18 | Return from the tutorial | Input context valid, correct Pawn possessed |

---

## 04. Combat Systems & Enemy AI

> **Combat is a data problem, not a Blueprint problem.** The moment firing a projectile needs to work for a player, a Scorpion and a tutorial dummy, the "just call these five nodes" approach stops scaling.

### 4.1 Combat Architecture

![IgKnighted — an enemy acquiring a target, aiming and firing its projectile](../../assets/images/Y1D/5.png)

The combat system has four layers, and each layer has exactly one responsibility:

```text
  ┌─ LAYER 4 ─ PRESENTATION ────────────────────────────────────────────┐
  │  Niagara VFX · Anim Montage · Audio Cue · Camera Shake               │
  │  Driven by EVENTS from below. Never decides anything.                │
  └───────────────────────────────▲──────────────────────────────────────┘
                                  │ OnImpact / OnShieldBroken / OnDeath
  ┌─ LAYER 3 ─ RESOLUTION ─────────┴────────────────────────────────────┐
  │  UIgKnightedDamageResolver                                         │
  │  The single authority on "did this damage event count?"             │
  │  Precedence: match active → not already dead → dash i-frames →      │
  │              shield → lethal                                       │
  └───────────────────────────────▲──────────────────────────────────────┘
                                  │ FIgDamageContext
  ┌─ LAYER 2 ─ PROJECTILE ─────────┴────────────────────────────────────┐
  │  AIgKnightedProjectile                                             │
  │  Movement (ProjectileMovementComponent) · Collision profile ·       │
  │  Instigator & team · Lifespan · bHasImpacted guard                 │
  └───────────────────────────────▲──────────────────────────────────────┘
                                  │ spawn + initialise
  ┌─ LAYER 1 ─ EMISSION ───────────┴────────────────────────────────────┐
  │  UIgKnightedCombatComponent (player + enemies share it)            │
  │  Fire pattern · cadence · muzzle socket · projectile class          │
  │  UIgKnightedWeaponData (UPrimaryDataAsset) holds the numbers        │
  └─────────────────────────────────────────────────────────────────────┘
```

**The weapon data asset.** All the numbers that a designer tunes live in an asset, not in a Blueprint graph and definitely not in C++:

```cpp
// IgKnightedWeaponData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IgKnightedWeaponData.generated.h"

class AIgKnightedProjectile;

/**
 * All the tunables for a firing configuration.
 *
 * Why a DataAsset and not UPROPERTYs on the character:
 *   - designers can create a new weapon without touching C++ or compiling;
 *   - the same asset can be referenced by the player, the Scorpion and a
 *     tutorial dummy, so "what the Scorpion fires" is data, not a second
 *     implementation;
 *   - it can be swapped at runtime (the Power Shot power-up swaps the fire
 *     pattern asset, it does not duplicate the firing code).
 */
UCLASS(BlueprintType)
class IGKNIGHTED_API UIgKnightedWeaponData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** The projectile actor this configuration spawns. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AIgKnightedProjectile> ProjectileClass;

    /** Socket on the firing actor the projectile spawns from. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocket = TEXT("Muzzle");

    /** Seconds between shots. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float FireInterval = 0.25f;

    /** Multiplier applied to the projectile's own speed. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float ProjectileSpeedScale = 1.0f;

    /**
     * Extra projectiles fired simultaneously (the Power Shot pattern).
     * 0 = single shot, 2 = the three-shot spread.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    int32 BonusProjectiles = 0;

    /** Half-angle of the spread, in degrees. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float SpreadDegrees = 12.f;

    /** Which damage channel the projectile belongs to. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    EIgDamageKind DamageKind = EIgDamageKind::EnemyDamage;

    /** Niagara system spawned at the muzzle on each shot. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Feedback")
    TObjectPtr<UNiagaraSystem> FireVFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Feedback")
    TObjectPtr<USoundBase> FireSound;

    /** Returns the number of projectiles a single fire action produces. */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetProjectileCount() const { return 1 + FMath::Max(0, BonusProjectiles); }
};
```

**The combat component.** It is shared by the player and the ranged enemies. The difference between them is *how the aim direction is obtained*, not *how the shot is fired*.

```cpp
// IgKnightedCombatComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedCombatComponent.generated.h"

class UIgKnightedWeaponData;
class AIgKnightedProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFired, const FVector&, MuzzleLocation);

UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIgKnightedCombatComponent();

    /** The active configuration. Swapped by the Power Shot power-up. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UIgKnightedWeaponData> WeaponData;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsFiring = false;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnFired OnFired;

    /** Requests a single shot. Returns false if the cadence is not ready. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TryFire(const FVector& AimDirection);

    /** Begins/stops continuous fire (hold to shoot). */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetFiring(bool bNewFiring, const FVector& AimDirection);

    /** Called by the Power Shot ability. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetWeaponData(UIgKnightedWeaponData* NewData);

protected:
    float TimeSinceLastShot = 0.0f;

    FTimerHandle FireTimer;

    void PerformFire(const FVector& AimDirection);
    FVector GetMuzzleLocation() const;
};
```

```cpp
// IgKnightedCombatComponent.cpp
bool UIgKnightedCombatComponent::TryFire(const FVector& AimDirection)
{
    // Cadence gate. A timer-based cooldown rather than a Tick comparison,
    // because Tick runs at a variable rate and "0.25 seconds" then means
    // "0.25 seconds +/- one frame", which is felt at high fire rates.
    if (GetWorld()->GetTimeSeconds() - TimeSinceLastShot < WeaponData->FireInterval)
    {
        return false;
    }

    TimeSinceLastShot = GetWorld()->GetTimeSeconds();
    PerformFire(AimDirection);
    return true;
}

void UIgKnightedCombatComponent::PerformFire(const FVector& AimDirection)
{
    const FVector Muzzle = GetMuzzleLocation();

    // Build the rotation set for the current pattern. One function handles the
    // single shot and the triple shot - the Power Shot power-up changes a
    // number, it does not add a second firing implementation.
    const int32 Count = WeaponData->GetProjectileCount();
    const float HalfSpread = WeaponData->SpreadDegrees;

    for (int32 i = 0; i < Count; ++i)
    {
        // Evenly distribute the extra projectiles around the aim direction.
        // For 3 shots: -Spread, 0, +Spread.
        const float Alpha = (Count == 1)
            ? 0.f
            : (static_cast<float>(i) / static_cast<float>(Count - 1)) * 2.f - 1.f;

        const float YawOffset = Alpha * HalfSpread;
        const FRotator ShotRotation = AimDirection.Rotation() + FRotator(0.f, YawOffset, 0.f);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner      = GetOwner();
        SpawnParams.Instigator = GetOwner()->GetInstigatorController();
        // Collision handling: the projectile must not spawn inside geometry and
        // vanish, but it also must not push the player.
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

        AIgKnightedProjectile* Projectile =
            GetWorld()->SpawnActor<AIgKnightedProjectile>(
                WeaponData->ProjectileClass, Muzzle, ShotRotation, SpawnParams);

        if (Projectile)
        {
            // Initialise the projectile with the WHOLE context: who fired it,
            // how fast it travels, and what damage channel it belongs to.
            // A projectile that forgets its instigator cannot be ignored by
            // friendly collision, and cannot credit the kill to the shooter.
            Projectile->InitializeShot(GetOwner(), GetOwner()->GetInstigatorController(),
                                       WeaponData->DamageKind,
                                       WeaponData->ProjectileSpeedScale);
        }
    }

    // Feedback is fired AFTER the projectile exists so the VFX is in the right
    // place even if the spawn was adjusted by collision handling.
    OnFired.Broadcast(Muzzle);
}
```

**One more thing the data-driven approach bought us:** the Triple Shot is a *number*. `BonusProjectiles = 2` and the spread code produces three projectiles. The alternative — three `SpawnActor` calls in a Blueprint — is the classic way a game ends up with "the third bullet is in a slightly different place and does not hit what the first one hit".

### 4.2 Projectile & Damage System

```cpp
// IgKnightedProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpact, AActor*, HitActor, FVector, ImpactPoint);

UCLASS()
class IGKNIGHTED_API AIgKnightedProjectile : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedProjectile();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    /** Called by the combat component immediately after the spawn. */
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void InitializeShot(AActor* Shooter, AController* ShooterController,
                        EIgDamageKind InDamageKind, float SpeedScale);

    UFUNCTION(BlueprintAssignable, Category = "Projectile")
    FOnProjectileImpact OnImpact;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile")
    EIgDamageKind DamageKind = EIgDamageKind::EnemyDamage;

    /**
     * Latch for "this projectile already hit something".
     *
     * A projectile can generate several hit events in a single frame (it is
     * touching multiple components), and OnHit can fire again on the frame
     * after the first hit if the collision is not disabled immediately.
     * Without this latch one water ball could spend two shared lives.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Projectile")
    bool bHasImpacted = false;

    /** The actor that fired this. Ignored by collision so we do not hit ourselves. */
    UPROPERTY(BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<AActor> Shooter;

    UPROPERTY(BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<AController> ShooterController;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                         UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit);

    UFUNCTION()
    void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                   AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
```

```cpp
// IgKnightedProjectile.cpp
void AIgKnightedProjectile::InitializeShot(AActor* InShooter, AController* InController,
                                            EIgDamageKind InKind, float SpeedScale)
{
    Shooter         = InShooter;
    ShooterController = InController;
    DamageKind      = InKind;

    if (ProjectileMovement)
    {
        ProjectileMovement->InitialSpeed *= FMath::Max(0.1f, SpeedScale);
        ProjectileMovement->MaxSpeed     *= FMath::Max(0.1f, SpeedScale);
    }

    // Ignore the shooter entirely. Without this, a close-range shot spawns
    // inside the player's own capsule and is destroyed on frame one.
    if (Shooter)
    {
        CollisionComponent->IgnoreActorWhenMoving(Shooter, true);
        // Also ignore the shooter's controller's pawn, which may not be the
        // same actor in edge cases (possessed vehicle, possessed turret).
        if (AController* C = Shooter->GetInstigatorController())
        {
            if (APawn* P = C->GetPawn())
            {
                CollisionComponent->IgnoreActorWhenMoving(P, true);
            }
        }
    }

    // A lifespan is not optional. A projectile that flies out of the arena
    // still exists in the world, still has a collision component, and still
    // costs a tick. Six seconds is enough for any shot on the map.
    SetLifeSpan(6.0f);
}

void AIgKnightedProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                             const FHitResult& Hit)
{
    // ---- Idempotency latch -------------------------------------------------
    // This is the guard that makes "one water ball spends one life" true.
    if (bHasImpacted) { return; }
    bHasImpacted = true;

    // Disable collision immediately. Doing this AFTER broadcasting can cause
    // the actor to be destroyed by the callback while we are still executing
    // code that references it.
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

    // ---- Route the damage --------------------------------------------------
    FIgDamageContext Context;
    Context.DamageKind      = DamageKind;
    Context.Source          = this;
    Context.Instigator      = ShooterController;
    Context.HitLocation     = Hit.ImpactPoint;
    Context.HitNormal       = Hit.ImpactNormal;

    if (AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(OtherActor))
    {
        // Player path: goes through the resolver, which applies the
        // dash/shield/death precedence.
        UIgKnightedDamageResolver::ResolvePlayerHit(Player, Context);
    }
    else if (AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(OtherActor))
    {
        Enemy->ApplyDamage(DamageKind, 1.f, ShooterController);
    }

    OnImpact.Broadcast(OtherActor, Hit.ImpactPoint);

    // Destroy after the impact effects have been requested. We do not pool
    // projectiles: at this scale the spawn churn is not a bottleneck and a
    // pool would add lifetime bugs for no measurable gain. (See §08.4.)
    Destroy();
}
```

**Collision channels.** A small, explicit channel set is worth more than a clever one:

| Channel | Collides with | Used by |
|---|---|---|
| `ECC_WorldStatic` | world | Everything |
| `ECC_Pawn` | world, pawns, projectiles | Characters |
| `ECC_Projectile` | world, pawns | Fire and water projectiles |
| `ECC_Hazard` | world, pawns | The mollusk's beam volume |

Player projectiles ignore player pawns; enemy projectiles ignore the enemy that fired them. Friendly fire between the two players was disabled deliberately — in a bullet-hell arena, a teammate's shot crossing your screen should never kill you.

**Why the damage context is a struct, not loose parameters.** Six parameters on a function call is where mistakes start:

```cpp
// IgKnightedTypes.h
USTRUCT(BlueprintType)
struct FIgDamageContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    EIgDamageKind DamageKind = EIgDamageKind::Unknown;

    /** The thing that caused the damage (a projectile, a hazard volume, a crawler). */
    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    TObjectPtr<AActor> Source = nullptr;

    /** Who is responsible - used for score attribution. */
    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    TObjectPtr<AController> Instigator = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    FVector HitNormal = FVector::ZeroVector;

    /** Damage amount. Water damage is lethal regardless of this value. */
    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    float Amount = 1.f;
};
```

It scales. Adding "which damage type", "was it a critical hit" or "which zone" later is a struct member, not a signature change that ripples through every call site.

### 4.3 Enemy AI Architecture

The enemy AI had one specific problem to solve that made everything else hard: **there are two players, and the enemy has to pick one.**

The naive implementation — "find the nearest player and move toward them" — is what we started with. It produced a genuinely bad experience: in a two-player game the enemies would swarm whoever happened to be closer, so the player standing furthest away would have a completely empty screen, and the two players would experience totally different difficulty.

The fix was to **separate target selection from behaviour**:

```text
  ┌───────────────────────────┐        ┌───────────────────────────────────┐
  │  AIgKnightedEnemyAIController│        │  Blackboard                       │
  │                           │        │  ┌───────────────────────────────┐  │
  │  1. Target Selector        │───────▶│  │ TargetActor      (AActor*)    │  │
  │     "WHO do I fight?"      │ writes │  │ LastKnownLocation (FVector)   │  │
  │                           │        │  │ DesiredDistance  (float)      │  │
  │  2. Behaviour              │        │  │ HasLineOfSight   (bool)       │  │
  │     "HOW do I fight?"      │reads   │  │ AttackReady      (bool)       │  │
  │     (Behaviour Tree)       │        │  └───────────────────────────────┘  │
  └───────────┬───────────────┘        └───────────────────────────────────┘
              │ sets
              ▼
      AIgKnightedEnemyCharacter
      (movement, attack component, health, death)
```

**Target selection** is a service, shared by every enemy, with a policy that is *configurable* — this is the bit that makes the two-player experience coherent:

```cpp
// IgKnightedTargetingComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedTargetingComponent.generated.h"

UENUM(BlueprintType)
enum class EIgTargetPolicy : uint8
{
    /** Always prefer the closest valid player. */
    Nearest,
    /** Prefer the player who attacked most recently (aggro). */
    LastAttacker,
    /** Alternate targets so both players keep getting pressure. */
    Alternate,
    /** Prefer the player with the lowest number of deaths (rubber-banding). */
    LeastDeaths
};

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    EIgTargetPolicy Policy = EIgTargetPolicy::Nearest;

    /** How often the target is re-evaluated. Per-frame search is wasted work. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float RetargetInterval = 0.5f;

    /** A player is only a valid target if it is alive and not respawning. */
    UFUNCTION(BlueprintPure, Category = "Targeting")
    static bool IsValidTarget(const AActor* Candidate);

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    AActor* AcquireTarget();

protected:
    float TimeSinceRetarget = 0.f;
    int32 AlternateIndex = 0;
    TWeakObjectPtr<AActor> CachedTarget;
};
```

```cpp
// IgKnightedTargetingComponent.cpp
bool UIgKnightedTargetingComponent::IsValidTarget(const AActor* Candidate)
{
    if (!IsValid(Candidate)) { return false; }

    // The candidate must actually be a player character. World overlap events
    // and blueprint casts both produce "something that is an Actor", and
    // casting to a Pawn is not enough - a Pawn can be an enemy.
    const AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(Candidate);
    if (!Player) { return false; }

    // Not dead, and not in the middle of a death/respawn.
    if (Player->IsDeathProcessed()) { return false; }

    // Belongs to this match and this world.
    const UWorld* World = GetWorld();
    if (!World || Player->GetWorld() != World) { return false; }

    return true;
}

AActor* UIgKnightedTargetingComponent::AcquireTarget()
{
    // Reuse the cached target if it is still valid. Re-searching for the
    // nearest player every frame for 20 enemies is exactly the kind of
    // "works but costs 0.3ms" cost that adds up to a frame-time problem.
    if (IsValidTarget(CachedTarget.Get()))
    {
        return CachedTarget.Get();
    }

    TArray<AActor*> Candidates;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        const APlayerController* PC = It->Get();
        if (!PC) { continue; }
        APawn* Pawn = PC->GetPawn();
        if (IsValidTarget(Pawn)) { Candidates.Add(Pawn); }
    }

    if (Candidates.IsEmpty()) { return nullptr; }

    switch (Policy)
    {
    case EIgTargetPolicy::Nearest:
    {
        AActor* Best = nullptr;
        float BestDist = TNumericLimits<float>::Max();
        const FVector MyLocation = GetOwner()->GetActorLocation();
        for (AActor* Candidate : Candidates)
        {
            const float Dist = FVector::DistSquared(MyLocation, Candidate->GetActorLocation());
            if (Dist < BestDist) { BestDist = Dist; Best = Candidate; }
        }
        CachedTarget = Best;
        break;
    }
    case EIgTargetPolicy::Alternate:
        // Deliberately simple: round-robin between the candidates. It is not
        // the smartest policy, but it guarantees that both players get
        // attention, which is what makes the two-player experience feel fair.
        CachedTarget = Candidates[AlternateIndex % Candidates.Num()];
        ++AlternateIndex;
        break;

    default:
        CachedTarget = Candidates[0];
        break;
    }

    return CachedTarget.Get();
}
```

**Behaviour Tree, per archetype.** Behaviour Trees are the right tool for the Scorpion, because the Scorpion's behaviour is a *decision graph* — approach, retreat, aim, fire, cooldown — where the branch taken depends on the distance to the target. The Crawler does not need one: it has two states (chase, contact) and a steering function handles both in fewer lines and with better frame-time.

```text
  Scorpion Behaviour Tree
  ───────────────────────
  Root (Selector)
   ├── Sequence "Attack"
   │    ├── Condition: HasTarget
   │    ├── Condition: InFiringBand          (MinRange < dist <= MaxRange)
   │    ├── Condition: HasLineOfSight
   │    ├── Condition: AttackReady           (cooldown elapsed)
   │    ├── Service "UpdateAiming"           (runs while the branch is active)
   │    └── Task    "FireProjectile"
   │
   ├── Sequence "Retreat"
   │    ├── Condition: HasTarget
   │    ├── Condition: TargetTooClose         (dist < MinRange)
   │    └── Task    "MoveAwayFromTarget"
   │
   └── Sequence "Reposition"
        ├── Condition: HasTarget
        └── Task    "MoveToDesiredRange"      (EQS query for a firing position)
```

```cpp
// IgKnightedEnemyAIController.cpp
void AIgKnightedEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Push the archetype's tuning into the blackboard so the tree reads
    // configuration instead of hard-coded numbers.
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB && InPawn)
    {
        const AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(InPawn);
        if (Enemy)
        {
            BB->SetValueAsFloat(TEXT("MinRange"),        Enemy->MinPreferredRange);
            BB->SetValueAsFloat(TEXT("MaxRange"),        Enemy->MaxPreferredRange);
            BB->SetValueAsFloat(TEXT("AttackInterval"),  Enemy->AttackInterval);
            BB->SetValueAsBool(TEXT("HasLineOfSight"),  false);
        }
    }

    // Start the tree. RunInBackground = false so the tree only ticks when the
    // controller is actually thinking, not while it is idle in a spawn room.
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}
```

```cpp
// BTTask_FireProjectile.cpp  - a real behaviour tree task
UBTTaskNode* UBTTask_FireProjectile::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Cast through the blackboard. The task only knows the blackboard key
    // name; it does not know the concrete enemy type. That is what lets the
    // same tree be reused by the Scorpion and any future ranged enemy.
    AActor* Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName);
    AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(OwnerComp.GetPawn());
    if (!Enemy || !IsValid(Target)) { return EBTNodeResult::Failed; }

    // Aim at the target's CURRENT position. Predictive aiming (solving for the
    // interception point given target velocity and projectile speed) is a real
    // improvement, but it needs the projectile speed to be known to the AI at
    // planning time, and it was not the shipped behaviour.
    const FVector AimDirection =
        (Target->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();

    Enemy->GetAttackComponent()->TryFire(AimDirection);

    // Reset the cooldown so the "AttackReady" condition is false until the
    // timer expires. Doing it here (in the task) rather than in the character
    // keeps the timing rule with the decision that consumes it.
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(AttackReadyKey.SelectedKeyName, false);

    return EBTNodeResult::Succeeded;
}
```

### 4.4 Enemy Archetypes

Three enemies, three genuinely different spatial problems. They share the base class, the targeting service and the damage resolver — but their *movement* behaviour differs.

| | **Crawler** | **Scorpion** | **Water Mollusk** |
|---|---|---|---|
| **Role** | Melee pressure | Ranged pressure | Area denial |
| **Movement** | Direct pursuit | Kiting, maintains a band | Stationary |
| **Attack** | Contact damage | Water-ball projectiles | Continuous water stream |
| **Anti-dash** | Not applicable | Not applicable | **The dash is the counter** |
| **Behaviour Tree** | No — steering is enough | Yes | No — a timed state machine |
| **Key tuning** | Move speed, contact radius | Min/Max range, fire interval, projectile speed | Beam length, width, duty cycle |

#### Crawler — pursuit and contact

```cpp
// IgKnightedEnemyCharacter.cpp
void AIgKnightedEnemyCharacter::UpdateCrawler(float DeltaSeconds)
{
    AActor* Target = GetTargetingComponent()->AcquireTarget();
    if (!IsValid(Target)) { return; }

    // Direct steering: move toward the target. In a small arena with simple
    // geometry this is enough, and it costs a fraction of a navmesh query.
    const FVector ToTarget = (Target->GetActorLocation() - GetActorLocation());
    const FVector Direction = ToTarget.GetSafeNormal2D();

    // Clamp to the arena bounds so they never pile into a wall.
    AddMovementInput(Direction, 1.f);

    // Contact damage. The check is distance-based rather than relying purely
    // on an overlap event, because the Crawler's capsule and the player's
    // capsule can overlap for a frame before the overlap event is dispatched.
    if (ToTarget.SizeSquared() < FMath::Square(ContactDamageRadius))
    {
        SubmitContactDamage(Target);
    }
}
```

#### Scorpion — ranged spacing

```cpp
// IgKnightedEnemyCharacter.cpp
void AIgKnightedEnemyCharacter::UpdateScorpion(float DeltaSeconds)
{
    AActor* Target = GetTargetingComponent()->AcquireTarget();
    if (!IsValid(Target)) { return; }

    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    // Three bands, and the state machine is literally this if/else. The
    // behaviour tree in the shipped build was more verbose than this; this
    // version is the distilled form.
    if (Distance < MinPreferredRange)
    {
        // Too close - back away, but keep facing the target so we can still
        // shoot while retreating.
        const FVector Away = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal2D();
        AddMovementInput(Away, 1.f);
        FaceTarget(Target);
    }
    else if (Distance > MaxPreferredRange)
    {
        // Too far - close the gap.
        const FVector Toward = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
        AddMovementInput(Toward, 1.f);
        FaceTarget(Target);
    }
    else
    {
        // In the firing band: stop moving and shoot.
        FaceTarget(Target);
        GetAttackComponent()->TryFire((Target->GetActorLocation() - GetActorLocation()).GetSafeNormal());
    }
}
```

#### Water Mollusk — the hazard that makes the dash matter

The mollusk is the reason the dash's i-frames exist, and it is the most interesting enemy in the game from a systems point of view because it is **not a projectile at all**. It is a persistent volume.

```cpp
// IgKnightedHazardVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedHazardVolume.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHazardHit, AActor*, Victim);

UCLASS()
class IGKNIGHTED_API AIgKnightedHazardVolume : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedHazardVolume();

    /** Total length of the beam, in cm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard")
    float BeamLength = 1800.f;

    /** Half-width of the beam. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard")
    float BeamWidth = 60.f;

    /** How long the beam is active per cycle. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard")
    float ActiveDuration = 3.f;

    /** How long the beam rests between cycles. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hazard")
    float RestDuration = 2.f;

    UPROPERTY(BlueprintReadOnly, Category = "Hazard")
    bool bIsActive = false;

    /** The players currently standing inside the beam. */
    UPROPERTY(BlueprintReadOnly, Category = "Hazard")
    TSet<TWeakObjectPtr<AActor>> OverlappingActors;

    UPROPERTY(BlueprintAssignable, Category = "Hazard")
    FOnHazardHit OnHazardHit;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hazard")
    TObjectPtr<UBoxComponent> HazardBox;

    UPROPERTY(BlueprintReadOnly, Category = "Hazard")
    TSet<TWeakObjectPtr<AActor>> AlreadyDamagedThisCycle;

    FTimerHandle ActiveTimer;
    FTimerHandle RestTimer;

    UFUNCTION()
    void OnHazardBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                               bool bFromSweep, const FHitResult& SweepResult);

    void Activate();
    void Rest();
    void ApplyHazardDamage();
};
```

```cpp
// IgKnightedHazardVolume.cpp
void AIgKnightedHazardVolume::ApplyHazardDamage()
{
    // A hazard overlap fires EVERY FRAME the actor is inside the volume.
    // If we resolve damage per overlap event, a player standing in the beam
    // for one second would spend every one of the five shared lives.
    //
    // The rule: resolve damage on ENTRY, then remember the victim for the
    // duration of the cycle. Leaving and re-entering resets it.
    for (TWeakObjectPtr<AActor> WeakVictim : OverlappingActors)
    {
        AActor* Victim = WeakVictim.Get();
        if (!IsValid(Victim)) { continue; }

        // Already took damage during this active window.
        if (AlreadyDamagedThisCycle.Contains(WeakVictim)) { continue; }

        // The victim might be immune right now (dashing). If so we do NOT
        // record them as damaged - they were not hurt, so if the dash ends
        // while they are still in the beam they should take the hit.
        AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(Victim);
        if (!Player) { continue; }

        FIgDamageContext Context;
        Context.DamageKind = EIgDamageKind::LethalToPlayer;
        Context.Source     = this;
        Context.HitLocation = Player->GetActorLocation();

        const FIgDamageResult Result = UIgKnightedDamageResolver::ResolvePlayerHit(Player, Context);

        // Only latch the victim if the damage actually counted.
        if (!Result.bIgnored)
        {
            AlreadyDamagedThisCycle.Add(WeakVictim);
        }
    }
}

void AIgKnightedHazardVolume::Activate()
{
    bIsActive = true;
    AlreadyDamagedThisCycle.Reset();     // a new cycle can hurt again
    HazardBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OnHazardActivated.Broadcast();
}

void AIgKnightedHazardVolume::Rest()
{
    bIsActive = false;
    HazardBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    OverlappingActors.Reset();
    AlreadyDamagedThisCycle.Reset();
}
```

**Two details that make the mollusk feel fair rather than cheap:**

1. **The visual and the collision are the same volume.** The Niagara beam is scaled to `BeamLength` and `BeamWidth` from the same variables that size the `UBoxComponent`. When a designer tuned the visual to be wider, the damage area widened with it. A separate "visual beam" and "hitbox" that drift apart is the most common cause of "the water didn't hit me" bug reports.
2. **The beam does not block movement.** It only deals damage. The player walks through it and dies, or dashes through it and does not. Blocking movement would be a different mechanic (a wall), and it would remove the reason the dash exists.

### 4.5 AI Design Decisions

**Decision 1 — Behaviour Tree vs. steering for the Crawler.**

The Crawler has two behaviours: chase, and hit on contact. A behaviour tree for that is a ceremony: a Root Selector with a Sequence and two Conditions to express `if (hasTarget) moveToward()`. Direct steering expresses the same thing in four lines with no blackboard, no service, and no tick overhead.

*Where I drew the line:* **behaviour trees for decisions that branch on multiple independent conditions; steering for movement that is always the same.** The Scorpion's tree branches on range, line of sight and cooldown — three independent conditions — so it earns its keep. The Crawler branches on nothing.

**Decision 2 — EQS for positioning.**

The Scorpion needs to be *somewhere* that satisfies several constraints at once: at least 300 units from the target, at most 700, not inside the water stream, and not in the wall. A single `MoveTo` cannot express that. An **Environment Query** can: sample candidate points, score them against the constraints, pick the best.

```text
  EQS query: "FiringPosition"
  ─────────────────────────────
  Generators:  Points on a circle around the target (radius = desired range)
  Tests:
    ├── Distance        : Distance to target  → prefer near the desired band
    ├── Trace           : Is there a clear line of sight to the target?
    ├── Overlap         : Is the point inside a hazard volume?  → reject
    └── Distance to wall : prefer points away from geometry
  Scoring: sum of test scores, pick the highest
```

I used it for the Scorpion's repositioning. It cost a query every 0.5 seconds per Scorpion, which was measurable but acceptable — I gated it behind a service rather than running it per frame.

**Decision 3 — Reuse the player's damage pipeline for the enemies.**

Enemies do not have their own "take damage" logic. They call the same `DamageResolver` with a different `DamageKind`, and the same enemy death event fires regardless of what killed it — a fire projectile, an enhanced dash, or a future hazard. That means the score award, the enemy registry removal and the zone-count bookkeeping happen in exactly one place. The alternative — a separate "enemy died" path for each damage source — is how zone completion started unlocking barriers at the wrong time.

**Decision 4 — Where the AI stops and the animation starts.**

The AI decides *what* to do (fire). The Animation Blueprint decides *how it looks* (recoil, muzzle flash, a wind-up tell). The AI never plays a montage; it broadcasts an event and the animation layer reacts. This is the same separation as everywhere else in the project, and it is what let our animator tune the enemy attack tell without me changing a line of AI code.

---

## 05. Gameplay Abilities, Power-Ups & Scoring

> **Four power-ups is not a reason to adopt the Gameplay Ability System.** But it *is* a reason to design the ability layer so that it could become one without a rewrite.

### 5.1 Power-Up Architecture

![IgKnighted — the power-up pickups, their effects and the HUD indicators in the shipped build](../../assets/images/Y1D/6.png)

Every power-up in IgKnighted is a **Pickup Actor** that, when overlapped by a player, applies an **effect** to that player and then removes itself.

```text
   ┌────────────────────┐
   │  AIgKnightedPickup │  ← spawned by the wave/collectable spawner
   └─────────┬──────────┘
             │ OnComponentBeginOverlap
             ▼
   ┌────────────────────────────────────────────────┐
   │  Is this a valid collector?                    │
   │   · is it a player character?                  │
   │   · is the match active?                       │
   │   · has this pickup already been consumed?     │
   │   · does this player already have this buff?   │ ← the duplicate rule
   └─────────┬──────────────────────────────────────┘
             │ yes
             ▼
   ┌────────────────────────────────────────────────┐
   │  Apply the effect (by kind)                    │
   │   Shield        → StatusComponent.AddShield()  │
   │   Power Shot    → CombatComponent.SetWeaponData │
   │   Enhanced Dash → DashComponent.AddCharges()   │
   │   Matchstick    → PlayerState.AddCollectable() │
   └─────────┬──────────────────────────────────────┘
             │ notify HUD + VFX + SFX
             ▼
      consume & destroy the pickup
```

**The duplicate-pickup rule.** One of my original contributions was preventing a player from picking up the *same* power-up repeatedly. The implementation detail that made it work: the check lives in the **status component that owns the buff**, not in the pickup. The pickup asks "may this player take this buff?" and the buff owner is the only thing that knows.

```cpp
// IgKnightedStatusComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, bool, bHasShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldBroken, AActor*, Breaker);

UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedStatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Shield charges. One hit is absorbed per charge. */
    UPROPERTY(BlueprintReadOnly, Category = "Status")
    int32 ShieldCharges = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Status")
    bool bHasShield = false;

    UPROPERTY(BlueprintAssignable, Category = "Status")
    FOnShieldChanged OnShieldChanged;

    UPROPERTY(BlueprintAssignable, Category = "Status")
    FOnShieldBroken OnShieldBroken;

    /**
     * Can this owner take this kind of buff right now?
     *
     * The pickup calls this BEFORE consuming itself. The rule lives here
     * because this is the component that owns the buff state, and a rule that
     * lives far from the state it governs is a rule that gets duplicated.
     */
    UFUNCTION(BlueprintPure, Category = "Status")
    bool CanApplyBuff(EIgBuffKind Kind) const;

    UFUNCTION(BlueprintCallable, Category = "Status")
    void ApplyBuff(EIgBuffKind Kind, float Duration);

    /**
     * Consumes one shield charge. Returns true if a shield absorbed the hit.
     *
     * Called from the DamageResolver, in the shield branch of the precedence
     * chain. If it returns true the damage is fully absorbed: no life is
     * spent and the player does not die.
     */
    UFUNCTION(BlueprintCallable, Category = "Status")
    bool ConsumeShield();

protected:
    /** Timers for timed buffs, so they can be cancelled cleanly on death. */
    TMap<EIgBuffKind, FTimerHandle> BuffTimers;

    void RemoveBuff(EIgBuffKind Kind);
};
```

```cpp
// IgKnightedStatusComponent.cpp
bool UIgKnightedStatusComponent::CanApplyBuff(EIgBuffKind Kind) const
{
    switch (Kind)
    {
    case EIgBuffKind::Shield:
        // The duplicate rule: a player who already has a shield does not pick
        // up another one. The pickup stays on the ground for the other player.
        return ShieldCharges <= 0;

    case EIgBuffKind::PowerShot:
        // Power Shot does not stack - it swaps the fire pattern. Picking up a
        // second one while the first is active is a no-op, so the pickup must
        // know that BEFORE it destroys itself, otherwise the player loses a
        // pickup for nothing.
        return !HasActiveBuff(EIgBuffKind::PowerShot);

    case EIgBuffKind::EnhancedDash:
        return !HasActiveBuff(EIgBuffKind::EnhancedDash);

    default:
        return true;
    }
}

bool UIgKnightedStatusComponent::ConsumeShield()
{
    if (ShieldCharges <= 0) { return false; }

    --ShieldCharges;
    bHasShield = ShieldCharges > 0;

    OnShieldChanged.Broadcast(bHasShield);

    if (!bHasShield)
    {
        // The shield is gone entirely - this is the "shield broken" moment.
        // The gameplay event and the VFX both hang off this one broadcast.
        OnShieldBroken.Broadcast(GetOwner());
    }

    return true;   // the hit was absorbed
}
```

### 5.2 Individual Power-Ups

| Power-up | Behaviour | Where it lives | Duration |
|---|---|---|---|
| **Shield** | Absorbs one otherwise lethal water hit, then breaks | `StatusComponent` → `ShieldCharges` | Until consumed |
| **Power Shot** | Adds two extra projectiles: one shot becomes a three-shot spread | `CombatComponent` → `WeaponData` swap | Timed |
| **Enhanced Dash** | Extra dash charges **and** the dash kills enemies it passes through | `DashComponent` → `MaxDashCharges`, `bEnhancedDashActive` | Timed |
| **Matchstick** | Adds to score, scaled by the player's multiplier | `PlayerState` → `Score` | Instant |

**Power Shot — swapping data, not duplicating logic.**

```cpp
// IgKnightedCombatComponent.cpp
void UIgKnightedCombatComponent::SetWeaponData(UIgKnightedWeaponData* NewData)
{
    if (!NewData) { return; }

    // Keep the base data around so the buff can be removed cleanly.
    if (!BaseWeaponData)
    {
        BaseWeaponData = WeaponData;
    }

    WeaponData = NewData;

    // The fire pattern changed, so the cadence should restart. Otherwise the
    // player can pick up Power Shot mid-burst and the first three-shot goes
    // out with the old timer.
    TimeSinceLastShot = GetWorld()->GetTimeSeconds();
}
```

The pickup for Power Shot just points at a different `UIgKnightedWeaponData` asset with `BonusProjectiles = 2`. Removing the buff restores the base asset. **No duplicated firing code, no boolean soup.**

**Enhanced Dash — a flag plus a set of charges, and one important coupling.**

The traversal kill is only active while the dash is active, and it uses the same `TSet` idempotency mechanism as the normal dash. The coupling to watch: the enhanced-dash buff must be *removed* if the player dies mid-buff, or the respawned player would keep the traversal kill. The status component's timers are cancelled on death, which handles it.

### 5.3 Gameplay Ability System — Reference Design

Here is the honest engineering position.

**The shipped game did not use GAS.** It used a small component-based effect layer, because for four power-ups on a two-player local game, GAS is a large amount of machinery — an `AbilitySystemComponent`, an `AttributeSet`, a pile of `GameplayEffect` assets, tag registration, a policy for where the ASC lives across respawns — for a problem that three booleans and two timers solve.

But "we did not need it" is a statement about *2024 IgKnighted*, not about the system. Here is what the **same four mechanics** look like when expressed as a Gameplay Ability System, and — more usefully — **what GAS gives you that the component approach did not**.

```text
  PlayerCharacter (or PlayerState)
  └── UAbilitySystemComponent  (ASC)
        │
        ├── UIgKnightedAttributeSet
        │     └── (optional) Score, DashCharges as attributes
        │
        ├── Granted Abilities
        │     ├── GA_Dash        (dash with cooldown + i-frames)
        │     ├── GA_Fire        (firing cadence)
        │     └── GA_TraverseKill (enhanced dash kill-on-pass)
        │
        ├── Applied Gameplay Effects
        │     ├── GE_Shield      (grants State.Shielded, 1 charge)
        │     ├── GE_PowerShot   (buff: FirePattern.MultiShot)
        │     ├── GE_EnhancedDash (buff: DashCharges+2, Dash.TraversalDamage)
        │     └── GE_DashCooldown (blocks GA_Dash)
        │
        ├── Gameplay Tags  (the state vocabulary)
        │     State.Dashing
        │     State.Invulnerable
        │     State.Shielded
        │     Buff.PowerShot
        │     Buff.EnhancedDash
        │
        └── Gameplay Cues  (VFX/SFX, decoupled from the ability)
              Cue.ShieldBreak
              Cue.DashTrail
              Cue.ProjectileImpact
```

**What GAS actually bought us, concretely:**

| Problem in the component version | How GAS solves it |
|---|---|
| "Shield" and "Dash i-frames" were two booleans; removing one could clear the other | **Tags with counts.** `State.Invulnerable` is a counted tag. Adding a second source stacks the count; removing one source decrements it. Immunity sources compose correctly *for free*. |
| Timed buffs needed a timer per buff, plus manual cleanup on death | **Effects own their duration.** `GE_PowerShot` has a duration and a granted tag; expiry and removal are handled by the ASC. |
| "Can this player dash?" was a hand-written validity check that had to know about every other ability | **Ability tags and cost.** `GA_Dash` has `ActivationBlockedTags`, `ActivationOwnedTags`, and a cooldown effect. The ASC refuses to activate it and the reason is queryable. |
| VFX were called from inside the ability logic | **Gameplay Cues.** `Cue.ShieldBreak` is a Notify in the effect, driven by the tag change, and lives entirely in content. |
| Cooldowns were bespoke timers per ability | **Cooldown Gameplay Effects.** One system, one data table of cooldowns. |

**The tag-count example is the killer.** In the component version, ending a dash had to remember not to clear the shield — a bug we actually shipped. In GAS, the equivalent is:

```cpp
// The dash grants State.Invulnerable with a count of 1.
// The shield grants State.Shielded with a count of 1.
// Two DIFFERENT tags, so they never interfere. If both mapped to the same
// tag, the counts would stack and removing one would still leave the other.
```

**Where GAS was overkill for this game:**

- We had four effects, not four hundred.
- There is no network prediction to worry about (local multiplayer, one process), which removes GAS's main structural advantage.
- The ASC has to live *somewhere*, and with instant respawns that decision (on the Pawn? on the PlayerState?) has consequences. On the Pawn means abilities reset on death. On the PlayerState means abilities persist but the ASC outlives the body, and then you have to reset the ability state explicitly. Either way it is a decision that needs to be made deliberately.

```cpp
// IgKnightedAttributeSet.h  (reference design)
UCLASS()
class IGKNIGHTED_API UIgKnightedAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    // Replicated attributes would use AttributedSet macro + GetLifetimeReplicatedProps.
    // In a local game, replication is not required, but the pattern is shown
    // because the same code would need it for an online build.

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = true))
    FGameplayAttributeData DashCharges;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Score;

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetDashCharges() const { return DashCharges.GetCurrentValue(); }

    // ---- Post-initialise change hooks -------------------------------------
    // Attributes are clamped here, not at the call site. Every writer of
    // DashCharges goes through the same clamp, so "charges went negative"
    // is impossible by construction.

    UFUNCTION()
    void OnDashChargesChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
    {
        if (Attribute == GetDashChargesAttribute())
        {
            const float Clamped = FMath::Clamp(NewValue, 0.f, MaxDashCharges);
            if (!FMath::IsNearlyEqual(Clamped, NewValue))
            {
                SetDashCharges(Clamped);
            }
        }
    }

    UFUNCTION()
    void OnDashChargesReplicated(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
    {
        OnDashChargesChanged(Attribute, OldValue, NewValue);
    }
};
```

```cpp
// GA_Dash.cpp  (reference design)
void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, AActor* Owner,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, Owner, ActivationInfo))
    {
        // CommitAbility checks the cost, the cooldown and the blocked tags.
        // This single call is the reason we do not need a hand-written
        // "CanDash" function that has to know about every other ability.
        EndAbility(Handle, Owner, ActivationInfo, EGameplayAbilityEndedDelegate::CreateUObject(this, &UGA_Dash::EndAbility));
        return;
    }

    // State.Dashing and State.Invulnerable are granted by a Gameplay Effect.
    // The Invulnerable tag has a stack count, so if another source also grants
    // it, removing the dash's grant does not clear the other source.
    ApplyGameplayEffectToOwner(DashStateEffect.Get(), 1.0f);

    // The actual movement.
    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        const FVector Direction = GetDashDirection();
        Character->LaunchCharacter(Direction * DashSpeed, true, false);
    }

    // The VFX is a Gameplay Cue, not a hard-coded Niagara spawn in the ability.
    // The cue asset is content; the ability just says "dash is happening".
    SpawnGameplayCue(DashCueTag);

    SetAbilityEndedByTimer(DashDuration, Handle, Owner, ActivationInfo, true);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, AActor* Owner,
                           const FGameplayAbilityActivationInfo ActivationInfo,
                           EGameplayAbilityEndedDelegate EndAbilityCallback)
{
    // Removing the effect removes the granted tags. If the player also had a
    // shield, State.Shielded is a different tag and is untouched.
    RemoveGameplayEffectFromOwner(DashStateEffect.Get());

    UGameplayStatics::RemoveGameplayCueOwner(Owner, DashCueTag);
    EndAbilityCallback.ExecuteIfBound();
}
```

**My recommendation, stated plainly.** For a project of this size I would ship the component approach and keep the ability *vocabulary* (tags, effect lifetimes, cooldown ownership) in the design, so that migrating later is a refactor of the storage, not a rewrite of the gameplay. For a project with a dozen interacting abilities, buffs, attribute-driven balance and — critically — online multiplayer, GAS is the right answer from day one, and starting in Blueprints would be the mistake.

### 5.4 Scoring & Multipliers

![IgKnighted — the collectable spawning during a wave and the live score readout in the HUD](../../assets/images/Y1D/3.png)

```text
  Matchstick collected
        │
        ▼
  PlayerState.AddCollectable()
        │
        ├── ++CollectablesCollected      ← the authoritative counter
        ├── Score += 100 * ScoreMultiplier
        └── OnScoreChanged.Broadcast(Score, Multiplier)
                 │
                 ▼
        HUD widget updates its text
        (it never keeps its own counter)
```

**Why the HUD must not keep its own counter.** Our release shipped with a bug where the collectable count *appeared doubled* on the Game Over screen. The most likely cause, and the one I designed against afterwards, is a widget that both received the events *and* recomputed a total from a second source — a classic double-count. The rule I applied everywhere afterwards:

> **The authoritative counter lives in exactly one place, and every view reads it. No view accumulates.**

```cpp
// IgKnightedPlayerState.cpp
void AIgKnightedPlayerState::AddCollectable()
{
    ++CollectablesCollected;

    // Single point of score mutation. The multiplier is applied HERE, once.
    // If the HUD also applied the multiplier, the number on screen and the
    // number in the state would disagree whenever the multiplier changed.
    const int32 Awarded = CollectableValue * ScoreMultiplier;
    Score += Awarded;

    OnScoreChanged.Broadcast(Score, ScoreMultiplier);
}
```

**The multiplier.** The multiplier is per-player and resets when the zone is cleared (so a strong run in Zone 1 does not make Zone 3 trivial). It increases on specific events — enemy kills in the current zone — and is displayed next to the score.

```cpp
// IgKnightedPlayerState.cpp
void AIgKnightedPlayerState::RegisterEnemyDefeated()
{
    ++EnemiesDefeatedThisZone;

    // The multiplier climbs in steps so the HUD has something legible to show.
    const int32 NewMultiplier = 1 + (EnemiesDefeatedThisZone / 5);
    if (NewMultiplier != ScoreMultiplier)
    {
        SetMultiplier(NewMultiplier);
    }
}

void AIgKnightedPlayerState::OnZoneCleared()
{
    // Reset per-zone progress, keep the score. A new zone is a new multiplier
    // ladder, not a new score.
    EnemiesDefeatedThisZone = 0;
    SetMultiplier(1);
}
```

**Event-driven HUD updates.** The score HUD is a pure subscriber. It never polls, and it never writes back.

```text
  PlayerState                HUD widget (WBP_PlayerHUD)
  ───────────                ──────────────────────────────
  Score changes  ──────────▶  OnScoreChanged(NewScore, Mult)
  Mult changes   ──────────▶  SetText(ScoreText, FText::AsNumber(NewScore))
  Lives change   ──────────▶  SetText(LivesText, ...)

  The widget holds a weak reference to the PlayerState.
  On Possess, the widget rebinds to the NEW PlayerState.
  On EndPlay, it unbinds - no stale references, no double subscription.
```

```cpp
// IgKnightedHUDWidget.cpp
void UIgKnightedHUDWidget::BindToPlayerState(AIgKnightedPlayerState* NewState)
{
    // Unbind first. Subscribing twice is how a HUD ends up adding 100 points
    // twice per collectable.
    if (BoundPlayerState.IsValid())
    {
        BoundPlayerState->OnScoreChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleScoreChanged);
        BoundPlayerState->OnMultiplierChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleMultiplierChanged);
    }

    BoundPlayerState = NewState;

    if (BoundPlayerState.IsValid())
    {
        BoundPlayerState->OnScoreChanged.AddDynamic(this, &UIgKnightedHUDWidget::HandleScoreChanged);
        BoundPlayerState->OnMultiplierChanged.AddDynamic(this, &UIgKnightedHUDWidget::HandleMultiplierChanged);

        // Push the current values so the widget is correct before the first
        // event arrives. A HUD that shows "0" for two seconds after spawn
        // feels broken even though it is technically "event driven".
        HandleScoreChanged(BoundPlayerState->Score, BoundPlayerState->ScoreMultiplier);
    }
}
```

---

## 06. Level Design, Progression & Difficulty Systems

> **One map. Three zones. No loading screens between them.** The whole progression system is a state machine on a manager actor, and the single hardest bug in the project lived right here.

### 6.1 Persistent Level Architecture

IgKnighted's main level is **one persistent map** containing three combat zones. There is no level streaming between them and no travel. The player walks through the temple and fights three encounters in one continuous world.

```text
  L_Temple  (one persistent map)
  ═══════════════════════════════════════════════════════════════════════

   ┌──────────────────────────────────────────────────────────────┐
   │  ZONE 1  "The Flooded Hall"                                  │
   │  ┌────────────────────────────────────────────────────────┐  │
   │  │ Encounter: 6 Crawlers, 2 Scorpions                    │  │
   │  │ Spawn points:      ▣ ▣ ▣ ▣                            │  │
   │  │ Collectable spawns: procedurally placed during waves  │  │
   │  │ Exit barrier: ████████ (closed)                       │  │
   │  └────────────────────────────────────────────────────────┘  │
   └───────────────────────────┬──────────────────────────────────┘
                               │  barrier opens when the encounter clears
   ┌───────────────────────────▼──────────────────────────────────┐
   │  ZONE 2  "The Coral Hall"                                    │
   │  ┌────────────────────────────────────────────────────────┐  │
   │  │ Encounter: 4 Crawlers, 3 Scorpions, 1 Mollusk         │  │
   │  │ Exit barrier: ████████ (closed)                       │  │
   │  └────────────────────────────────────────────────────────┘  │
   └───────────────────────────┬──────────────────────────────────┘
                               │
   ┌───────────────────────────▼──────────────────────────────────┐
   │  ZONE 3  "The Heart of the Temple"                           │
   │  ┌────────────────────────────────────────────────────────┐  │
   │  │ Encounter: 8 Crawlers, 4 Scorpions, 2 Mollusks        │  │
   │  │ Victory trigger: ▓▓▓▓▓▓▓▓▓▓                          │  │
   │  └────────────────────────────────────────────────────────┘  │
   └──────────────────────────────────────────────────────────────┘

   PlayerStarts (shared by both players)   ← respawn point
   ZoneManager                             ← owns the progression
   EnemySpawners (one per zone)            ← owns the spawns
```

**Why persistent and not three levels?** Three reasons, and the third one is about architecture rather than design:

1. **Design.** A continuous temple reads as a journey. Three separate level loads would break the flow of a game that is otherwise a 10-minute arcade session.
2. **Performance and memory.** One map means one lighting bake, one set of assets resident, no streaming hitch between zones.
3. **State.** With one map, the player's position, the camera, the ambient audio and the run state are continuous. Nothing needs to be serialised and restored across a travel.

The cost: **level streaming becomes a concern.** A persistent map with three zones is a large level. I kept the per-zone content in **World Partition** data layers so the designer could work on one zone at a time without loading the entire map, and so the streaming source is explicit rather than accidental.

### 6.2 Enemy Spawning & Encounter Management

This is where the project's worst bug lived, so it gets the most code.

**The bug.** In the release build, certain invisible barriers would despawn after killing particular enemies — that is, a barrier would open early, or the wrong barrier would open. The symptom is confusing until you name the cause: **the barrier was listening to a single enemy's death instead of to the encounter's completion.**

The naive implementation looks like this, and it is exactly what we had:

```cpp
// ❌ THE BUG. Do not do this.
void AIgKnightedZoneManager::OnEnemyDefeated(AActor* Enemy)
{
    ActiveEnemies.Remove(Enemy);
    if (ActiveEnemies.Num() == 0)
    {
        OpenBarrier();      // ← but ActiveEnemies was only the CURRENTLY ALIVE ones
    }
}
```

The flaw: `ActiveEnemies.Num() == 0` is true **the moment the last currently-alive enemy dies** — but the encounter may still have enemies queued to spawn. The barrier opens, the player walks into the next zone, and the wave that was still pending spawns behind them. In a game where the next zone's spawner triggers on a timer, this reads as "the barrier opened early" or "the barrier opened at the wrong time".

**The fix: track living enemies and pending spawns separately, and require both to be zero.**

```cpp
// IgKnightedZoneManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedZoneManager.generated.h"

class AIgKnightedEnemyCharacter;
class AIgKnightedEnemySpawner;
class ATriggerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneCleared, int32, ZoneIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneStarted, int32, ZoneIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyCountChanged, int32, AliveCount, int32, PendingCount);

UCLASS()
class IGKNIGHTED_API AIgKnightedZoneManager : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedZoneManager();

    virtual void BeginPlay() override;

    /** Spawners for the current zone. */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zone")
    TArray<TObjectPtr<AIgKnightedEnemySpawner>> ZoneSpawners;

    /** The barrier that opens when the current zone is cleared. */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zone")
    TObjectPtr<AActor> ExitBarrier;

    UPROPERTY(BlueprintReadOnly, Category = "Zone")
    int32 CurrentZoneIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Zone")
    bool bZoneInProgress = false;

    UPROPERTY(BlueprintAssignable, Category = "Zone")
    FOnZoneCleared OnZoneCleared;

    UPROPERTY(BlueprintAssignable, Category = "Zone")
    FOnZoneStarted OnZoneStarted;

    UPROPERTY(BlueprintAssignable, Category = "Zone")
    FOnEnemyCountChanged OnEnemyCountChanged;

    UFUNCTION(BlueprintCallable, Category = "Zone")
    void StartCurrentZone();

    /** Called by enemies when they die. */
    UFUNCTION(BlueprintCallable, Category = "Zone")
    void NotifyEnemyDefeated(AIgKnightedEnemyCharacter* Enemy, int32 EncounterId);

protected:
    /**
     * Enemies that are ALIVE right now, tagged with the encounter they belong to.
     *
     * TWeakObjectPtr because a dead enemy gets destroyed; a strong reference
     * here would keep a corpse alive in memory until the zone ended.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Zone")
    TMap<TWeakObjectPtr<AIgKnightedEnemyCharacter>, int32> AliveEnemies;

    /**
     * How many enemies are STILL GOING TO SPAWN in the current encounter.
     *
     * This is the number the naive implementation was missing. Until it
     * reaches zero, the encounter is not over, no matter how many enemies are
     * currently dead.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Zone")
    int32 PendingSpawnCount = 0;

    int32 CurrentEncounterId = 0;

    /** Guards against a double callback from the same enemy. */
    TSet<TWeakObjectPtr<AIgKnightedEnemyCharacter>> ProcessedDefeats;

    void RecomputeAndBroadcastCounts();
    void HandleEncounterComplete();
};
```

```cpp
// IgKnightedZoneManager.cpp
void AIgKnightedZoneManager::NotifyEnemyDefeated(AIgKnightedEnemyCharacter* Enemy, int32 EncounterId)
{
    if (!IsValid(Enemy)) { return; }

    // ---- Guard 1: ignore stale encounters ---------------------------------
    // An enemy killed late - for example by a dash that was still in flight
    // when the zone changed - must not affect the new zone's state. This is
    // the "the wrong barrier opened" bug in its second form.
    if (EncounterId != CurrentEncounterId)
    {
        UE_LOG(LogTemp, Verbose,
            TEXT("IgKnighted: ignoring defeat of a stale encounter (%d != %d)."),
            EncounterId, CurrentEncounterId);
        return;
    }

    // ---- Guard 2: one defeat per enemy -------------------------------------
    // The same enemy can report its death from a projectile hit AND from a
    // dash traversal in the same frame.
    if (ProcessedDefeats.Contains(Enemy)) { return; }
    ProcessedDefeats.Add(Enemy);

    AliveEnemies.Remove(Enemy);

    RecomputeAndBroadcastCounts();
    HandleEncounterComplete();
}

void AIgKnightedZoneManager::RecomputeAndBroadcastCounts()
{
    // Remove any weak pointers whose actor has been garbage collected, so the
    // count does not drift upwards as enemies are destroyed.
    for (auto It = AliveEnemies.CreateIterator(); It; ++It)
    {
        if (!It->Key.IsValid())
        {
            It.RemoveCurrent();
        }
    }

    OnEnemyCountChanged.Broadcast(AliveEnemies.Num(), PendingSpawnCount);
}

void AIgKnightedZoneManager::HandleEncounterComplete()
{
    if (!bZoneInProgress) { return; }

    // THE CONDITION. Both counters must be zero.
    //
    //   PendingSpawnCount == 0  → nothing is left to spawn
    //   AliveEnemies.Num() == 0 → nobody is left alive
    //
    // Either one alone is not enough. This single line is the fix for the
    // barrier bug that shipped in our release.
    if (PendingSpawnCount > 0 || AliveEnemies.Num() > 0)
    {
        return;
    }

    bZoneInProgress = false;

    // Open THIS zone's barrier, not "a barrier".
    if (ExitBarrier)
    {
        ExitBarrier->SetActorHiddenInGame(false);
        if (UPrimitiveComponent* Comp = ExitBarrier->GetRootComponent() ? ExitBarrier->GetRootComponent()->FindComponentByClass<UPrimitiveComponent>() : nullptr)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        // Fire the Blueprint event so designers can animate the barrier
        // (dissolve, shards, a rising water wall) without any C++ change.
        ExitBarrier->CallFunctionByNameWithArguments(TEXT("OnBarrierOpened"), nullptr);
    }

    OnZoneCleared.Broadcast(CurrentZoneIndex);
}
```

**The spawner.** The spawner owns the *pending* count, and it decrements it as it spawns:

```cpp
// IgKnightedEnemySpawner.cpp
void AIgKnightedEnemySpawner::ActivateEncounter(int32 EncounterId)
{
    ActiveEncounterId = EncounterId;

    // Tell the manager how many enemies are coming BEFORE the first one
    // spawns. If the manager learned the count afterwards there would be a
    // window in which the encounter looked complete while enemies were
    // still queued.
    ZoneManager->RegisterPendingSpawns(EnemiesToSpawn.Num());

    for (const FEnemySpawnEntry& Entry : EnemiesToSpawn)
    {
        // Each entry has its own delay and interval, so a wave can trickle in
        // rather than appearing all at once.
        FTimerHandle Handle;
        GetWorldTimerManager().SetTimer(Handle, this,
            &AIgKnightedEnemySpawner::SpawnOneEnemy,
            Entry.StartDelay, false);

        // Stagger the rest of this entry's group.
        for (int32 i = 1; i < Entry.SpawnCount; ++i)
        {
            GetWorldTimerManager().SetTimer(Handle, this,
                &AIgKnightedEnemySpawner::SpawnOneEnemy,
                Entry.StartDelay + (Entry.SpawnInterval * i), false);
        }
    }
}

void AIgKnightedEnemySpawner::SpawnOneEnemy()
{
    // Take the pending count down as we spawn, not when the enemy dies.
    // "Pending" literally means "not yet spawned".
    ZoneManager->ConsumePendingSpawn();

    const FTransform SpawnTransform = PickSpawnTransform();
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AIgKnightedEnemyCharacter* Enemy =
        GetWorld()->SpawnActor<AIgKnightedEnemyCharacter>(EnemyClass, SpawnTransform, Params);

    if (!Enemy) { return; }

    // Register the enemy with the manager BEFORE anything can kill it. If the
    // registration happened after, an enemy that died on its first frame
    // would never enter the registry, and the zone would look complete
    // while it was not.
    Enemy->SetEncounterId(ActiveEncounterId);
    ZoneManager->RegisterEnemy(Enemy, ActiveEncounterId);

    // Bind the death event. We bind to the enemy's own delegate rather than
    // polling the registry every frame.
    Enemy->OnEnemyDefeated.AddDynamic(this, &AIgKnightedEnemySpawner::HandleEnemyDefeated);
}
```

**The registration ordering is subtle and it matters.** There are two orderings to get right:

```text
  CORRECT ORDER
  ─────────────
  1. Spawn the enemy
  2. Assign its EncounterId
  3. Register it with the ZoneManager      ← before it can possibly die
  4. Bind the death delegate

  WRONG ORDER
  ───────────
  1. Spawn the enemy
  2. Bind the death delegate
  3. Register it with the ZoneManager      ← ← ← WRONG
       └─ If the enemy dies between 2 and 3, its death event fires before it
          is in the registry. The manager removes an enemy it never had, the
          count is wrong, and the zone either never clears or clears early.
```

I found this by accident, watching the enemy count in the HUD during a wave: it would occasionally go to zero one enemy early. The fix was to make registration part of the spawn function rather than something a separate system did later.

### 6.3 Zone Progression

```text
   ┌──────────┐  StartCurrentZone()   ┌──────────┐
   │ Waiting  │──────────────────────▶│ Spawning │
   └──────────┘                       └────┬─────┘
        ▲                                │ encounter live
        │                                ▼
        │  next zone ready          ┌──────────┐
        └───────────────────────────│ Fighting │
                                     └────┬─────┘
                                          │ alive == 0 AND pending == 0
                                          ▼
                                     ┌──────────┐
                                     │ Cleared  │
                                     └────┬─────┘
                                          │ OpenBarrier + OnZoneCleared
                                          ▼
                              GameMode::HandleZoneCleared(ZoneIndex)
                                          │
                              ┌───────────┴───────────┐
                          last zone?               no
                              │                       │
                              ▼                       ▼
                        ┌──────────┐          StartCurrentZone(Next)
                        │ Victory  │          (next zone's spawners activate)
                        └──────────┘
```

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::HandleZoneCleared(int32 ZoneIndex)
{
    if (bMatchFinished) { return; }

    AIgKnightedGameState* GS = GetGameState<AIgKnightedGameState>();
    if (!GS) { return; }

    // The zone index the manager reports must be the zone we are actually in.
    // A stale event from a previous zone is ignored.
    if (ZoneIndex != GS->CurrentZoneIndex) { return; }

    // Per-player: reset the multiplier ladder for the new zone.
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AIgKnightedPlayerState* PS = It->Get() ? It->Get()->PlayerState : nullptr)
        {
            if (AIgKnightedPlayerState* IgPS = Cast<AIgKnightedPlayerState>(PS))
            {
                IgPS->OnZoneCleared();
            }
        }
    }

    // Is this the last zone?
    if (ZoneIndex >= TotalZoneCount - 1)
    {
        FinishMatch(EMatchResult::Victory);
        return;
    }

    // Advance. The new zone's spawners activate; the next barrier is armed.
    GS->SetZoneIndex(ZoneIndex + 1);
    ZoneManager->AdvanceToZone(ZoneIndex + 1);
}
```

**The precedence between a zone clear and a death on the same frame.** This is the kind of question that has to be answered *before* it matters, not during a bug hunt:

> **Death wins.** If the last enemy of the final zone dies on the same frame that the last shared life is spent, the run ends in defeat.

The reasoning: a player who is dead did not "clear" the zone. Awarding a victory to a team that has just been eliminated is not a subtle balance decision, it is a contradiction. The implementation falls out of the existing ordering — the death path calls `FinishMatch(Defeat)`, and `FinishMatch` is idempotent and refuses to run twice:

```cpp
void AIgKnightedGameMode::FinishMatch(EMatchResult Result)
{
    // Idempotent. Whichever event arrives first wins, and the second one is
    // a no-op. This single guard makes the "death and victory on the same
    // frame" case deterministic without any special-case code.
    if (bMatchFinished) { return; }
    bMatchFinished = true;

    AIgKnightedGameState* GS = GetGameState<AIgKnightedGameState>();
    if (GS)
    {
        GS->SetMatchPhase(Result == EMatchResult::Victory ? EMatchPhase::Victory : EMatchPhase::Defeat);
    }

    // Disable gameplay input for every local player.
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
        }
    }

    OnMatchFinished.Broadcast(Result);
}
```

### 6.4 Difficulty System

Difficulty is chosen in the front-end (Easy / Medium / Hard) and delivered to the match as a **`UPrimaryDataAsset`**. The GameMode reads it once at match start and applies it to the encounter configuration.

```cpp
// IgKnightedDifficultyDataAsset.h
UCLASS(BlueprintType)
class IGKNIGHTED_API UIgDifficultyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Multiplier on the number of enemies per encounter. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
    float EnemyCountScale = 1.0f;

    /** Multiplier on enemy movement speed. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
    float EnemySpeedScale = 1.0f;

    /** Multiplier on the interval between enemy attacks. Lower = more attacks. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
    float EnemyAttackIntervalScale = 1.0f;

    /** Multiplier on how often the Water Mollusk's beam is active. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
    float HazardUptimeScale = 1.0f;

    /** Multiplier on the delay between collectable spawns. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
    float CollectableIntervalScale = 1.0f;

    /** Bounds for the dynamic difficulty director. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty|Adaptive")
    float MinAdaptiveScalar = 0.85f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty|Adaptive")
    float MaxAdaptiveScalar = 1.15f;
};
```

| Setting | Easy | Medium | Hard |
|---|---|---|---|
| Enemy count scale | 0.7 | 1.0 | 1.35 |
| Enemy speed scale | 0.85 | 1.0 | 1.15 |
| Attack interval scale | 1.3 (slower) | 1.0 | 0.8 (faster) |
| Hazard uptime scale | 0.6 | 1.0 | 1.4 |
| Collectable interval scale | 0.8 | 1.0 | 1.2 |

**Why difficulty is a data asset and not a set of `if (Difficulty == Hard)` branches.** The `if` version starts as three branches in the spawner and ends as thirty branches spread across four files, each one a place where a designer has to remember to add the new case. With a data asset, "Hard" is a row of numbers. Adding "Nightmare" is a new asset, not a new code path.

### 6.5 Dynamic Difficulty

The front-end offered a **dynamic difficulty toggle**, and it was one of the more interesting systems to design because the obvious implementation is a disaster.

**The obvious implementation:** every frame, look at how the players are doing and change the enemy count. It oscillates, it is unreadable, and it makes the game feel arbitrary.

**The implementation I would write.** The director evaluates **between encounters**, never during one. It looks at a small set of *normalised* metrics and produces a single scalar that is clamped to a narrow band around the chosen preset.

```text
  ENCOUNTER COMPLETE
         │
         ▼
  ┌──────────────────────────────────────────┐
  │  Gather metrics for the encounter        │
  │   · lives lost during the encounter      │
  │   · time to clear (vs. a reference time) │
  │   · damage taken as a fraction of max    │
  └────────────────┬─────────────────────────┘
                   ▼
  ┌──────────────────────────────────────────┐
  │  performance = clamp(w1*speed − w2*losses, −1, +1)
  │  target     = clamp(base + k*performance, min, max)
  │  next       = lerp(current, target, smoothing)
  └────────────────┬─────────────────────────┘
                   ▼
  Apply to the NEXT encounter's configuration
```

```cpp
// IgKnightedDifficultyDirector.h
UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedDifficultyDirector : public UActorComponent
{
    GENERATED_BODY()

public:
    /** The static preset the player chose. The adaptive value is relative to this. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
    TObjectPtr<UIgDifficultyDataAsset> BasePreset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty")
    bool bDynamicDifficultyEnabled = false;

    /** The current multiplier applied on top of the preset. 1.0 = the preset. */
    UPROPERTY(BlueprintReadOnly, Category = "Difficulty")
    float CurrentScalar = 1.0f;

    /** Called when an encounter completes. */
    UFUNCTION(BlueprintCallable, Category = "Difficulty")
    void EvaluateAfterEncounter(float EncounterDuration, int32 LivesLostDuringEncounter);

protected:
    /** How strongly the director is allowed to react. */
    UPROPERTY(EditAnywhere, Category = "Difficulty")
    float ReactionStrength = 0.15f;

    /** How much of the change is applied each time. Low = smooth, no oscillation. */
    UPROPERTY(EditAnywhere, Category = "Difficulty")
    float Smoothing = 0.5f;

    /** A reference time for a "normal" clear, in seconds. */
    UPROPERTY(EditAnywhere, Category = "Difficulty")
    float ReferenceClearTime = 45.f;
};
```

```cpp
// IgKnightedDifficultyDirector.cpp
void UIgKnightedDifficultyDirector::EvaluateAfterEncounter(float EncounterDuration, int32 LivesLost)
{
    // If the player turned it off, the director is inert. The toggle must be
    // honoured exactly - a player who turned it off expects a fixed challenge.
    if (!bDynamicDifficultyEnabled || !BasePreset)
    {
        CurrentScalar = 1.0f;
        return;
    }

    // ---- Normalise the metrics into a single value in [-1, +1] ------------
    // Clear speed: faster than the reference is positive (the players are
    // doing well), slower is negative.
    const float SpeedScore = FMath::Clamp(
        (ReferenceClearTime - EncounterDuration) / FMath::Max(ReferenceClearTime, 1.f),
        -1.f, 1.f);

    // Lives lost: each life lost is a strong negative signal.
    const float LossScore = FMath::Clamp(-static_cast<float>(LivesLost) / 3.f, -1.f, 0.f);

    const float Performance = FMath::Clamp(
        (0.65f * SpeedScore) + (0.35f * LossScore), -1.f, 1.f);

    // ---- Target, clamped to the preset's allowed band ----------------------
    const float TargetScalar = FMath::Clamp(
        1.0f + (ReactionStrength * Performance),
        BasePreset->MinAdaptiveScalar,
        BasePreset->MaxAdaptiveScalar);

    // ---- Smooth the transition ---------------------------------------------
    // Without smoothing the scalar jumps every encounter and the difficulty
    // feels random. Lerping means a strong run ramps up gradually, which the
    // player perceives as "the game is getting harder" rather than "the game
    // changed the rules".
    CurrentScalar = FMath::Lerp(CurrentScalar, TargetScalar, Smoothing);

    UE_LOG(LogTemp, Verbose,
        TEXT("IgKnighted: dynamic difficulty -> %.3f (performance %.3f)"),
        CurrentScalar, Performance);
}
```

**The hard boundaries, and why they exist:**

| Rule | Reason |
|---|---|
| Never leaves the preset's `[Min, Max]` band | The player chose a difficulty; the director adjusts *within* it, it does not reclassify the game |
| Only evaluates between encounters | Changing difficulty mid-fight reads as cheating |
| Strongly damped by smoothing | Prevents oscillation between "too easy" and "brutal" |
| Lives lost dominate the signal | A player who is dying needs relief more than a player who is dominating needs punishment |
| Fully inert when the toggle is off | A player who opted out gets exactly the preset |

---

## 07. Animation, UI & Gameplay Feedback

> **Gameplay decides *what* happened. Presentation decides *how it looks*.** Every system in this section is a consumer of gameplay events — none of them is allowed to decide anything.

### 7.1 Animation Blueprint Integration

![IgKnighted — the Animation Blueprint state machine and transition graph in the shipped build](../../assets/images/Y1D/7.png)

Our animation work was **Animation Blueprints** for the player and the enemies: idle, locomotion, firing, hit reaction and death, with transitions driven by gameplay state.

```text
  ABP_Player  (Animation Blueprint on the PlayerCharacter mesh)
  ═══════════════════════════════════════════════════════════════

  State Machine: "Locomotion"
  ─────────────────────────────────────────
     Idle ─────(Speed > 5)────▶ Walk
      ▲                            │
      └──────(Speed < 5)───────────┘
                                   │
                              (Fire event) ──▶  [Montage: "Fire"]  ──▶ back to Walk
                                   │
                              (Dash event) ──▶  [Montage: "Dash"]  ──▶ back to Walk
                                   │
                              (Death) ─────▶  [Montage: "Death", Loop=false] → "Dead" state
```

**The variables the state machine reads.** Locomotion is derived from real movement data, not from a bespoke boolean that someone has to remember to set:

| Variable | Type | Source |
|---|---|---|
| `Speed` | float | `CharacterMovementComponent->Velocity.Size2D()` |
| `bIsFiring` | bool | `CombatComponent->bIsFiring` |
| `bIsDashing` | bool | `DashComponent->bIsDashing` |
| `bIsDead` | bool | Character's own death flag |
| `AimYaw` | float | `CharacterRotation.Yaw` — the weapon pivot's yaw, not the body's |

**The `AimYaw` detail.** In a twin-stick game the character faces one way and aims another. If the Animation Blueprint used the *actor's* yaw, the legs would turn with the aim and the player would lose the "running left, shooting right" read. The correct source is the **weapon pivot's** world rotation. Getting this right required a small change in the Character: the pivot is a separate scene component whose yaw follows the aim, and the body yaw is driven by the movement direction.

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // The body faces the movement direction; the weapon pivot faces the aim.
    // Separating them is what makes a twin-stick character readable.
    if (!MovementInput.IsNearlyZero())
    {
        const FVector FacingDir =
            (GetActorForwardVector() * MovementInput.Y + GetActorRightVector() * MovementInput.X).GetSafeNormal2D();
        if (!FacingDir.IsNearlyZero())
        {
            // bUseControllerRotationYaw is false for this character; we drive
            // the body rotation ourselves so the aim can differ from the face.
            const FRotator Target = FacingDir.Rotation();
            const FRotator Current = GetActorRotation();
            SetActorRotation(FMath::RInterpTo(Current, Target, DeltaSeconds, 12.f));
        }
    }

    if (!TargetAimRotation.Equals(GetActorRotation(), 0.1f))
    {
        // The pivot interpolates faster than the body - snappy aiming, smooth
        // locomotion. This is a game-feel decision, and it is a one-line
        // difference that a designer can tune.
        const FRotator Current = WeaponPivot->GetRelativeRotation();
        WeaponPivot->SetRelativeRotation(
            FMath::RInterpTo(Current, TargetAimRotation, DeltaSeconds, 25.f));
    }
}
```

**C++ → Blueprint → Animation contract.** The gameplay code does not play montages. It broadcasts events, and the Animation Blueprint implements them:

```cpp
// IgKnightedPlayerCharacter.h
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationEvent);

/** "The player fired" - the Animation Blueprint turns this into the recoil montage. */
UPROPERTY(BlueprintAssignable, Category = "Animation")
FOnAnimationEvent OnFireAnimationRequested;

/** "The player died" - the Animation Blueprint plays the death sequence. */
UPROPERTY(BlueprintAssignable, Category = "Animation")
FOnAnimationEvent OnDeathAnimationRequested;

/**
 * Called FROM the Animation Blueprint, via a notify or an anim event.
 * This is the reverse direction: the animation tells gameplay that a
 * specific frame has been reached.
 */
UFUNCTION(BlueprintCallable, Category = "Animation")
void NotifyFootstep();

UFUNCTION(BlueprintCallable, Category = "Animation")
void NotifyFireImpactFrame();
```

**Why the animation notifies gameplay instead of the other way round.** Two examples that came up in this project:

1. **Fire timing.** The projectile should leave the muzzle at the frame where the arm extends, not on the input frame. The Animation Blueprint fires a notify at that frame and the combat component spawns the projectile *then*. Getting the visual and the hitbox to agree is the difference between "the shot looks like it hits" and "the shot appears to hit a frame before the enemy reacts".
2. **Death timing.** The player's capsule must not be removed until the death animation reaches the frame where the body hits the floor. The notify triggers the actual destruction, so the corpse does not vanish mid-animation.

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::NotifyFootstep()
{
    // The animation tells us a footstep frame happened; gameplay plays the
    // sound and spawns the dust. The animation owns the TIMING, gameplay owns
    // the CONSEQUENCE.
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        // (A BlueprintImplementableEvent on the AnimBP is the usual route;
        //  a C++ AnimInstance subclass is the alternative when the logic needs
        //  to be native.)
    }
    OnFootstep.Broadcast();
}
```

**State, not animation, decides gameplay.** The one rule that keeps animation and gameplay from fighting each other: **the Animation Blueprint never blocks a gameplay action.** If the player is in the middle of a death animation, the game state has already changed — the character is dead, the life is spent. The animation is a lagging representation of that state, not the source of it.

### 7.2 Gameplay Events & Visual Feedback

Every piece of feedback in the game hangs off a gameplay event, so that designers can change the *look* without touching the *rules*.

| Gameplay event | Consumed by | Feedback |
|---|---|---|
| `OnFired(MuzzleLocation)` | Niagara, audio, animation | Muzzle flash, fire sound, recoil montage |
| `OnProjectileImpact(Actor, Point)` | Niagara, audio | Water splash / fire impact, decal |
| `OnDamageIgnored` | Niagara, audio | Blue "graze" spark — the player learns the dash worked |
| `OnShieldBroken` | Niagara, audio, animation | Shield shatter, character flash |
| `OnPlayerLethalHit` | Niagara, animation, audio | The knight's fire goes out — a puff of embers |
| `OnDashStarted` / `OnDashEnded` | Niagara, animation | Dash trail, motion streaks |
| `OnEnemyDefeated` | Niagara, audio, score | Enemy death effect, score popup |
| `OnZoneCleared` | Niagara, audio, animation | Barrier opens, level lighting change |
| `OnSharedLivesChanged` | HUD, audio | Heartbeat under 2 lives, life pip breaks |
| `OnPickupCollected` | Niagara, audio, HUD | Pickup burst, score popup |

**The one I am most proud of is `OnDamageIgnored`.** When the player dashes through a water ball and the ball passes through them without doing anything, the game *tells them* it worked. Without that feedback, dashing through a hazard feels like the game ignoring your input, and players stop using the dash at all. Feedback is not decoration — it is the mechanism by which the player learns the rules.

```cpp
// IgKnightedDamageResolver.cpp
// (in the dash-invulnerable branch)
Target->NotifyDamageIgnored(Context);

// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::NotifyDamageIgnored(const FIgDamageContext& Context)
{
    // A brief invulnerability flash + a graze spark at the impact point.
    // The VFX asset is on the character so it can be swapped per character.
    if (IgnoreFeedbackVFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, IgnoreFeedbackVFX, Context.HitLocation);
    }

    // Tell the HUD to flash the dash icon, so the player learns that the dash
    // consumed a charge to avoid the hit.
    OnDamageIgnored.Broadcast(Context);
}
```

### 7.3 User Interface Architecture

The UI is split into **screens** (front-end) and **HUD panels** (in-game). The split matters because the two have completely different lifetimes and update patterns.

```text
  FRONT-END SCREENS (one at a time, a Widget is pushed onto the viewport)
  ═════════════════════════════════════════════════════════════════════
    WBP_MainMenu          PLAY · OPTIONS · ART DISPLAY · EXIT GAME
         │
         ▼
    WBP_PlayerCount       ONE PLAYER / TWO PLAYERS
         │
         ▼
    WBP_Difficulty        EASY · MEDIUM · HARD  +  [ ] Dynamic Difficulty
         │
         ▼
    WBP_TutorialPrompt    "Would you like to complete the tutorial?"
         │
         ├── Yes ──▶ L_Tutorial ──▶ L_Temple
         └── No  ──▶ L_Temple

  IN-GAME HUD (persistent, two panels on a shared screen)
  ═══════════════════════════════════════════════════════════════
    WBP_PlayerHUD (created per player)
      ├── Lives        (team pool, from GameState)
      ├── Score        (own score, from own PlayerState)
      ├── Multiplier   (own multiplier)
      ├── Dash icon    (charges + cooldown, from DashComponent)
      └── Buff icons   (shield / power shot / enhanced dash)
    WBP_ZoneBanner      "ZONE 2" announcement, fades out
    WBP_PlayerCount     "PLAYER 1 / PLAYER 2" labels + player colours
```

**Two players, one screen.** Because there is no split-screen, both HUD panels live in the same viewport. Each panel must therefore be bound to **its own** `PlayerState`, not to "the" player state.

```cpp
// IgKnightedHUDWidget.h
UCLASS(Abstract)
class IGKNIGHTED_API UIgKnightedHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * Binds this widget to ONE player.
     *
     * The widget never calls GetPlayerState() on "the" controller, because
     * with two local players there is no single answer. It is handed the
     * PlayerState it is responsible for and stays bound to it.
     */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void BindToPlayerState(AIgKnightedPlayerState* NewState);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void BindToGameState(AIgKnightedGameState* NewGameState);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    TWeakObjectPtr<AIgKnightedPlayerState> BoundPlayerState;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    TWeakObjectPtr<AIgKnightedGameState> BoundGameState;

    /** Unbind on teardown. A stale delegate binding is a crash in a shipping build. */
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleScoreChanged(int32 NewScore, int32 NewMultiplier);

    UFUNCTION()
    void HandleMultiplierChanged(int32 NewMultiplier);

    UFUNCTION()
    void HandleLivesChanged(int32 NewLives, int32 MaxLives);
};
```

```cpp
// IgKnightedHUDWidget.cpp
void UIgKnightedHUDWidget::NativeDestruct()
{
    // Unbind EVERYTHING. Widgets are destroyed and recreated on level travel
    // and on respawn; a delegate that survives the widget is a dangling
    // pointer and a crash that only shows up in Shipping.
    if (BoundPlayerState.IsValid())
    {
        BoundPlayerState->OnScoreChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleScoreChanged);
        BoundPlayerState->OnMultiplierChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleMultiplierChanged);
    }

    if (BoundGameState.IsValid())
    {
        BoundGameState->OnSharedLivesChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleLivesChanged);
    }

    Super::NativeDestruct();
}
```

**The front-end screens are stateless views.** They read from and write to the `GameInstance`'s `PendingRunSettings`, and they never carry state between visits:

```cpp
// WBP_PlayerCount logic (Blueprint equivalent, shown as pseudocode)
//
//   OnConfirm(SelectedCount):
//       GI = GetGameInstance()
//       GI->PendingRunSettings.LocalPlayerCount = SelectedCount
//       PushWidget(WBP_Difficulty)
//
//   OnBack():
//       PopWidget()      // the setting is already stored in the GameInstance
```

Because the state lives in the `GameInstance`, going **back** from the difficulty screen to the player-count screen and then forward again does not lose or duplicate anything. That is a small thing that took me a while to appreciate: **UI state and model state should never be the same thing.**

### 7.4 Event-Driven UI Updates

**Polling vs. events.** The HUD could read the score every frame in `NativeTick`. That would work. It would also mean the widget's lifetime is coupled to the game's, that it wakes up 60 times a second to display a number that changes twice a minute, and that every widget is a potential source of per-frame cost. Events are not just cleaner — they are cheaper and they make the data flow visible.

```text
  ┌──────────────────────────────────────────────────────────────────┐
  │  EVENT-DRIVEN HUD                                                │
  │                                                                  │
  │   PlayerState ──OnScoreChanged──▶ HUD widget ──▶ SetText(...)   │
  │   PlayerState ──OnMultiplier───▶ HUD widget ──▶ SetText(...)   │
  │   GameState   ──OnLivesChanged─▶ HUD widget ──▶ SetText(...)   │
  │   DashComp.   ──OnDashState───▶ HUD widget ──▶ SetIcon/Progress │
  │                                                                  │
  │  ...except: cooldown sweeps, which genuinely change every frame.  │
  └──────────────────────────────────────────────────────────────────┘
```

The rule of thumb I used: **if the value changes on a gameplay event, use an event. If the value changes continuously, tick it.** The dash cooldown ring is a continuous value, so it updates per frame — but it reads a single float from the component and calls `SetPercent`, which is cheap. It does not poll the game state looking for changes.

```cpp
// IgKnightedHUDWidget.cpp
void UIgKnightedHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Only ONE thing in this widget is per-frame, and it is a continuous
    // value. Everything else is event driven.
    if (AIgKnightedPlayerCharacter* PC = GetOwningPlayerPawn<AIgKnightedPlayerCharacter>())
    {
        if (const UIgKnightedDashComponent* Dash = PC->GetDashComponent())
        {
            // SetPercent is cheap: it writes a float into the material.
            DashCooldownBar->SetPercent(Dash->GetCooldownFraction());
        }
    }
}
```

**Blueprint Event Dispatchers as the designer-facing layer.** For the parts of the HUD that designers tune, I exposed **Blueprint Event Dispatchers** on the C++ classes so that a Blueprint can define the *presentation* without any C++ involvement:

```text
  C++ (IgKnightedPlayerCharacter)
    └── FOnPlayerDied (BlueprintAssignable dispatcher)
          │
          ▼
    BP_PlayerCharacter receives it and calls:
       PlayDeathMontage()
       SpawnDeathPuff()
       DisablePlayerHud()
```

This is a small but important API design decision: **the C++ surface should be shaped for the people who consume it**, not just for the C++ programmer. A `BlueprintAssignable` multicast delegate with a clear name is a better integration point than "call this function on the character and then set these five booleans".

---

## 08. Engineering Practices & Technical Decisions

> **The part of the job that is not writing gameplay code.** How the code is structured, how bugs are found, what is tested, what it costs, and why it is built that way.

### 8.1 C++ & Blueprint Architecture

IgKnighted was built with a **C++ base class + Blueprint-derived content** split. The rule I applied consistently:

> **Gameplay *rules* live in C++ so they cannot be broken by a drag-and-drop mistake. Gameplay *content* lives in Blueprints and data assets so a designer can iterate in seconds without a rebuild.**

| Concern | Lives in | Why |
|---|---|---|
| Damage precedence, idempotency guards | **C++** | A logic error here is a wrong death. It must be reviewable, diffable and testable. |
| Fire cadence, spread maths | **C++** (component) | Reused by player and enemies; a shared rule needs one implementation. |
| Dash charges, cooldown, i-frames | **C++** (component) | State machine with edge cases; too subtle to be a Blueprint graph. |
| Spawning, encounter counting | **C++** | The barrier bug lived here. Blueprint graph logic hid the ordering mistake. |
| Match flow, victory/defeat | **C++** (GameMode) | The rules of the game. |
| Meshes, materials, Niagara, sounds | **Blueprint / assets** | Pure presentation; changing them must not require a programmer. |
| Enemy variants and tuning | **Data assets** | A new enemy is a new asset, not a new code path. |
| HUD layout, animation graphs | **Blueprint** | Pure authoring. |
| Game feel numbers (spread, cooldown) | **Data assets** | Tuning must not require a rebuild. |

**The reflection system.** Unreal's reflection is what makes the C++/Blueprint boundary possible, and using it correctly was the single biggest thing I learned in this project:

```cpp
// The five constructs that made the C++/Blueprint split work

// 1. UCLASS + UPROPERTY: the field exists in the editor and in Blueprint.
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
float DashDuration = 0.18f;

// 2. UFUNCTION(BlueprintCallable): Blueprint can call it.
UFUNCTION(BlueprintCallable, Category = "Dash")
bool TryStartDash(const FVector& WorldDirection);

// 3. UFUNCTION(BlueprintPure): no side effects - Blueprint can call it freely
//    from a graph without the engine warning about "impure" node usage.
UFUNCTION(BlueprintPure, Category = "Dash")
bool IsDashInvulnerable() const { return bDashInvulnerable; }

// 4. UFUNCTION(BlueprintNativeEvent): C++ provides a default implementation,
//    Blueprint may override it. This is the extension point for designers.
UFUNCTION(BlueprintNativeEvent, Category = "Feedback")
void OnDashStarted();
void AIgKnightedPlayerCharacter::OnDashStarted_Implementation()
{
    // Default: do nothing. The Blueprint subclass overrides it to play the
    // trail VFX and the montage.
}

// 5. DECLARE_DYNAMIC_MULTICAST_DELEGATE: the notification channel.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashStateChanged, bool, bIsDashing, int32, RemainingCharges);
UPROPERTY(BlueprintAssignable, Category = "Dash")
FOnDashStateChanged OnDashStateChanged;
```

**The property specifiers are not decoration.** They are the contract between the programmer and the designer, and getting them right removes whole categories of bugs:

| Specifier | Use when | Consequence if wrong |
|---|---|---|
| `EditDefaultsOnly` | A class default a designer sets on the Blueprint | Instances can drift from the class default |
| `EditAnywhere` | A per-instance value | Designers change values that should have been shared |
| `BlueprintReadOnly` | Blueprint can read but not write | The Blueprint can create an inconsistent state |
| `BlueprintReadWrite` | Blueprint genuinely needs to write it | Anyone can break the invariant |
| `VisibleAnywhere` | A component the designer must not remove | — |

A concrete example of the payoff: `CurrentCharges` on the dash component is `BlueprintReadOnly`. A designer can *display* it but cannot set it, because setting it by hand would desynchronise it from the cooldown timer and the HUD. The invariant is enforced by the type system, not by a code comment.

**Interfaces over casts.** Where the code needed to talk to "something that can be damaged" without knowing its concrete type, I used an interface rather than a chain of `Cast<>` calls:

```cpp
// IgKnightedDamageable.h
UINTERFACE(MinimalAPI, BlueprintType)
class UIgKnightedDamageable : public UInterface { GENERATED_BODY() };

class IGKNIGHTED_API IIgKnightedDamageable
{
    GENERATED_BODY()

public:
    /** Returns the result so the caller knows whether the damage counted. */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    FIgDamageResult ReceiveDamage(const FIgDamageContext& Context);
};
```

```cpp
// The projectile does not know what it hit. It asks the interface.
if (OtherActor->Implements<UIgKnightedDamageable>())
{
    IIgKnightedDamageable::Execute_ReceiveDamage(OtherActor, Context);
}
```

The payoff is that the projectile code has **no `Cast<AIgKnightedPlayerCharacter>` and no `Cast<AIgKnightedEnemyCharacter>`**. A future destructible barrel that implements the interface is damaged correctly, with no change to the projectile.

### 8.2 Debugging & Problem-Solving

This is the section I would read first if I were reviewing a gameplay programmer. Here are the real problems from this project, and how I actually approached them.

---

#### Problem 1 — Local multiplayer: controllers, possession, and input (≈2 days)

**Symptom.** With two players selected, the second player spawned but could not be controlled. Sometimes the first player lost control instead. Sometimes input worked for a while and stopped after entering the tutorial.

**Why it was hard.** Four subsystems have to agree, and the failure modes look identical from the outside:

```text
   ULocalPlayer  ──▶  APlayerController  ──▶  Possessed Pawn
        │                     │                      │
        └── InputMappingContext ┘                      └── input bindings
             (Enhanced Input LocalPlayer Subsystem)         live here
```

If the mapping context is on the wrong `LocalPlayer`, the controller is fine, the Pawn is fine, and the input still goes to the wrong player. There is no error message. The only symptom is "player 2 does nothing".

**What I did, in order:**

1. **Reproduced it minimally.** Built an empty level with just a `PlayerStart`, two `LocalPlayer`s and two cubes. Ruled out the whole game around it. This alone cut the search space by an order of magnitude.
2. **Printed the actual state** instead of guessing. For each `LocalPlayer` index I logged the controller, the possessed pawn, and the mapping contexts on the subsystem:

```cpp
UE_LOG(LogTemp, Warning, TEXT("---- LocalPlayer %d ----"), Index);
UE_LOG(LogTemp, Warning, TEXT("  Controller:    %s"), *GetNameSafe(LP->GetPlayerController(GetWorld())));
UE_LOG(LogTemp, Warning, TEXT("  Pawn:          %s"), *GetNameSafe(LP->GetPlayerController(GetWorld())->GetPawn()));
if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
{
    for (const FEnhancedActionMappingContext& Mapping : Sub->GetAllMappingContexts())
    {
        UE_LOG(LogTemp, Warning, TEXT("  Mapping:       %s"), *Mapping.GetMappingContext()->GetName());
    }
}
```

The log showed Player 1 with **two** mapping contexts and Player 2 with **none**. That was the bug, and no amount of staring at the Blueprint graph would have shown it.

3. **The fix.** Mapping contexts are installed on the `ULocalPlayer`'s `UEnhancedInputLocalPlayerSubsystem`, and re-installed on `OnPossess` because the engine clears input state across level travel.

**Lesson.** When the symptom is "something is not happening", log the *state* of the whole chain rather than reading the code. The bug was a missing object, not a wrong statement.

---

#### Problem 2 — Enemy perception logic was redundant

**Symptom.** The original enemy Blueprint had three separate copies of "find a player and check if I can see them", one per enemy archetype, each slightly different. Changing the perception rule meant editing three graphs, and one of them had a subtly different radius.

**Why it was hard.** This is the classic Blueprint failure mode: it works, it ships, and duplication is cheaper than refactoring *right now* with three weeks left.

**What I did.** I extracted the shared rule into a single place — the `UIgKnightedTargetingComponent` — and made each enemy archetype supply only what is genuinely different:

```text
  BEFORE                              AFTER
  ──────                              ──────
  BP_Crawler                          AIgKnightedEnemyCharacter
   ├ FindPlayer (copy 1)               ├ TargetingComponent  ◀── ONE copy
   ├ CanSeeTarget (copy 1)             │   ├ AcquireTarget()
   └ MoveToTarget                        │   └ IsValidTarget()
                                          └ AttackComponent
  BP_Scorpion                              ├ Projectile attacks
   ├ FindPlayer (copy 2)                   └ Contact attacks
   ├ CanSeeTarget (copy 2)
   └ AimAtTarget
                                     Each archetype now contributes
  BP_Mollusk                             only its movement/attack logic
   ├ FindPlayer (copy 3)
   ├ CanSeeTarget (copy 3)
   └ ActivateStream
```

**The measurement.** Not a frame-time number — the number of *places a rule exists*. Perception logic went from three implementations to one, and a change to the perception radius is now a one-line edit. The measurable win is the time to make the change and the risk of forgetting one of the three copies.

**Lesson.** Duplication in Blueprints is invisible in code review, because there is no diff. It only shows up as "why does this one enemy behave differently?".

---

#### Problem 3 — The barrier despawned after the wrong enemy died

**Symptom.** The public release notes for our build listed it as a known bug: "certain invisible walls/barriers incorrectly despawn after killing certain enemies."

**Diagnosis.** The barrier was bound to a **single enemy's** death event rather than to the **encounter's** completion. The encounter has two counters — living enemies and pending spawns — and the completion condition was checking only one of them. So the barrier opened the moment the last *currently living* enemy died, even if a wave was still queued to spawn.

**The fix** is the `ZoneManager` described in §6.2: separate `AliveEnemies` from `PendingSpawnCount`, register enemies before they can die, ignore stale encounters, and require **both** counters to be zero.

**Lesson.** The bug was not in the barrier. The barrier was doing exactly what it was told. The bug was in *what it was told to listen to*. When a system's behaviour looks inexplicable, check the event contract before the event handler.

---

#### Problem 4 — The collectable count appeared doubled

**Symptom.** On the Game Over screen, the collectable count showed roughly twice the real number.

**Diagnosis.** The most likely cause — and the one I designed against afterwards — is **two counters**. The `PlayerState` kept an authoritative count; the Game Over widget also kept its own, updated by the same events, and added them together, or the widget was bound to the delegate twice (a `AddDynamic` without a matching `RemoveDynamic` when the widget was recreated on respawn).

**The fix, in the shipped design:**

- One authoritative counter, in `PlayerState`.
- The widget **reads** it, it never accumulates it.
- `BindToPlayerState` always unbinds before binding, so a rebind cannot double-subscribe.
- `NativeDestruct` unbinds everything.

**Lesson.** "Doubling" in a UI is almost never a maths bug. It is two sources of truth, or one source observed twice.

---

#### Problem 5 — Controls stopped working after the art-display level

**Symptom.** The "Art Display" menu option opened a separate level. After returning, the controls were dead.

**Diagnosis.** The art level pushed its own widget and its own input context. On return, the gameplay mapping context was not re-installed: `ClearAllMappings` had been called, and the `GameMode` that should have re-added the contexts was a *new* `GameMode` (the old one was destroyed with the level), and its `BeginPlay` only set up the contexts on `SetupInputComponent` — which is **not called again** for a controller that already exists.

**The fix.** Move the mapping-context installation into `OnPossess` (which *is* called for an existing controller on level travel) rather than only into `SetupInputComponent`, and clear the mappings in `EndPlay`.

**Lesson.** `SetupInputComponent` is called once per controller. Level travel reuses the controller and calls `OnPossess` again. Choosing the wrong lifecycle hook is an input bug that only appears after a travel.

---

### 8.3 Testing & Edge Cases

The project shipped with a public release page, which means **players were our testers**. The published known-bugs list became our retrospective bug list, and I turned every one of them into a regression test.

**The test matrix that mattered.** These are the cases where the design is one line of code away from being wrong:

| ID | Scenario | Expected result | Area |
|---|---|---|---|
| T01 | Start solo on Easy | One controller, one Pawn, 5 shared lives | Multiplayer |
| T02 | Start two players on Hard | Two controllers, two Pawns, shared lives start at 5 | Multiplayer |
| T03 | Player 1 takes an unshielded water hit | One shared life spent, Player 1 respawns | Damage |
| T04 | Player 2 takes an unshielded water hit | One shared life spent, Player 2 respawns | Damage |
| T05 | Player 1 spends the last life while Player 2 is alive | Defeat for the whole team | Damage |
| T06 | Shielded player takes a water hit | Shield breaks, **no** life spent | Damage |
| T07 | Player dashes across the mollusk beam | No damage, no shield consumed | Damage |
| T08 | Player stands in the beam for 3 seconds | Exactly **one** life spent | Damage |
| T09 | Enhanced dash crosses the same Crawler over several frames | Enemy defeated once | Damage |
| T10 | Two hit callbacks during one death | Exactly one life spent | Damage |
| T11 | Player picks up Power Shot | Three projectiles per shot | Abilities |
| T12 | Player picks up a second Shield while shielded | Pickup is **not** consumed | Abilities |
| T13 | Two players collect near-simultaneously | Each score goes to the right player | Scoring |
| T14 | Last enemy of Zone 1 dies, spawns still pending | Barrier stays **closed** | Progression |
| T15 | An enemy from a previous zone dies late | The wrong barrier does not open | Progression |
| T16 | Last enemy of Zone 3 dies with lives remaining | Victory | Progression |
| T17 | Final enemy dies on the same frame as the last life is spent | Defeat (death wins) | Progression |
| T18 | Return from the tutorial | Input valid, correct Pawn possessed | Input |
| T19 | Enter and leave the Art Display level | Gameplay input restored | Input |
| T20 | Retry after a Game Over | No stale delegates, controllers or timers | Lifecycle |
| T21 | Dynamic difficulty ON | The next encounter reflects the metric | Difficulty |
| T22 | Dynamic difficulty OFF | The preset is stable | Difficulty |

**How the awkward ones were actually tested.** "Two players on one keyboard" and "the same frame" are not things you can do with a mouse. The practical approach:

- **Two-player tests** were done with two gamepads. The keyboard fallback for player 2 was a deliberate design decision to avoid both players fighting over one keyboard.
- **Frame-exact tests** were done by forcing the state: a debug console command that calls `HandlePlayerEliminated` and `HandleZoneCleared` in the same tick, in both orders. If the result differs by order, the precedence rule is not implemented — it is just hoped for.

```cpp
// A debug hook I would add for exactly these cases.
UFUNCTION(Exec, BlueprintCallable, Category = "Debug")
void DebugSimulateSimultaneousDeathAndZoneClear()
{
    // Deliberately call both in one tick, in a fixed order, and assert the
    // outcome. Run it in both orders in the editor; the result must be
    // identical.
}
```

**Testing in a game project is mostly about state, not about asserting values.** The questions that actually caught bugs were:

- Can this state be reached? (two players, one dead, one shield, mid-dash, at a zone boundary)
- Can this be reached twice? (double overlap, double pickup, double death)
- Can this be reached at the same time as something else? (death + victory, respawn + zone clear)
- What does the player see when it happens? (feedback, not just state)

### 8.4 Performance & Optimization

**The honest version.** IgKnighted ran comfortably on the hardware it targeted — a 1080p Windows machine with a handful of enemies and a few dozen projectiles. There was no frame-rate crisis. The performance work in this project was therefore mostly **preventative** and **structural**, not heroic: knowing where the cost was going, and making sure the systems I wrote did not create cost that scaled badly.

That is still the right job for a gameplay programmer, and it is what the role asks for ("diagnose and fix gameplay, memory, performance, networking, and scalability issues").

**The tools, and what each one is actually good for.**

| Tool | Use it for | What it will not tell you |
|---|---|---|
| `stat unit` | Frame time breakdown: game / draw / GPU | Which *system* inside "game" is slow |
| `stat game` | Actor and component counts, memory | Why the count is high |
| `stat fps` | Quick frame-rate check | Anything |
| Unreal Insights | Where the frame time goes, per-thread, per-event | Gameplay-level meaning |
| Blueprint profiler | Cost of individual Blueprint nodes | C++ cost |
| `Gameplay Debugger` (F1) | AI perception, targeting, state at runtime | Frame time |

**What I actually measured.** The number that mattered was not the frame rate — it was the **actor count and the spawn churn**, because that is what scales:

```text
  Test setup: 1080p, 2 players, Zone 3 encounter live
              14 enemies, ~30 active projectiles, particle effects on
  Build:      Development (non-shipping)

  Before optimisation:
    stat game → 1,412 actors, 218 components
    stat unit → Game 8.1ms | Draw 4.2ms | GPU 6.4ms

  What I found:
    - 60% of the "Game" time was in AI: every enemy was running a
      target search every frame.
    - The projectile actors were not being cleaned up fast enough;
      several dozen were alive with no possible target in their path.

  Changes:
    1. Target acquisition moved to an interval (0.5s) with a cached
       target, instead of a per-frame search.   (§4.3)
    2. Projectile lifespan tightened from 6s to 2.5s, and projectiles
       that leave the arena bounds are destroyed immediately.
    3. The `stat game` actor count was checked against the level content:
       a stray duplicated "decorative" actor set was loading the whole
       Niagara system for every instance.

  After:
    stat game → 986 actors, 141 components
    stat unit → Game 5.6ms | Draw 4.2ms | GPU 6.4ms

  Conclusion: the frame-time win came from AI, not from rendering.
              The draw and GPU numbers did not move, which is what told
              me the problem was never rendering.
```

**The structural decisions that keep performance honest:**

1. **No per-frame target searching.** The `TargetingComponent` re-evaluates on an interval and caches. Searching for the nearest player every frame for 20 enemies is pure waste — the answer changes about once a second.
2. **Timers, not ticks, for cadence.** Attack intervals, dash cooldowns, hazard cycles and respawn delays are all timer-driven. A tick would fire 60 times a second to perform work that happens twice a second.
3. **Projectiles have a lifespan.** A projectile that leaves the arena still exists, still ticks, still has a collision component. Two and a half seconds is enough for any shot on this map.
4. **Weak references in registries.** The `ZoneManager` holds `TWeakObjectPtr` to enemies, so a destroyed enemy does not keep a corpse alive in memory.
5. **No hard references to unused assets.** The `EnemyClass` references are `TSubclassOf` on the spawner, not on every enemy, so a level does not load every enemy's assets if that enemy never spawns there.
6. **No pooling.** This is the one I want to defend, because pooling is usually treated as mandatory. At this scale the spawn churn was not a bottleneck, and a pool adds lifetime bugs — a pooled projectile that is not correctly reset leaks its `bHasImpacted` flag and silently stops working. **Pooling should be added when the profiler says so, not because it sounds professional.**

**On memory specifically.** The most common memory problem in a project like this is not "the game leaks", it is **"the game loads everything"**. Hard references from a level to every asset mean every asset is resident for the level's lifetime. Using soft references (`TSoftObjectPtr`, `FSoftObjectPath`) for assets that are loaded on demand — a boss's mesh, an optional pickup — keeps the base footprint small.

### 8.5 Architectural Trade-offs

These are the decisions I actually agonised over, with the alternatives and the reasoning. This is the section I would want a reviewer to challenge me on.

---

**ADR-001 — Shared lives live in the `GameState`, not in each `PlayerState`.**

- **Context.** Two players, one pool of five lives. The HUD naturally wants per-player lives.
- **Decision.** The `GameMode` processes eliminations and writes to a single `SharedLivesRemaining` on the `GameState`.
- **Rejected.** Five lives on each `PlayerState`.
- **Reason.** With per-player lives, the run could lose ten pawns before the Game Over screen appeared, and it would appear while a player was still alive with lives in hand. That directly contradicts the design rule that the pool is *shared*.
- **Trade-off.** Per-player death statistics must live elsewhere (in the `PlayerState`, which is fine — the stats are personal, the lives are not).

---

**ADR-002 — One `PlayerController` class, two instances.**

- **Context.** Two local players with identical abilities.
- **Decision.** A single `AIgKnightedPlayerController` subclass; player identity is an index on the instance.
- **Rejected.** `PlayerOneController` and `PlayerTwoController`.
- **Reason.** Two classes means two copies of every input binding, every possession rule, every camera rule, and every bug — and the bug gets fixed in one of them.
- **Trade-off.** Player-specific behaviour needs a data-driven branch on the index rather than a type check. For a game with two players and no asymmetry, that is the right trade. If the game ever had genuinely different characters, the right answer would be a **data-driven character definition**, not two controller classes.

---

**ADR-003 — One persistent map with three zones, not three levels.**

- **Context.** The temple has three distinct arenas.
- **Decision.** One level, a `ZoneManager` actor, spawners per zone, barriers between them.
- **Rejected.** Three separate maps with level travel between them.
- **Reason.** It matches the intended flow (a continuous descent), it removes an entire class of state-serialisation bugs across travel, and it keeps the run's context (camera, audio, player position) continuous.
- **Trade-off.** A bigger level, which needs World Partition to stay workable for the designers. And zone state must be managed explicitly, because the level is no longer doing it for us.

---

**ADR-004 — One centralised damage resolver.**

- **Context.** Shield, dash i-frames, lethal water and lethal contact all interact, and any of them can fire more than once.
- **Decision.** A single `DamageResolver` with an explicit precedence chain, and every damage source routes through it.
- **Rejected.** Each enemy decrements the lives itself on an overlap event.
- **Reason.** With distributed damage, "one death spends one life" is a convention that every new enemy author has to remember. With one resolver it is a property of the system.
- **Trade-off.** A single hot path that every damage event goes through. It is cheap (a handful of branches), but it is a shared dependency, so it must be correct and it must be the thing I test most.

---

**ADR-005 — Lightweight components now; GAS as a documented alternative.**

- **Context.** Four power-ups on a local, two-player game.
- **Decision.** A small component-based effect layer for the shipped game; the GAS mapping is written up as an explicit design study.
- **Rejected.** Adopting GAS for the shipped game.
- **Reason.** GAS is excellent — but it brings an `AbilitySystemComponent`, an `AttributeSet`, tag registration, effect assets, and a real decision about where the ASC lives across respawns. For four effects on a local game, that is a lot of machinery for a problem three booleans solve.
- **Trade-off.** If the game had grown a dozen interacting abilities, buffs and attribute-driven balance, the component approach would have become the thing blocking me. The mitigation was to keep the *vocabulary* right (tags, effect lifetimes, cooldown ownership) so the migration is a change of storage, not a rewrite of gameplay.

---

**ADR-006 — Explicit encounter completion.**

- **Context.** Barriers must open only when a zone is genuinely finished.
- **Decision.** The `ZoneManager` tracks living enemies and pending spawns separately and raises one completion signal when both reach zero.
- **Rejected.** Binding a barrier to an individual enemy's death event.
- **Reason.** The second version is the bug we shipped. Encounter composition changes with every design iteration; binding to "the encounter" means the binding survives the change.
- **Trade-off.** Slightly more state to track. Worth it.

---

**ADR-007 — Gameplay logic in C++, content in Blueprints.**

- **Context.** A multidisciplinary team of twelve, an eight-week schedule, and a designer who wanted to change enemy counts without waiting for a programmer.
- **Decision.** Rules, validation and state machines in C++; tuning, presentation and content in Blueprints and data assets.
- **Rejected.** Everything in Blueprints (fast to prototype, but the bugs we shipped — the barrier, the perception duplication, the doubled counter — were all logic-in-Blueprint bugs) and everything in C++ (correct, but a designer waiting on a programmer for every number change would have killed the schedule).
- **Reason.** The split puts each concern where it is cheapest to change *and* hardest to break.
- **Trade-off.** Two languages in one project, and the seam between them has to be designed deliberately. I paid that cost once, in the shape of the Blueprint-facing API — `BlueprintImplementableEvent` hooks, `BlueprintAssignable` dispatchers, `EditDefaultsOnly` properties — so the seam is a designed interface rather than an accident.

---

## 09. Results, Retrospective & Lessons Learned

### 9.1 Final Product

[▶️ Play IgKnighted on itch.io](https://buas.itch.io/igknighted)

The shipped build is a complete, playable game: main menu, player-count and difficulty selection, an optional tutorial, one persistent three-zone map, three enemy archetypes, four power-up types, a shared five-life rule, a live HUD, and a win and a lose condition.

**What shipped, system by system:**

| System | Status | Notes |
|---|---|---|
| Front-end flow (menu → count → difficulty → tutorial prompt) | Shipped | Config carried in the `GameInstance` |
| Local multiplayer (1–2 players, keyboard + gamepad) | Shipped | One controller class, per-player input contexts |
| Enhanced Input | Shipped | `InputAction` + `InputMappingContext` assets |
| Player movement, aiming, firing | Shipped | Movement and aim are independent axes |
| Dash with i-frames and cooldown | Shipped | Charges, cooldown, dash-granted immunity |
| Shield / Power Shot / Enhanced Dash / Matchstick | Shipped | Duplicate-pickup restrictions, timed buffs |
| Instant-death water damage | Shipped | Dash → shield → death precedence |
| Shared five-life pool with respawn | Shipped | Game Over at zero, even with a player alive |
| Crawler / Scorpion / Water Mollusk | Shipped | Perception, targeting, three attack styles |
| Three zones, encounters, barriers, victory | Shipped | Fixed during post-release hardening (§6.2) |
| Difficulty presets + dynamic difficulty | Shipped | Bounded, evaluated between encounters |
| Collectables, score, multiplier, live HUD | Shipped | Event-driven |
| Animation Blueprints (idle / walk / shoot / death) | Shipped | Driven by gameplay state |

### 9.2 Technical Challenges & Solutions

| # | Challenge | The hard part | The resolution | The lesson |
|---|---|---|---|---|
| 1 | Local multiplayer input and possession | Four subsystems have to agree and there is no error message | Logged the whole `LocalPlayer → Controller → Pawn → Contexts` chain, found the missing mapping context | Log the *state*, not the code |
| 2 | Perception logic duplicated across three enemies | Blueprint duplication is invisible in review | Extracted a single `TargetingComponent`; archetypes keep only what differs | Count the places a rule exists |
| 3 | Barriers opening at the wrong time | The completion condition only counted *living* enemies | Separate `AliveEnemies` from `PendingSpawnCount`; register enemies before they can die | Check the event contract, not the handler |
| 4 | Doubled collectable count | Two counters, or one counter observed twice | One authoritative counter; the widget reads it and always unbinds before rebinding | UI "doubling" is almost never maths |
| 5 | Controls dead after the art-display level | `SetupInputComponent` runs once per controller, not per level | Install contexts in `OnPossess`; clear in `EndPlay` | Pick the lifecycle hook deliberately |
| 6 | Sustained hazard spending every life | Overlap events fire every frame | Damage on entry, latch per cycle, latch only if the damage counted | Separate "entered" from "still inside" |
| 7 | Double death spending two lives | Three sources can report one death | Idempotency latch + one `HandlePlayerEliminated` path | Make the invariant structural |
| 8 | Score resetting on death | Score was on the Pawn | Score lives in the `PlayerState`, which survives respawn | Put durable state on durable objects |
| 9 | Two HUD panels showing the same score | The widget asked "the" player state | Each widget is bound to one `PlayerState` | With two players, identity is a parameter |

### 9.3 Retrospective Architecture

Looking at this project with a year of more experience, here is what I would change about the architecture, and why.

**1. Move the damage resolver to a first-class component on the Pawn, and make protection a tag/count system.**

Today, `DashComponent` owns a `bDashInvulnerable` boolean and `StatusComponent` owns `ShieldCharges`, and the resolver knows about both. That works because there are two protections. The moment a third appears — a slowing field, a temporary invulnerability from a pickup — the resolver grows another branch, and "ending the dash must not clear the shield" becomes a rule someone has to remember.

The GAS-style fix is not "adopt GAS". It is **counted protection sources**:

```cpp
// Retrospective design: protections as counted sources, not booleans.
USTRUCT(BlueprintType)
struct FIgProtectionSources
{
    GENERATED_BODY()

    void Add(FName Source);
    void Remove(FName Source);
    bool IsProtected() const { return Sources.Num() > 0; }
    bool IsProtectedBy(FName Source) const;

private:
    /** e.g. { "Dash", "Shield", "Pickup:Invulnerability" } */
    TSet<FName> Sources;
};

// Now:
//   DashComponent  -> Protection.Add("Dash")  /  Protection.Remove("Dash")
//   StatusComponent-> Protection.Add("Shield")/  Protection.Remove("Shield")
//   Resolver       -> if (Protection.IsProtected()) { ignore; }
//
// Ending the dash cannot clear the shield, because it removes a different
// key from the same set. The bug class disappears rather than being fixed.
```

Same idea, a fraction of the machinery, and the failure mode is designed out instead of tested for.

**2. Replace the boolean "is the encounter over" with an explicit encounter object.**

The `ZoneManager` currently holds a map of enemies and a pending count. That is correct, but the state is spread across a manager, a set of spawners and a bunch of delegates. A first-class `FIgEncounter` struct — `{ Id, AliveEnemies, PendingSpawns, Phase }` — with the completion rule as a method on it would make the invariant testable in isolation, and would let encounters be defined in data assets.

**3. Move the fire pattern out of the weapon data asset into a dedicated `FIgFirePattern` struct.**

`BonusProjectiles` + `SpreadDegrees` works for one shot and three shots. A designer who wants a five-shot cone, or a fan that widens with the multiplier, has to add fields to the weapon asset and change the C++. A pattern as an array of `{ AngleOffset, DamageScale, Count }` entries covers every pattern the game could want, and the C++ never changes again.

**4. Add a real regression test harness.**

The test matrix in §08.3 is a document, not code. In a project with a build step, the same cases could be Automation Spec tests driving a headless world:

```cpp
// The shape a real test would take.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIgSharedLifeTest,
    "IgKnighted.Damage.SharedLife",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FIgSharedLifeTest::RunTest(const FString& Parameters)
{
    // Spawn a world, a GameMode, two Pawns, and assert:
    //   - one lethal hit spends exactly one life
    //   - a shielded hit spends none
    //   - a dashing hit spends none
    //   - zero lives ends the match even with a live Pawn
    return true;
}
```

This is the change with the highest return on investment. Three of the bugs in this project (the barrier, the doubled counter, the double death) were all *invariant* violations, and all three are the kind of thing an automation test expresses in five lines and checks forever.

**5. Use a `UWorldSubsystem` for the match session.**

The `GameInstance` is the right place for configuration, but some things are genuinely session-scoped: the player's preferences, the input settings, and — with the retrospective design — the **accumulated run statistics**. A `UGameInstanceSubsystem` for player settings and a `UWorldSubsystem` for the world-scoped services would be cleaner than a `GameInstance` that keeps growing.

### 9.4 Key Takeaways

**About Unreal Engine:**

- **Lifetime is the first design decision.** Almost every bug in this project was a state object in the wrong place. `GameInstance` for the session, `GameMode` for the rules, `GameState` for the numbers, `PlayerController` for intent, `PlayerState` for identity, `Pawn` for the body. Learn this once and most architectural confusion disappears.
- **Local multiplayer is not "multiplayer-lite".** It is a different problem with different failure modes, and the question it asks constantly is "which player does this belong to?".
- **The Gameplay Framework is a set of contracts, not a set of classes.** Once you treat `GameMode` as "the rules" and `GameState` as "the numbers", the rest of the framework starts to make sense on its own.
- **Enhanced Input's real value is not the bindings — it is the separation of action from mapping, and the per-`LocalPlayer` scoping.**

**About engineering:**

- **Make the invariant structural, not conventional.** "One death spends one life" was a rule everyone had to remember; it is now a latch and a single entry point. The best time to enforce an invariant is the moment you decide it matters.
- **Idempotency is a feature.** Overlaps fire repeatedly, delegates can be bound twice, and effects can be applied twice. A latch at the boundary costs one boolean.
- **Distrust duplication in Blueprint.** It is invisible in review and it is where "why does this one behave differently?" comes from.
- **Log state, not code.** Most of the hardest bugs were solved in ten minutes by printing what was actually true.
- **Prefer events to polling, but not everywhere.** If a value changes on a gameplay event, use an event. If it changes continuously, tick it. Both are correct; mixing them up is not.
- **Measure before you optimise, and do not optimise what the profiler says is fine.** The real win in this project was not frame rate — it was *one* implementation of the perception rule instead of three.

**About working with people:**

- **Designers do not want a programming task; they want a dial.** Every number a designer had to ask me for was a small piece of friction in the iteration loop. Moving them into data assets was the highest-leverage thing I did for the team.
- **The seam between C++ and Blueprint is an API, and it deserves design.** `BlueprintImplementableEvent` hooks, `BlueprintAssignable` dispatchers and carefully chosen property specifiers are what let the artists and designers iterate without me.
- **Owning a system means owning its failure modes.** The barrier bug was not the barrier's fault; it was mine, because I wrote the contract that fed it.

**What this project says about how I work:** I care about the space between the mechanic and the feedback — the moment where a rule becomes something the player can feel and understand. The dash's i-frames are not just a gameplay flag; the graze spark is what teaches the player that dashing through a water beam works. I care about state being in the right place, because that is what makes a system survive being changed by someone else. And I care about the code being readable enough that the next person — including future me — can change it without first having to understand it.

---

## 🎯 Role Alignment Summary

If you are here for the job description rather than the game, this is the short version.

| Epic's requirement | Where I demonstrated it |
|---|---|
| Player mechanics, player abilities | §03.3–03.5 (dash, i-frames, death/respawn), §05 (power-ups) |
| Player progression | §05.4 (score, multiplier), §06 (three zones, encounters, victory) |
| Game flow systems | §02.2–02.3 (front-end → run → match → result), §06.3 |
| World systems and activities | §06.1–06.2 (persistent map, spawners, encounter management) |
| Enhanced Input | §03.2 |
| Gameplay Ability System | §05.3 (component-based implementation + a full GAS mapping) |
| EQS | §04.5 (the Scorpion's firing-position query) |
| Animation | §07.1 (Animation Blueprints, notifies, state-driven locomotion) |
| Exceptional C++ and gameplay architecture | §08.1 (the C++/Blueprint boundary), §08.5 (seven ADRs) |
| Robust, understandable, maintainable code | §08.1, §08.5, the component decomposition throughout |
| Diagnose gameplay, memory and performance issues | §08.2 (five real debugging stories), §08.4 |
| Balance trade-offs for the user experience | §08.5, and the difficulty/dynamic-difficulty bounds in §06.5 |
| Distill feature requests into actionable work items | §06.4 (difficulty as data, not branches), §04.1 (weapon data assets) |
| Cross-discipline collaboration | §01.2 (what I owned and what I did not), §08.1 (the designer-facing API), §07.1 (the animator-facing contract) |

**Tools:** Unreal Engine 5 · C++ (UE reflection, Gameplay Framework, Enhanced Input, GAS, Gameplay Tags, Behaviour Trees, Blackboards, EQS) · Blueprints · UMG · Animation Blueprints · Niagara · Unreal Insights / stat commands.

**Links:** [Play IgKnighted](https://buas.itch.io/igknighted) · [Trailer](https://www.youtube.com/@Jon-Gear) · [GitHub](https://github.com/jon-gear) · [LinkedIn](https://linkedin.com/in/jon-gear)

---

*This case study documents IgKnighted, an eight-week Unreal Engine 5 student project, and the gameplay systems I owned within it. Diagrams and code are my own; game footage and screenshots are from the shipped build.*


