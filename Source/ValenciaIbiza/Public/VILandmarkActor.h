#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIGeo.h"
#include "VILandmarkActor.generated.h"

UENUM()
enum class EVILandmarkKind : uint8
{
	ValenciaPort,
	IbizaPort,
	WaypointBuoy,
	DistantLand
};

UCLASS()
class VALENCIAIBIZA_API AVILandmarkActor : public AActor
{
	GENERATED_BODY()

public:
	AVILandmarkActor();

	void Configure(EVILandmarkKind InKind, const FString& InLabel, const FVILatLon& InPos, const FVIGeo& InGeo);
	void RefreshPose(const FVector2D& VisualOriginEN);
	FVILatLon GetLatLon() const { return LatLon; }

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

protected:
	virtual void BeginPlay() override;

private:
	void BuildValencia();
	void BuildIbiza();
	void BuildBuoy();
	void BuildDistant();

	EVILandmarkKind Kind = EVILandmarkKind::WaypointBuoy;
	FString Label;
	FVILatLon LatLon;
	FVIGeo Geo;
	bool bBuilt = false;
};
