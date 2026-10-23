#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IgKnightedAimProvider.h"
#include "IgKnightedDamageable.h"
#include "IgKnightedTypes.h"
#include "IgKnightedEnemyCharacter.generated.h"

class UIgKnightedCombatComponent;
class UIgKnightedEnemyDefinition;
class UIgKnightedTargetingComponent;

/**
 * Shared enemy base. Archetypes differ in movement and attack data, not in the
 * damage pipeline, the death path or the score award.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedEnemyCharacter : public ACharacter, public IIgKnightedDamageable, public IIgKnightedAimProvider
{
    GENERATED_BODY()

public:
    AIgKnightedEnemyCharacter();

    virtual void BeginPlay() override;

    // ---- IIgKnightedDamageable ----
    virtual FIgDamageResult ApplyGameplayDamage_Implementation(const FIgDamageContext& Context) override;
    virtual bool IsDead_Implementation() const override { return bDefeated; }

    // ---- IIgKnightedAimProvider ----
    virtual FVector GetMuzzleLocation() const override;
    virtual FVector GetAimDirection() const override;

    UFUNCTION(BlueprintPure, Category = "Enemy")
    UIgKnightedTargetingComponent* GetTargetingComponent() const { return Targeting; }

    UFUNCTION(BlueprintPure, Category = "Enemy")
    UIgKnightedCombatComponent* GetCombatComponent() const { return Combat; }

    UFUNCTION(BlueprintPure, Category = "Enemy")
    UIgKnightedEnemyDefinition* GetDefinition() const { return Definition; }

    /** Scorpion: one aimed shot at the target's location at spawn time. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void FireAtTarget(AActor* Target);

    /** Mollusk: an evenly spaced ring, not a spread — a different question. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void FireRadialBurst();

    /** Crawler: the only thing that can damage a player on contact. */
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void PerformMeleeAttack();

    UFUNCTION(BlueprintImplementableEvent, Category = "Presentation")
    void BP_OnDefeated();

protected:
    void HandleDefeated(const FIgDamageContext& Context);
    void ApplyDefinition();

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TObjectPtr<UIgKnightedEnemyDefinition> Definition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UIgKnightedCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UIgKnightedTargetingComponent> Targeting;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    int32 ScoreValue = 50;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    float ScoreMultiplierPerKill = 0.1f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    float DeathPresentationSeconds = 1.6f;

private:
    bool bDefeated = false;
};
