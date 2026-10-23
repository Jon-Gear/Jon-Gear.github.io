---
layout: project
title: "IgKnighted — Unreal Engine 5 Gameplay Programming Case Study"
role: "Gameplay Programmer"

version: none

date: 2024-06-28
categories: [featured_projects]
image: /assets/images/Y1D/main.jpg
description: "A local 1–2 player co-op bullet-hell arena shooter built in Unreal Engine 5. A feature-by-feature case study of the gameplay framework, the player, power-ups, three enemy archetypes, the level and its dynamic difficulty — and what each one taught me about building systems other people can use."
contributions: "Gameplay Framework & Core Architecture | Player: Input, Controller, Pawn, State & Abilities | Collectables & Power-Ups | Enemy AI: Crawler, Scorpion, Mollusk | Level, Spawning & Dynamic Difficulty"
tools: "Unreal Engine 5, C++, Blueprints, Enhanced Input, Gameplay Tags, Behavior Trees, UMG, Animation Blueprints, Niagara"
team_size: "4 Programmers | 3 Designers | 5 Artists"
platforms: "Windows"
duration: "8 Weeks (May 2024 - Jun 2024)"
---

# 💼 IgKnighted — A Stylized Local Multiplayer Arena Shooter

[▶️ Play IgKnighted on itch.io](https://buas.itch.io/igknighted)

IgKnighted is a couch co-op arena shooter built in Unreal Engine 5 by a team of twelve over eight weeks. Two players descend through a three-part flooded temple, fight three enemy archetypes, collect power-ups, and share a single pool of five lives. It shipped complete: front-end flow, tutorial, difficulty selection, and real win and lose conditions.

I was one of **four programmers**. This page goes through every major feature I worked on — framework, player, power-ups, enemies, level — and treats each one as an engineering problem: what it had to do, what broke, how I fixed it, and what the fix cost.

| | |
|---|---|
| **Role** | Gameplay Programmer |
| **Team** | 4 programmers, 3 designers, 5 artists |
| **Duration** | 8 weeks |
| **Engine** | Unreal Engine 5, C++ and Blueprints |
| **Platform** | Windows (local 1–2 players) |
| **Status** | Shipped on itch.io |

*Earlier versions of this write-up: [v1](/projects/year-1-block-d-long-v1/) · [v2](/projects/year-1-block-d-long-v2/).*

---

# 👤 Zhangir Before the Project

I came into IgKnighted as someone who knew how a game engine works, not how Unreal works.

My background was Unity and self-taught programming. I understood the concepts that transfer: a scene graph, prefabs versus instances, an update loop, a component model, an input abstraction. What I did not have was Unreal's vocabulary or its opinions. I had never used the Gameplay Framework beyond opening a template. I did not know why `GameMode` and `GameState` were two different objects, or what a `PlayerState` was for, or what survives a level travel. I had never touched Enhanced Input, Behaviour Trees, Gameplay Tags, UMG, Animation Blueprints or Niagara.

I had also never shipped a game with other people. My previous work was single-player prototypes where I could keep every piece of state in the one Blueprint that used it, because nothing else existed and nothing had to survive anything. That is a very forgiving environment, and it had taught me habits that do not scale:

- **"Put the variable where the event happens."** In a prototype with one character, that is where it belongs. In a co-op game with two players, respawns and three maps, it is how state ends up somewhere that dies.
- **"If it works when I press play, it works."** Prototypes do not have level travel, retries, or a second player. This project had all three.
- **"Blueprints are for designers, C++ is for programmers."** I did not have a useful model of when to use which. I learned that the real distinction is between *rules* that must not be broken and *content* that must be cheap to iterate.

So the honest starting point is: **I could build a prototype, and I could not build a system.** I knew what a character controller was; I did not know what a gameplay framework was for. I had never been the person another programmer depends on.

**What I wanted from the project** was to stop being someone who can make a prototype and become someone who can build something another person can safely change. Everything below is measured against that.

The constraints were real: eight weeks, four programmes each owning a vertical slice, three designers who needed to tune enemies without waiting for a compile, five artists who needed hooks for animation and VFX, and a public itch.io release. Players would find the bugs we did not.

---

# 🔨 My Contributions

## What the game actually is

IgKnighted is a **twin-stick, top-down, local co-op bullet hell**. One or two players share one screen and one pool of five lives, fight through three connected parts of a temple, and try to reach the end without losing the pool.

The loop is:

```text
menu (choose 1-2 players, difficulty)
  → tutorial (optional)
    → arena: part 1 → barrier opens → part 2 → barrier opens → part 3
        ↑                                      ↓
        └── respawn (2.5 s, same part) ──── death (one shared life)
                                            ↓
                                   five lives gone → defeat
                                   part 3 cleared → victory
```

The design constraints that shaped every system:

| Design rule | Consequence for the code |
|---|---|
| Two players, one screen, one process | Per-`LocalPlayer` input; one shared camera; identity must be explicit |
| Five lives **shared**, not five each | One authoritative pool; one writer; one death = one life |
| Instant death, no health bar | Damage is a *resolution* problem, not a hit-point problem |
| Dash is the only defence | i-frame window must be exact, not approximate |
| Three enemy archetypes | One brain, three sets of numbers — not three Blueprints |
| Three parts, sequential | Encounter completion is a state machine, not a counter |
| 8 weeks, 4 programmers | Every system needs a contract the others can build against |

Everything I owned sits on one of those rows. The sections below follow the game's own structure — framework, player, power-ups, enemies, level — because that is how the work was divided and how the systems depend on each other.

| # | Section | What I owned |
|---|---|---|
| 1 | **Gameplay Framework & Core Architecture** | Game Instance, Game Mode, Game State — configuration, rules and match state, and the lifetime each one has to match |
| 2 | **Player** | Input, Player Controller, Player Pawn, Player State, and the four abilities: Move, Shoot, Dash, Die |
| 3 | **Powerups** | Collectable pickups, Shield, Power Shot, Enhanced Dash |
| 4 | **Enemies** | Crawler, Scorpion, Mollusk behaviours, and the AI Controller that drives them |
| 5 | **Level** | The three-part arena, enemy spawning, dynamic difficulty |

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted" target="_blank" rel="noopener">⚙️ See Code Examples</a>

---

## 📋 01. Gameplay Framework & Core Architecture

![IgKnighted — the front end where a run is configured](../../assets/images/Y1D/1.png)

### 🎯 What was the Goal?

Unreal hands you a framework, not a blank canvas. The framework classes all sound like "the game", but they exist for different lengths of time, and that lifetime is a gameplay rule. My job was not to invent a manager object and route everything through it; it was to learn which object owns which lifetime, and then work *with* that split instead of against it.

```text
Application lifetime
└── UGameInstance        the SESSION     survives every level and every match
    └── World / level
        ├── AGameMode    the RULES       dies with the level
        └── AGameState   the NUMBERS     dies with the level
```

The rule I ended up with was: `GameInstance` is the session, `GameMode` is the rules, `GameState` is the numbers. Everything that configures a run lives in the session; everything that decides lives in the rules; everything the UI or another system reads lives in the numbers. The player-side objects — controller, `PlayerState`, Pawn — are covered in §02, and the AI controller in §04.

---

### Challenge 1: The Gameplay Framework

#### ❓ What was the Problem?

My first versions treated the framework classes as interchangeable places to put "game stuff". Run settings went on the widget that collected them, then on the `GameMode`. The match result and the lives were written from whichever system noticed them first. Each choice worked until the object holding the value was destroyed.

Two bugs made the mistake visible. The first was menu → tutorial → arena: pick Hard, play the tutorial, and the arena is Medium. Nothing errors — the arena simply never received the choice, because the tutorial is a different level and the `GameMode` holding it was gone. The second was the ending: spending the last life and clearing the last zone could both try to end the match, sometimes in the same frame, and which screen appeared depended on tick order.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

The hard part was not the code. It was unlearning the assumption that "the game" is one object. I came from prototypes where a single manager held everything, and I kept looking for Unreal's equivalent. My first instinct was the widget, because that is where the click happened. My second was the `GameMode`, because that is the object you are already looking at when you think about the rules. Both felt right, and both failed silently on a path I had not tested.

I did not have a model of Unreal's lifetimes. I remember drawing the object diagram on paper, crossing it out twice, and only then understanding that `BeginPlay` runs again for every level, that `GameMode` is recreated per world, and that `GameInstance` is the only object in the list that survives both the world and the match. What finally made it click was tracing one value — the chosen difficulty — through menu → tutorial → arena and watching exactly where it died.

The same pattern repeated with the match numbers. I put them on the `GameMode` because it was the object making the decisions, then found that the HUD and the zone systems needed to read them without depending on the rules class. Reading Unreal's documentation on `AGameModeBase` versus `AGameStateBase` helped, but the real understanding came from the failed version. The best-practice answer is not clever: configuration that survives travel belongs in the `GameInstance`, rules that only exist inside one match belong in the `GameMode`, and shared state that other systems read belongs in the `GameState`. I only trusted that after doing it the other way twice.

#### 💡 Solution. How Did I Approach the Problem?

**The session — `UGameInstance`.** The run configuration moved into the `GameInstance`, and the setter validates at the boundary:

```cpp
// IgKnightedGameInstance.cpp
void UIgKnightedGameInstance::SetPendingRunSettings(const FIgRunSettings& NewSettings)
{
    // Validation belongs at the boundary. Every consumer downstream can then
    // assume PlayerCount is 1 or 2 and the difficulty is a real value.
    PendingRunSettings = NewSettings;
    PendingRunSettings.PlayerCount = FMath::Clamp(PendingRunSettings.PlayerCount, 1, 2);

    UE_LOG(LogTemp, Log, TEXT("Run settings: %d player(s), difficulty %d, tutorial %s, dynamic %s"),
        PendingRunSettings.PlayerCount,
        static_cast<int32>(PendingRunSettings.Difficulty),
        PendingRunSettings.bTutorialEnabled ? TEXT("on") : TEXT("off"),
        PendingRunSettings.bDynamicDifficulty ? TEXT("on") : TEXT("off"));
}
```

```cpp
// IgKnightedGameInstance.h
/**
 * Exists for the lifetime of the application. Front-end screens write the run
 * configuration here; the GameMode reads it exactly once at BeginPlay.
 *
 * Nothing that must survive a level travel lives in a widget or a GameMode.
 */
UCLASS()
class IGKNIGHTED_API UIgKnightedGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Run")
    void SetPendingRunSettings(const FIgRunSettings& NewSettings);

    UFUNCTION(BlueprintPure, Category = "Run")
    FIgRunSettings GetPendingRunSettings() const { return PendingRunSettings; }

    UFUNCTION(BlueprintCallable, Category = "Run")
    void ClearPendingRunSettings();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
    FIgRunSettings PendingRunSettings;
};
```

The `GameMode` reads it **once**, at `BeginPlay`, and everything downstream reads the GameMode's copy:

```cpp
// IgKnightedGameMode.cpp
    // Read the run configuration exactly once. Everything downstream reads the
    // GameMode's copy, so the UI can be closed and the setting cannot drift.
    RunSettings = FIgRunSettings();
    if (const UIgKnightedGameInstance* GameInstance = GetGameInstance<UIgKnightedGameInstance>())
    {
        RunSettings = GameInstance->GetPendingRunSettings();
    }
```

![F1](../../assets/images/Y1D/blueprints/F1.png)


That "once" is deliberate. If the `GameMode` re-read the settings every time it needed the difficulty, the run could change halfway through because a menu widget wrote a new value. Reading once turns the configuration into an immutable input to the match.

**The rules — `AGameModeBase`.** `FinishMatch` became the only way to end a match, and its first action is to refuse to run twice:

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::FinishMatch(EMatchResult Result)
{
    if (MatchResult != EMatchResult::None)
    {
        return;
    }

    MatchResult = Result;

    AIgKnightedGameState* GameState = GetGameState<AIgKnightedGameState>();
    if (GameState)
    {
        GameState->SetMatchResult(Result);
        GameState->SetMatchPhase(EMatchPhase::Ending);
    }

    GetWorldTimerManager().ClearAllTimersForObject(this);
    RespawnTimers.Reset();

    if (GameState)
    {
        GameState->SetMatchPhase(EMatchPhase::Finished);
        GameState->OnMatchFinished.Broadcast(Result);
    }

    BP_OnMatchFinished(Result);
}
```

Clearing every timer owned by the `GameMode` is what guarantees that nothing scheduled by the match survives its ending: no respawn fires on the results screen, and no zone completion runs after defeat.

The victory path checks that defeat has not already won:

```cpp
// IgKnightedGameMode.cpp
void AIgKnightedGameMode::HandleFinalZoneCleared()
{
    // Defeat wins. If the last life was spent in this frame, the damage path has
    // already called FinishMatch(Defeat), so MatchResult is no longer None and
    // this call is refused.
    if (GetMatchPhase() != EMatchPhase::InProgress || MatchResult != EMatchResult::None)
    {
        return;
    }

    FinishMatch(EMatchResult::Victory);
}
```

The *order* is enforced upstream: the zone manager defers its own completion check by one tick, so the frame's damage has been fully processed before victory is even proposed (this is §05, Challenge 1). The `GameMode` only has to enforce "first ending wins".

![Blueprint 1](../../assets/images/Y1D/blueprints/F2_1.png)

![Blueprint 1](../../assets/images/Y1D/blueprints/F2_2.png)


**The numbers — `AGameStateBase`.** The `GameState` remembers what the match is currently doing. It deliberately holds no pointers to Pawns or enemies, which is what makes it safe for the HUD and the zone systems to read:

```cpp
// IgKnightedGameState.h
/**
 * The match's numbers. The GameMode decides; the GameState remembers.
 * Nothing here holds a pointer to a Pawn or an enemy.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnSharedLivesChanged OnSharedLivesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnMatchPhaseChanged OnMatchPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Match")
    FOnMatchFinished OnMatchFinished;

    UFUNCTION(BlueprintPure, Category = "Match")
    int32 GetSharedLives() const { return SharedLivesRemaining; }

    UFUNCTION(BlueprintPure, Category = "Match")
    int32 GetMaxSharedLives() const { return MaxSharedLives; }

    UFUNCTION(BlueprintPure, Category = "Match")
    EMatchPhase GetMatchPhase() const { return MatchPhase; }

    UFUNCTION(BlueprintPure, Category = "Match")
    EMatchResult GetMatchResult() const { return MatchResult; }

    void InitializeMatch(int32 InMaxLives);
    void SetSharedLives(int32 NewLives);
    void SetMatchPhase(EMatchPhase NewPhase);
    void SetMatchResult(EMatchResult NewResult) { MatchResult = NewResult; }

private:
    UPROPERTY(VisibleAnywhere, Category = "Match")
    int32 SharedLivesRemaining = 5;

    UPROPERTY(EditDefaultsOnly, Category = "Match")
    int32 MaxSharedLives = 5;

    UPROPERTY(VisibleAnywhere, Category = "Match")
    EMatchPhase MatchPhase = EMatchPhase::FrontEnd;

    UPROPERTY(VisibleAnywhere, Category = "Match")
    EMatchResult MatchResult = EMatchResult::None;
};
```

Four values, each with a job: `MatchPhase` says which part of the flow the game is in, `MatchResult` records how it ended, `SharedLivesRemaining` is the team's remaining lives, and `MaxSharedLives` is the cap the HUD draws against. The setters follow the same pattern everywhere — early-out if nothing changed, clamp to a legal range, then broadcast:

```cpp
// IgKnightedGameState.cpp
void AIgKnightedGameState::SetMatchPhase(EMatchPhase NewPhase)
{
    if (MatchPhase == NewPhase)
    {
        return;
    }

    MatchPhase = NewPhase;
    OnMatchPhaseChanged.Broadcast(MatchPhase);
}
```

Who writes these values is a rule, not a convenience: only the `GameMode` spends a life or ends the match. The `GameState` stores, clamps and broadcasts. Score and per-player identity are not here at all — they are per-player state and live on `PlayerState` (§02, Challenge 4).

#### ✅ Problem Solved

Menu → tutorial → arena holds Hard. So does menu → arena, retry, and returning from the results screen. The match has one owner and one terminal transition, so the previous ambiguities became explicit outcomes: the final enemy dying on the same frame as the last life is a defeat, the results screen schedules nothing, and a retry starts with no stale timers.

More useful than the fixes: the team had a rule for the next feature. When someone asked "where should the new counter live?", the answer was a question — does it configure the run (session), decide something (rules), or does something else need to read it (numbers)? That is the framework's model, adopted rather than fought.

#### 📉 Cost / Limitations / Tradeoffs

- **The `GameInstance` is global mutable state.** It is reachable from anywhere, which makes it convenient and easy to abuse. I limited it to one struct and one setter, but nothing in the engine enforces that discipline.
- **Reading once means no mid-run difficulty change.** That is correct for this game, but it is a decision, not a law. A settings menu that changes difficulty live would need a different model — and would need to define what happens to the current encounter.
- **The `GameState` is not replicated.** It is local co-op, so plain `UPROPERTY`s and delegates are enough; a networked build would need `ReplicatedUsing` and a server-authoritative writer. The lifetime split would still be the same.
- **Clamping hides bugs.** If something ever sets the lives to `-3`, the clamp turns it into a quiet zero instead of an assertion. In a shipping build that is the right choice; in development, an `ensure` would have caught the caller sooner.

---

### 💥 Impact of Contribution

**Before:** difficulty could reset at a level boundary, two systems could end the match, and state lived wherever the first implementation happened to put it. Every new feature risked another silent lifetime bug.

**After:** there is one home for configuration, one home for rules and one home for the numbers the rest of the game reads. The framework's lifetimes are the design, not an obstacle to it: the run survives a level change, the match has one ending, and the HUD can read state without depending on the class that decides it. When the team added a damage source or a zone, there was an obvious place for its state to live.

That is the difference between using Unreal's objects and working *with* Unreal's model.

---

## 📋 02. Player

![IgKnighted — Enhanced Input actions and the per-player mapping contexts](../../assets/images/Y1D/4.png)

### 🎯 What was the Goal?

"Player" is not one object. In Unreal it is four objects plus a body, and each one exists so that a different kind of state can survive a different kind of event:

| Object | Owns | Survives |
|---|---|---|
| `PlayerController` | input devices, aim intent | Pawn death, level travel |
| `APawn` | movement, aim direction, components | nothing (dies and is replaced) |
| `PlayerState` | identity, score, multiplier, colour | Pawn death |
| Components | one ability each: combat, dash, status | the Pawn's lifetime |
| `GameMode` | the *rules* that use all of the above | the level |

The goal was to make a player that plays the same with one player, with two, with a gamepad, with a mouse, after a death and after a retry — without four special cases in four systems.

The four abilities were the actual game feel, so each needed to be a small, legible rule:

- **Move** — fast, camera-relative, no diagonal advantage.
- **Shoot** — hold to fire at a fixed cadence; the shot goes where you aimed.
- **Dash** — the only defence; it must work *before* the hit that would have killed you.
- **Die** — one life, no revives in place, a short presentation, then a respawn.

---

### Challenge 1: Input

#### ❓ What was the Problem?

Input worked on the first Pawn and stopped working after a respawn or a level transition. The keys were still bound — pressing Fire ran the handler — but the handler got a null `GetPawn()` and returned silently.

The second symptom was multiplayer-specific: when the second player joined, both players' mapping contexts ended up on one `ULocalPlayer`, so player 1's dash fired player 2's dash, or neither.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

Input was the first place where the template stopped being a tutorial and became a set of assumptions I had to understand. I began by copying the template: bind actions in `SetupInputComponent`, add the mapping context in `BeginPlay`. It worked for one player in one level, then failed on respawn — the new Pawn had no input — and on the second player, whose context was installed against the wrong `LocalPlayer`.

I kept patching the Pawn before I understood the ownership. In Unreal, the controller is the thing that persists and owns input; the Pawn is the body it drives. `SetupInputComponent` and `OnPossess` look like interchangeable hooks, but they run at different times: `SetupInputComponent` once per controller, `OnPossess` every time a Pawn is possessed, including respawn and the initial possession after `CreatePlayer`. Once I mapped those lifetimes out, the fix was straightforward: bind actions in `SetupInputComponent`, add the mapping context in `OnPossess`.

The second lesson was that Enhanced Input fails silently. A missing context does not throw; it simply produces no value, so I spent time debugging the Pawn when the input was never arriving. The accumulation bug was the same kind of ignorance: I called `AddMappingContext` without `ClearAllMappings`, and contexts piled up across possessions. Both fixes came from reading the Enhanced Input documentation carefully and then verifying the assumption in a two-player test — not from guessing again.

#### 💡 Solution. How Did I Approach the Problem?

Bindings once, contexts on possession:

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
    if (!Input)
    {
        UE_LOG(LogTemp, Error,
            TEXT("IgKnighted: InputComponent is not an EnhancedInputComponent. "
                 "Check Project Settings > Input > Default Input Component Class."));
        return;
    }

    // Bindings are installed once per controller. Mapping contexts are not.
    Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnMoveTriggered);
    Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnAimTriggered);
    Input->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnMouseAimTriggered);
    Input->BindAction(FireAction, ETriggerEvent::Started, this, &AIgKnightedPlayerController::OnFireStarted);
    Input->BindAction(FireAction, ETriggerEvent::Completed, this, &AIgKnightedPlayerController::OnFireCompleted);
    Input->BindAction(DashAction, ETriggerEvent::Started, this, &AIgKnightedPlayerController::OnDashStarted);
}

void AIgKnightedPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // OnPossess runs again on respawn and on level travel; SetupInputComponent
    // does not. Installing the contexts here is what makes input survive both.
    ApplyGameplayMappings();
}
```

The subsystem is resolved from the controller's own `LocalPlayer`, and each player owns their own stack:

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::ApplyGameplayMappings()
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (!Subsystem)
    {
        return;
    }

    // The subsystem is per LocalPlayer. This is the line that separates
    // "works with one player" from "works with two": each controller installs
    // its own context stack on its own subsystem.
    Subsystem->ClearAllMappings();

    if (GameplayKeyboardContext)
    {
        Subsystem->AddMappingContext(GameplayKeyboardContext, 0);
    }

    if (GameplayGamepadContext)
    {
        // Same priority as the keyboard context: Enhanced Input routes by the
        // device that produced the input, so a player can switch mid-session.
        // Priorities are reserved for modal UI (menus install at 100).
        Subsystem->AddMappingContext(GameplayGamepadContext, 0);
    }
}
```

Both device contexts live at priority `0` on purpose. Enhanced Input already routes by device; priorities are for *modal* contexts (a menu at priority 100 blocks gameplay without unbinding anything).

#### ✅ Problem Solved

Input survives respawn, retry, tutorial and level travel, and each player only drives their own Pawn. The device can change mid-run: pick up the gamepad and keep playing. The failure mode is now an explicit log at setup, not a silent no-op.

#### 📉 Cost / Limitations / Tradeoffs

- **`ClearAllMappings` is blunt.** It is correct because gameplay is the only context installed by the controller, but a project with modal UI would want to remove only its own contexts (`RemoveMappingContext`) rather than clear the stack.
- **Keyboard sharing is a design constraint, not an input constraint.** Two players on one keyboard works only because the bindings use different key sets; the system would not stop us from binding the same key twice.
- **No rebinding.** Actions are assets, so a rebinding UI is possible, but nothing in this code reads or writes player-specific mappings.
- **The error is logged, not fatal.** With a non-Enhanced input component the game runs and silently ignores everything. That is a deliberate shipping choice and a debugging hazard.

---

### Challenge 2: Aiming

#### ❓ What was the Problem?

Mouse aim looked right and gamepad aim did not. With the stick, the character aimed in world space: push right and you aimed east, regardless of where the camera was pointing. With the mouse, the cursor aim worked, but the weapon's rotation came from the actor rather than the cursor, so the muzzle and the shot disagreed by up to a frame.

The deeper problem was ownership. The controller knew about devices, the Pawn knew about the muzzle, and the projectile asked a third object which way "forward" was.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

My first aim code was copied from the template and worked with the mouse: deproject the cursor, rotate the weapon. The stick version used `GetControlRotation` directly, and it felt wrong — pushing up aimed slightly away from the camera's centre, and the character's facing kept leaking into the result. I tried normalising the stick, then using the actor's forward vector, which made aim follow movement instead of the stick.

What I was missing was that three coordinate spaces are in play. Screen space is where the cursor is. Control-rotation space is the fixed camera's yaw, which is what "up on the stick" should mean. Actor space is where the body happens to be facing, which is a gameplay consequence and must not be an input. Unreal's best practice is to express aim in camera space and let the controller own the intent; the Pawn orients to movement and the weapon orients to aim, on purpose.

Once I understood that, the implementation became two small functions with one rule: the controller collects intent, the Pawn converts it, and neither writes the body's rotation from input. The bugs I had been chasing were all the same mistake — asking the actor a question that only the camera could answer.

#### 💡 Solution. How Did I Approach the Problem?

The Pawn exposes one aim value and two ways to set it; the controller decides which device speaks:

```cpp
// IgKnightedPlayerController.cpp
void AIgKnightedPlayerController::OnAimTriggered(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->AimFromStick(Value.Get<FVector2D>());
    }
}

void AIgKnightedPlayerController::OnMouseAimTriggered(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->AimAtCursor();
    }
}
```

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::AimFromStick(const FVector2D& Input)
{
    if (bIsDead || Input.SizeSquared() < FMath::Square(0.15f))
    {
        return;
    }

    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AimDirection = (Forward * Input.Y + Right * Input.X).GetSafeNormal2D();
}

void AIgKnightedPlayerCharacter::AimAtCursor()
{
    if (bIsDead)
    {
        return;
    }

    APlayerController* Controller = GetController<APlayerController>();
    if (!Controller)
    {
        return;
    }

    FHitResult Hit;
    if (Controller->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        AimDirection = (Hit.ImpactPoint - WeaponPivot->GetComponentLocation()).GetSafeNormal2D();
    }
}
```

The projectile never asks either of them. It asks the Pawn's aim *contract*:

```cpp
// IgKnightedAimProvider.h
UINTERFACE(MinimalAPI)
class UIgKnightedAimProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * How a combat component asks "where do I shoot from, and in which direction?"
 * without knowing whether its owner is a player, a Scorpion or a turret.
 */
class IGKNIGHTED_API IIgKnightedAimProvider
{
    GENERATED_BODY()

public:
    virtual FVector GetMuzzleLocation() const = 0;
    virtual FVector GetAimDirection() const = 0;
};
```

That interface is why the same combat component fires for the player and for the Scorpion. The shooter does not need to know what it is.

#### ✅ Problem Solved

Gamepad aim is camera-relative; mouse aim hits what is under the cursor; both write the same `AimDirection`, and the muzzle follows the same value. The visual weapon pivot interpolates toward it, but shots use the exact direction, so a fast-moving cursor cannot produce a shot that lags the crosshair.

#### 📉 Cost / Limitations / Tradeoffs

- **Mouse aim depends on a visibility trace hitting something.** Aiming at the sky, or at a surface with no collision, keeps the previous direction. In a top-down arena with a floor this is invisible; in an open 3D game it would need a fallback plane.
- **No aim assist for the gamepad.** The deadzone is fixed at `0.15` and aiming is raw. Playtests wanted it, the schedule did not have it.
- **The body and the weapon rotate differently**, which is correct but means "which way am I facing?" has two answers. The HUD and animation code have to know which one they mean.
- **`GetHitResultUnderCursor` uses the mouse position of that player's viewport.** It is correct for one viewport per player, which is what split-screen would need — and wrong for a shared viewport with two players. We used one shared camera, so only one player could use mouse aim by design.

---

### Challenge 3: The Pawn

#### ❓ What was the Problem?

The character started as the third-person template: a `SpringArmComponent` and a `CameraComponent` on the Pawn. The level has a fixed, level-placed camera — the game is top-down and both players must share one view. The Pawn's camera was therefore either redundant or, when enabled by default, a second camera that the viewport could pick.

The other problem was weight. Combat, dash, buffs and death were all in the character class. Every feature added a member, and the class became the place where unrelated rules met.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I removed the camera from the Pawn because the level has a fixed camera, and assumed the template would keep working. It did not. The template's `Move` and `Aim` use the controller's rotation, which the SpringArm had been driving; once the SpringArm was gone, control rotation came from whichever view target the level happened to set, and using its pitch pushed the player into the floor.

I tried locking the controller's rotation, then deriving axes from the actor's forward vector, then clamping pitch — each fix moved the bug. Reading about view targets and `AutoActivateForPlayer` explained where the rotation came from, and the best-practice answer was to derive movement and stick aim from yaw only, because a top-down game never wants the camera's pitch in its movement plane.

Splitting the Pawn into components was a smaller version of the same lesson. I moved code into `StatusComponent`, `CombatComponent` and `DashComponent` but kept casting to the concrete character, so the split was cosmetic. Unreal's component model only pays off if the component finds its dependencies through `FindComponentByClass` and talks to interfaces; that is what made the status component reusable by pickups and the combat component reusable by enemies.

#### 💡 Solution. How Did I Approach the Problem?

The Pawn owns composition and nothing else:

```cpp
// IgKnightedPlayerCharacter.cpp
AIgKnightedPlayerCharacter::AIgKnightedPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);

    Tags.Add(TEXT("Ig.Player"));
    // The body turns with movement; the weapon pivot turns with aim.
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

    WeaponPivot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPivot"));
    WeaponPivot->SetupAttachment(GetMesh());

    Combat = CreateDefaultSubobject<UIgKnightedCombatComponent>(TEXT("Combat"));
    Dash = CreateDefaultSubobject<UIgKnightedDashComponent>(TEXT("Dash"));
    Status = CreateDefaultSubobject<UIgKnightedStatusComponent>(TEXT("Status"));
}
```

Movement reads the *controller's* yaw rather than the Pawn's rotation, because the camera is a fixed top-down actor the Pawn does not own. (The function itself is in Challenge 5; the important part here is the dependency — the Pawn needs a controller for movement, but not for its identity.)

And the visual aim is a smoothed copy, applied to a pivot, not to the body:

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsDead || AimDirection.IsNearlyZero())
    {
        return;
    }

    const FRotator Desired(0.f, AimDirection.Rotation().Yaw, 0.f);
    const FRotator Smoothed = FMath::RInterpTo(WeaponPivot->GetComponentRotation(), Desired, DeltaSeconds, AimInterpSpeed);
    WeaponPivot->SetWorldRotation(FRotator(0.f, Smoothed.Yaw, 0.f));
}
```


#### ✅ Problem Solved

One camera, owned by the level, shared by both players. No duplicate view targets, no SpringArm fighting the level camera. The Pawn is a thin composition of body, pivot and three components, and each component can be tested in isolation.

#### 📉 Cost / Limitations / Tradeoffs

- **The Pawn needs a `Tick` for a cosmetic problem.** The pivot interpolation is presentation only; a material or animation-driven aim would remove the tick. At two players it is nothing; at scale it is the kind of tick that adds up.
- **Smoothing means the visual weapon lags the true aim.** Shots use the true direction, so it is "correct", but a player watching the muzzle sees a small lie. Slower smoothing looked better and lied more.
- **A top-down fixed camera is a design decision that removes features.** No camera shake on that actor, no zoom, no split-screen. We traded camera work for readability with two players.
- **The component split still uses `FindComponentByClass`.** It is a runtime lookup, not a compile-time dependency, so a Pawn without a `Combat` component fails at runtime rather than at compile time. The alternative — constructing the components' dependencies — would be type-safe and more code.

---

### Challenge 4: Player State

#### ❓ What was the Problem?

Two players, two HUD panels, and after a respawn one panel showed the other player's numbers or counted each score change twice.

The doubling was the visible bug: kill an enemy worth 50, and the score went up by 100. The cause was two subscriptions to the same delegate — the widget was recreated on respawn and bound again without unbinding, so one broadcast ran two handlers.

The other half was identity. `PlayerState` existed, but the HUD was finding the player by iterating pawns, so "which panel is mine?" was answered by array order.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I came from single-player prototypes, so the score went on the Character and the HUD polled it. Death destroyed the Pawn and the score vanished with it. My next attempt used `APlayerState`'s built-in `Score`, which compiled and then shadowed the base member in confusing ways. Then I tried letting the HUD apply the multiplier, and the HUD and the results screen started disagreeing about the total.

Each attempt was a reasonable guess about where Unreal wants state to live. The documentation says `PlayerState` is the per-player object that survives the Pawn, but I only internalised it when a respawn test showed the score reset. The multiplier taught me the second half of the rule: derived values should be computed once, by the object that owns the inputs, not by every view. Renaming to `MatchScore` was the point where I stopped fighting the base class and started working with it.

The best-practice version is boring: durable per-player facts on `PlayerState`, one function that changes them, and views that only read.

#### 💡 Solution. How Did I Approach the Problem?

The widget refuses to bind twice, seeds itself from the current value, and unsubscribes on destruction:

```cpp
// IgKnightedHUDWidget.cpp
void UIgKnightedHUDWidget::BindToPlayerState(AIgKnightedPlayerState* InPlayerState)
{
    if (BoundPlayerState == InPlayerState)
    {
        return;
    }

    // Always unbind before binding. A widget recreated on respawn that only
    // adds the delegate is the classic way a counter is observed twice.
    if (BoundPlayerState)
    {
        BoundPlayerState->OnMatchScoreChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleMatchScoreChanged);
    }

    BoundPlayerState = InPlayerState;

    if (BoundPlayerState)
    {
        BoundPlayerState->OnMatchScoreChanged.AddDynamic(this, &UIgKnightedHUDWidget::HandleMatchScoreChanged);

        BP_OnMatchScoreChanged(BoundPlayerState->GetMatchScore());

        // ... (dash-charge binding omitted: charges are body state)
    }
}
```

Colour and multiplier are player identity, so they live on the `PlayerState` too:

```cpp
// IgKnightedPlayerState.cpp
FLinearColor AIgKnightedPlayerState::GetPlayerColour() const
{
    if (PlayerColours.IsValidIndex(PlayerIndex))
    {
        return PlayerColours[PlayerIndex];
    }

    return PlayerIndex == 0 ? FLinearColor::Red : FLinearColor::Blue;
}

// ...

void AIgKnightedPlayerState::AddMultiplier(float Delta)
{
    const float Previous = Multiplier;
    Multiplier = FMath::Clamp(Multiplier + Delta, 1.f, MaxMultiplier);

    if (!FMath::IsNearlyEqual(Previous, Multiplier))
    {
        OnMultiplierChanged.Broadcast(Multiplier);
    }
}
```

The multiplier clamps at the owner, and broadcasts only on a real change. That last detail is what stopped the HUD flashing when a kill awarded a multiplier that was already at the cap.

![Blueprint 1](../../assets/images/Y1D/blueprints/F3_1.png)
![Blueprint 1](../../assets/images/Y1D/blueprints/F3_2.png)
![Blueprint 1](../../assets/images/Y1D/blueprints/F3_3.png)
![Blueprint 1](../../assets/images/Y1D/blueprints/F3_4.png)



#### ✅ Problem Solved

Each panel shows exactly one player's score, exactly once. The colour is stable across death and respawn because the object that owns it is. The "who am I?" question has a data answer (`PlayerIndex`) rather than an ordering answer.

#### 📉 Cost / Limitations / Tradeoffs

- **The widget still reaches into the Pawn for dash charges** (`GetBoundDashComponent`), because charges are body state. That is correct, but it means the widget has two sources: a durable one and a temporary one, and it must handle the temporary one being null.
- **Colour is chosen by index, not by the player.** There is no customisation; with more than two players the array runs out and falls back to red/blue.
- **`RemoveDynamic` must be called with the exact same object.** It is, in both places, but nothing enforces it. A helper that owns the binding would make this impossible to get wrong.
- **Multiplier is `float` and score is `int32`.** The rounding happens in `AddMatchScore`, so the displayed score is stable — but a multiplier like `1.05` effectively rounds in the player's favour on every kill. A fixed-point multiplier would make the maths exact.

---

### The Abilities

Four abilities, each a small rule with a place it is allowed to live. The pattern is the same every time: **input asks the Pawn, the Pawn asks the component, the component owns the state.**

| Ability | Owner | Rule | Event |
|---|---|---|---|
| Move | Pawn + `CharacterMovement` | camera-relative, normalised, no diagonal bonus | `Move` |
| Shoot | `CombatComponent` | hold to fire at weapon cadence, symmetric spread | `StartFiring` / `StopFiring` |
| Dash | `DashComponent` | i-frames first, charges, one at a time | `TryStartDash` |
| Die | Pawn presentation + `GameMode` rule | latch, one life, one respawn | `StartDeathPresentation` |

---

### Challenge 5: Move

#### ❓ What was the Problem?

The first movement implementation passed the raw 2D axis into `AddMovementInput` for both axes. Pressing W and D together moved the player `1.41×` faster than pressing W alone, because the two inputs were added as two full-length vectors.

In a bullet-hell arena, diagonal movement is not a cosmetic bug: it is the difference between dodging a ring and being hit by it, and every experienced player finds it within a minute.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I copied the template's `AddMovementInput(Forward * Value.Y + Right * Value.X)` and assumed Unreal normalised input for me. It does not: pressing forward and right moved at about 1.41 times the speed of pressing forward. My first fix normalised the raw axis, which made slow stick pushes as fast as full pushes. The second normalised before the sprint multiplier, which reintroduced the inconsistency.

What I eventually understood is an ordering rule: the input vector is not the movement vector. A deadzone, a sprint multiplier and an analogue stick all compose into the same value, so normalisation has to happen once, after all of them. The camera lesson from the Pawn challenge repeated here — with no SpringArm, control rotation's pitch is not part of movement, so the axes have to be built from yaw only.

Neither fix needed a new API. They needed me to stop assuming the engine handled a detail it does not, and to test the number (speed) rather than only the direction.

#### 💡 Solution. How Did I Approach the Problem?

Normalise after the deadzone, project onto the camera's yaw plane:

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::Move(const FVector2D& Input)
{
    if (bIsDead || Input.IsNearlyZero())
    {
        return;
    }

    // The level camera is fixed and top-down. Control rotation is its yaw, so
    // movement stays camera-relative even though the Pawn owns no camera.
    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // Normalising the 2D vector is what stops diagonal movement from being
    // 1.41x faster than cardinal movement.
    const FVector2D Normalised = Input.GetSafeNormal();

    AddMovementInput(Forward, Normalised.Y);
    AddMovementInput(Right, Normalised.X);
}
```

Speed, acceleration and turn rate are data on the Character Movement component. The feel we landed on was high acceleration and a fast turn rate (`720°/s`), because a twin-stick game should change direction immediately.

#### ✅ Problem Solved

Diagonal speed equals cardinal speed. Movement is camera-relative on every device, and the top-down camera cannot push the player into the floor.

#### 📉 Cost / Limitations / Tradeoffs

- **`AddMovementInput` still accumulates.** Two calls per frame are correct here because the vector was normalised first; calling it from two systems (say, a knockback and an input) would reintroduce the same bug.
- **Movement is input-driven only.** There is no acceleration curve authored for knockback or ice; it is one `MaxWalkSpeed`. A richer movement model would need its own component.
- **`GetControlRotation` is read every frame.** It is cheap, but it is a hidden dependency on the controller: a Pawn without one cannot move meaningfully.
- **No animation-driven root motion.** Movement and animation are separate systems; the animation blueprint reads speed, it does not drive it. Fine for this game, wrong for anything with committed attacks.

---

### Challenge 6: Shoot

#### ❓ What was the Problem?

Hold-to-fire originally spawned a projectile **per input event**, which meant the fire rate depended on how fast the player could tap. Then the fix moved the timer into the animation, and the fire rate depended on the animation's length, so a weapon tweak required an animation change.

The spread had a subtler bug: three projectiles were spawned at `0`, `+half`, `+2×half` instead of `-half`, `0`, `+half`. The gun was not shooting where the crosshair was.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

My first weapon spawned a projectile on every input event, which made the fire rate depend on how fast the player could tap. I then moved the timer into the animation because that is what the tutorial did, and the fire rate started depending on montage length — a designer could not tune the weapon without an animator. I tried a second timer driven by the montage, which only moved the coupling.

The version that worked treats cadence as gameplay data and the montage as presentation: the combat component owns a timer built from the weapon's `FireInterval`, and an animation notify is an opt-in alternative for weapons that need it. That is the engine-idiomatic split I should have reached sooner: gameplay state in C++, animation notifies as optional presentation.

Spread had a similar origin. I wrote the obvious loop — `angle = i * step` — and assumed the shots were centred because the crosshair was. Playtesting showed the fan starting at the axis and going one way. The fix was not a different step but a different parameterisation: an alpha from 0 to 1 mapped to `[-half, +half]`. The lesson was to test the maths against the picture, not to trust the first formula that compiles.

#### 💡 Solution. How Did I Approach the Problem?

The component owns cadence, driven by weapon data, and can optionally defer the actual spawn to an animation notify:

```cpp
// IgKnightedCombatComponent.cpp
void UIgKnightedCombatComponent::StartFiring()
{
    if (bIsFiring || !ActiveWeaponData)
    {
        return;
    }

    bIsFiring = true;
    FireOnce();

    if (ActiveWeaponData->FireInterval > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            FireTimer, this, &UIgKnightedCombatComponent::FireOnce, ActiveWeaponData->FireInterval, true);
    }
}

// ...

void UIgKnightedCombatComponent::FireOnce()
{
    if (!bIsFiring || !GetOwner() || GetOwner()->IsPendingKillPending())
    {
        return;
    }

    BP_PlayFireMontage();

    if (!bUseAnimationNotify)
    {
        SpawnProjectileFromMuzzle();
    }
}
```

The montage is presentation. When the weapon explicitly opts into an animation notify, the notify spawns the shot; otherwise the timer does.

And the spread is parameterised symmetrically:

```cpp
// IgKnightedCombatComponent.cpp
const int32 TotalShots = 1 + Weapon->BonusProjectiles;
const float HalfSpreadDegrees = Weapon->SpreadDegrees * 0.5f;

FActorSpawnParameters Params;
Params.Owner = GetOwner();
Params.Instigator = Cast<APawn>(GetOwner());
Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

for (int32 ShotIndex = 0; ShotIndex < TotalShots; ++ShotIndex)
{
    // One shot is dead centre. A spread is symmetric around the aim axis,
    // so three shots are -half, 0, +half — not 0, +half, +half+half.
    const float Alpha = TotalShots > 1
        ? static_cast<float>(ShotIndex) / static_cast<float>(TotalShots - 1)
        : 0.5f;

    const float AngleOffset = FMath::Lerp(-HalfSpreadDegrees, HalfSpreadDegrees, Alpha);
    const FVector Direction = BaseDirection.RotateAngleAxis(AngleOffset, FVector::UpVector);

    AIgKnightedProjectile* Projectile = GetWorld()->SpawnActor<AIgKnightedProjectile>(
        Weapon->ProjectileClass, Origin, Direction.Rotation(), Params);

    if (Projectile)
    {
        Projectile->InitializeProjectile(Direction, Weapon->ProjectileSpeed, Weapon->Damage);
    }
}
```

The `bUseAnimationNotify` flag exists because one archetype (the Mollusk) has a wind-up in its animation that *must* match the shot. The weapon opts in; the timer is the default, not the rule.


#### ✅ Problem Solved

Fire rate is a number in a data asset. With `bUseAnimationNotify` off, cadence is exact regardless of animation; with it on, the shot lands on the animation's frame. Three-shot spread is symmetric around the crosshair, and one shot is always dead centre.

#### 📉 Cost / Limitations / Tradeoffs

- **Two paths for one event** is a real cost: a weapon that opts into the notify can silently stop firing if the montage is missing or interrupted. That happened once, and the fix was a fallback timer, which now needs testing too.
- **The timer is a fixed cadence, not a curve.** No spin-up, no burst pattern, no overheat. Those are all weapon-data features that would go here.
- **Projectiles spawn at the muzzle socket and are independent actors.** No hit-scan, no homing, no pooling. For a bullet hell with tens of projectiles this is fine; at hundreds it would need an object pool.
- **The spread rotates around world up.** Correct for a top-down game; wrong for a 3D shooter with pitched aim.

---

### Challenge 7: Dash

#### ❓ What was the Problem?

The dash applied invulnerability in the first tick after the input. A hazard that resolved *this* frame — a water volume, a projectile already overlapping — therefore killed the player on the frame the dash began. Players reported it as "the dash does not work sometimes", which is the worst kind of bug report: true, intermittent, and about a frame they cannot see.

The second bug was the dash *through* enemies: the enhanced dash is supposed to kill what it passes through, and it was killing the same enemy twice when the overlap persisted for two frames.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I implemented invulnerability the way the template suggested: set a bool when the dash montage starts, clear it when the montage ends. It felt one frame late — players were hit by projectiles that visually passed through them — and I kept moving the flag earlier until I understood why. Damage in this game is pull-based: the victim asks "am I invulnerable?" when damage is resolved, so the window has to be open before movement starts, not when the animation begins.

The traversal kill was the opposite lesson. I first used `OnComponentBeginOverlap`, which missed fast dashes; then a sphere overlap every frame, which killed the same enemy on consecutive frames. Unreal's overlap callbacks do not give you swept semantics for free, so the dash has to remember which actors it has already killed during this dash and reset that set when a new dash starts.

Both fixes came from tracing a single frame and asking what had already happened by then. The engine's hooks are correct; they are just not the abstraction I assumed.

#### 💡 Solution. How Did I Approach the Problem?

The flag, the charge and the timer are set in the same function, before the first tick:

```cpp
// IgKnightedDashComponent.cpp
bool UIgKnightedDashComponent::TryStartDash()
{
    if (bDashing || CurrentCharges <= 0 || !GetOwner())
    {
        return false;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return false;
    }

    DashDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal2D();

    if (DashDirection.IsNearlyZero())
    {
        if (const IIgKnightedAimProvider* Aim = Cast<IIgKnightedAimProvider>(GetOwner()))
        {
            DashDirection = Aim->GetAimDirection().GetSafeNormal2D();
        }
    }

    if (DashDirection.IsNearlyZero())
    {
        DashDirection = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
    }

    --CurrentCharges;
    bDashing = true;

    // Invulnerability is enabled BEFORE the first frame of movement. Enabling it
    // in the first tick would let a hazard that resolves this frame kill the
    // player at the exact moment the dash "started".
    bInvulnerable = true;
    TraversedActors.Reset();

    SetComponentTickEnabled(true);

    OnDashStateChanged.Broadcast(true, true);
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);

    GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &UIgKnightedDashComponent::EndDash, DashDuration, false);
    return true;
}
```

Movement is written every frame rather than launched once, so the distance is predictable:

```cpp
// IgKnightedDashComponent.cpp
if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
{
    // Set the velocity every frame rather than launching once, so the dash
    // covers a predictable distance regardless of braking settings.
    OwnerCharacter->GetCharacterMovement()->Velocity = DashDirection * DashSpeed;
    OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
```

And the traversal keeps a set of what this dash has already hit:

```cpp
// IgKnightedDashComponent.cpp
if (TraversedActors.Contains(OtherActor))
{
    continue;
}

// The sweep can generate several overlaps across consecutive frames.
// One enemy, one kill, one entry.
TraversedActors.Add(OtherActor);

if (IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor))
{
    FIgDamageContext Context;
    Context.Instigator = GetOwner();
    Context.HitLocation = OtherActor->GetActorLocation();
    Context.Kind = EIgDamageKind::DashTraversal;

    Damageable->ApplyGameplayDamage(Context);
}
```

Charges refill **one at a time**, with a cooldown per charge, because "two charges" should mean two dashes with a gap — not a full refill after one wait.

#### ✅ Problem Solved

The dash protects the frame it starts, so it can be used *into* a hazard. Each enemy is killed once per dash. Charges are a resource with a visible refill, and the enhanced dash's extra charge is a different maximum, not a different ability.

#### 📉 Cost / Limitations / Tradeoffs

- **`bInvulnerable` is a boolean, not a timer.** It is set and cleared by the dash only. Two sources of invulnerability (a hit i-frame, say) would need a counter or a small state stack.
- **The traversal is a sphere overlap per frame during the dash.** At `2600` speed and `75` radius it is cheap, but it is still an N²-ish query in a crowd. A capsule sweep would be more precise and more code.
- **Enemies killed by traversal award score** because the damage context carries the player as instigator. That is intended, and it means the dash is a scoring tool, not only a defensive one.
- **`SetMovementMode(MOVE_Walking)` forces the mode.** A dash while airborne would snap the character to the ground. Fine in a flat arena, wrong for a platformer.

---

### Challenge 8: Die

#### ❓ What was the Problem?

Death was one of three things depending on where you looked: the character played a montage, the GameMode took a life, and the HUD showed a respawn countdown. Triggering them independently meant the montage could play twice, the life could be spent twice, or the Pawn could be destroyed before the animation finished.

There was also the "dead but shooting" bug: `bIsDead` stopped movement but not `StartFire`, so a corpse could still fire for the duration of the death animation.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I treated death as an animation. The Animation Blueprint played a death montage and a notify destroyed the actor. It worked until an interrupt skipped the notify, and the life was never spent. I then tried destroying the body immediately, which is the obvious implementation of "dead", and lost the animation.

The problem was that I had one concept doing three jobs: the rule (the team loses a life, the run may end), the state (this body must not act), and the presentation (fall, effects). Unreal encourages this confusion because animation notifies can call gameplay code. The best-practice split I eventually adopted is that the Pawn latches `bIsDead` and asks the GameMode to spend the life at the moment damage is lethal; presentation reads that state and is never allowed to decide it.

That also solved the respawn case: the body can stay for the animation because "dead" is a flag, not a destroyed actor. The animation became a consumer, which is what it should have been from the start.

#### 💡 Solution. How Did I Approach the Problem?

The Pawn only presents; the GameMode decides:

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::StartDeathPresentation()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    Combat->StopFiring();
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // The animation notify calls FinishDeath() at the frame where the body hits
    // the floor, so the corpse does not vanish mid-animation.
    BP_OnDeathStarted();
}

void AIgKnightedPlayerCharacter::FinishDeath()
{
    Destroy();
}
```

Every entry point is gated by `bIsDead`, including the ability entry points, so "dead" is a property of the body rather than a set of disabled inputs:

```cpp
// IgKnightedPlayerCharacter.cpp
void AIgKnightedPlayerCharacter::StartFire()
{
    if (!bIsDead && Combat)
    {
        Combat->StartFiring();
    }
}

// ...

void AIgKnightedPlayerCharacter::TryDash()
{
    if (!bIsDead && Dash)
    {
        Dash->TryStartDash();
    }
}
```

The GameMode's `HandlePlayerEliminated` is the rule: it latches on the Pawn, spends one life, calls `StartDeathPresentation`, and schedules the respawn — or finishes the match.

![Blueprint 1](../../assets/images/Y1D/blueprints/F5.png)

#### ✅ Problem Solved

One death spends one life and plays one presentation. A dead Pawn cannot move, fire, dash or be damaged again. The respawn comes from the controller (which survives), not from the corpse.

#### 📉 Cost / Limitations / Tradeoffs

- **The presentation can hang.** If the death montage never reaches its notify (interrupted, missing asset), the Pawn is never destroyed. In practice `DeathPresentationSeconds` on enemies and a respawn timer on players bound this, but it is a real failure mode.
- **`Destroy()` in a notify is abrupt.** It works because nothing else references the Pawn at that moment; a pooled Pawn would return to the pool instead.
- **There is no death camera or slow-motion.** The shared camera is fixed, so the presentation is limited to the character's animation and the HUD.
- **Respawn is on a timer, not a checkpoint.** The player returns to the start of the current part with full abilities (the status and dash components reset). That is a design choice; a checkpoint system would need level-side state.

---

### 💥 Impact of Contribution

**Before:** input died after transitions, gamepad aim was world-relative, the character had a camera it did not need, HUD counters doubled after respawn, diagonal movement was 41% faster, fire rate depended on tapping, spread was asymmetric, the dash protected the frame *after* it started, and a dead player could still shoot.

**After:** the player layer has a contract at every seam. The controller owns devices; the Pawn owns the body; each ability owns exactly one resource; the HUD binds to a durable identity rather than to a body. Two players, two devices and two HUD panels work through the same code path, and every ability is implemented once.

Most of what I actually learned here was not Unreal API. It was that **a feature is not finished when it works — it is finished when the next feature cannot break it.**

---

## 📋 03. Powerups

![IgKnighted — collectable pickups, the shield and the Power Shot buff](../../assets/images/Y1D/6.png)

### 🎯 What was the Goal?

Power-ups are the game's only positive resource. A pickup must:

1. Be **readable**: the player should know what they are walking into before they touch it.
2. Be **refusable**: walking over a shield while already shielded must not waste the pickup or silently stack.
3. Be **temporary** except where the design says otherwise, and expire in a way the player can see coming.
4. Be **fair in co-op**: whichever player touches it gets it, and the other player sees why.

There are three buffs and one instant award:

| Pickup | Effect | Duration | Stacking rule |
|---|---|---|---|
| **Shield** | Absorbs the next lethal hit | until spent | cannot pick up a second while shielded |
| **Power Shot** | Replaces the weapon with a stronger one | 10 s | cannot refresh while active |
| **Enhanced Dash** | Two dash charges instead of one | 10 s | cannot refresh while active |
| **Matchstick** | Instant score | — | always collectable |

The design constraint that shaped the code: **a buff is a state, not an event.** "You have a shield" has to be observable by the damage resolver, the HUD and the pickup itself, without any of them owning it.

---

### Challenge 1: Collectables

#### ❓ What was the Problem?

The first implementation had a Blueprint per pickup: `BP_ShieldPickup`, `BP_PowerShotPickup`, `BP_DashPickup`, `BP_MatchstickPickup`. Each had its own overlap event, its own "already collected" boolean and its own respawn timer. They drifted immediately:

- One respawned after 8 seconds, another after 12, because nobody wrote the number down.
- Two of them checked "can this player take it?" and two did not, so a shielded player could waste a shield pickup.
- The score pickup awarded 25 points, but one of the others accidentally awarded score too, because it had been copied from the matchstick.

And a single touch could collect a pickup twice: the sphere and the mesh both generated overlap events, so one walk-over awarded the matchstick's 25 points twice. With a 12-second respawn, that doubled the intended score income, and a playtester noticed within a minute.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I built the first pickups as separate Blueprints because that is how most tutorials present them, and it was fine for two. By the fourth, the same overlap, hide/show and respawn logic existed four times, and a bug fixed in one was still present in the others. I then tried a data-driven base class with a `BuffKind` switch in Blueprint; the logic was shared but the switch still meant every new buff touched one more place.

The version that worked is one pickup actor plus a `BuffKind` and a duration, with the effects applied by the status component. That is the Unreal-idiomatic shape — an actor that describes a pickup, data that configures it, and a component that owns the effect — and it only became obvious after duplicating the alternative.

The double-collect bug came from the same lack of precision. I assumed only the mesh had collision; the sphere also generated overlaps, so one touch ran the collect path twice. The lesson was to have exactly one component be the authority for gameplay overlap and to check the collision setup in the editor instead of assuming what the components do.

#### 💡 Solution. How Did I Approach the Problem?

One class, one overlap source, and the refusal question asked of the component that owns the buff:

```cpp
// IgKnightedPowerUpPickup.cpp
void AIgKnightedPowerUpPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(OtherActor);
    if (!bAvailable || !Player || !CanBeCollectedBy(Player))
    {
        return;
    }

    Collect(Player);
}

bool AIgKnightedPowerUpPickup::CanBeCollectedBy(const AIgKnightedPlayerCharacter* Player) const
{
    if (!Player || Player->IsDead_Implementation())
    {
        return false;
    }

    if (BuffKind == EIgBuffKind::Matchstick)
    {
        return true;
    }

    const UIgKnightedStatusComponent* Status = Player->GetStatusComponent();
    return Status && Status->CanAcceptBuff(BuffKind);
}
```

The pickup asks; the status component answers. The pickup does not know what a shield is, only that a Shield buff might be refused.

```cpp
// IgKnightedPowerUpPickup.cpp
void AIgKnightedPowerUpPickup::SetPickupAvailable(bool bNewAvailable)
{
    bAvailable = bNewAvailable;
    SetActorHiddenInGame(!bNewAvailable);
    SetActorEnableCollision(bNewAvailable);
}
```

The mesh is decoration only; the sphere is the single authority for overlap:

```cpp
// IgKnightedPowerUpPickup.cpp
Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
Mesh->SetupAttachment(OverlapSphere);
Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
```

That one line (`NoCollision` on the mesh) was the fix for the doubled pickup: with both the sphere and the mesh generating overlap events, one touch called `Collect` twice.

> **📸 Blueprint capture:** `BP_PowerUpPickup` — the `BuffKind` and duration exposed on the instance. *(Replace this block with a blueprintue.com screenshot or embed.)*

#### ✅ Problem Solved

One class, four data configurations. All pickups refuse correctly, respawn on the same schedule, and the score pickup cannot be collected twice in one touch. Changing a duration is a data edit, not a Blueprint edit, and there is no second implementation to forget.

#### 📉 Cost / Limitations / Tradeoffs

- **The pickup still knows about `EIgBuffKind`.** It is a small dependency and it is why `Matchstick` needs a special case ("instant award, not a status"). A pure "effect object" (`UIgPickupEffect` with `CanApply` / `Apply`) would remove the branch and is the cleaner design at the next scale.
- **Respawn is unconditional.** Every pickup comes back after 12 seconds, including one the player deliberately refused. Design wanted a steady supply; a "consumed for the run" pickup would need a different flag.
- **Availability is a timer, not a "player has left" latch.** A player standing exactly where a matchstick respawns may re-collect it. It is at most 25 points per 12 seconds and it is not exploitable enough to fix with a leave-volume latch, but it is not *correct* either.
- **No placement validation.** A pickup can be placed inside geometry and will never be collectable. Nothing catches that at build time.

---

### Challenge 2: Shield

#### ❓ What was the Problem?

The shield was a `bool`. It worked until the design asked two questions that a boolean cannot answer:

1. "Can the shield absorb more than one hit?" — it could not, but the bar looked like it might.
2. "Did the shield break, and should the HUD show that?" — there was no event for "the shield is gone"; the HUD polled the bool each frame.

There was a worse bug in the damage order. The shield was checked **before** dash invulnerability, so dashing through a shield-breaking hit consumed the shield and left the player without either. Players experienced it as "my shield disappeared without protecting me", which was exactly true.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I implemented the shield as a bool on the Character because that was the smallest thing that worked, then moved it to the status component when the damage path needed it. The ordering bug came from writing the checks in the order I thought of them: shield first, then dash, then the lethal branch. Players reported their shield disappearing without protecting them, and they were right — dashing through a hazard consumed the shield because the dash check came later.

I tried moving the dash check above the shield and the bug disappeared, but I did not understand it yet. The insight was that a damage pipeline's order is a design statement: dash is the skill expression, so it must be free and must not spend the shield. Once I wrote that down, the resolver became a short list of priorities instead of a sequence I had to remember.

The other lesson was ownership: the shield is a status, not a weapon or a movement ability. Putting it on the resolver would have made the resolver stateful, which is the one thing it must not be. Unreal does not enforce that; it is a design decision I had to make explicitly.

#### 💡 Solution. How Did I Approach the Problem?

The shield became a charge count with one event, owned by the status component:

```cpp
// IgKnightedStatusComponent.cpp
bool UIgKnightedStatusComponent::ConsumeShield()
{
    if (ShieldCharges <= 0)
    {
        return false;
    }

    --ShieldCharges;
    OnShieldChanged.Broadcast(ShieldCharges);
    return true;
}
```

The resolver encodes the priority once:

```cpp
// IgKnightedDamageResolver.cpp
// 1. Dash i-frames. Checked first because dashing is a free action: it must
//    not consume the shield, and it must work even while shielded.
if (UIgKnightedDashComponent* Dash = Victim->FindComponentByClass<UIgKnightedDashComponent>())
{
    // Queried at resolution time, not at delivery time: a projectile that
    // was fired before the dash started still counts as dodged.
    if (Dash->IsInvulnerable())
    {
        Result.Outcome = EIgDamageOutcome::Ignored;
        OnDamageIgnored.Broadcast(Victim, Context);
        return Result;
    }
}

// 2. Shield. A consumable resource, spent once, before the lethal branch.
if (UIgKnightedStatusComponent* Status = Victim->FindComponentByClass<UIgKnightedStatusComponent>())
{
    if (Status->ConsumeShield())
    {
        Result.Outcome = EIgDamageOutcome::Absorbed;
        OnShieldBroken.Broadcast(Victim);
        return Result;
    }
}
```

The resolver itself is a **world subsystem with no state** — it reads components and broadcasts outcomes. That is why the water volume, the projectile and the melee attack all produce identical behaviour without knowing about each other.

#### ✅ Problem Solved

The shield absorbs exactly one hit, emits one event, and is spent only after the dash check. The HUD stops polling. The damage pipeline has a documented order that every damage source inherits.

#### 📉 Cost / Limitations / Tradeoffs

- **`ShieldCharges` is an `int32` that can only be 0 or 1.** It is a resource for a design that currently has one charge; a "shield with three hits" would work without touching the resolver, which was the point.
- **The shield has no duration.** It lasts until spent, so a cautious player can carry it across the whole zone. That is the design; it also means the pickup is worth more the better you are, which inverts the difficulty curve slightly.
- **No damage types bypass the shield.** Everything is absorbed equally. A "shield-piercing" attack would need the resolver to ask the context, not the component.
- **The HUD learns about the shield from two places**: `OnShieldChanged` and `OnShieldBroken` (from the resolver). They are different events — "your shield went down" vs "your shield was spent" — and the UI has to know that. A single outcome event on the victim would unify them.

---

### Challenge 3: Power Shot

#### ❓ What was the Problem?

Power Shot replaced the player's weapon with a stronger one and put it back after ten seconds. The first implementation set `ActiveWeaponData` directly, which had a side effect nobody expected: the fire timer kept running with the *old* cadence, and if the player was holding fire during the swap, the weapon switched but the rate did not.

Worse, two Power Shot pickups in quick succession started two expiry timers. The first timer fired and restored the base weapon while the second buff was still supposed to be active. The player lost the buff early with no feedback.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I swapped `ActiveWeaponData` directly because that is how the weapon is set at spawn, and it worked until a player held fire while picking up Power Shot. The swap happened, but the cadence timer kept running on the old interval. My first fix restarted the timer from the pickup, which leaked knowledge of the combat component into the pickup and duplicated the cadence rule.

The component already had `StopFiring` and `StartFiring`, so the fix was to route the swap through one function that remembered whether the player was firing and rebuilt the timer around the new weapon. That is the pattern I should have used from the beginning: one entry point per state change, and the owner preserves the invariants.

The double-buff bug was the same pattern applied to legality. I let two pickups start two expiry timers because there was no rule saying a second Power Shot must be refused. Adding `CanAcceptBuff` moved that decision to the status component, where the duration lives. Unreal's delegates and timers do not protect you from two owners; the API is indifferent.

#### 💡 Solution. How Did I Approach the Problem?

Weapon swaps go through one function that preserves the trigger state:

```cpp
// IgKnightedCombatComponent.cpp
void UIgKnightedCombatComponent::SetActiveWeaponData(UIgKnightedWeaponData* NewWeapon)
{
    if (!NewWeapon)
    {
        return;
    }

    const bool bWasFiring = bIsFiring;
    StopFiring();

    ActiveWeaponData = NewWeapon;

    if (bWasFiring)
    {
        StartFiring();
    }
}
```

Preserving `bWasFiring` and restarting the cadence is what keeps the trigger held across the swap; the stop/start is also what rebuilds the timer around the new weapon's `FireInterval`.

And the status component refuses a second application of the same buff:

```cpp
// IgKnightedStatusComponent.cpp
bool UIgKnightedStatusComponent::CanAcceptBuff(EIgBuffKind Kind) const
{
    switch (Kind)
    {
    case EIgBuffKind::Shield:
        // A second shield while shielded is not consumed. The pickup asks this
        // question; it does not decide the answer.
        return ShieldCharges == 0;

    case EIgBuffKind::PowerShot:
        return Combat && PowerShotWeaponData && Combat->GetWeaponData() != PowerShotWeaponData;

    case EIgBuffKind::EnhancedDash:
        return !bEnhancedDashActive;

    case EIgBuffKind::Matchstick:
        return true;

    default:
        return false;
    }
}
```

Expiry restores the base weapon, and the timer is cleared on respawn before it can fire against a new body:

```cpp
// IgKnightedStatusComponent.cpp
void UIgKnightedStatusComponent::ExpirePowerShot()
{
    if (Combat)
    {
        Combat->RestoreBaseWeaponData();
    }

    OnBuffExpired.Broadcast(EIgBuffKind::PowerShot);
}
```

#### ✅ Problem Solved

Holding fire through a Power Shot pickup keeps firing, at the new weapon's cadence. The buff lasts exactly one duration, a second pickup is refused rather than silently replacing the timer, and the HUD gets an expiry event to show the countdown ending.

#### 📉 Cost / Limitations / Tradeoffs

- **The buff is implemented as a weapon swap, not as a modifier.** That means it cannot stack with another weapon buff, and it resets any custom weapon state the player had. It also means the damage numbers live in a separate weapon asset that has to stay in sync with the base weapon's feel.
- **`CanAcceptBuff` compares pointers**, so re-picking the same Power Shot asset is refused, but a different asset with the same effect would be accepted. Identity is the asset, not the buff kind.
- **The expiry timer is cleared on respawn**, which is correct, but there is no "remaining time" API. A HUD countdown has to be driven by the buff event and reconstructed, or polled from a value the component does not expose.
- **No visual weapon swap hook.** `BP_PlayFireMontage` plays the base montage; the upgraded weapon looks different in data but the animation is the same. A full weapon visual would need a socket swap and an anim layer.

---

### Challenge 4: Enhanced Dash

#### ❓ What was the Problem?

Enhanced Dash raised `MaxCharges` from 1 to 2 for ten seconds. The first version set `MaxCharges = 2` and left `CurrentCharges` alone. A player who had just dashed had one charge and picked up the buff — and still had one charge, so the buff appeared to do nothing until the next recharge.

The second version set `CurrentCharges = MaxCharges` on pickup, which fixed the first problem and created an exploit: dash, pick up the buff, get two charges, dash twice, pick up the next buff. The buff was a free refill rather than an upgrade.

Then there was expiry. If the buff expired while the player had two charges, `MaxCharges` dropped to 1 but `CurrentCharges` stayed at 2, and the HUD showed `2/1`.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I changed `MaxCharges` when the buff started and assumed `CurrentCharges` would follow. It does not — it is a separate value — so the HUD showed 1/2 after the pickup and the buff looked broken. I then set `CurrentCharges = MaxCharges` on activation, which fixed the display and created a free refill: dash, pick up the buff, get two charges, dash twice.

Expiry was the third version of the bug: `MaxCharges` dropped but `CurrentCharges` stayed above it, and the HUD showed 2/1. I tried clearing the recharge timer in two places and the components disagreed about whether the buff was active.

What I eventually understood is the same lesson as the weapon swap: every writer of a derived value has to go through one entry point, and the timer must have exactly one owner. The `DashComponent` now clamps in both directions and broadcasts once, and the status component owns the duration. That is not something Unreal enforces; it is a contract the two components have to keep.

#### 💡 Solution. How Did I Approach the Problem?

The maximum change is a single entry point, and it clamps in both directions:

```cpp
// IgKnightedDashComponent.cpp
void UIgKnightedDashComponent::SetEnhancedDashActive(bool bActive)
{
    if (bEnhancedDashActive == bActive)
    {
        return;
    }

    bEnhancedDashActive = bActive;
    MaxCharges = bActive ? EnhancedMaxCharges : BaseMaxCharges;

    if (bActive)
    {
        CurrentCharges = MaxCharges;
    }
    else
    {
        CurrentCharges = FMath::Min(CurrentCharges, MaxCharges);
    }

    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);
}
```

The refill in that function is deliberate, and it was a design decision made after playtesting: without it, picking up the buff while a charge was spent felt like nothing had happened. It is safe only because the buff is rare and cannot be refreshed, so it is a reward rather than a loop. The clamping on expiry is the part that has to be right either way.

And respawn resets the whole edge state, so a buff cannot leak into the next life:

```cpp
// IgKnightedDashComponent.cpp
void UIgKnightedDashComponent::ResetForRespawn()
{
    GetWorld()->GetTimerManager().ClearTimer(DashTimer);
    GetWorld()->GetTimerManager().ClearTimer(RechargeTimer);

    bDashing = false;
    bInvulnerable = false;
    DashDirection = FVector::ZeroVector;
    TraversedActors.Reset();
    SetComponentTickEnabled(false);

    CurrentCharges = MaxCharges;

    OnDashStateChanged.Broadcast(false, false);
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);
}
```

`StatusComponent::ResetForRespawn` clears the buff timers and calls `Dash->SetEnhancedDashActive(false)`, so the maximum is restored before the next spawn.

#### ✅ Problem Solved

The buff visibly adds a charge the moment it is picked up, the HUD never shows more charges than the maximum, and dying with the buff active cannot leave the next body upgraded. Expiry is a single path through one flag.

#### 📉 Cost / Limitations / Tradeoffs

- **The buff changes a maximum, and maxima are read in several places.** All readers now tolerate `MaxCharges` changing, but nothing enforces it — a new reader that caches the maximum would reintroduce the bug.
- **The activation refill is a design choice, not a rule.** It is documented in a comment because it looks like a bug. A future designer could disagree, and the code would not push back.
- **Two components know about "enhanced dash"**: `StatusComponent` owns the duration, `DashComponent` owns the effect. The coupling is a direct pointer found in `BeginPlay`, not an interface, so a dash component on something without a status component silently does nothing.
- **No stacking, no refresh.** Ten seconds is ten seconds. If a player picks up a second one at second nine, it is refused; that is the intended rule and it is invisible in the UI.

---

### 💥 Impact of Contribution

**Before:** four pickups with four implementations, two of which respawned on different schedules; a score exploit from a double overlap; a shield that could be wasted by dashing; a Power Shot that could be cancelled early by its own second pickup; and a dash buff that showed `2/1` on the HUD after expiry.

**After:** one pickup class, one status component, one resolver and one set of events. Adding a buff is a new enum value and a branch in `CanAcceptBuff` / `TryApplyBuff` — not a new Blueprint, timer and HUD binding. The damage pipeline's priority order is written down once and inherited by every source, which is what made the later enemy work possible.

---

## 📋 04. Enemies

![IgKnighted — the three enemy archetypes and the shared behaviour tree](../../assets/images/Y1D/5.png)

### 🎯 What was the Goal?

Three archetypes, each asking the player a different question:

| Archetype | Role | Movement | Attack | The question it asks |
|---|---|---|---|---|
| **Crawler** | Rusher | Fast, direct | Melee swing, short telegraph | "Can you keep space?" |
| **Scorpion** | Skirmisher | Keeps a preferred range | Single aimed projectile | "Can you dodge a shot aimed at you?" |
| **Mollusk** | Artillery | Slow, area denial | Radial ring of projectiles | "Can you read a pattern and commit?" |

The design constraint was that **the archetypes must not be three codebases**. They share a body, a damage pipeline, a death path, a score award and a brain. What differs is numbers and a Behaviour Tree — not the rules.

That constraint is what made the enemy work testable. If a bug appeared in "enemy dies", it appeared in all three, or it appeared in none. There was no third implementation to forget.

---

### Challenge 1: The Crawler

#### ❓ What was the Problem?

The Crawler's first implementation damaged the player whenever the two capsules overlapped. That is the most natural way to write a melee enemy: it touches you, you take damage.

It also meant that a Crawler standing on a player dealt damage **every frame** — sixty times a second. With no health bar, the shared pool of five lives was gone in under a tenth of a second. The first playtest with a Crawler in the room ended with the tester staring at the Game Over screen, asking what had hit them.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

My first melee used `OnComponentBeginOverlap`, because that is the node everyone reaches for. It fired when the capsules touched, not when the swing landed, so damage happened before the animation; and a player standing still inside the enemy took exactly one hit, which sounds correct until the enemy spawns on top of them and never generates a begin-overlap at all.

I tried a timer on begin-overlap, then moved the damage to the attack animation's notify and did a single sphere overlap in front of the enemy at that moment. The notify version matched the animation and made re-entry irrelevant, which taught me that an overlap callback is an entry event, not a "contact is happening" state. The overlap is continuous; the attack is discrete.

The filtering bug was a separate lesson. I first cast to `AIgKnightedPlayerCharacter` in the melee code, which coupled the enemy to the player class. The damage interface exists precisely so an enemy does not need to know who it hit; the tag check belongs behind that interface. I moved it there after the cast made a second damage source awkward to add.

#### 💡 Solution. How Did I Approach the Problem?

Contact damage was removed entirely. A Crawler deals damage only from the animation notify, via an explicit sphere query:

```cpp
// IgKnightedEnemyCharacter.cpp
void AIgKnightedEnemyCharacter::PerformMeleeAttack()
{
    if (!Definition || bDefeated)
    {
        return;
    }

    // Deliberately NOT a capsule overlap. A crawler standing on a player used to
    // deal damage every frame; this is only called from the attack animation's
    // notify, so one swing is one hit.
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(IgMeleeSwing), false, this);
    const FCollisionShape Shape = FCollisionShape::MakeSphere(Definition->MeleeRadius);
    const FVector Centre = GetActorLocation() + GetActorForwardVector() * 40.f;

    GetWorld()->OverlapMultiByChannel(
        Overlaps, Centre, FQuat::Identity, ECC_Pawn, Shape, Params);

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* OtherActor = Overlap.GetActor();
        if (!IsValid(OtherActor) || !OtherActor->ActorHasTag(TEXT("Ig.Player")))
        {
            continue;
        }

        if (IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor))
        {
            FIgDamageContext Context;
            Context.Instigator = this;
            Context.HitLocation = OtherActor->GetActorLocation();
            Context.Kind = EIgDamageKind::EnemyContact;

            Damageable->ApplyGameplayDamage(Context);
        }
    }
}
```

Three details carry the fix:

- **`bDefeated` is checked first.** A dying Crawler's montage can still fire a notify; a dead enemy must not damage anyone.
- **The query is tagged `Ig.Player`, not cast to a class.** The melee code does not include `IgKnightedPlayerCharacter.h`.
- **The notify is the clock.** One swing is one call, so the rate is the animation's rate, which is a number designers can tune without touching damage code.

![Blueprint 1](../../assets/images/Y1D/blueprints/F8_1.png)
![Blueprint 1](../../assets/images/Y1D/blueprints/F8_2.png)


#### ✅ Problem Solved

A Crawler deals one hit per swing, no matter how long it stands on the player. The same notify-based path works for a future enemy with a different montage. Death gates the attack without any input state.

#### 📉 Cost / Limitations / Tradeoffs

- **The attack is timing-dependent.** If the montage is interrupted before the notify, the swing deals no damage even though it visibly connected. That is the correct trade for a telegraphed attack, but it means animation and combat are coupled through the notify and must be tested together.
- **The sphere is offset forward by 40 cm.** It is an approximation of a claw swing, tuned by eye. A proper hit volume would be a socket-aligned capsule on the mesh.
- **The Crawler has no "back off" behaviour in this sample.** It closes distance and swings; the rusher pressure comes from speed and numbers, not from a decision to disengage.
- **A dead Crawler is still queryable for one frame.** `bDefeated` guards damage, but the capsule collision is disabled only inside `HandleDefeated`, so the order of operations matters if the notify and the death land in the same frame.

---

### Challenge 2: The Scorpion

#### ❓ What was the Problem?

The Scorpion shot at the player's position from the blackboard. That is exactly what a Behaviour Tree tutorial tells you to do: write the target's location to a key, have the fire task read the key.

The result was an enemy that could not hit a moving player. The blackboard location is written when targeting re-evaluates — once per interval, and again when the target changes — so by the time the shot spawned, the location was up to a second old. Against a player strafing at full speed, every shot landed where the player had been.

Worse, at close range the shot was aimed from the Scorpion's **actor origin**, while the projectile spawned at the **muzzle socket**. The direction and the origin disagreed, so the shot passed visibly to one side of the crosshair.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

Because a Behaviour Tree can only read blackboard keys, I wrote the target's live location into the blackboard every tick and aimed from that value. The shots trailed behind a strafing player, and I kept increasing how often I wrote the key, which made the tree more expensive without making the aim correct.

I tried aiming directly at the actor from the tree, which meant adding a new task for something that is not a decision. The split I eventually understood is that the blackboard is for decisions — where to move, whether to attack, what the last known location was — and the world is the source for facts that must be current at fire time. The enemy's weapon asks the targeting component for the live actor when it fires; the tree keeps using the snapshot for movement.

That is the Unreal-idiomatic use of a blackboard: a projection of state the tree needs, not a cache of everything the enemy knows. I had been treating it as the latter because it was the only place the tree could read.

#### 💡 Solution. How Did I Approach the Problem?

The fire task asks the Pawn to fire, and the Pawn reads the live target:

```cpp
// IgKnightedEnemyCharacter.cpp
void AIgKnightedEnemyCharacter::FireAtTarget(AActor* Target)
{
    if (!Definition || !Definition->ProjectileClass || !IsValid(Target))
    {
        return;
    }

    const FVector Origin = GetMuzzleLocation();
    const FVector AimPoint = Target->GetActorLocation();
    const FVector Direction = (AimPoint - Origin).GetSafeNormal();

    if (Direction.IsNearlyZero())
    {
        return;
    }

    // Spawn at the muzzle and aim at the target's location *now*. The enemy's
    // facing is cosmetic: a body mid-turn must not decide where the shot goes.
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (AIgKnightedProjectile* Shot = GetWorld()->SpawnActor<AIgKnightedProjectile>(
            Definition->ProjectileClass, Origin, Direction.Rotation(), Params))
    {
        Shot->InitializeProjectile(Direction, Definition->ProjectileSpeed, Definition->ProjectileDamage);
    }
}
```

The Behaviour Tree still uses the blackboard, but only for decisions:

```text
Selector
├── Sequence [HasTarget?]
│   ├── Move To (TargetActor)        ← reads the live actor, not a location
│   ├── Wait (AttackCooldown)
│   └── Task: Fire At Target          ← queries the Pawn for the live position
└── Sequence [no target]
    └── Wait (EvaluationInterval)
```

`LastKnownLocation` still exists, and it is used for exactly one thing: chasing a player who has just broken line of sight. It is a memory, not an aim point.



#### ✅ Problem Solved

Scorpions hit a strafing player, because the shot is aimed at the player's current position from the muzzle. The shot and the visual agree. The split is explicit: blackboard = decisions and memory, live actor = aim.

#### 📉 Cost / Limitations / Tradeoffs

- **A projectile aimed at the current position still misses a player who changes direction after the shot.** That is correct for a projectile — leading the target would make the Scorpion unfair at range — but it means the Scorpion is a pressure enemy, not a sniper.
- **`AlwaysSpawn` can place a projectile inside geometry** if the muzzle is embedded in a wall. The projectile will then collide immediately and disappear. The alternative (adjust-if-possible) can teleport a shot to a weird origin, which looks worse.
- **There is no fire cone or facing requirement.** The Scorpion can shoot backwards while retreating. The animation covers it visually, but the rule permits it. A `Dot(Forward, ToTarget)` gate would be one line, and would make the turn rate matter.
- **Prediction is not modelled.** Players learn quickly that strafing beats Scorpions entirely; the archetype's difficulty comes from numbers and other enemies, not from the projectile itself.

---

### Challenge 3: The Mollusk

#### ❓ What was the Problem?

The Mollusk fires a burst of projectiles in a circle. The first implementation spawned eight projectiles at `i * 45°`, starting from the actor's forward vector. Two things were wrong:

1. The ring's gaps were always in the same world directions, so a player could stand in one of them and never be hit. Eight shots looked like a threat and were not.
2. The shots spawned at the actor origin rather than the muzzle, so at close range the first projectile appeared behind the player's character and the ring looked off-centre.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I built the radial attack by looping the shot count and multiplying by a step, which produced a fan that started at the aim axis. In the editor it looked like a ring; in play the gap rotated with the target, so the "ring" followed the player instead of surrounding them. I tried rotating the whole pattern by the actor's yaw, which made the gap track the player's position — worse, because now the safe spot moved with them.

The fix was to parameterise around the full circle with an alpha, then offset the whole ring toward the target so one shot is always aimed at them. That is a design choice as much as a maths choice: an evenly spaced ring is fair, and an evenly spaced ring with one aimed shot is readable.

The origin bug was the same kind of assumption. I spawned from the actor location, which looks right in a wide shot and wrong at melee range. Moving the origin to the muzzle socket made the ring start where the projectiles visibly start. Both fixes came from testing at the distances players actually fight at.

#### 💡 Solution. How Did I Approach the Problem?

The ring is offset so one projectile points at the target, and every shot spawns at the muzzle:

```cpp
// IgKnightedEnemyCharacter.cpp
void AIgKnightedEnemyCharacter::FireRadialBurst()
{
    if (!Definition || !Definition->ProjectileClass)
    {
        return;
    }

    const int32 ShotCount = FMath::Max(1, Definition->RadialShotCount);

    // A ring is not a spread. Every shot is spawned relative to the last one by
    // 360/Count, and the whole ring is offset so one projectile points at the
    // player. Rotating the ring by a random amount would make the pattern
    // unreadable, which is the opposite of a telegraphed attack.
    const AActor* Target = Targeting ? Targeting->GetCurrentTarget() : nullptr;
    const float BaseYaw = Target
        ? (Target->GetActorLocation() - GetActorLocation()).Rotation().Yaw
        : GetActorRotation().Yaw;
    const float Step = 360.f / static_cast<float>(ShotCount);

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
    {
        const FRotator ShotRotation(0.f, BaseYaw + Step * ShotIndex, 0.f);
        const FVector Direction = ShotRotation.Vector();

        if (AIgKnightedProjectile* Shot = GetWorld()->SpawnActor<AIgKnightedProjectile>(
                Definition->ProjectileClass, GetMuzzleLocation(), ShotRotation, Params))
        {
            Shot->InitializeProjectile(Direction, Definition->ProjectileSpeed, Definition->ProjectileDamage);
        }
    }
}
```

The base yaw is computed from the Mollusk's centre, but every projectile spawns from the muzzle. At this scale the difference is centimetres; if the muzzle were far off-centre, the ring would need to be offset from the muzzle instead, and the first shot would no longer point exactly at the player.



#### ✅ Problem Solved

Eight shots form a ring with a gap, and the gap moves with the player. Standing still is never safe, but the pattern is learnable: one shot comes at you, and the dodge is lateral. Spawn origin is consistent, so the pattern looks like a pattern at every range.

#### 📉 Cost / Limitations / Tradeoffs

- **The ring is centred on the shooter, so a target very close to the Mollusk is inside the ring's origin.** At point-blank range the projectiles spawn past the player. A minimum range or an inner ring would fix it; the archetype instead relies on the player not being that close, which is a design assumption, not an enforced rule.
- **`RadialShotCount` is a number in data, and the ring's density is a difficulty knob the difficulty assets do not currently touch.** Tuning it per difficulty would need a per-difficulty definition, which does not exist yet.
- **Every projectile is an actor with its own collision and movement.** Fine at eight per burst; a bullet-hell pattern with hundreds would need a pooled system.
- **No telegraph beyond the animation.** The attack is readable because the Mollusk is slow, not because the code guarantees a wind-up. If the montage is skipped, the ring appears instantly.

---

### Challenge 4: The AI Controller

#### ❓ What was the Problem?

The targeting component cached a target and the AI controller wrote it to the blackboard. That is two copies of the same fact, and they disagreed in one specific case: **the target died**.

The player's Pawn is destroyed after the death presentation. The targeting component's next evaluation correctly found no target and broadcast `OnTargetChanged(nullptr)`. But the blackboard key still held the destroyed actor, because the controller was not clearing it — nothing told the tree that the target was gone. Enemies kept moving to the corpse's last location and firing at it. In a two-player game they ignored the surviving player until the next evaluation happened to pick them up.

The second bug was subtler: when the controller was re-possessed, it bound `OnTargetChanged` again without unbinding, so one target change ran the handler twice. Harmless for a write, fatal for anything with a side effect.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I assumed the blackboard would keep itself in sync because the tree was the AI. It does not: a key is a snapshot, and if nobody writes it, the tree reads the old value forever. My versions either wrote the target only on change and left a stale key after the target died, or cleared it from the enemy's death code, which split ownership between the enemy and the controller.

The lifecycle was the second lesson. `OnPossess` runs on spawn and respawn; `OnUnPossess` runs when the Pawn dies or is replaced. Binding in `OnPossess` without unbinding in `OnUnPossess` accumulates handlers, and `AddDynamic` does not warn. Once I read how possession and the blackboard component relate, unbinding became as important as binding.

The best-practice shape is a controller that projects the targeting component's state into the blackboard — including the explicit "no target" case — and owns the binding for exactly as long as it possesses the body.

#### 💡 Solution. How Did I Approach the Problem?

Every write to the blackboard goes through one function, including the null case:

```cpp
// IgKnightedAIController.cpp
void AIgKnightedAIController::WriteTargetToBlackboard(AActor* NewTarget)
{
    UBlackboardComponent* Blackboard = GetBlackboardComponent();
    if (!Blackboard)
    {
        return;
    }

    // The blackboard is a projection of the targeting component, never a second
    // source of truth. Every write goes through this function.
    Blackboard->SetValueAsObject(TargetKey, IsValid(NewTarget) ? NewTarget : nullptr);
    Blackboard->SetValueAsBool(HasTargetKey, IsValid(NewTarget));

    if (IsValid(NewTarget))
    {
        Blackboard->SetValueAsVector(LastKnownLocationKey, NewTarget->GetActorLocation());
        SetFocus(NewTarget, EAIFocusPriority::Gameplay);
    }
    else
    {
        // Without this the tree keeps chasing the location of a player that
        // ceased to exist — the "enemy attacks a corpse" bug.
        ClearFocus(EAIFocusPriority::Gameplay);
    }
}
```

The controller seeds from current state on possession and unbinds on unpossession:

```cpp
// IgKnightedAIController.cpp
void AIgKnightedAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

    AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(InPawn);
    if (!Enemy)
    {
        return;
    }

    if (UIgKnightedTargetingComponent* Targeting = Enemy->GetTargetingComponent())
    {
        BoundTargeting = Targeting;
        Targeting->OnTargetChanged.AddUniqueDynamic(this, &AIgKnightedAIController::HandleTargetChanged);

        // Seed from current state: the component may already have a target from
        // a previous possession, and a blackboard that starts empty makes the
        // tree idle for one evaluation interval for no reason.
        WriteTargetToBlackboard(Targeting->GetCurrentTarget());
    }
}

void AIgKnightedAIController::OnUnPossess()
{
    if (UIgKnightedTargetingComponent* Targeting = BoundTargeting.Get())
    {
        Targeting->OnTargetChanged.RemoveDynamic(this, &AIgKnightedAIController::HandleTargetChanged);
    }

    BoundTargeting.Reset();
    Super::OnUnPossess();
}
```

`AddUniqueDynamic` is used for the bind specifically because a double bind is a bug that does not announce itself; `AddDynamic` would silently add the handler twice.

#### ✅ Problem Solved

Enemies stop attacking a corpse. A target loss propagates as a blackboard `null` plus `HasTarget = false`, so the tree's decorators see the truth immediately. Repossession cannot accumulate handlers, and the blackboard is seeded rather than empty.

#### 📉 Cost / Limitations / Tradeoffs

- **Two writes per change** (object and bool) can tear if another system writes the boolean. Nothing else does, but nothing enforces it either. A single struct key would be atomic.
- **`LastKnownLocation` is written on every target change, not continuously.** That is exactly right for "chase where they were", and wrong for "search around here" behaviour that wants a fresher memory.
- **Focus is set at the controller level, but the Pawn's facing is driven by movement.** In this game they do not fight, because the body orients to movement and the weapon aims separately. In a game where facing matters for damage, `SetFocus` and `bOrientRotationToMovement` would need an explicit priority.
- **The controller is one class for three archetypes.** The tree and the definition carry the difference. An archetype needing different *possession* logic (a swarm controller driving several pawns, for example) would need a subclass — which is the correct place to add one.

---

### 💥 Impact of Contribution

**Before:** one Crawler could erase the shared life pool in a second; Scorpions could not hit a strafing player and fired from the wrong origin; the Mollusk's ring was a static pattern that a player could stand inside; and enemies kept attacking a player who had already died.

**After:** every enemy attack is one deliberate event with one origin and one target. The archetypes differ in a data asset and a Behaviour Tree, and the shared code paths — damage, death, score, possession — are implemented once. Adding a fourth archetype is a definition, a tree and an animation, not a new damage pipeline.

The larger point: **three enemies that share a pipeline are easier to debug than one enemy that has its own.** Every bug I fixed in the Crawler's death path was already fixed for the Scorpion and the Mollusk, because they never had a separate one.

---

## 📋 05. Level

![IgKnighted — zone progression, barriers and the encounter flow](../../assets/images/Y1D/3.png)

### 🎯 What was the Goal?

One arena, three parts, one camera, no loading screens between them:

```text
┌──────────────────────────────────────────────────────────┐
│                        PART 3                            │
│   tight corridors · Mollusk + Scorpion mix · final wave   │
├─────────────── barrier ──────────────────────────────────┤
│                        PART 2                            │
│   open pool · hazards · Scorpion introduction            │
├─────────────── barrier ──────────────────────────────────┤
│                        PART 1                            │
│   tutorial-shaped · Crawlers · first power-up            │
└──────────────────────────────────────────────────────────┘
```

The level has to do four jobs:

1. **Teach without text** — part 1 introduces each enemy in a space where the player can retreat.
2. **Escalate** — parts 2 and 3 add hazards and mixes, not just more enemies.
3. **Gate progress** — a part is cleared when its encounter is cleared, not when the player reaches a trigger.
4. **Survive retries** — dying and respawning must not leave a half-cleared part, an open barrier or a stale wave.

The hard constraint was **no level streaming and no checkpoints**. Every retry is the same level, the same actors, reset by code.

---

### Challenge 1: The Three Parts

#### ❓ What was the Problem?

Each part had a spawner and a barrier. When the spawner finished spawning, the barrier opened. When all enemies died, the next part started.

The bug: the barrier opened as soon as the spawner had spawned its last enemy, so a player could run into part 2 while the last Crawler of part 1 was still alive behind them. Part 2's spawner then started its own encounter, and the zone manager — which tracked a single "current encounter" — began counting part 1's enemies as part 2's. The result was a part that never completed, or a victory that fired with enemies still alive.

The second bug was the race at the end of the run: the final enemy dying and the last player dying in the same frame. Whichever completion check ran first won.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I treated an encounter as a counter: spawn N enemies, decrement on each death, complete at zero. It broke when the spawner was still spawning, because the counter reached zero before the encounter had actually started. I tried checking the spawner's timer instead, which is a different lifetime from the enemies it creates.

The same-frame race was the harder one. I first relied on tick order, and sometimes Victory appeared and was then overwritten by Defeat. Reading about timer managers and deferred calls gave me the mechanism, but the design rule came from the failure: completion is a derived condition over two owners' state (spawning finished, alive count zero), and it has to be evaluated after the frame's damage has been resolved.

The version that works uses a generation counter and a one-tick deferred check. That was not the first thing I tried; it was what remained after the counter, the spawner check and the tick-order assumption each failed in a different playtest.

#### 💡 Solution. How Did I Approach the Problem?

The zone manager owns one encounter at a time and knows both counts:

```cpp
// IgKnightedZoneManager.cpp
void AIgKnightedZoneManager::BeginEncounter(int32 ZoneIndex, int32 PendingSpawns)
{
    ++EncounterGeneration;

    CurrentZoneIndex = ZoneIndex;
    AliveEnemies.Reset();
    PendingSpawnCount = FMath::Max(0, PendingSpawns);
    bEncounterActive = true;
    bCompletionCheckScheduled = false;

    OnZoneStarted.Broadcast(ZoneIndex);

    // Handles the degenerate case of an encounter with zero enemies.
    ScheduleCompletionCheck();
}

void AIgKnightedZoneManager::NotifyEnemySpawned(AActor* Enemy)
{
    if (!bEncounterActive || !IsValid(Enemy) || AliveEnemies.Contains(Enemy))
    {
        return;
    }

    AliveEnemies.Add(Enemy);
    PendingSpawnCount = FMath::Max(0, PendingSpawnCount - 1);
}

void AIgKnightedZoneManager::NotifyEnemyDefeated(AActor* Enemy)
{
    if (!AliveEnemies.Remove(Enemy))
    {
        return;
    }

    // Deliberately deferred by one tick. Damage resolution runs first, so if the
    // last life is spent in this frame the GameMode has already recorded Defeat
    // before the completion rule can claim Victory.
    ScheduleCompletionCheck();
}
```

The deferred check validates its generation before completing anything:

```cpp
// IgKnightedZoneManager.cpp
void AIgKnightedZoneManager::EvaluateCompletion()
{
    bCompletionCheckScheduled = false;

    // A stale check must not complete a newer encounter.
    if (!bEncounterActive || PendingCheckGeneration != EncounterGeneration)
    {
        return;
    }

    // Destroyed actors that never reported a death are pruned here; the counter
    // is authoritative, not the actor lifetime.
    for (auto It = AliveEnemies.CreateIterator(); It; ++It)
    {
        if (!It->IsValid())
        {
            It.RemoveCurrent();
        }
    }

    if (PendingSpawnCount > 0 || AliveEnemies.Num() > 0)
    {
        return;
    }

    CompleteEncounter();
}
```

One tick is enough: damage resolution and the GameMode's `FinishMatch` happen in the same frame as the kill, and the check runs at the start of the next one. That is what makes "defeat wins" true without the zone manager knowing anything about lives.

#### ✅ Problem Solved

A barrier opens only when its part is empty of enemies. A stale completion check cannot clear a newer part. The final-frame race resolves deterministically because the completion is deferred past the frame in which the last life may have been spent.

#### 📉 Cost / Limitations / Tradeoffs

- **"One tick" is a scheduling assumption.** It works because the kill and the life spend happen in the same frame; a system that deferred damage across frames would need a stronger guarantee than a timer.
- **`AliveEnemies` is a `TArray<TWeakObjectPtr>` with linear `Contains`/`Remove`.** For 30 enemies it is nothing; for a thousand it would be the wrong container.
- **The zone manager is a singleton found by `GetActorOfClass`.** Convenient and fragile: two zone managers in a level silently make one of them win.
- **Barriers are level actors with a `SetOpen` API, and the manager knows them by index.** A designer adding a part has to keep the array order and the index in sync; there is no validation that the array matches the number of parts.

---

### Challenge 2: Enemy Spawning

#### ❓ What was the Problem?

The spawner counted a spawn as complete when `SpawnActor` returned. The enemy was alive and registered in the encounter *after* that point. In the window between spawn and registration, the enemy could be damaged and killed — particularly by an enhanced dash or a shot that was already in flight.

The result: an enemy died while the zone manager still considered it a pending spawn. The manager's completion check saw `PendingSpawnCount > 0` and waited for an enemy that no longer existed to spawn. The part never completed.

The second symptom was an empty arena at the start of a part: the spawner waited one full `SpawnInterval` before its first spawn, so the player walked into an empty room and had a second of nothing.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

Registering the enemy after `SpawnActor` was the obvious order: create the thing, then tell the manager about it. The manager then pruned the enemy as invalid, or the enemy died before registration and the encounter's alive count never matched. My first fix moved registration after `FinishSpawning`, which removed the invalid-actor case but left a window where the enemy could act before the manager knew it existed.

I did not know that Unreal's spawn has two phases until I read `SpawnActorDeferred`. `BeginPlay` runs in `FinishSpawning`, so the actor is valid before it is "spawned", and the window between the two calls is exactly where registration belongs: after the object exists, before it can act or be acted upon.

That is an engine detail I could not have guessed from the API names. The lesson was to read what the spawn flow actually does, then place the protocol inside it, instead of choosing an order that merely compiles.

#### 💡 Solution. How Did I Approach the Problem?

Register before finishing the spawn, and spawn the first enemy immediately:

```cpp
// IgKnightedEnemySpawner.cpp
void AIgKnightedEnemySpawner::BeginEncounter()
{
    if (!EnemyClass || !GetWorld())
    {
        return;
    }

    SpawnedCount = 0;

    const AIgKnightedGameMode* GameMode = GetWorld()->GetAuthGameMode<AIgKnightedGameMode>();
    const float CountScale = GameMode ? GameMode->GetActiveTuning().EnemyCountScale : 1.f;

    TargetCount = FMath::Max(1, FMath::RoundToInt(BaseEnemyCount * CountScale));

    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->RegisterSpawner(this);
        ZoneManager->BeginEncounter(ZoneIndex, TargetCount);
    }

    // Spawn the first enemy immediately so the player does not walk into an
    // empty arena and wait for the timer.
    SpawnOne();

    if (SpawnInterval > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimer, this, &AIgKnightedEnemySpawner::SpawnOne, SpawnInterval, true);
    }
}

void AIgKnightedEnemySpawner::SpawnOne()
{
    if (SpawnedCount >= TargetCount)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    if (!EnemyClass)
    {
        return;
    }

    const FTransform SpawnTransform = PickSpawnTransform();

    AIgKnightedEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AIgKnightedEnemyCharacter>(
        EnemyClass,
        SpawnTransform,
        this,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (!Enemy)
    {
        return;
    }

    // Register BEFORE FinishSpawning. From this point the enemy is alive as far
    // as the encounter is concerned, so it cannot be damaged and killed while
    // the ZoneManager still believes it is a pending spawn.
    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->NotifyEnemySpawned(Enemy);
    }

    UGameplayStatics::FinishSpawningActor(Enemy, SpawnTransform);
    ++SpawnedCount;
}
```

#### ✅ Problem Solved

An enemy cannot be killed before it is registered, so the encounter cannot wait forever for a death that already happened. The first enemy exists on the frame the part begins. Restarting a part wipes the encounter, destroys leftovers and starts a new generation.

#### 📉 Cost / Limitations / Tradeoffs

- **The spawner and the manager share ownership of the same list.** The spawner tells the manager, the enemy tells the manager, and the manager prunes. The protocol is documented but not enforced; a second spawner for the same zone would double-register.
- **`TargetCount` is a single number per spawner.** A wave with a mix of archetypes needs either several spawners (with separate registrations into one encounter) or a spawn table. This sample has the former only.
- **`PickSpawnTransform` is uniform random over points.** No weighting, no spacing, no avoidance. In part 3 this can put three Mollusks on one point.
- **Spawn points are level-placed actors.** The spawner has no fallback beyond its own transform, which is inside the arena — a missing reference spawns enemies in the middle of the player.

---

### Challenge 3: Dynamic Difficulty

#### ❓ What was the Problem?

Difficulty was authored: Easy, Medium and Hard each scaled enemy count. Playtesting showed the usual problem with authored difficulty — a good player on Medium was bored, and a struggling player on Medium was stuck, and neither of them had chosen wrong.

The first adaptive version reacted to player deaths by reducing enemy count. It worked, in the sense that the number changed. It felt terrible, because it changed *after* the failure and without the player being able to see it: the room they had just lost became easier, which reads as the game pitying them.

The second version reacted continuously to kills per second, and oscillated: a good wave raised the scale, which raised the count, which lowered kills per second, which lowered the scale. The difficulty visibly breathed.

#### 🧩 Why Was it Hard? What Struggles Did I Face?

I started with a formula that scaled enemy count by kills and lives lost, and it oscillated: a good run got harder, which produced a bad run, which got easier. I had not normalised by part length, so five kills in a thirty-second part looked the same as five in a two-minute part. I then clamped harder, which made the system inert, and added smoothing, which hid the oscillation without fixing the signal.

The best practice I landed on came from watching the logs more than from a document: normalise every input, evaluate only between encounters so a mid-fight change cannot read as cheating, and damp the output so the player perceives a ramp rather than a rule change. The director stays inside the chosen difficulty's band; it adjusts within the player's choice, it does not reclassify the game.

That last constraint is a design decision, not an engine one, but the implementation lesson was Unreal-specific: a subsystem that ticks and broadcasts is easy to write, and a subsystem that is pure and testable is harder, but the second is what let me tune this without playing the game each time.

#### 💡 Solution. How Did I Approach the Problem?

The director is a pure class with no world dependency, so it can be tested with numbers:

```cpp
// IgKnightedDifficultyDirector.cpp
float UIgKnightedDifficultyDirector::Evaluate(const FIgDirectorInput& Input, float DeltaSeconds)
{
    // Normalised signals, not raw counts. A raw enemy count means something
    // different in Zone 1 and Zone 3.
    const float LifePressure = FMath::Clamp(static_cast<float>(Input.LivesLostThisZone) / 3.f, 0.f, 1.f);

    const float KillsPerSecond =
        static_cast<float>(Input.EnemiesDefeatedThisZone) / FMath::Max(1.f, Input.TimeInZoneSeconds);
    const float Dominance = FMath::Clamp(KillsPerSecond * 4.f, 0.f, 1.f);

    // Losing lives is the strongest signal. Clearing fast is a weaker one, so a
    // player who is dominating gets pressure, not punishment.
    const float TargetScale = 1.f - (0.15f * LifePressure) + (0.10f * Dominance);

    const float Desired = FMath::Clamp(TargetScale, MinScale, MaxScale);

    // Damping is what prevents the oscillation that makes dynamic difficulty
    // feel arbitrary.
    CurrentScale = FMath::FInterpTo(CurrentScale, Desired, DeltaSeconds, 1.f / FMath::Max(0.01f, SmoothingSeconds));

    return CurrentScale;
}
```

Three properties make it defensible:

- **The band is bounded** (`0.85`–`1.15`). The director can nudge the count, not rewrite the difficulty. A player on Medium stays on Medium.
- **Life loss dominates.** It is weighted 1.5× the dominance reward, and it is the signal that most reliably means "this is too hard right now".
- **It moves slowly.** With a smoothing time of several seconds, a single bad fight does not change the next room; a bad *part* does.

The authored tuning still owns everything else:

```cpp
// IgKnightedDifficultyDataAsset.h
/**
 * Difficulty is a row of numbers, not a set of branches spread across four files.
 * Adding a new difficulty is a new asset, not a new code path.
 */
UCLASS(BlueprintType)
class IGKNIGHTED_API UIgKnightedDifficultyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    FIgDifficultyTuning Easy;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    FIgDifficultyTuning Medium;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    FIgDifficultyTuning Hard;

    UFUNCTION(BlueprintPure, Category = "Difficulty")
    FIgDifficultyTuning GetTuning(EIgDifficulty Difficulty) const
    {
        switch (Difficulty)
        {
        case EIgDifficulty::Easy:   return Easy;
        case EIgDifficulty::Hard:   return Hard;
        default:                    return Medium;
        }
    }

    const FIgDifficultyTuning& GetTuningRef(EIgDifficulty Difficulty) const
    {
        switch (Difficulty)
        {
        case EIgDifficulty::Easy:   return Easy;
        case EIgDifficulty::Hard:   return Hard;
        default:                    return Medium;
        }
    }
};
```

#### ✅ Problem Solved

The scale stays inside a band, moves toward a target rather than reacting to a single event, and is computed from normalised signals so it means the same thing in every part. Designers keep a single authored table; the director only perturbs one column of it.

#### 📉 Cost / Limitations / Tradeoffs

- **Only `EnemyCountScale` is consumed today.** `FIgDifficultyTuning` also declares speed, attack interval, hazard uptime and collectable interval; nothing reads them yet. That is the honest state of the sample — the struct was designed for the full tuning, and shipping a changing enemy count was the part that fit the schedule.
- **The director is not driven by the GameMode in this sample.** It is a pure evaluator (which is why it is testable), but wiring it to a timer and to the active tuning is the integration step, not something the class does itself.
- **"Lives lost in this zone" needs a reset the director does not own.** The caller must zero the counters at a part boundary; a director with internal memory would be harder to reuse across parts.
- **The band is the design.** `0.85`–`1.15` is a 30% swing in enemy count; a wider band would let a struggling player feel the game give up on them, and a narrower one would not be worth having.

---

### 💥 Impact of Contribution

**Before:** barriers opened with enemies still alive, parts could never complete, the final frame could show both outcomes, an enemy could die before the encounter knew it existed, and the adaptive difficulty oscillated badly enough to be noticeable.

**After:** the level is a state machine with one active encounter and an explicit generation, spawns are registered before they can die, and difficulty is a bounded, damped adjustment on top of an authored table. Every retry reconstructs the same arena from the same actors, so the two-player, five-life run is repeatable — which is what made playtesting the difficulty possible at all.

---

# 📚 What I Learned

I came in able to build a prototype and left able to build a system. The difference is not knowledge of an API; it is knowing which object a piece of state belongs to, and being able to say why.

**1. Architecture is a statement about lifetimes.** `GameInstance`, `GameMode`, `GameState` — every bug in §01 was a piece of state living in an object that died too early. Once I could answer "what must this survive?", the framework's lifetime model stopped being trivia and became a design tool I use in every engine.

**2. Blueprints and C++ are not "designers and programmers".** The useful line is between *rules* and *content*. Ownership, damage order, scoring and match termination are C++ because a content edit must not be able to break them. Enemy numbers, durations, colours, spawn counts and tuning curves are data because trying them should not need a compile. I stopped asking "which language?" and started asking "who needs to change this, and what must they not be able to break?"

**3. The bug is usually the ordering, not the line.** Shield before dash, victory before defeat, spawn registration after spawn, mapping contexts before possession. In each case the individual lines were correct; the sequence was not. I now write the order down as part of the design, because an order that exists only in code is an order nobody can review.

**4. One writer per fact.** Score in two places double-counted. Target in two places attacked a corpse. Lives in two places gave a two-player team ten lives. The fix is not "be careful"; it is making the second write impossible. A fact with two owners will eventually disagree with itself.

**5. Event-driven UI is a lifetime problem.** A widget that binds a delegate and is not destroyed cleanly is a crash waiting for a respawn. Unbind-before-bind and unbind-on-destruct became habits, and the HUD stopped being the place where counters were computed. Views read; they do not own.

**6. Data-driven is not "put numbers in a data asset".** A definition only pays off if the code has no branches left for the archetype. If `ApplyDefinition` still had `if (Archetype == Mollusk)`, the data asset would be decoration. The measure is: what would I have to change to add a fourth archetype?

**7. Feel bugs are arithmetic bugs.** Diagonal movement at 1.41×, spread that starts at the crosshair instead of around it, a ring whose gaps never move, invulnerability that starts one frame late. None of these crashed; all of them were the difference between the game feeling fair and feeling broken. I learned to test the *number*, not just the behaviour: measure the speed, count the shots on each side of the axis, check the frame the flag flips.

**8. A prototype is allowed to be wrong; a system is not allowed to be ambiguous.** My previous work never had a second player, a retry or another programmer. This project had all three, and every ambiguity became a bug in a path I had not imagined. The work that mattered most was not writing the feature; it was writing down the contract so that the next feature could not break it.

**9. Testing is a matrix, not a happy path.** The bugs that shipped were interactions: two players dying in one frame, a retry after the tutorial, a dash through a hazard, a buff expiring during a death. None of them are exotic; they are simply not on the path you take when you press play. I now write the matrix before the feature: every entry point, every device, every transition, every simultaneous event.

---

# 🔄 Future Improvements

These are the things I would do next, in the order I would do them. They are not "polish"; they are the places where the current design would fail under more content.

### Make the damage pipeline a first-class system

The resolver is a stateless world subsystem with three outcomes and one priority order. It should expose a small **pre-damage hook** so a shield, an i-frame source and a future armour system register their interception instead of being hard-coded in order. The current order is correct; it is not extensible. A `FIgDamageRule` list evaluated in priority order would make "add a new defence" a data change.

### Finish the difficulty tuning

`FIgDifficultyTuning` declares five knobs and only `EnemyCountScale` is consumed. The honest next step is to route speed, attack interval, hazard uptime and collectable interval through the same definition that enemies already read, so authored difficulty is a full row and the adaptive director can move any column inside its band. That also makes the director worth wiring, instead of leaving it as a testable pure class.

### Give the archetypes their own trees, and make that data

The sample has one controller and one tree shape. Three archetypes need three trees (or one tree with three branch sets), selected by the definition alongside the movement speed. The important constraint is that the *controller* stays one class: the difference must be a `UBehaviorTree*` in the enemy definition, not a subclass.

### Replace the per-frame traversal overlap with a sweep

`KillActorsCrossedThisFrame` queries a sphere at the current position every dash frame. A capsule sweep from last frame's position to this one would be exact at any speed and would remove the "fast dash skips an enemy" edge case. It is also the right shape for a future "dash through a wall" rule.

### Pool projectiles

Every shot is an actor with collision, movement and a lifespan. At the current counts this is fine; a bullet-hell pattern with hundreds would allocate hundreds of actors per second. A `UProjectilePool` subsystem with resurrect-on-spawn semantics is the natural fix, and it is also the change that would make a Niagara-based projectile viable.

### Give the zone manager a real part model

Barriers, spawners and part indices are three parallel arrays that a designer must keep aligned. A `FIgLevelPart` struct — spawners, barrier, hazard set, music state — would replace the alignment by convention with one array, and would let a part be authored rather than assembled.

### Add a leave-volume latch to pickups

The matchstick can be re-collected by a player who stands still on it across a respawn. The correct fix is not a longer respawn timer; it is a `bPlayerInside` latch that only clears after an end-overlap, so availability is a function of the player having left, not of the timer having fired.

### Build the results screen on a ledger

The score is a running total. A per-part, per-archetype breakdown would need an append-only event list (`FIgScoreEvent`) rather than a counter — which is also what a "best run" or replay feature would need. Doing it now, while the score API is one function, would be cheap; doing it later means changing every caller.

---

# 👤 Zhangir After the Project

At the end of the eight weeks I could do the thing I could not do at the start: **open an empty Unreal project and build the systems a co-op game needs, without a tutorial.**

Concretely, that means:

- I can look at a piece of state in a design conversation and say which object owns it, what it must survive, and what is allowed to write it.
- I can build the Gameplay Framework for a new game — the run configuration, the match rules, the shared numbers, the per-player identity, the input stack, the possession flow — from a blank `GameMode`.
- I can build an enemy that is a data definition plus a Behaviour Tree, with a shared damage pipeline and a controller that keeps the blackboard honest.
- I can write a damage resolution order, a spawn protocol and an encounter state machine, and explain why each ordering is the way it is.
- I can work in C++ and Blueprints as one codebase, and place a piece of logic in whichever one the *team* needs it in.
- I know what "done" means for a system, which is not "it works" but "the next feature cannot break it silently".

I also know how much I still do not know. I have not shipped a networked game, so replication is theory for me. I have not built a save system or a content pipeline at scale. I have not had to maintain a live game where the architecture is two years old and the original authors have left. Those are the next problems, and I know the shape of them now: they are all lifetime questions.

What changed most is not the list of APIs. It is that I stopped thinking of a feature as a thing I write and started thinking of it as a **contract I publish** — to the designer who tunes it, to the artist who animates it, and to the next programmer who has to change it without breaking the run.

*IgKnighted is playable on [itch.io](https://buas.itch.io/igknighted). The code excerpts on this page are from the systems I wrote, cleaned up and collected in [`_code_samples/igknighted`](https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted).*

<a class="cta-link" href="https://github.com/Jon-Gear/Jon-Gear.github.io/tree/main/_code_samples/igknighted" target="_blank" rel="noopener">⚙️ See Code Examples</a>

