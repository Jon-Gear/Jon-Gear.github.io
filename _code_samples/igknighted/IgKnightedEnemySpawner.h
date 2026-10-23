#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedEnemySpawner.generated.h"

class AIgKnightedEnemyCharacter;
class AIgKnightedZoneManager;

/**
 * Owns the pending-spawn count. The spawner decides when a wave is queued; the
 * ZoneManager decides when the encounter is finished.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void BeginEncounter();

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void RestartEncounter();

protected:
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void SpawnOne();

    UPROPERTY(EditInstanceOnly, Category = "Spawner")
    int32 ZoneIndex = 0;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    TSubclassOf<AIgKnightedEnemyCharacter> EnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    int32 BaseEnemyCount = 6;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    float SpawnInterval = 1.25f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    bool bAutoStart = false;

    UPROPERTY(EditInstanceOnly, Category = "Spawner")
    TArray<TObjectPtr<AActor>> SpawnPoints;

private:
    FTransform PickSpawnTransform() const;

    int32 SpawnedCount = 0;
    int32 TargetCount = 0;

    FTimerHandle SpawnTimer;
};
