#include "IgKnightedDamageResolver.h"

#include "IgKnightedDashComponent.h"
#include "IgKnightedStatusComponent.h"

bool UIgKnightedDamageResolver::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

FIgDamageResult UIgKnightedDamageResolver::ResolvePlayerDamage(AActor* Victim, const FIgDamageContext& Context)
{
    FIgDamageResult Result;

    if (!IsValid(Victim) || !Context.IsValid())
    {
        return Result;
    }

    // 1. Dash i-frames. Checked first because dashing is a free action: it must
    //    not consume the shield, and it must work even while shielded.
    if (UIgKnightedDashComponent* Dash = Victim->FindComponentByClass<UIgKnightedDashComponent>())
    {
        // Queried at resolution time, not at delivery time: a projectile that
        // was fired before the dash started still counts as dodged.
        if (Dash->IsInvulnerable())
        {
            Result.Outcome = EIgDamageOutcome::Ignored;
            OnDamageIgnored.Broadcast(Victim, Context);
            return Result;
        }
    }

    // 2. Shield. A consumable resource, spent once, before the lethal branch.
    if (UIgKnightedStatusComponent* Status = Victim->FindComponentByClass<UIgKnightedStatusComponent>())
    {
        if (Status->ConsumeShield())
        {
            Result.Outcome = EIgDamageOutcome::Absorbed;
            OnShieldBroken.Broadcast(Victim);
            return Result;
        }
    }

    // 3. Lethal. The GameMode is the only object allowed to spend the life; this
    //    broadcast is its notification, not a second ledger.
    Result.Outcome = EIgDamageOutcome::Lethal;
    Result.bKilledTarget = true;
    OnLethalDamage.Broadcast(Victim, Context);

    return Result;
}
