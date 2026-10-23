#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedTypes.h"
#include "IgKnightedZoneManager.generated.h"

class AIgKnightedBarrier;
class AIgKnightedEnemySpawner;

/**
 * One authoritative answer to "is this encounter finished?".
 *
 * Living enemies and pending spawns are tracked separately. The encounter is
 * complete only when BOTH are zero — the bug we shipped was checking only the
 * first one, which opened a barrier while a wave was still queued.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedZoneManager : public AActor
{
    GENERATED_BODY()

public:
    static AIgKnightedZoneManager* Get(const UObject* WorldContextObject);

    UPROPERTY(BlueprintAssignable, Category = "Zones")
    FOnZoneStarted OnZoneStarted;

    UPROPERTY(BlueprintAssignable, Category = "Zones")
    FOnZoneCleared OnZoneCleared;

    UPROPERTY(BlueprintAssignable, Category = "Zones")
    FOnBarrierStateChanged OnBarrierStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Zones")
    void BeginEncounter(int32 ZoneIndex, int32 PendingSpawns);

    /** Called by the spawner BEFORE FinishSpawning, so an enemy can never die unregistered. */
    UFUNCTION(BlueprintCallable, Category = "Zones")
    void NotifyEnemySpawned(AActor* Enemy);

    UFUNCTION(BlueprintCallable, Category = "Zones")
    void NotifyEnemyDefeated(AActor* Enemy);

    UFUNCTION(BlueprintCallable, Category = "Zones")
    void RestartActiveEncounter();

    UFUNCTION(BlueprintPure, Category = "Zones")
    bool IsEncounterComplete() const;

    UFUNCTION(BlueprintPure, Category = "Zones")
    int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }

    UFUNCTION(BlueprintPure, Category = "Zones")
    int32 GetPendingSpawnCount() const { return PendingSpawnCount; }

    void RegisterSpawner(AIgKnightedEnemySpawner* InSpawner) { ActiveSpawner = InSpawner; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditInstanceOnly, Category = "Zones")
    TArray<TObjectPtr<AIgKnightedBarrier>> ZoneBarriers;

    UPROPERTY(EditDefaultsOnly, Category = "Zones")
    int32 FinalZoneIndex = 2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zones")
    int32 CurrentZoneIndex = INDEX_NONE;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zones")
    int32 PendingSpawnCount = 0;

private:
    void ScheduleCompletionCheck();
    void EvaluateCompletion();
    void CompleteEncounter();
    void SetBarrierOpen(int32 ZoneIndex, bool bOpen);

    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> AliveEnemies;

    UPROPERTY()
    TObjectPtr<AIgKnightedEnemySpawner> ActiveSpawner;

    bool bEncounterActive = false;
    bool bCompletionCheckScheduled = false;

    int32 EncounterGeneration = 0;
    int32 PendingCheckGeneration = INDEX_NONE;
};
