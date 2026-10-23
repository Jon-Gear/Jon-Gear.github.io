#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IgKnightedTypes.h"
#include "IgKnightedDifficultyDirector.generated.h"

/**
 * Dynamic difficulty evaluates BETWEEN encounters, never during one, and only
 * inside a narrow band around the preset the player chose.
 */
UCLASS(BlueprintType)
class IGKNIGHTED_API UIgKnightedDifficultyDirector : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Director")
    void Reset();

    /** Returns the scalar applied on top of the selected preset for the next encounter. */
    UFUNCTION(BlueprintCallable, Category = "Director")
    float Evaluate(const FIgDirectorInput& Input, float DeltaSeconds);

    UFUNCTION(BlueprintPure, Category = "Director")
    float GetCurrentScale() const { return CurrentScale; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Director")
    float MinScale = 0.85f;

    UPROPERTY(EditDefaultsOnly, Category = "Director")
    float MaxScale = 1.15f;

    UPROPERTY(EditDefaultsOnly, Category = "Director")
    float SmoothingSeconds = 0.25f;

    UPROPERTY(VisibleAnywhere, Category = "Director")
    float CurrentScale = 1.f;
};
