#include "IgKnightedGameMode.h"

#include "IgKnightedDamageResolver.h"
#include "IgKnightedDifficultyDataAsset.h"
#include "IgKnightedGameInstance.h"
#include "IgKnightedGameState.h"
#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerController.h"
#include "IgKnightedPlayerState.h"
#include "IgKnightedZoneManager.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AIgKnightedGameMode::AIgKnightedGameMode()
{
    GameStateClass = AIgKnightedGameState::StaticClass();
    PlayerStateClass = AIgKnightedPlayerState::StaticClass();
    PlayerControllerClass = AIgKnightedPlayerController::StaticClass();
    DefaultPawnClass = AIgKnightedPlayerCharacter::StaticClass();
}

void AIgKnightedGameMode::BeginPlay()
{
    Super::BeginPlay();

    AIgKnightedGameState* GameState = GetGameState<AIgKnightedGameState>();
    check(GameState);

    // Read the run configuration exactly once. Everything downstream reads the
    // GameMode's copy, so the UI can be closed and the setting cannot drift.
    RunSettings = FIgRunSettings();
    if (const UIgKnightedGameInstance* GameInstance = GetGameInstance<UIgKnightedGameInstance>())
    {
        RunSettings = GameInstance->GetPendingRunSettings();
    }

    ActiveTuning = DifficultyAsset
        ? DifficultyAsset->GetTuning(RunSettings.Difficulty)
        : FIgDifficultyTuning();

    GameState->SetMatchResult(EMatchResult::None);
    GameState->InitializeMatch(MaxSharedLives);
    GameState->SetMatchPhase(EMatchPhase::InProgress);

    if (UIgKnightedDamageResolver* Resolver = GetDamageResolver())
    {
        Resolver->OnLethalDamage.AddDynamic(this, &AIgKnightedGameMode::HandleLethalDamage);
    }

    EnsureLocalPlayers(RunSettings.PlayerCount);
}

void AIgKnightedGameMode::EnsureLocalPlayers(int32 DesiredCount)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        return;
    }

    // Count what already exists. createPlayer on a LocalPlayer that survives a
    // level travel is how a two-player game turns into a four-player game.
    for (int32 Index = GameInstance->GetNumLocalPlayers(); Index < DesiredCount; ++Index)
    {
        UGameplayStatics::CreatePlayer(this, -1, /*bSpawnPlayerController=*/true);
    }

    int32 PlayerIndex = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AIgKnightedPlayerController* Controller = Cast<AIgKnightedPlayerController>(It->Get());
        if (!Controller)
        {
            continue;
        }

        Controller->LocalPlayerIndex = PlayerIndex;

        if (AIgKnightedPlayerState* PlayerState = Controller->GetPlayerState<AIgKnightedPlayerState>())
        {
            PlayerState->SetPlayerIndex(PlayerIndex);
        }

        ++PlayerIndex;
    }
}

UIgKnightedDamageResolver* AIgKnightedGameMode::GetDamageResolver() const
{
    return GetWorld() ? GetWorld()->GetSubsystem<UIgKnightedDamageResolver>() : nullptr;
}

EMatchPhase AIgKnightedGameMode::GetMatchPhase() const
{
    const AIgKnightedGameState* GameState = GetGameState<AIgKnightedGameState>();
    return GameState ? GameState->GetMatchPhase() : EMatchPhase::FrontEnd;
}

void AIgKnightedGameMode::HandleLethalDamage(AActor* Victim, const FIgDamageContext& Context)
{
    if (AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(Victim))
    {
        HandlePlayerEliminated(Player, Context);
    }
}

void AIgKnightedGameMode::HandlePlayerEliminated(AIgKnightedPlayerCharacter* Victim, const FIgDamageContext& Context)
{
    AIgKnightedGameState* GameState = GetGameState<AIgKnightedGameState>();
    if (!GameState || GameState->GetMatchPhase() != EMatchPhase::InProgress)
    {
        return;
    }

    if (MatchResult != EMatchResult::None || !IsValid(Victim))
    {
        return;
    }

    if (HandledDeaths.Contains(Victim))
    {
        return;
    }
    HandledDeaths.Add(Victim);

    Victim->StartDeathPresentation();

    const int32 RemainingLives = GameState->GetSharedLives() - 1;
    GameState->SetSharedLives(RemainingLives);

    if (RemainingLives <= 0)
    {
        FinishMatch(EMatchResult::Defeat);
        return;
    }

    if (AIgKnightedPlayerController* Controller = Cast<AIgKnightedPlayerController>(Victim->GetController()))
    {
        ScheduleRespawn(Controller);
    }
}

void AIgKnightedGameMode::ScheduleRespawn(AIgKnightedPlayerController* Controller)
{
    if (!IsValid(Controller))
    {
        return;
    }

    FTimerHandle& Handle = RespawnTimers.FindOrAdd(Controller);
    FTimerDelegate Delegate;
    Delegate.BindUObject(this, &AIgKnightedGameMode::RespawnPlayer, Controller);

    GetWorldTimerManager().SetTimer(Handle, Delegate, RespawnDelaySeconds, false);
}

void AIgKnightedGameMode::RespawnPlayer(AIgKnightedPlayerController* Controller)
{
    RespawnTimers.Remove(Controller);

    if (!IsValid(Controller))
    {
        return;
    }

    // A death restarts the current wave, so the encounter is symmetric for both
    // players and a respawn cannot drop the player back into the hazard that
    // killed them.
    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->RestartActiveEncounter();
    }

    RestartPlayerAtPlayerStart(Controller, FindPlayerStart(Controller));
    BP_OnPlayerRespawned(Controller);
}

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
