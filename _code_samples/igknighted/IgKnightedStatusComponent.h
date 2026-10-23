#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedStatusComponent.generated.h"

class UIgKnightedCombatComponent;
class UIgKnightedDashComponent;
class UIgKnightedWeaponData;

/**
 * The buff owner. The duplicate-pickup rule lives here, because this is the
 * only object that knows whether a buff is already active.
 */
UCLASS(ClassGroup = (IgKnighted), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedStatusComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Status")
    FOnShieldChanged OnShieldChanged;

    UPROPERTY(BlueprintAssignable, Category = "Status")
    FOnBuffExpired OnBuffExpired;

    UFUNCTION(BlueprintCallable, Category = "Status")
    bool TryApplyBuff(EIgBuffKind Kind, float DurationSeconds);

    UFUNCTION(BlueprintPure, Category = "Status")
    bool CanAcceptBuff(EIgBuffKind Kind) const;

    UFUNCTION(BlueprintCallable, Category = "Status")
    bool ConsumeShield();

    UFUNCTION(BlueprintPure, Category = "Status")
    bool HasShield() const { return ShieldCharges > 0; }

    UFUNCTION(BlueprintPure, Category = "Status")
    bool IsEnhancedDashActive() const { return bEnhancedDashActive; }

    /** Death cancels every timed buff: the timers belong to the body. */
    UFUNCTION(BlueprintCallable, Category = "Status")
    void ResetForRespawn();

protected:
    virtual void BeginPlay() override;

    void ExpirePowerShot();
    void ExpireEnhancedDash();

    UPROPERTY(EditDefaultsOnly, Category = "Status")
    TObjectPtr<UIgKnightedWeaponData> PowerShotWeaponData;

    UPROPERTY(EditDefaultsOnly, Category = "Status")
    float DefaultBuffDuration = 10.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    int32 ShieldCharges = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    bool bEnhancedDashActive = false;

private:
    FTimerHandle PowerShotTimer;
    FTimerHandle EnhancedDashTimer;

    UPROPERTY()
    TObjectPtr<UIgKnightedCombatComponent> Combat;

    UPROPERTY()
    TObjectPtr<UIgKnightedDashComponent> Dash;
};
