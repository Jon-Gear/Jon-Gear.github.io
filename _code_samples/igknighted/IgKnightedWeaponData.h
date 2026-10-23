#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IgKnightedWeaponData.generated.h"

class AIgKnightedProjectile;

UCLASS(BlueprintType)
class IGKNIGHTED_API UIgKnightedWeaponData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AIgKnightedProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float FireInterval = 0.25f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float ProjectileSpeed = 2200.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float Damage = 1.f;

    // Extra projectiles added to the base shot. Power Shot sets this to 2.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "8"))
    int32 BonusProjectiles = 0;

    // Total cone width in degrees. The spread is symmetric around the aim axis.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0", ClampMax = "45"))
    float SpreadDegrees = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USoundBase> FireSound;
};
