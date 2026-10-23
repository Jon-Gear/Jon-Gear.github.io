#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

/**
 * Damage is delivered through IIgKnightedDamageable. The projectile has no
 * Cast<Player> and no Cast<Enemy>: anything that implements the interface is
 * hit correctly.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedProjectile : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedProjectile();

    void InitializeProjectile(const FVector& Direction, float Speed, float InDamage);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    TObjectPtr<UProjectileMovementComponent> Movement;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float Damage = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float LifeSpanSeconds = 2.5f;

private:
    // One projectile can report more than one hit in a frame. This latch makes
    // the first one authoritative.
    bool bHasImpacted = false;
};
