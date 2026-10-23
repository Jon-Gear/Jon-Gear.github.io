#include "IgKnightedDifficultyDirector.h"

void UIgKnightedDifficultyDirector::Reset()
{
    CurrentScale = 1.f;
}

float UIgKnightedDifficultyDirector::Evaluate(const FIgDirectorInput& Input, float DeltaSeconds)
{
    // Normalised signals, not raw counts. A raw enemy count means something
    // different in Zone 1 and Zone 3.
    const float LifePressure = FMath::Clamp(static_cast<float>(Input.LivesLostThisZone) / 3.f, 0.f, 1.f);

    const float KillsPerSecond =
        static_cast<float>(Input.EnemiesDefeatedThisZone) / FMath::Max(1.f, Input.TimeInZoneSeconds);
    const float Dominance = FMath::Clamp(KillsPerSecond * 4.f, 0.f, 1.f);

    // Losing lives is the strongest signal. Clearing fast is a weaker one, so a
    // player who is dominating gets pressure, not punishment.
    const float TargetScale = 1.f - (0.15f * LifePressure) + (0.10f * Dominance);

    const float Desired = FMath::Clamp(TargetScale, MinScale, MaxScale);

    // Damping is what prevents the oscillation that makes dynamic difficulty
    // feel arbitrary.
    CurrentScale = FMath::FInterpTo(CurrentScale, Desired, DeltaSeconds, 1.f / FMath::Max(0.01f, SmoothingSeconds));

    return CurrentScale;
}
