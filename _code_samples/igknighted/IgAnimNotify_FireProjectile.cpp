#include "IgAnimNotify_FireProjectile.h"

#include "IgKnightedCombatComponent.h"

void UAnimNotify_IgFireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp)
    {
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        return;
    }

    if (UIgKnightedCombatComponent* Combat = Owner->FindComponentByClass<UIgKnightedCombatComponent>())
    {
        Combat->SpawnProjectileFromMuzzle();
    }
}
