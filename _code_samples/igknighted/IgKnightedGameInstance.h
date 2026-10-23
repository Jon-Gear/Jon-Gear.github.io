#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IgKnightedTypes.h"
#include "IgKnightedGameInstance.generated.h"

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
