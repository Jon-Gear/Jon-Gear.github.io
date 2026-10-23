#include "IgKnightedGameState.h"

void AIgKnightedGameState::InitializeMatch(int32 InMaxLives)
{
    MaxSharedLives = FMath::Max(1, InMaxLives);

    const int32 Previous = SharedLivesRemaining;
    SharedLivesRemaining = MaxSharedLives;

    OnSharedLivesChanged.Broadcast(SharedLivesRemaining, SharedLivesRemaining - Previous);
}

void AIgKnightedGameState::SetSharedLives(int32 NewLives)
{
    // Clamping here is the last line of defence for invariant #1:
    // SharedLivesRemaining is always in [0, MaxSharedLives] and never negative.
    const int32 Clamped = FMath::Clamp(NewLives, 0, MaxSharedLives);
    if (Clamped == SharedLivesRemaining)
    {
        return;
    }

    const int32 Delta = Clamped - SharedLivesRemaining;
    SharedLivesRemaining = Clamped;

    OnSharedLivesChanged.Broadcast(SharedLivesRemaining, Delta);
}

void AIgKnightedGameState::SetMatchPhase(EMatchPhase NewPhase)
{
    if (MatchPhase == NewPhase)
    {
        return;
    }

    MatchPhase = NewPhase;
    OnMatchPhaseChanged.Broadcast(MatchPhase);
}
