#include "IgKnightedPlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "IgKnightedCombatComponent.h"
#include "IgKnightedDamageResolver.h"
#include "IgKnightedDashComponent.h"
#include "IgKnightedStatusComponent.h"
#include "IgKnightedWeaponData.h"

AIgKnightedPlayerCharacter::AIgKnightedPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);

    Tags.Add(TEXT("Ig.Player"));
    // The body turns with movement; the weapon pivot turns with aim.
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

    WeaponPivot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPivot"));
    WeaponPivot->SetupAttachment(GetMesh());

    Combat = CreateDefaultSubobject<UIgKnightedCombatComponent>(TEXT("Combat"));
    Dash = CreateDefaultSubobject<UIgKnightedDashComponent>(TEXT("Dash"));
    Status = CreateDefaultSubobject<UIgKnightedStatusComponent>(TEXT("Status"));
}

void AIgKnightedPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    AimDirection = GetActorForwardVector().GetSafeNormal2D();
    WeaponPivot->SetWorldRotation(FRotator(0.f, AimDirection.Rotation().Yaw, 0.f));
}

void AIgKnightedPlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsDead || AimDirection.IsNearlyZero())
    {
        return;
    }

    const FRotator Desired(0.f, AimDirection.Rotation().Yaw, 0.f);
    const FRotator Smoothed = FMath::RInterpTo(WeaponPivot->GetComponentRotation(), Desired, DeltaSeconds, AimInterpSpeed);
    WeaponPivot->SetWorldRotation(FRotator(0.f, Smoothed.Yaw, 0.f));
}

void AIgKnightedPlayerCharacter::Move(const FVector2D& Input)
{
    if (bIsDead || Input.IsNearlyZero())
    {
        return;
    }

    // The level camera is fixed and top-down. Control rotation is its yaw, so
    // movement stays camera-relative even though the Pawn owns no camera.
    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // Normalising the 2D vector is what stops diagonal movement from being
    // 1.41x faster than cardinal movement.
    const FVector2D Normalised = Input.GetSafeNormal();

    AddMovementInput(Forward, Normalised.Y);
    AddMovementInput(Right, Normalised.X);
}

void AIgKnightedPlayerCharacter::AimFromStick(const FVector2D& Input)
{
    if (bIsDead || Input.SizeSquared() < FMath::Square(0.15f))
    {
        return;
    }

    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AimDirection = (Forward * Input.Y + Right * Input.X).GetSafeNormal2D();
}

void AIgKnightedPlayerCharacter::AimAtCursor()
{
    if (bIsDead)
    {
        return;
    }

    APlayerController* Controller = GetController<APlayerController>();
    if (!Controller)
    {
        return;
    }

    FHitResult Hit;
    if (Controller->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        AimDirection = (Hit.ImpactPoint - WeaponPivot->GetComponentLocation()).GetSafeNormal2D();
    }
}

void AIgKnightedPlayerCharacter::StartFire()
{
    if (!bIsDead && Combat)
    {
        Combat->StartFiring();
    }
}

void AIgKnightedPlayerCharacter::StopFire()
{
    if (Combat)
    {
        Combat->StopFiring();
    }
}

void AIgKnightedPlayerCharacter::TryDash()
{
    if (!bIsDead && Dash)
    {
        Dash->TryStartDash();
    }
}

FVector AIgKnightedPlayerCharacter::GetMuzzleLocation() const
{
    const UIgKnightedWeaponData* Weapon = Combat ? Combat->GetWeaponData() : nullptr;
    const FName SocketName = Weapon ? Weapon->MuzzleSocketName : FName(TEXT("Muzzle"));

    return GetMesh()->DoesSocketExist(SocketName)
        ? GetMesh()->GetSocketLocation(SocketName)
        : WeaponPivot->GetComponentLocation();
}

FIgDamageResult AIgKnightedPlayerCharacter::ApplyGameplayDamage_Implementation(const FIgDamageContext& Context)
{
    FIgDamageResult Result;

    if (bIsDead)
    {
        return Result;
    }

    UIgKnightedDamageResolver* Resolver = GetWorld()
        ? GetWorld()->GetSubsystem<UIgKnightedDamageResolver>()
        : nullptr;

    if (!Resolver)
    {
        return Result;
    }

    return Resolver->ResolvePlayerDamage(this, Context);
}

void AIgKnightedPlayerCharacter::StartDeathPresentation()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    Combat->StopFiring();
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // The animation notify calls FinishDeath() at the frame where the body hits
    // the floor, so the corpse does not vanish mid-animation.
    BP_OnDeathStarted();
}

void AIgKnightedPlayerCharacter::FinishDeath()
{
    Destroy();
}

bool AIgKnightedPlayerCharacter::IsFiring() const
{
    return Combat && Combat->IsFiring();
}

bool AIgKnightedPlayerCharacter::IsDashing() const
{
    return Dash && Dash->IsDashing();
}

float AIgKnightedPlayerCharacter::GetAimYaw() const
{
    return WeaponPivot->GetComponentRotation().Yaw;
}
