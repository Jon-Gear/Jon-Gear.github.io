#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedDashComponent.generated.h"

/**
 * Charges, cooldown, i-frames and the enhanced-dash traversal kill.
 *
 * Invulnerability is derived from the dash's own lifetime, never written by
 * anything else. That is what lets the shield and the dash coexist.
 */
UCLASS(ClassGroup = (IgKnighted), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedDashComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIgKnightedDashComponent();

    UPROPERTY(BlueprintAssignable, Category = "Dash")
    FOnDashStateChanged OnDashStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Dash")
    FOnDashChargesChanged OnDashChargesChanged;

    UFUNCTION(BlueprintCallable, Category = "Dash")
    bool TryStartDash();

    UFUNCTION(BlueprintCallable, Category = "Dash")
    void EndDash();

    /** Called on respawn: timers and flags belong to the body, so they are reset with it. */
    UFUNCTION(BlueprintCallable, Category = "Dash")
    void ResetForRespawn();

    void SetEnhancedDashActive(bool bActive);

    UFUNCTION(BlueprintPure, Category = "Dash")
    bool IsDashing() const { return bDashing; }

    UFUNCTION(BlueprintPure, Category = "Dash")
    bool IsInvulnerable() const { return bInvulnerable; }

    UFUNCTION(BlueprintPure, Category = "Dash")
    int32 GetCurrentCharges() const { return CurrentCharges; }

    UFUNCTION(BlueprintPure, Category = "Dash")
    int32 GetMaxCharges() const { return MaxCharges; }

    UFUNCTION(BlueprintPure, Category = "Dash")
    bool IsEnhancedDashActive() const { return bEnhancedDashActive; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float DashSpeed = 2600.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float DashDuration = 0.18f;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float ChargeCooldown = 1.4f;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    int32 MaxCharges = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    int32 EnhancedMaxCharges = 2;

    UPROPERTY(EditDefaultsOnly, Category = "Dash")
    float TraversalKillRadius = 75.f;

    /** Read-only in Blueprint: the timer and the HUD are the only writers. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    int32 CurrentCharges = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool bEnhancedDashActive = false;

private:
    void RechargeOneCharge();
    void KillActorsCrossedThisFrame();

    bool bDashing = false;
    bool bInvulnerable = false;
    FVector DashDirection = FVector::ZeroVector;

    int32 BaseMaxCharges = 1;

    FTimerHandle DashTimer;
    FTimerHandle RechargeTimer;

    TSet<TWeakObjectPtr<AActor>> TraversedActors;
};
