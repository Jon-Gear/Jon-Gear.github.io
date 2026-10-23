#include "IgKnightedProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "IgKnightedDamageable.h"

AIgKnightedProjectile::AIgKnightedProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(14.f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->SetGenerateOverlapEvents(false);

    RootComponent = Collision;

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->UpdatedComponent = Collision;
    Movement->bRotationFollowsVelocity = true;
    Movement->bShouldBounce = false;
    Movement->ProjectileGravityScale = 0.f;
}

void AIgKnightedProjectile::BeginPlay()
{
    Super::BeginPlay();

    Collision->OnComponentHit.AddDynamic(this, &AIgKnightedProjectile::OnProjectileHit);

    // A shot that leaves the arena still ticks and still collides. The lifespan
    // is the cheapest possible bound on that cost.
    SetLifeSpan(LifeSpanSeconds);
}

void AIgKnightedProjectile::InitializeProjectile(const FVector& Direction, float Speed, float InDamage)
{
    Damage = InDamage;

    Movement->Velocity = Direction.GetSafeNormal() * Speed;

    if (AActor* OwnerActor = GetOwner())
    {
        Movement->IgnoreActorWhenMoving(OwnerActor, true);
    }
}

void AIgKnightedProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasImpacted || !IsValid(OtherActor) || OtherActor == GetOwner())
    {
        return;
    }

    bHasImpacted = true;

    if (IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor))
    {
        FIgDamageContext Context;
        Context.Instigator = GetOwner();
        Context.HitLocation = Hit.ImpactPoint;
        Context.HitNormal = Hit.ImpactNormal;
        Context.Kind = EIgDamageKind::Projectile;

        Damageable->ApplyGameplayDamage(Context);
    }

    Destroy();
}
