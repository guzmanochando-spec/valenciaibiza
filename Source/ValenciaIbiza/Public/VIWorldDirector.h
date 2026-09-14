#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIGeo.h"
#include "VIVoyage.h"
#include "VIWorldDirector.generated.h"

class AVIShipPawn;
class AVIOceanActor;
class AVILandmarkActor;
class AVIIbizaHorizon;

UCLASS()
class VALENCIAIBIZA_API AVIWorldDirector : public AActor
{
	GENERATED_BODY()

public:
	AVIWorldDirector();

	void InitializeVoyage(AVIShipPawn* InShip);
	virtual void Tick(float DeltaSeconds) override;

	const FVIVoyage& GetVoyage() const { return Voyage; }
	const FVIGeo& GetGeo() const { return Geo; }
	FVector2D GetVisualOriginEN() const { return VisualOriginEN; }

protected:
	virtual void BeginPlay() override;

private:
	void SpawnLandmarks();
	void RebaseIfNeeded();
	void RefreshLandmarks();

	FVIVoyage Voyage;
	FVIGeo Geo;
	FVector2D VisualOriginEN = FVector2D::ZeroVector;

	UPROPERTY()
	TObjectPtr<AVIShipPawn> Ship;

	UPROPERTY()
	TObjectPtr<AVIOceanActor> Ocean;

	UPROPERTY()
	TArray<TObjectPtr<AVILandmarkActor>> Landmarks;

	UPROPERTY()
	TObjectPtr<AVIIbizaHorizon> IbizaHorizon;
};
