#include "IgKnightedHUDWidget.h"

#include "IgKnightedDashComponent.h"
#include "IgKnightedGameState.h"
#include "IgKnightedPlayerCharacter.h"
#include "IgKnightedPlayerState.h"

void UIgKnightedHUDWidget::BindToPlayerState(AIgKnightedPlayerState* InPlayerState)
{
    if (BoundPlayerState == InPlayerState)
    {
        return;
    }

    // Always unbind before binding. A widget recreated on respawn that only
    // adds the delegate is the classic way a counter is observed twice.
    if (BoundPlayerState)
    {
        BoundPlayerState->OnMatchScoreChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleMatchScoreChanged);
    }

    BoundPlayerState = InPlayerState;

    if (BoundPlayerState)
    {
        BoundPlayerState->OnMatchScoreChanged.AddDynamic(this, &UIgKnightedHUDWidget::HandleMatchScoreChanged);

        BP_OnMatchScoreChanged(BoundPlayerState->GetMatchScore());

        if (UIgKnightedDashComponent* Dash = GetBoundDashComponent())
        {
            Dash->OnDashChargesChanged.AddUniqueDynamic(this, &UIgKnightedHUDWidget::HandleDashChargesChanged);
        }
    }
}

void UIgKnightedHUDWidget::BindToGameState(AIgKnightedGameState* InGameState)
{
    if (BoundGameState == InGameState)
    {
        return;
    }

    if (BoundGameState)
    {
        BoundGameState->OnSharedLivesChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleSharedLivesChanged);
    }

    BoundGameState = InGameState;

    if (BoundGameState)
    {
        BoundGameState->OnSharedLivesChanged.AddDynamic(this, &UIgKnightedHUDWidget::HandleSharedLivesChanged);
        BP_OnSharedLivesChanged(BoundGameState->GetSharedLives(), 0);
    }
}

void UIgKnightedHUDWidget::NativeDestruct()
{
    UnbindAll();
    Super::NativeDestruct();
}

void UIgKnightedHUDWidget::UnbindAll()
{
    if (BoundPlayerState)
    {
        BoundPlayerState->OnMatchScoreChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleMatchScoreChanged);
    }

    if (BoundGameState)
    {
        BoundGameState->OnSharedLivesChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleSharedLivesChanged);
    }

    if (UIgKnightedDashComponent* Dash = GetBoundDashComponent())
    {
        Dash->OnDashChargesChanged.RemoveDynamic(this, &UIgKnightedHUDWidget::HandleDashChargesChanged);
    }
}

UIgKnightedDashComponent* UIgKnightedHUDWidget::GetBoundDashComponent() const
{
    if (!BoundPlayerState)
    {
        return nullptr;
    }

    APlayerController* Controller = BoundPlayerState->GetPlayerController();
    AIgKnightedPlayerCharacter* Character = Controller ? Cast<AIgKnightedPlayerCharacter>(Controller->GetPawn()) : nullptr;
    return Character ? Character->GetDashComponent() : nullptr;
}

FLinearColor UIgKnightedHUDWidget::GetBoundPlayerColour() const
{
    return BoundPlayerState ? BoundPlayerState->GetPlayerColour() : FLinearColor::White;
}

void UIgKnightedHUDWidget::HandleMatchScoreChanged(int32 NewScore)
{
    BP_OnMatchScoreChanged(NewScore);
}

void UIgKnightedHUDWidget::HandleSharedLivesChanged(int32 NewLives, int32 Delta)
{
    BP_OnSharedLivesChanged(NewLives, Delta);
}

void UIgKnightedHUDWidget::HandleDashChargesChanged(int32 CurrentCharges, int32 MaxCharges)
{
    BP_OnDashChargesChanged(CurrentCharges, MaxCharges);
}
