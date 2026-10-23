#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IgKnightedTypes.h"
#include "IgKnightedTargetingComponent.generated.h"

/**
 * "Which player does this enemy care about?" lives in one component instead of
 * being re-implemented (slightly differently) in every enemy Blueprint.
 */
UCLASS(ClassGroup = (IgKnighted), meta = (BlueprintSpawnableComponent))
class IGKNIGHTED_API UIgKnightedTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIgKnightedTargetingComponent();

    UPROPERTY(BlueprintAssignable, Category = "Targeting")
    FOnTargetChanged OnTargetChanged;

    UFUNCTION(BlueprintPure, Category = "Targeting")
    AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void ForceReevaluate();

    /** Applied from the enemy's data asset at BeginPlay: one brain, three sets of numbers. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void Configure(float InSightRadius, EIgTargetPolicy InPolicy, float InEvaluationInterval);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float EvaluationInterval = 0.4f;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float SightRadius = 1800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    EIgTargetPolicy Policy = EIgTargetPolicy::Nearest;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    FVector ArenaCentre = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
    TWeakObjectPtr<AActor> CurrentTarget;

private:
    void EvaluateTargets();
    bool HasLineOfSight(const AActor* Candidate) const;
    float ScoreCandidate(const AActor* Candidate, float Distance) const;

    FTimerHandle EvaluationTimer;
};
