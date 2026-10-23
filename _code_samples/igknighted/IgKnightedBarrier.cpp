#include "IgKnightedBarrier.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AIgKnightedBarrier::AIgKnightedBarrier()
{
    PrimaryActorTick.bCanEverTick = false;

    BlockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingVolume"));
    BlockingVolume->SetCollisionProfileName(TEXT("BlockAll"));
    RootComponent = BlockingVolume;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(BlockingVolume);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetOpen(false);
}

void AIgKnightedBarrier::SetOpen(bool bOpen)
{
    this->bOpen = bOpen;

    BlockingVolume->SetCollisionEnabled(bOpen ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
    BlockingVolume->SetGenerateOverlapEvents(!bOpen);

    // The visual is allowed to animate out; the collision is authoritative.
    BP_OnOpenChanged(bOpen);
}
