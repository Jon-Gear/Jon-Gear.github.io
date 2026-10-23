#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IgKnightedTypes.h"
#include "IgKnightedEnemyDefinition.generated.h"

class AIgKnightedProjectile;

/**
 * One archetype is one asset. The Crawler, the Scorpion and the Mollusk share
 * the same body class, the same targeting component and the same Behaviour Tree
 * shape; only these numbers differ.
 */
UCLASS(BlueprintType)
class IGKNIGHTED_API UIgKnightedEnemyDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Identity")
    EIgEnemyArchetype Archetype = EIgEnemyArchetype::Crawler;

    // ---- Movement ----

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float MoveSpeed = 340.f;

    /**
     * Distance the enemy tries to hold while attacking. Zero means melee: the
     * enemy closes all the way and never backs off on its own.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float PreferredRange = 0.f;

    // ---- Perception ----

    UPROPERTY(EditDefaultsOnly, Category = "Perception")
    float SightRadius = 1800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Perception")
    EIgTargetPolicy TargetPolicy = EIgTargetPolicy::Nearest;

    UPROPERTY(EditDefaultsOnly, Category = "Perception")
    float EvaluationInterval = 0.4f;

    // ---- Attack ----

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    TSubclassOf<AIgKnightedProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    FName MuzzleSocket = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float AttackCooldown = 1.6f;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float ProjectileSpeed = 1400.f;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float ProjectileDamage = 1.f;

    /** 1 = single aimed shot. A Molusk telegraphed ring is 8 or 12. */
    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    int32 RadialShotCount = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float MeleeRadius = 110.f;

    // ---- Score ----

    UPROPERTY(EditDefaultsOnly, Category = "Score")
    int32 ScoreValue = 50;
};
