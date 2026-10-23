#include "IgKnightedEnemyCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "IgKnightedCombatComponent.h"
#include "IgKnightedDamageable.h"
#include "IgKnightedEnemyDefinition.h"
#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerState.h"
#include "IgKnightedProjectile.h"
#include "IgKnightedTargetingComponent.h"
#include "IgKnightedZoneManager.h"

AIgKnightedEnemyCharacter::AIgKnightedEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // A tag rather than a type check: the dash traversal only needs to know
    // "this is something the player's dash is allowed to kill".
    Tags.Add(TEXT("Ig.Enemy"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    Combat = CreateDefaultSubobject<UIgKnightedCombatComponent>(TEXT("Combat"));
    Targeting = CreateDefaultSubobject<UIgKnightedTargetingComponent>(TEXT("Targeting"));
}

FVector AIgKnightedEnemyCharacter::GetMuzzleLocation() const
{
    const FName SocketName = Definition ? Definition->MuzzleSocket : FName(TEXT("Muzzle"));

    return GetMesh()->DoesSocketExist(SocketName)
        ? GetMesh()->GetSocketLocation(SocketName)
        : GetActorLocation();
}

void AIgKnightedEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    ApplyDefinition();
}

void AIgKnightedEnemyCharacter::ApplyDefinition()
{
    if (!Definition)
    {
        return;
    }

    // The archetype is a row of numbers. Nothing here branches on Crawler,
    // Scorpion or Mollusk: behaviour comes from the tree and the range.
    GetCharacterMovement()->MaxWalkSpeed = Definition->MoveSpeed;
    ScoreValue = Definition->ScoreValue;

    if (Targeting)
    {
        Targeting->Configure(Definition->SightRadius, Definition->TargetPolicy, Definition->EvaluationInterval);
    }
}

void AIgKnightedEnemyCharacter::FireAtTarget(AActor* Target)
{
    if (!Definition || !Definition->ProjectileClass || !IsValid(Target))
    {
        return;
    }

    const FVector Origin = GetMuzzleLocation();
    const FVector AimPoint = Target->GetActorLocation();
    const FVector Direction = (AimPoint - Origin).GetSafeNormal();

    if (Direction.IsNearlyZero())
    {
        return;
    }

    // Spawn at the muzzle and aim at the target's location *now*. The enemy's
    // facing is cosmetic: a body mid-turn must not decide where the shot goes.
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (AIgKnightedProjectile* Shot = GetWorld()->SpawnActor<AIgKnightedProjectile>(
            Definition->ProjectileClass, Origin, Direction.Rotation(), Params))
    {
        Shot->InitializeProjectile(Direction, Definition->ProjectileSpeed, Definition->ProjectileDamage);
    }
}

void AIgKnightedEnemyCharacter::FireRadialBurst()
{
    if (!Definition || !Definition->ProjectileClass)
    {
        return;
    }

    const int32 ShotCount = FMath::Max(1, Definition->RadialShotCount);

    // A ring is not a spread. Every shot is spawned relative to the last one by
    // 360/Count, and the whole ring is offset so one projectile points at the
    // player. Rotating the ring by a random amount would make the pattern
    // unreadable, which is the opposite of a telegraphed attack.
    const AActor* Target = Targeting ? Targeting->GetCurrentTarget() : nullptr;
    const float BaseYaw = Target
        ? (Target->GetActorLocation() - GetActorLocation()).Rotation().Yaw
        : GetActorRotation().Yaw;
    const float Step = 360.f / static_cast<float>(ShotCount);

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 ShotIndex = 0; ShotIndex < ShotCount; ++ShotIndex)
    {
        const FRotator ShotRotation(0.f, BaseYaw + Step * ShotIndex, 0.f);
        const FVector Direction = ShotRotation.Vector();

        if (AIgKnightedProjectile* Shot = GetWorld()->SpawnActor<AIgKnightedProjectile>(
                Definition->ProjectileClass, GetMuzzleLocation(), ShotRotation, Params))
        {
            Shot->InitializeProjectile(Direction, Definition->ProjectileSpeed, Definition->ProjectileDamage);
        }
    }
}

void AIgKnightedEnemyCharacter::PerformMeleeAttack()
{
    if (!Definition || bDefeated)
    {
        return;
    }

    // Deliberately NOT a capsule overlap. A crawler standing on a player used to
    // deal damage every frame; this is only called from the attack animation's
    // notify, so one swing is one hit.
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(IgMeleeSwing), false, this);
    const FCollisionShape Shape = FCollisionShape::MakeSphere(Definition->MeleeRadius);
    const FVector Centre = GetActorLocation() + GetActorForwardVector() * 40.f;

    GetWorld()->OverlapMultiByChannel(
        Overlaps, Centre, FQuat::Identity, ECC_Pawn, Shape, Params);

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* OtherActor = Overlap.GetActor();
        if (!IsValid(OtherActor) || !OtherActor->ActorHasTag(TEXT("Ig.Player")))
        {
            continue;
        }

        if (IIgKnightedDamageable* Damageable = Cast<IIgKnightedDamageable>(OtherActor))
        {
            FIgDamageContext Context;
            Context.Instigator = this;
            Context.HitLocation = OtherActor->GetActorLocation();
            Context.Kind = EIgDamageKind::EnemyContact;

            Damageable->ApplyGameplayDamage(Context);
        }
    }
}

FVector AIgKnightedEnemyCharacter::GetAimDirection() const
{
    const AActor* Target = Targeting ? Targeting->GetCurrentTarget() : nullptr;
    if (Target)
    {
        return (Target->GetActorLocation() - GetMuzzleLocation()).GetSafeNormal();
    }

    return GetActorForwardVector();
}

FIgDamageResult AIgKnightedEnemyCharacter::ApplyGameplayDamage_Implementation(const FIgDamageContext& Context)
{
    FIgDamageResult Result;

    if (bDefeated)
    {
        return Result;
    }

    HandleDefeated(Context);

    Result.Outcome = EIgDamageOutcome::Lethal;
    Result.bKilledTarget = true;
    return Result;
}

void AIgKnightedEnemyCharacter::HandleDefeated(const FIgDamageContext& Context)
{
    // One enemy, one death event, regardless of which source killed it:
    // a projectile, an enhanced dash, or a future hazard.
    bDefeated = true;

    if (Combat)
    {
        Combat->StopFiring();
    }

    if (Targeting)
    {
        Targeting->SetComponentTickEnabled(false);
    }

    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (AIgKnightedZoneManager* ZoneManager = AIgKnightedZoneManager::Get(this))
    {
        ZoneManager->NotifyEnemyDefeated(this);
    }

    if (AIgKnightedPlayerCharacter* Killer = Cast<AIgKnightedPlayerCharacter>(Context.Instigator.Get()))
    {
        if (AIgKnightedPlayerState* PlayerState = Killer->GetPlayerState<AIgKnightedPlayerState>())
        {
            PlayerState->AddMatchScore(ScoreValue);
            PlayerState->AddMultiplier(ScoreMultiplierPerKill);
        }
    }

    BP_OnDefeated();

    // The death notification counts immediately; the corpse disappears only
    // after the animation has finished.
    SetLifeSpan(DeathPresentationSeconds);
}
