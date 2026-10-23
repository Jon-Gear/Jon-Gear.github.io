#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IgKnightedAimProvider.h"
#include "IgKnightedDamageable.h"
#include "IgKnightedTypes.h"
#include "IgKnightedPlayerCharacter.generated.h"

class UIgKnightedCombatComponent;
class UIgKnightedDashComponent;
class UIgKnightedStatusComponent;

/**
 * The player's body. Composition only: the rules live in the components.
 * The Character wires input intent to those components and owns presentation.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerCharacter : public ACharacter, public IIgKnightedAimProvider, public IIgKnightedDamageable
{
    GENERATED_BODY()

public:
    AIgKnightedPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // ---- Intent (called by the PlayerController) ----
    void Move(const FVector2D& Input);
    void AimFromStick(const FVector2D& Input);
    void AimAtCursor();
    void StartFire();
    void StopFire();
    void TryDash();

    // ---- IIgKnightedAimProvider ----
    virtual FVector GetMuzzleLocation() const override;
    virtual FVector GetAimDirection() const override { return AimDirection; }

    // ---- IIgKnightedDamageable ----
    virtual FIgDamageResult ApplyGameplayDamage_Implementation(const FIgDamageContext& Context) override;
    virtual bool IsDead_Implementation() const override { return bIsDead; }

    void StartDeathPresentation();
    void FinishDeath();

    // ---- Animation Blueprint reads these; it never writes them ----
    UFUNCTION(BlueprintPure, Category = "State")
    bool IsFiring() const;

    UFUNCTION(BlueprintPure, Category = "State")
    bool IsDashing() const;

    UFUNCTION(BlueprintPure, Category = "State")
    bool IsDeadState() const { return bIsDead; }

    UFUNCTION(BlueprintPure, Category = "State")
    float GetAimYaw() const;

    UFUNCTION(BlueprintPure, Category = "Components")
    UIgKnightedCombatComponent* GetCombatComponent() const { return Combat; }

    UFUNCTION(BlueprintPure, Category = "Components")
    UIgKnightedDashComponent* GetDashComponent() const { return Dash; }

    UFUNCTION(BlueprintPure, Category = "Components")
    UIgKnightedStatusComponent* GetStatusComponent() const { return Status; }

    UFUNCTION(BlueprintImplementableEvent, Category = "Presentation")
    void BP_OnDeathStarted();

protected:
    /** Yaw follows the aim, while the body follows movement. Twin-stick read. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim")
    TObjectPtr<USceneComponent> WeaponPivot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UIgKnightedCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UIgKnightedDashComponent> Dash;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UIgKnightedStatusComponent> Status;

    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    float AimInterpSpeed = 20.f;

    UPROPERTY(EditDefaultsOnly, Category = "Aim")
    float AimTraceDistance = 10000.f;

private:
    FVector AimDirection = FVector::ForwardVector;
    bool bIsDead = false;
};
