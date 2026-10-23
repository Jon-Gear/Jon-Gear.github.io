#include "IgKnightedHazardVolume.h"

#include "Components/BoxComponent.h"

#include "IgKnightedDamageable.h"

AIgKnightedHazardVolume::AIgKnightedHazardVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    DamageVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageVolume"));
    DamageVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    DamageVolume->SetGenerateOverlapEvents(true);
    RootComponent = DamageVolume;
}

void AIgKnightedHazardVolume::BeginNewCycle()
{
    LatchedThisCycle.Reset();
}

void AIgKnightedHazardVolume::EndCycle()
{
    LatchedThisCycle.Reset();
}

void AIgKnightedHazardVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsValid(OtherActor) || !OtherActor->ActorHasTag(TEXT("Ig.Player")))
    {
        return;
    }

    if (LatchedThisCycle.Contains(OtherActor))
    {
        return;
    }

    IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor);
    if (!Damageable)
    {
        return;
    }

    FIgDamageContext Context;
    Context.Instigator = this;
    Context.HitLocation = OtherActor->GetActorLocation();
    Context.Kind = EIgDamageKind::Water;
    Context.SourceId = SourceId;

    const FIgDamageResult Result = Damageable->ApplyGameplayDamage(Context);

    // Latch only when the hit actually cost a life. If the player dashed through
    // or the shield absorbed it, the next overlap inside the same beam cycle is
    // still allowed to resolve.
    if (Result.Outcome == EIgDamageOutcome::Lethal)
    {
        LatchedThisCycle.Add(OtherActor);
    }
}

void AIgKnightedHazardVolume::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
    // Leaving the beam and walking back in is a new hit, even in the same cycle.
    LatchedThisCycle.Remove(OtherActor);
}
