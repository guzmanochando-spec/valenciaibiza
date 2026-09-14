#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIGeo.h"
#include "VIIbizaHorizon.generated.h"

/** Distant Ibiza: fades in with haze, then grows relief. Not a pop-in prop. */
UCLASS()
class VALENCIAIBIZA_API AVIIbizaHorizon : public AActor
{
	GENERATED_BODY()

public:
	AVIIbizaHorizon();

	void Configure(const FVIGeo& InGeo, const FVILatLon& Massif);
	void UpdateFromObserver(const FVILatLon& Observer, const FVector2D& VisualOriginEN);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

protected:
	virtual void BeginPlay() override;

private:
	void BuildMassif();
	void ApplyOpacity(float Opacity);

	FVIGeo Geo;
	FVILatLon Massif;
	bool bBuilt = false;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Meshes;
};
