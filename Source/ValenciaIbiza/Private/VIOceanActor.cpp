#include "VIOceanActor.h"
#include "VIShapeKit.h"
#include "VIFalMedia.h"

AVIOceanActor::AVIOceanActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AVIOceanActor::BeginPlay()
{
	Super::BeginPlay();
	BuildSea();
}

void AVIOceanActor::BuildSea()
{
	UStaticMesh* Plane = FVIShapeKit::Plane();
	if (!Plane)
	{
		return;
	}

	const FLinearColor Deep(0.02f, 0.12f, 0.18f, 1.0f);
	const FLinearColor Band(0.04f, 0.22f, 0.28f, 1.0f);

	// Engine plane is 100cm. Scale 800 → 80km tile, 3x3 grid around the ship.
	const float Tile = 800.0f;
	int32 i = 0;
	for (int32 x = -1; x <= 1; ++x)
	{
		for (int32 y = -1; y <= 1; ++y)
		{
			const FName Name(*FString::Printf(TEXT("Sea_%d_%d"), x, y));
			UStaticMeshComponent* Comp = FVIShapeKit::Attach(
				this, Root, Plane,
				FVector(x * Tile * 100.0f, y * Tile * 100.0f, 0.0f),
				FRotator::ZeroRotator,
				FVector(Tile, Tile, 1.0f),
				(x == 0 && y == 0) ? Band : Deep,
				Name);
			if (Comp)
			{
				FVIFalMedia::ApplyIfPresent(Comp, TEXT("ocean"));
			}
			++i;
		}
	}
}

void AVIOceanActor::Follow(const FVector& ShipLocation)
{
	SetActorLocation(FVector(ShipLocation.X, ShipLocation.Y, 0.0f));
}
