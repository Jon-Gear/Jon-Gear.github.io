#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IgKnightedTypes.h"
#include "IgKnightedGameMode.generated.h"

class AIgKnightedPlayerCharacter;
class AIgKnightedPlayerController;
class AIgKnightedZoneManager;
class UIgKnightedDamageResolver;
class UIgKnightedDifficultyDataAsset;

/**
 * The rules of the match. The only object allowed to spend a shared life,
 * end the run, or respawn a player.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIgKnightedGameMode();

    UFUNCTION(BlueprintPure, Category = "Match")
    UIgKnightedDamageResolver* GetDamageResolver() const;

    UFUNCTION(BlueprintPure, Category = "Match")
    EMatchPhase GetMatchPhase() const;

    UFUNCTION(BlueprintPure, Category = "Match")
    FIgDifficultyTuning GetActiveTuning() const { return ActiveTuning; }

    UFUNCTION(BlueprintPure, Category = "Match")
    bool IsMatchInProgress() const { return GetMatchPhase() == EMatchPhase::InProgress; }

    /** Single entry point for "a player pawn died". Idempotent per pawn. */
    void HandlePlayerEliminated(AIgKnightedPlayerCharacter* Victim, const FIgDamageContext& Context);

    /** Raised by the ZoneManager after its completion rule has been satisfied. */
    void HandleFinalZoneCleared();

    UFUNCTION(BlueprintImplementableEvent, Category = "Match")
    void BP_OnPlayerRespawned(AIgKnightedPlayerController* Controller);

    UFUNCTION(BlueprintImplementableEvent, Category = "Match")
    void BP_OnMatchFinished(EMatchResult Result);

protected:
    virtual void BeginPlay() override;

    /** Creates the missing LocalPlayers (never re-creates existing ones) and stamps identity. */
    void EnsureLocalPlayers(int32 DesiredCount);

    UFUNCTION()
    void HandleLethalDamage(AActor* Victim, const FIgDamageContext& Context);

    void FinishMatch(EMatchResult Result);
    void ScheduleRespawn(AIgKnightedPlayerController* Controller);
    void RespawnPlayer(AIgKnightedPlayerController* Controller);

    UPROPERTY(EditDefaultsOnly, Category = "Match")
    int32 MaxSharedLives = 5;

    UPROPERTY(EditDefaultsOnly, Category = "Match")
    float RespawnDelaySeconds = 2.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Match")
    TObjectPtr<UIgKnightedDifficultyDataAsset> DifficultyAsset;

private:
    UPROPERTY()
    FIgRunSettings RunSettings;

    UPROPERTY()
    FIgDifficultyTuning ActiveTuning;

    UPROPERTY()
    EMatchResult MatchResult = EMatchResult::None;

    // A pawn instance can report its death from several sources in one frame.
    // Membership here is the idempotency latch for "one death spends one life".
    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> HandledDeaths;

    TMap<TWeakObjectPtr<AController>, FTimerHandle> RespawnTimers;
};
