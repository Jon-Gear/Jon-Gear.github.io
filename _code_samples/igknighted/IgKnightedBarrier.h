#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IgKnightedBarrier.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class IGKNIGHTED_API AIgKnightedBarrier : public AActor
{
    GENERATED_BODY()

public:
    AIgKnightedBarrier();

    UFUNCTION(BlueprintCallable, Category = "Barrier")
    void SetOpen(bool bOpen);

    UFUNCTION(BlueprintPure, Category = "Barrier")
    bool IsOpen() const { return bOpen; }

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Barrier")
    void BP_OnOpenChanged(bool bOpen);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrier")
    TObjectPtr<UBoxComponent> BlockingVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Barrier")
    TObjectPtr<UStaticMeshComponent> Mesh;

private:
    bool bOpen = false;
};
