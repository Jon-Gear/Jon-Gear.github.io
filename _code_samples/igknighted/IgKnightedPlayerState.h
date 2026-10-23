#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IgKnightedTypes.h"
#include "IgKnightedPlayerState.generated.h"

/**
 * Identity and durable statistics. This object survives Pawn death, which is
 * exactly why the score lives here and not on the Character.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnMatchScoreChanged;

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnMultiplierChanged OnMultiplierChanged;

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetMatchScore() const { return MatchScore; }

    UFUNCTION(BlueprintPure, Category = "Score")
    float GetScoreMultiplier() const { return Multiplier; }

    UFUNCTION(BlueprintPure, Category = "Player")
    int32 GetPlayerIndex() const { return PlayerIndex; }

    UFUNCTION(BlueprintPure, Category = "Player")
    FLinearColor GetPlayerColour() const;

    void SetPlayerIndex(int32 InIndex);

    /** Base points are scaled by the multiplier here, once. No view does this maths. */
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddMatchScore(int32 BasePoints);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddMultiplier(float Delta);

    UFUNCTION(BlueprintCallable, Category = "Score")
    void ResetMultiplier();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    int32 MatchScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    float Multiplier = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 PlayerIndex = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Player")
    TArray<FLinearColor> PlayerColours;

    UPROPERTY(EditDefaultsOnly, Category = "Score")
    float MaxMultiplier = 5.f;
};
