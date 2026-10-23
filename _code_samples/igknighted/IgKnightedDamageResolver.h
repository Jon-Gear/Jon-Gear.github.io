#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "IgKnightedTypes.h"
#include "IgKnightedDamageResolver.generated.h"

/**
 * The single place where a hit becomes an outcome.
 *
 * Precedence is a property of this function, not a convention every enemy
 * author has to remember:
 *   1. dash i-frames      -> ignored (free action, no resource spent)
 *   2. shield             -> absorbed (consumable resource)
 *   3. otherwise          -> lethal (one shared life)
 */
UCLASS()
class IGKNIGHTED_API UIgKnightedDamageResolver : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Damage")
    FOnDamageIgnored OnDamageIgnored;

    UPROPERTY(BlueprintAssignable, Category = "Damage")
    FOnShieldBroken OnShieldBroken;

    UPROPERTY(BlueprintAssignable, Category = "Damage")
    FOnLethalDamage OnLethalDamage;

    UFUNCTION(BlueprintCallable, Category = "Damage")
    FIgDamageResult ResolvePlayerDamage(AActor* Victim, const FIgDamageContext& Context);

    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
};
