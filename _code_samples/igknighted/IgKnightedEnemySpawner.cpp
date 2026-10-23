#include "IgKnightedEnemySpawner.h"

#include "IgKnightedEnemyCharacter.h"
#include "IgKnightedGameMode.h"
#include "IgKnightedZoneManager.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AIgKnightedEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->RegisterSpawner(this);
    }

    if (bAutoStart)
    {
        BeginEncounter();
    }
}

void AIgKnightedEnemySpawner::BeginEncounter()
{
    if (!EnemyClass || !GetWorld())
    {
        return;
    }

    SpawnedCount = 0;

    const AIgKnightedGameMode* GameMode = GetWorld()->GetAuthGameMode<AIgKnightedGameMode>();
    const float CountScale = GameMode ? GameMode->GetActiveTuning().EnemyCountScale : 1.f;

    TargetCount = FMath::Max(1, FMath::RoundToInt(BaseEnemyCount * CountScale));

    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->RegisterSpawner(this);
        ZoneManager->BeginEncounter(ZoneIndex, TargetCount);
    }

    // Spawn the first enemy immediately so the player does not walk into an
    // empty arena and wait for the timer.
    SpawnOne();

    if (SpawnInterval > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimer, this, &AIgKnightedEnemySpawner::SpawnOne, SpawnInterval, true);
    }
}

void AIgKnightedEnemySpawner::SpawnOne()
{
    if (SpawnedCount >= TargetCount)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
        return;
    }

    if (!EnemyClass)
    {
        return;
    }

    const FTransform SpawnTransform = PickSpawnTransform();

    AIgKnightedEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AIgKnightedEnemyCharacter>(
        EnemyClass,
        SpawnTransform,
        this,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (!Enemy)
    {
        return;
    }

    // Register BEFORE FinishSpawning. From this point the enemy is alive as far
    // as the encounter is concerned, so it cannot be damaged and killed while
    // the ZoneManager still believes it is a pending spawn.
    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->NotifyEnemySpawned(Enemy);
    }

    UGameplayStatics::FinishSpawningActor(Enemy, SpawnTransform);
    ++SpawnedCount;
}

void AIgKnightedEnemySpawner::RestartEncounter()
{
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    BeginEncounter();
}

FTransform AIgKnightedEnemySpawner::PickSpawnTransform() const
{
    if (SpawnPoints.Num() > 0)
    {
        const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
        if (IsValid(SpawnPoints[Index]))
        {
            return SpawnPoints[Index]->GetActorTransform();
        }
    }

    return GetActorTransform();
}
