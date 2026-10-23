#include "IgBTTask_FireProjectile.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "IgKnightedCombatComponent.h"

UBTTask_IgFireProjectile::UBTTask_IgFireProjectile()
{
    NodeName = TEXT("Ig Fire Projectile");

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_IgFireProjectile, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_IgFireProjectile::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    if (bFailWithoutTarget)
    {
        const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
        const AActor* Target = Blackboard ? Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;

        if (!Target)
        {
            return EBTNodeResult::Failed;
        }
    }

    UIgKnightedCombatComponent* Combat = Pawn->FindComponentByClass<UIgKnightedCombatComponent>();
    if (!Combat)
    {
        return EBTNodeResult::Failed;
    }

    Combat->SpawnProjectileFromMuzzle();
    return EBTNodeResult::Succeeded;
}
