#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IgKnightedTypes.h"
#include "IgKnightedDamageable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UIgKnightedDamageable : public UInterface
{
    GENERATED_BODY()
};

/**
 * Anything that can receive damage without the damage source knowing its concrete type.
 * The projectile talks to this interface; it never casts to a player or an enemy class.
 */
class IGKNIGHTED_API IIgKnightedDamageable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    FIgDamageResult ApplyGameplayDamage(const FIgDamageContext& Context);
    virtual FIgDamageResult ApplyGameplayDamage_Implementation(const FIgDamageContext& Context) = 0;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    bool IsDead() const;
    virtual bool IsDead_Implementation() const = 0;
};
