#include "IgKnightedDashComponent.h"

#include "IgKnightedAimProvider.h"
#include "IgKnightedDamageable.h"

#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UIgKnightedDashComponent::UIgKnightedDashComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UIgKnightedDashComponent::BeginPlay()
{
    Super::BeginPlay();

    BaseMaxCharges = MaxCharges;
    CurrentCharges = MaxCharges;
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);
}

bool UIgKnightedDashComponent::TryStartDash()
{
    if (bDashing || CurrentCharges <= 0 || !GetOwner())
    {
        return false;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return false;
    }

    DashDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal2D();

    if (DashDirection.IsNearlyZero())
    {
        if (const IIgKnightedAimProvider* Aim = Cast<IIgKnightedAimProvider>(GetOwner()))
        {
            DashDirection = Aim->GetAimDirection().GetSafeNormal2D();
        }
    }

    if (DashDirection.IsNearlyZero())
    {
        DashDirection = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
    }

    --CurrentCharges;
    bDashing = true;

    // Invulnerability is enabled BEFORE the first frame of movement. Enabling it
    // in the first tick would let a hazard that resolves this frame kill the
    // player at the exact moment the dash "started".
    bInvulnerable = true;
    TraversedActors.Reset();

    SetComponentTickEnabled(true);

    OnDashStateChanged.Broadcast(true, true);
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);

    GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &UIgKnightedDashComponent::EndDash, DashDuration, false);
    return true;
}

void UIgKnightedDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bDashing)
    {
        SetComponentTickEnabled(false);
        return;
    }

    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        // Set the velocity every frame rather than launching once, so the dash
        // covers a predictable distance regardless of braking settings.
        OwnerCharacter->GetCharacterMovement()->Velocity = DashDirection * DashSpeed;
        OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }

    if (bEnhancedDashActive)
    {
        KillActorsCrossedThisFrame();
    }
}

void UIgKnightedDashComponent::EndDash()
{
    if (!bDashing)
    {
        return;
    }

    bDashing = false;
    bInvulnerable = false;
    DashDirection = FVector::ZeroVector;

    SetComponentTickEnabled(false);

    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        FVector& Velocity = OwnerCharacter->GetCharacterMovement()->Velocity;
        Velocity = Velocity.GetClampedToMaxSize(OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
    }

    OnDashStateChanged.Broadcast(false, false);

    if (CurrentCharges < MaxCharges)
    {
        GetWorld()->GetTimerManager().SetTimer(
            RechargeTimer, this, &UIgKnightedDashComponent::RechargeOneCharge, ChargeCooldown, false);
    }
}

void UIgKnightedDashComponent::RechargeOneCharge()
{
    if (CurrentCharges >= MaxCharges)
    {
        return;
    }

    ++CurrentCharges;
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);

    // Charges refill one at a time, so a spent charge cannot be refilled to full
    // instantly by waiting out a single cooldown.
    if (CurrentCharges < MaxCharges)
    {
        GetWorld()->GetTimerManager().SetTimer(
            RechargeTimer, this, &UIgKnightedDashComponent::RechargeOneCharge, ChargeCooldown, false);
    }
}

void UIgKnightedDashComponent::KillActorsCrossedThisFrame()
{
    TArray<FOverlapResult> Overlaps;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(IgDashTraversal), false, GetOwner());
    const FCollisionShape Shape = FCollisionShape::MakeSphere(TraversalKillRadius);

    GetWorld()->OverlapMultiByObjectType(
        Overlaps,
        GetOwner()->GetActorLocation(),
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        Shape,
        Params);

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* OtherActor = Overlap.GetActor();

        if (!IsValid(OtherActor) || OtherActor == GetOwner() || !OtherActor->ActorHasTag(TEXT("Ig.Enemy")))
        {
            continue;
        }

        if (TraversedActors.Contains(OtherActor))
        {
            continue;
        }

        // The sweep can generate several overlaps across consecutive frames.
        // One enemy, one kill, one entry.
        TraversedActors.Add(OtherActor);

        if (IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor))
        {
            FIgDamageContext Context;
            Context.Instigator = GetOwner();
            Context.HitLocation = OtherActor->GetActorLocation();
            Context.Kind = EIgDamageKind::DashTraversal;

            Damageable->ApplyGameplayDamage(Context);
        }
    }
}

void UIgKnightedDashComponent::SetEnhancedDashActive(bool bActive)
{
    if (bEnhancedDashActive == bActive)
    {
        return;
    }

    bEnhancedDashActive = bActive;
    MaxCharges = bActive ? EnhancedMaxCharges : BaseMaxCharges;

    if (bActive)
    {
        CurrentCharges = MaxCharges;
    }
    else
    {
        CurrentCharges = FMath::Min(CurrentCharges, MaxCharges);
    }

    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);
}

void UIgKnightedDashComponent::ResetForRespawn()
{
    GetWorld()->GetTimerManager().ClearTimer(DashTimer);
    GetWorld()->GetTimerManager().ClearTimer(RechargeTimer);

    bDashing = false;
    bInvulnerable = false;
    DashDirection = FVector::ZeroVector;
    TraversedActors.Reset();
    SetComponentTickEnabled(false);

    CurrentCharges = MaxCharges;

    OnDashStateChanged.Broadcast(false, false);
    OnDashChargesChanged.Broadcast(CurrentCharges, MaxCharges);
}
