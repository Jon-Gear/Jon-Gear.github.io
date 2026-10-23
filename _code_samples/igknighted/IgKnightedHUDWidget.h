#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IgKnightedHUDWidget.generated.h"

class AIgKnightedGameState;
class AIgKnightedPlayerState;
class UIgKnightedDashComponent;

/**
 * A view. It reads the authoritative counters and never accumulates its own.
 * With two local players, each panel is bound to a specific PlayerState.
 */
UCLASS()
class IGKNIGHTED_API UIgKnightedHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void BindToPlayerState(AIgKnightedPlayerState* InPlayerState);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void BindToGameState(AIgKnightedGameState* InGameState);

    UFUNCTION(BlueprintPure, Category = "HUD")
    AIgKnightedPlayerState* GetBoundPlayerState() const { return BoundPlayerState; }

    UFUNCTION(BlueprintPure, Category = "HUD")
    UIgKnightedDashComponent* GetBoundDashComponent() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    FLinearColor GetBoundPlayerColour() const;

protected:
    virtual void NativeDestruct() override;

    UFUNCTION()
    void HandleMatchScoreChanged(int32 NewScore);

    UFUNCTION()
    void HandleSharedLivesChanged(int32 NewLives, int32 Delta);

    UFUNCTION()
    void HandleDashChargesChanged(int32 CurrentCharges, int32 MaxCharges);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void BP_OnMatchScoreChanged(int32 NewScore);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void BP_OnSharedLivesChanged(int32 NewLives, int32 Delta);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void BP_OnDashChargesChanged(int32 CurrentCharges, int32 MaxCharges);

private:
    void UnbindAll();

    UPROPERTY(BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AIgKnightedPlayerState> BoundPlayerState;

    UPROPERTY()
    TObjectPtr<AIgKnightedGameState> BoundGameState;
};
