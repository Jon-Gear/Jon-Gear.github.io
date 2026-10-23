#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "IgKnightedPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class AIgKnightedPlayerCharacter;

/**
 * One controller class, two (or more) instances. Player identity is an index
 * on the instance, not a subclass.
 */
UCLASS()
class IGKNIGHTED_API AIgKnightedPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIgKnightedPlayerController();

    /** 0 for Player 1, 1 for Player 2. Assigned by the GameMode after creation. */
    UPROPERTY(BlueprintReadOnly, Category = "Player")
    int32 LocalPlayerIndex = 0;

    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Idempotent: safe to call on BeginPlay, on possess, and after a level travel. */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void ApplyGameplayMappings();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void ClearMappings();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> GameplayKeyboardContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> GameplayGamepadContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> AimAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MouseAimAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> DashAction;

    UFUNCTION()
    void OnMoveTriggered(const FInputActionValue& Value);

    UFUNCTION()
    void OnAimTriggered(const FInputActionValue& Value);

    UFUNCTION()
    void OnMouseAimTriggered(const FInputActionValue& Value);

    UFUNCTION()
    void OnFireStarted(const FInputActionValue& Value);

    UFUNCTION()
    void OnFireCompleted(const FInputActionValue& Value);

    UFUNCTION()
    void OnDashStarted(const FInputActionValue& Value);

    AIgKnightedPlayerCharacter* GetIgPawn() const;
};
