#include "IgKnightedPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerState.h"

#include "Engine/LocalPlayer.h"

AIgKnightedPlayerController::AIgKnightedPlayerController()
{
    bShowMouseCursor = false;
}

void AIgKnightedPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
    if (!Input)
    {
        UE_LOG(LogTemp, Error,
            TEXT("IgKnighted: InputComponent is not an EnhancedInputComponent. "
                 "Check Project Settings > Input > Default Input Component Class."));
        return;
    }

    // Bindings are installed once per controller. Mapping contexts are not.
    Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnMoveTriggered);
    Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnAimTriggered);
    Input->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &AIgKnightedPlayerController::OnMouseAimTriggered);
    Input->BindAction(FireAction, ETriggerEvent::Started, this, &AIgKnightedPlayerController::OnFireStarted);
    Input->BindAction(FireAction, ETriggerEvent::Completed, this, &AIgKnightedPlayerController::OnFireCompleted);
    Input->BindAction(DashAction, ETriggerEvent::Started, this, &AIgKnightedPlayerController::OnDashStarted);
}

void AIgKnightedPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // OnPossess runs again on respawn and on level travel; SetupInputComponent
    // does not. Installing the contexts here is what makes input survive both.
    ApplyGameplayMappings();
}

void AIgKnightedPlayerController::ApplyGameplayMappings()
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer)
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    if (!Subsystem)
    {
        return;
    }

    // The subsystem is per LocalPlayer. This is the line that separates
    // "works with one player" from "works with two": each controller installs
    // its own context stack on its own subsystem.
    Subsystem->ClearAllMappings();

    if (GameplayKeyboardContext)
    {
        Subsystem->AddMappingContext(GameplayKeyboardContext, 0);
    }

    if (GameplayGamepadContext)
    {
        // Same priority as the keyboard context: Enhanced Input routes by the
        // device that produced the input, so a player can switch mid-session.
        // Priorities are reserved for modal UI (menus install at 100).
        Subsystem->AddMappingContext(GameplayGamepadContext, 0);
    }
}

void AIgKnightedPlayerController::ClearMappings()
{
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            Subsystem->ClearAllMappings();
        }
    }
}

void AIgKnightedPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearMappings();
    Super::EndPlay(EndPlayReason);
}

AIgKnightedPlayerCharacter* AIgKnightedPlayerController::GetIgPawn() const
{
    return Cast<AIgKnightedPlayerCharacter>(GetPawn());
}

void AIgKnightedPlayerController::OnMoveTriggered(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->Move(Value.Get<FVector2D>());
    }
}

void AIgKnightedPlayerController::OnAimTriggered(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->AimFromStick(Value.Get<FVector2D>());
    }
}

void AIgKnightedPlayerController::OnMouseAimTriggered(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->AimAtCursor();
    }
}

void AIgKnightedPlayerController::OnFireStarted(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->StartFire();
    }
}

void AIgKnightedPlayerController::OnFireCompleted(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->StopFire();
    }
}

void AIgKnightedPlayerController::OnDashStarted(const FInputActionValue& Value)
{
    if (AIgKnightedPlayerCharacter* IgPawn = GetIgPawn())
    {
        IgPawn->TryDash();
    }
}
