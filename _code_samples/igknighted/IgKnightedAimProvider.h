#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IgKnightedAimProvider.generated.h"

UINTERFACE(MinimalAPI)
class UIgKnightedAimProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * How a combat component asks "where do I shoot from, and in which direction?"
 * without knowing whether its owner is a player, a Scorpion or a turret.
 */
class IGKNIGHTED_API IIgKnightedAimProvider
{
    GENERATED_BODY()

public:
    virtual FVector GetMuzzleLocation() const = 0;
    virtual FVector GetAimDirection() const = 0;
};
