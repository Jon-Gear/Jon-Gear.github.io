#include "IgKnightedCombatComponent.h"

#include "IgKnightedAimProvider.h"
#include "IgKnightedProjectile.h"
#include "IgKnightedWeaponData.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

void UIgKnightedCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    ActiveWeaponData = BaseWeaponData;
}

void UIgKnightedCombatComponent::StartFiring()
{
    if (bIsFiring || !ActiveWeaponData)
    {
        return;
    }

    bIsFiring = true;
    FireOnce();

    if (ActiveWeaponData->FireInterval > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            FireTimer, this, &UIgKnightedCombatComponent::FireOnce, ActiveWeaponData->FireInterval, true);
    }
}

void UIgKnightedCombatComponent::StopFiring()
{
    bIsFiring = false;
    GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void UIgKnightedCombatComponent::FireOnce()
{
    if (!bIsFiring || !GetOwner() || GetOwner()->IsPendingKillPending())
    {
        return;
    }

    BP_PlayFireMontage();

    if (!bUseAnimationNotify)
    {
        SpawnProjectileFromMuzzle();
    }
}

void UIgKnightedCombatComponent::SpawnProjectileFromMuzzle()
{
    UIgKnightedWeaponData* Weapon = ActiveWeaponData;
    if (!Weapon || !Weapon->ProjectileClass)
    {
        return;
    }

    const IIgKnightedAimProvider* AimProvider = Cast<IIgKnightedAimProvider>(GetOwner());
    if (!AimProvider)
    {
        return;
    }

    const FVector Origin = AimProvider->GetMuzzleLocation();
    const FVector BaseDirection = AimProvider->GetAimDirection().GetSafeNormal();
    if (BaseDirection.IsNearlyZero())
    {
        return;
    }

    const int32 TotalShots = 1 + Weapon->BonusProjectiles;
    const float HalfSpreadDegrees = Weapon->SpreadDegrees * 0.5f;

    FActorSpawnParameters Params;
    Params.Owner = GetOwner();
    Params.Instigator = Cast<APawn>(GetOwner());
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 ShotIndex = 0; ShotIndex < TotalShots; ++ShotIndex)
    {
        // One shot is dead centre. A spread is symmetric around the aim axis,
        // so three shots are -half, 0, +half — not 0, +half, +half+half.
        const float Alpha = TotalShots > 1
            ? static_cast<float>(ShotIndex) / static_cast<float>(TotalShots - 1)
            : 0.5f;

        const float AngleOffset = FMath::Lerp(-HalfSpreadDegrees, HalfSpreadDegrees, Alpha);
        const FVector Direction = BaseDirection.RotateAngleAxis(AngleOffset, FVector::UpVector);

        AIgKnightedProjectile* Projectile = GetWorld()->SpawnActor<AIgKnightedProjectile>(
            Weapon->ProjectileClass, Origin, Direction.Rotation(), Params);

        if (Projectile)
        {
            Projectile->InitializeProjectile(Direction, Weapon->ProjectileSpeed, Weapon->Damage);
        }
    }

    OnFired.Broadcast(Origin);
}

void UIgKnightedCombatComponent::SetActiveWeaponData(UIgKnightedWeaponData* NewWeapon)
{
    if (!NewWeapon)
    {
        return;
    }

    const bool bWasFiring = bIsFiring;
    StopFiring();

    ActiveWeaponData = NewWeapon;

    if (bWasFiring)
    {
        StartFiring();
    }
}

void UIgKnightedCombatComponent::RestoreBaseWeaponData()
{
    if (BaseWeaponData)
    {
        SetActiveWeaponData(BaseWeaponData);
    }
}
