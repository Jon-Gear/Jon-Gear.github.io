#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedPowerUpPickup.generated.h"

class AIgKnightedPlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;

/**
 * A pickup asks "may this player take this?" and the buff owner answers.
 * The pickup never decides whether a buff is a duplicate.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedPowerUpPickup : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedPowerUpPickup();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    bool CanBeCollectedBy(const AIgKnightedPlayerCharacter* Player) const;
    void Collect(AIgKnightedPlayerCharacter* Player);
    void SetPickupAvailable(bool bAvailable);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<USphereComponent> OverlapSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    EIgBuffKind BuffKind = EIgBuffKind::Shield;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float DurationSeconds = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    int32 ScoreValue = 25;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float RespawnDelaySeconds = 12.f;

private:
    bool bAvailable = true;
    FTimerHandle RespawnTimer;
};
