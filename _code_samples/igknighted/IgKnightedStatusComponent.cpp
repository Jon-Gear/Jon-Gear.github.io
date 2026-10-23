#include "IgKnightedStatusComponent.h"

#include "IgKnightedCombatComponent.h"
#include "IgKnightedDashComponent.h"
#include "IgKnightedWeaponData.h"

#include "Engine/World.h"
#include "TimerManager.h"

void UIgKnightedStatusComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        Combat = Owner->FindComponentByClass<UIgKnightedCombatComponent>();
        Dash = Owner->FindComponentByClass<UIgKnightedDashComponent>();
    }
}

bool UIgKnightedStatusComponent::CanAcceptBuff(EIgBuffKind Kind) const
{
    switch (Kind)
    {
    case EIgBuffKind::Shield:
        // A second shield while shielded is not consumed. The pickup asks this
        // question; it does not decide the answer.
        return ShieldCharges == 0;

    case EIgBuffKind::PowerShot:
        return Combat && PowerShotWeaponData && Combat->GetWeaponData() != PowerShotWeaponData;

    case EIgBuffKind::EnhancedDash:
        return !bEnhancedDashActive;

    case EIgBuffKind::Matchstick:
        return true;

    default:
        return false;
    }
}

bool UIgKnightedStatusComponent::TryApplyBuff(EIgBuffKind Kind, float DurationSeconds)
{
    if (!CanAcceptBuff(Kind))
    {
        return false;
    }

    const float Duration = DurationSeconds > 0.f ? DurationSeconds : DefaultBuffDuration;

    switch (Kind)
    {
    case EIgBuffKind::Shield:
        ShieldCharges = 1;
        OnShieldChanged.Broadcast(ShieldCharges);
        return true;

    case EIgBuffKind::PowerShot:
        Combat->SetActiveWeaponData(PowerShotWeaponData);
        GetWorld()->GetTimerManager().SetTimer(
            PowerShotTimer, this, &UIgKnightedStatusComponent::ExpirePowerShot, Duration, false);
        return true;

    case EIgBuffKind::EnhancedDash:
        bEnhancedDashActive = true;

        if (Dash)
        {
            Dash->SetEnhancedDashActive(true);
        }

        GetWorld()->GetTimerManager().SetTimer(
            EnhancedDashTimer, this, &UIgKnightedStatusComponent::ExpireEnhancedDash, Duration, false);
        return true;

    case EIgBuffKind::Matchstick:
        // Matchstick is an instant score award, handled by the pickup.
        return false;

    default:
        return false;
    }
}

bool UIgKnightedStatusComponent::ConsumeShield()
{
    if (ShieldCharges <= 0)
    {
        return false;
    }

    --ShieldCharges;
    OnShieldChanged.Broadcast(ShieldCharges);
    return true;
}

void UIgKnightedStatusComponent::ExpirePowerShot()
{
    if (Combat)
    {
        Combat->RestoreBaseWeaponData();
    }

    OnBuffExpired.Broadcast(EIgBuffKind::PowerShot);
}

void UIgKnightedStatusComponent::ExpireEnhancedDash()
{
    bEnhancedDashActive = false;

    if (Dash)
    {
        Dash->SetEnhancedDashActive(false);
    }

    OnBuffExpired.Broadcast(EIgBuffKind::EnhancedDash);
}

void UIgKnightedStatusComponent::ResetForRespawn()
{
    GetWorld()->GetTimerManager().ClearTimer(PowerShotTimer);
    GetWorld()->GetTimerManager().ClearTimer(EnhancedDashTimer);

    if (Combat)
    {
        Combat->RestoreBaseWeaponData();
    }

    bEnhancedDashActive = false;

    if (Dash)
    {
        Dash->SetEnhancedDashActive(false);
    }

    if (ShieldCharges != 0)
    {
        ShieldCharges = 0;
        OnShieldChanged.Broadcast(ShieldCharges);
    }
}
