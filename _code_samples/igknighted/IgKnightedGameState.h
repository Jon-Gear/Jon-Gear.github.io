#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IgKnightedTypes.h"
#include "IgKnightedGameState.generated.h"

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
