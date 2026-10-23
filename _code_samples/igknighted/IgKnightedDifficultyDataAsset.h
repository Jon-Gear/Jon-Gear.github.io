#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IgKnightedTypes.h"
#include "IgKnightedDifficultyDataAsset.generated.h"

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
