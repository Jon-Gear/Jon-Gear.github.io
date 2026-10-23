#include "IgKnightedPowerUpPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerState.h"
#include "IgKnightedStatusComponent.h"

#include "Engine/World.h"
#include "TimerManager.h"

AIgKnightedPowerUpPickup::AIgKnightedPowerUpPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
    OverlapSphere->InitSphereRadius(60.f);
    OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    OverlapSphere->SetGenerateOverlapEvents(true);
    RootComponent = OverlapSphere;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(OverlapSphere);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Tags.Add(TEXT("Ig.Pickup"));
}

void AIgKnightedPowerUpPickup::BeginPlay()
{
    Super::BeginPlay();

    OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AIgKnightedPowerUpPickup::OnOverlap);
}

void AIgKnightedPowerUpPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AIgKnightedPlayerCharacter* Player = Cast<AIgKnightedPlayerCharacter>(OtherActor);
    if (!bAvailable || !Player || !CanBeCollectedBy(Player))
    {
        return;
    }

    Collect(Player);
}

bool AIgKnightedPowerUpPickup::CanBeCollectedBy(const AIgKnightedPlayerCharacter* Player) const
{
    if (!Player || Player->IsDead_Implementation())
    {
        return false;
    }

    if (BuffKind == EIgBuffKind::Matchstick)
    {
        return true;
    }

    const UIgKnightedStatusComponent* Status = Player->GetStatusComponent();
    return Status && Status->CanAcceptBuff(BuffKind);
}

void AIgKnightedPowerUpPickup::Collect(AIgKnightedPlayerCharacter* Player)
{
    if (!Player)
    {
        return;
    }

    if (BuffKind == EIgBuffKind::Matchstick)
    {
        if (AIgKnightedPlayerState* PlayerState = Player->GetPlayerState<AIgKnightedPlayerState>())
        {
            PlayerState->AddMatchScore(ScoreValue);
        }
    }
    else if (UIgKnightedStatusComponent* Status = Player->GetStatusComponent())
    {
        if (!Status->TryApplyBuff(BuffKind, DurationSeconds))
        {
            return;
        }
    }

    SetPickupAvailable(false);

    if (RespawnDelaySeconds > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            RespawnTimer, FTimerDelegate::CreateWeakLambda(this, [this]() { SetPickupAvailable(true); }),
            RespawnDelaySeconds, false);
    }
}

void AIgKnightedPowerUpPickup::SetPickupAvailable(bool bNewAvailable)
{
    bAvailable = bNewAvailable;
    SetActorHiddenInGame(!bNewAvailable);
    SetActorEnableCollision(bNewAvailable);
}
