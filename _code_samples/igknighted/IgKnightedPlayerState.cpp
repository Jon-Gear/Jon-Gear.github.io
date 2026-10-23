#include "IgKnightedPlayerState.h"

FLinearColor AIgKnightedPlayerState::GetPlayerColour() const
{
    if (PlayerColours.IsValidIndex(PlayerIndex))
    {
        return PlayerColours[PlayerIndex];
    }

    return PlayerIndex == 0 ? FLinearColor::Red : FLinearColor::Blue;
}

void AIgKnightedPlayerState::SetPlayerIndex(int32 InIndex)
{
    PlayerIndex = FMath::Max(0, InIndex);
}

void AIgKnightedPlayerState::AddMatchScore(int32 BasePoints)
{
    if (BasePoints <= 0)
    {
        return;
    }

    // The multiplier is applied in exactly one place. Every consumer reads the
    // resulting total; none of them recomputes it.
    MatchScore += FMath::RoundToInt(BasePoints * Multiplier);
    OnMatchScoreChanged.Broadcast(MatchScore);
}

void AIgKnightedPlayerState::AddMultiplier(float Delta)
{
    const float Previous = Multiplier;
    Multiplier = FMath::Clamp(Multiplier + Delta, 1.f, MaxMultiplier);

    if (!FMath::IsNearlyEqual(Previous, Multiplier))
    {
        OnMultiplierChanged.Broadcast(Multiplier);
    }
}

void AIgKnightedPlayerState::ResetMultiplier()
{
    if (FMath::IsNearlyEqual(Multiplier, 1.f))
    {
        return;
    }

    Multiplier = 1.f;
    OnMultiplierChanged.Broadcast(Multiplier);
}
