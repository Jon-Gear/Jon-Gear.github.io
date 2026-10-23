#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "IgBTTask_FireProjectile.generated.h"

/**
 * The task asks the combat component to fire. It does not build a projectile,
 * choose a socket or play an animation.
 */
UCLASS()
class IGKNIGHTED_API UBTTask_IgFireProjectile : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_IgFireProjectile();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    bool bFailWithoutTarget = true;
};
