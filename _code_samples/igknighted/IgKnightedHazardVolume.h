#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedHazardVolume.generated.h"

class UBoxComponent;

/**
 * A persistent damage volume (the Water Mollusk's beam).
 *
 * The overlap fires every frame the player is inside. The latch makes the first
 * lethal hit of a cycle the only one that counts; a dash or a shield does not
 * latch, so the hazard can still kill later in the same cycle.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedHazardVolume : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedHazardVolume();

    /** Called by the beam's on/off animation. */
    UFUNCTION(BlueprintCallable, Category = "Hazard")
    void BeginNewCycle();

    UFUNCTION(BlueprintCallable, Category = "Hazard")
    void EndCycle();

protected:
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hazard")
    TObjectPtr<UBoxComponent> DamageVolume;

    UPROPERTY(EditInstanceOnly, Category = "Hazard")
    int32 SourceId = 1;

private:
    TSet<TWeakObjectPtr<AActor>> LatchedThisCycle;
};
