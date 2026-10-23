#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "IgAnimNotify_FireProjectile.generated.h"

/**
 * The frame the arm reaches full extension is the frame the projectile exists.
 * The montage owns the timing; the gameplay code owns the effect.
 */
UCLASS(meta = (DisplayName = "Ig Fire Projectile"))
class IGKNIGHTED_API UAnimNotify_IgFireProjectile : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;

    virtual FString GetNotifyName_Implementation() const override { return TEXT("Ig Fire Projectile"); }
};
