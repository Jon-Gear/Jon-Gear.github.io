#include "IgKnightedGameInstance.h"

void UIgKnightedGameInstance::SetPendingRunSettings(const FIgRunSettings& NewSettings)
{
    // Validation belongs at the boundary. Every consumer downstream can then
    // assume PlayerCount is 1 or 2 and the difficulty is a real value.
    PendingRunSettings = NewSettings;
    PendingRunSettings.PlayerCount = FMath::Clamp(PendingRunSettings.PlayerCount, 1, 2);

    UE_LOG(LogTemp, Log, TEXT("Run settings: %d player(s), difficulty %d, tutorial %s, dynamic %s"),
        PendingRunSettings.PlayerCount,
        static_cast<int32>(PendingRunSettings.Difficulty),
        PendingRunSettings.bTutorialEnabled ? TEXT("on") : TEXT("off"),
        PendingRunSettings.bDynamicDifficulty ? TEXT("on") : TEXT("off"));
}

void UIgKnightedGameInstance::ClearPendingRunSettings()
{
    PendingRunSettings = FIgRunSettings();
}
