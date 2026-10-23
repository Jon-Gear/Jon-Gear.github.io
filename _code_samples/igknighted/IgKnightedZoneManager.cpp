#include "IgKnightedZoneManager.h"

#include "IgKnightedBarrier.h"
#include "IgKnightedEnemySpawner.h"
#include "IgKnightedGameMode.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AIgKnightedZoneManager* AIgKnightedZoneManager::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject || !WorldContextObject->GetWorld())
    {
        return nullptr;
    }

    return Cast<AIgKnightedZoneManager>(
        UGameplayStatics::GetActorOfClass(WorldContextObject, AIgKnightedZoneManager::StaticClass()));
}

void AIgKnightedZoneManager::BeginPlay()
{
    Super::BeginPlay();

    AliveEnemies.Reserve(64);

    for (int32 Index = 0; Index < ZoneBarriers.Num(); ++Index)
    {
        SetBarrierOpen(Index, false);
    }
}

void AIgKnightedZoneManager::BeginEncounter(int32 ZoneIndex, int32 PendingSpawns)
{
    ++EncounterGeneration;

    CurrentZoneIndex = ZoneIndex;
    AliveEnemies.Reset();
    PendingSpawnCount = FMath::Max(0, PendingSpawns);
    bEncounterActive = true;
    bCompletionCheckScheduled = false;

    OnZoneStarted.Broadcast(ZoneIndex);

    // Handles the degenerate case of an encounter with zero enemies.
    ScheduleCompletionCheck();
}

void AIgKnightedZoneManager::NotifyEnemySpawned(AActor* Enemy)
{
    if (!bEncounterActive || !IsValid(Enemy) || AliveEnemies.Contains(Enemy))
    {
        return;
    }

    AliveEnemies.Add(Enemy);
    PendingSpawnCount = FMath::Max(0, PendingSpawnCount - 1);
}

void AIgKnightedZoneManager::NotifyEnemyDefeated(AActor* Enemy)
{
    if (!AliveEnemies.Remove(Enemy))
    {
        return;
    }

    // Deliberately deferred by one tick. Damage resolution runs first, so if the
    // last life is spent in this frame the GameMode has already recorded Defeat
    // before the completion rule can claim Victory.
    ScheduleCompletionCheck();
}

void AIgKnightedZoneManager::ScheduleCompletionCheck()
{
    if (bCompletionCheckScheduled)
    {
        return;
    }

    bCompletionCheckScheduled = true;
    PendingCheckGeneration = EncounterGeneration;

    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AIgKnightedZoneManager::EvaluateCompletion);
}

void AIgKnightedZoneManager::EvaluateCompletion()
{
    bCompletionCheckScheduled = false;

    // A stale check must not complete a newer encounter.
    if (!bEncounterActive || PendingCheckGeneration != EncounterGeneration)
    {
        return;
    }

    // Destroyed actors that never reported a death are pruned here; the counter
    // is authoritative, not the actor lifetime.
    for (auto It = AliveEnemies.CreateIterator(); It; ++It)
    {
        if (!It->IsValid())
        {
            It.RemoveCurrent();
        }
    }

    if (PendingSpawnCount > 0 || AliveEnemies.Num() > 0)
    {
        return;
    }

    CompleteEncounter();
}

void AIgKnightedZoneManager::CompleteEncounter()
{
    bEncounterActive = false;

    const int32 ClearedZone = CurrentZoneIndex;

    SetBarrierOpen(ClearedZone, true);
    OnZoneCleared.Broadcast(ClearedZone);

    if (ClearedZone >= FinalZoneIndex)
    {
        if (AIgKnightedGameMode* GameMode = GetWorld()->GetAuthGameMode<AIgKnightedGameMode>())
        {
            GameMode->HandleFinalZoneCleared();
        }
    }
}

void AIgKnightedZoneManager::SetBarrierOpen(int32 ZoneIndex, bool bOpen)
{
    if (!ZoneBarriers.IsValidIndex(ZoneIndex) || !IsValid(ZoneBarriers[ZoneIndex]))
    {
        return;
    }

    ZoneBarriers[ZoneIndex]->SetOpen(bOpen);
    OnBarrierStateChanged.Broadcast(ZoneIndex, bOpen);
}

void AIgKnightedZoneManager::RestartActiveEncounter()
{
    if (!bEncounterActive)
    {
        return;
    }

    for (const TWeakObjectPtr<AActor>& WeakEnemy : AliveEnemies)
    {
        if (WeakEnemy.IsValid())
        {
            WeakEnemy->Destroy();
        }
    }

    AliveEnemies.Reset();
    PendingSpawnCount = 0;
    bEncounterActive = false;
    bCompletionCheckScheduled = false;
    ++EncounterGeneration;

    if (ActiveSpawner)
    {
        ActiveSpawner->RestartEncounter();
    }
}

bool AIgKnightedZoneManager::IsEncounterComplete() const
{
    return bEncounterActive && AliveEnemies.Num() == 0 && PendingSpawnCount == 0;
}
