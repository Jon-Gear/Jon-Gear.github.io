#include "IgKnightedTargetingComponent.h"

#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerState.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

UIgKnightedTargetingComponent::UIgKnightedTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIgKnightedTargetingComponent::BeginPlay()
{
    Super::BeginPlay();

    // A cached target re-evaluated on an interval, not a per-frame search.
    // Twenty enemies searching the world 60 times a second is pure waste when
    // the answer changes about once a second.
    GetWorld()->GetTimerManager().SetTimer(
        EvaluationTimer, this, &UIgKnightedTargetingComponent::EvaluateTargets, EvaluationInterval, true);

    EvaluateTargets();
}

void UIgKnightedTargetingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(EvaluationTimer);
    Super::EndPlay(EndPlayReason);
}

void UIgKnightedTargetingComponent::ForceReevaluate()
{
    EvaluateTargets();
}

void UIgKnightedTargetingComponent::Configure(float InSightRadius, EIgTargetPolicy InPolicy, float InEvaluationInterval)
{
    SightRadius = InSightRadius;
    Policy = InPolicy;
    EvaluationInterval = FMath::Max(0.05f, InEvaluationInterval);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(EvaluationTimer);
        World->GetTimerManager().SetTimer(
            EvaluationTimer, this, &UIgKnightedTargetingComponent::EvaluateTargets, EvaluationInterval, true);
    }

    EvaluateTargets();
}

void UIgKnightedTargetingComponent::EvaluateTargets()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    AIgKnightedPlayerCharacter* BestTarget = nullptr;
    float BestScore = -TNumericLimits<float>::Max();

    for (TActorIterator<AIgKnightedPlayerCharacter> It(GetWorld()); It; ++It)
    {
        AIgKnightedPlayerCharacter* Candidate = *It;

        if (!IsValid(Candidate) || Candidate->IsDead_Implementation())
        {
            continue;
        }

        const float Distance = FVector::Dist2D(Owner->GetActorLocation(), Candidate->GetActorLocation());
        if (Distance > SightRadius || !HasLineOfSight(Candidate))
        {
            continue;
        }

        const float Score = ScoreCandidate(Candidate, Distance);
        if (Score > BestScore)
        {
            BestScore = Score;
            BestTarget = Candidate;
        }
    }

    AActor* PreviousTarget = CurrentTarget.Get();
    CurrentTarget = BestTarget;

    if (PreviousTarget != BestTarget)
    {
        OnTargetChanged.Broadcast(BestTarget);
    }
}

float UIgKnightedTargetingComponent::ScoreCandidate(const AActor* Candidate, float Distance) const
{
    switch (Policy)
    {
    case EIgTargetPolicy::ClosestToCentre:
        return -FVector::Dist2D(Candidate->GetActorLocation(), ArenaCentre);

    case EIgTargetPolicy::LowestScore:
        // Spread pressure toward the player having the quieter run, so a strong
        // player does not monopolise every enemy in a two-player session.
        if (const AIgKnightedPlayerState* PlayerState = Candidate->GetPlayerState<AIgKnightedPlayerState>())
        {
            return -static_cast<float>(PlayerState->GetMatchScore());
        }
        return -Distance;

    case EIgTargetPolicy::Nearest:
    default:
        return -Distance;
    }
}

bool UIgKnightedTargetingComponent::HasLineOfSight(const AActor* Candidate) const
{
    if (!Candidate || !GetOwner())
    {
        return false;
    }

    const FVector Start = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 60.f);
    const FVector End = Candidate->GetActorLocation() + FVector(0.f, 0.f, 60.f);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(IgTargetLineOfSight), false, GetOwner());

    const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params);

    return !bBlocked || Hit.GetActor() == Candidate;
}
