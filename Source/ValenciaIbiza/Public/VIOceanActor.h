#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIOceanActor.generated.h"

UCLASS()
class VALENCIAIBIZA_API AVIOceanActor : public AActor
{
	GENERATED_BODY()

public:
	AVIOceanActor();
	void Follow(const FVector& ShipLocation);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

protected:
	virtual void BeginPlay() override;

private:
	void BuildSea();
};
