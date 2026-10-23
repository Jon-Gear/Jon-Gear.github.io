#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedCombatComponent.generated.h"

class AIgKnightedProjectile;
class UIgKnightedWeaponData;

/**
 * Firing is a data problem. The component owns cadence, spread and spawning;
 * the weapon asset owns every number a designer wants to tune.
 */
UCLASS(ClassGroup = (IgKnighted), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnFired OnFired;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartFiring();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopFiring();

    /** Called by the fire animation notify so the projectile leaves on the arm-extend frame. */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SpawnProjectileFromMuzzle();

    UFUNCTION(BlueprintPure, Category = "Combat")
    UIgKnightedWeaponData* GetWeaponData() const { return ActiveWeaponData; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetActiveWeaponData(UIgKnightedWeaponData* NewWeapon);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RestoreBaseWeaponData();

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsFiring() const { return bIsFiring; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void BP_PlayFireMontage();

    void FireOnce();

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TObjectPtr<UIgKnightedWeaponData> BaseWeaponData;

    UPROPERTY(VisibleInstanceOnly, Category = "Combat")
    TObjectPtr<UIgKnightedWeaponData> ActiveWeaponData;

    /**
     * When true, the animation notify is the spawn point: the montage decides
     * the exact frame. When false, the projectile spawns on the fire request.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    bool bUseAnimationNotify = false;

private:
    FTimerHandle FireTimer;
    bool bIsFiring = false;
};
