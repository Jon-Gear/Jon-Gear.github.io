#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IgKnightedTypes.generated.h"

// ============= Session & Match =============

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
    FrontEnd    UMETA(DisplayName = "Front End"),
    Starting    UMETA(DisplayName = "Starting"),
    InProgress  UMETA(DisplayName = "In Progress"),
    Ending      UMETA(DisplayName = "Ending"),
    Finished    UMETA(DisplayName = "Finished")
};

UENUM(BlueprintType)
enum class EMatchResult : uint8
{
    None     UMETA(DisplayName = "None"),
    Victory  UMETA(DisplayName = "Victory"),
    Defeat   UMETA(DisplayName = "Defeat")
};

UENUM(BlueprintType)
enum class EIgDifficulty : uint8
{
    Easy    UMETA(DisplayName = "Easy"),
    Medium  UMETA(DisplayName = "Medium"),
    Hard    UMETA(DisplayName = "Hard")
};

USTRUCT(BlueprintType)
struct FIgRunSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Run")
    int32 PlayerCount = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Run")
    EIgDifficulty Difficulty = EIgDifficulty::Medium;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Run")
    bool bTutorialEnabled = true;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Run")
    bool bDynamicDifficulty = false;
};

USTRUCT(BlueprintType)
struct FIgDifficultyTuning
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    float EnemyCountScale = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    float EnemySpeedScale = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    float AttackIntervalScale = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    float HazardUptimeScale = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Difficulty")
    float CollectableIntervalScale = 1.f;
};

// ============= Damage =============

UENUM(BlueprintType)
enum class EIgDamageKind : uint8
{
    Water           UMETA(DisplayName = "Water"),
    EnemyContact    UMETA(DisplayName = "Enemy Contact"),
    Projectile      UMETA(DisplayName = "Projectile"),
    DashTraversal   UMETA(DisplayName = "Dash Traversal"),
    Environmental   UMETA(DisplayName = "Environmental")
};

UENUM(BlueprintType)
enum class EIgDamageOutcome : uint8
{
    Ignored         UMETA(DisplayName = "Ignored By Dash"),
    Absorbed        UMETA(DisplayName = "Absorbed By Shield"),
    Lethal          UMETA(DisplayName = "Lethal"),
    NoTarget        UMETA(DisplayName = "No Target")
};

USTRUCT(BlueprintType)
struct FIgDamageContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    TWeakObjectPtr<AActor> Instigator;

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    FVector HitNormal = FVector::UpVector;

    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    EIgDamageKind Kind = EIgDamageKind::Water;

    // Source instance id: two hits from the same hazard cycle share an id,
    // so a single overlap latch can suppress the second one.
    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    int32 SourceId = INDEX_NONE;

    bool IsValid() const
    {
        return !HitLocation.ContainsNaN();
    }
};

USTRUCT(BlueprintType)
struct FIgDamageResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    EIgDamageOutcome Outcome = EIgDamageOutcome::NoTarget;

    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    bool bKilledTarget = false;
};

// ============= Enemies =============

UENUM(BlueprintType)
enum class EIgEnemyArchetype : uint8
{
    Crawler     UMETA(DisplayName = "Crawler"),
    Scorpion    UMETA(DisplayName = "Scorpion"),
    Mollusk     UMETA(DisplayName = "Mollusk")
};

// ============= Buffs & Targeting =============

UENUM(BlueprintType)
enum class EIgBuffKind : uint8
{
    Shield          UMETA(DisplayName = "Shield"),
    PowerShot       UMETA(DisplayName = "Power Shot"),
    EnhancedDash    UMETA(DisplayName = "Enhanced Dash"),
    Matchstick      UMETA(DisplayName = "Matchstick")
};

UENUM(BlueprintType)
enum class EIgTargetPolicy : uint8
{
    Nearest             UMETA(DisplayName = "Nearest"),
    ClosestToCentre     UMETA(DisplayName = "Closest To Arena Centre"),
    LowestScore         UMETA(DisplayName = "Lowest Score"),
    HighestThreat       UMETA(DisplayName = "Highest Threat")
};

// ============= Dynamic Difficulty =============

USTRUCT(BlueprintType)
struct FIgDirectorInput
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 LivesLostThisZone = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 EnemiesDefeatedThisZone = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Director")
    float TimeInZoneSeconds = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Director")
    float PlayerHealthFraction = 1.f;
};

// ============= Delegates =============

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFired, FVector, MuzzleLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, int32, ShieldCharges);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffExpired, EIgBuffKind, BuffKind);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSharedLivesChanged, int32, NewLives, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchPhaseChanged, EMatchPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFinished, EMatchResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageIgnored, AActor*, Victim, const FIgDamageContext&, Context);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldBroken, AActor*, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLethalDamage, AActor*, Victim, const FIgDamageContext&, Context);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDashStateChanged, bool, bDashing, bool, bInvulnerable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDashChargesChanged, int32, CurrentCharges, int32, MaxCharges);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplierChanged, float, NewMultiplier);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneStarted, int32, ZoneIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoneCleared, int32, ZoneIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBarrierStateChanged, int32, BarrierIndex, bool, bOpen);
