#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "IgKnightedAIController.generated.h"

class AIgKnightedEnemyCharacter;
class UIgKnightedTargetingComponent;
class UBehaviorTree;

/**
 * The brain of every archetype. It owns the Behaviour Tree and the blackboard;
 * the Pawn owns the body and the targeting component. The only thing crossing
 * between them is the target.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedAIController : public AAIController
{
    GENERATED_BODY()

public:
    AIgKnightedAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

protected:
    UFUNCTION()
    void HandleTargetChanged(AActor* NewTarget);

    void WriteTargetToBlackboard(AActor* NewTarget);

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName TargetKey = TEXT("TargetActor");

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName LastKnownLocationKey = TEXT("LastKnownLocation");

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    FName HasTargetKey = TEXT("HasTarget");

private:
    TWeakObjectPtr<UIgKnightedTargetingComponent> BoundTargeting;
};
