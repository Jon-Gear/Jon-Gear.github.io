#include "IgKnightedAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "IgKnightedEnemyCharacter.h"
#include "IgKnightedTargetingComponent.h"

AIgKnightedAIController::AIgKnightedAIController()
{
    // The controller ticks nothing itself: the tree, the Pawn and the targeting
    // component own their own timing.
    PrimaryActorTick.bCanEverTick = false;
}

void AIgKnightedAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

    AIgKnightedEnemyCharacter* Enemy = Cast<AIgKnightedEnemyCharacter>(InPawn);
    if (!Enemy)
    {
        return;
    }

    if (UIgKnightedTargetingComponent* Targeting = Enemy->GetTargetingComponent())
    {
        BoundTargeting = Targeting;
        Targeting->OnTargetChanged.AddUniqueDynamic(this, &AIgKnightedAIController::HandleTargetChanged);

        // Seed from current state: the component may already have a target from
        // a previous possession, and a blackboard that starts empty makes the
        // tree idle for one evaluation interval for no reason.
        WriteTargetToBlackboard(Targeting->GetCurrentTarget());
    }
}

void AIgKnightedAIController::OnUnPossess()
{
    if (UIgKnightedTargetingComponent* Targeting = BoundTargeting.Get())
    {
        Targeting->OnTargetChanged.RemoveDynamic(this, &AIgKnightedAIController::HandleTargetChanged);
    }

    BoundTargeting.Reset();
    Super::OnUnPossess();
}

void AIgKnightedAIController::HandleTargetChanged(AActor* NewTarget)
{
    WriteTargetToBlackboard(NewTarget);
}

void AIgKnightedAIController::WriteTargetToBlackboard(AActor* NewTarget)
{
    UBlackboardComponent* Blackboard = GetBlackboardComponent();
    if (!Blackboard)
    {
        return;
    }

    // The blackboard is a projection of the targeting component, never a second
    // source of truth. Every write goes through this function.
    Blackboard->SetValueAsObject(TargetKey, IsValid(NewTarget) ? NewTarget : nullptr);
    Blackboard->SetValueAsBool(HasTargetKey, IsValid(NewTarget));

    if (IsValid(NewTarget))
    {
        Blackboard->SetValueAsVector(LastKnownLocationKey, NewTarget->GetActorLocation());
        SetFocus(NewTarget, EAIFocusPriority::Gameplay);
    }
    else
    {
        // Without this the tree keeps chasing the location of a player that
        // ceased to exist — the "enemy attacks a corpse" bug.
        ClearFocus(EAIFocusPriority::Gameplay);
    }
}
