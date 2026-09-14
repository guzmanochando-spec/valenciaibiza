#include "VILandmarkActor.h"
#include "VIShapeKit.h"
#include "VIFalMedia.h"

AVILandmarkActor::AVILandmarkActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AVILandmarkActor::BeginPlay()
{
	Super::BeginPlay();
}

void AVILandmarkActor::Configure(EVILandmarkKind InKind, const FString& InLabel, const FVILatLon& InPos, const FVIGeo& InGeo)
{
	Kind = InKind;
	Label = InLabel;
	LatLon = InPos;
	Geo = InGeo;
	if (bBuilt)
	{
		return;
	}
	switch (Kind)
	{
	case EVILandmarkKind::ValenciaPort: BuildValencia(); break;
	case EVILandmarkKind::IbizaPort: BuildIbiza(); break;
	case EVILandmarkKind::DistantLand: BuildDistant(); break;
	default: BuildBuoy(); break;
	}
	bBuilt = true;
}

void AVILandmarkActor::RefreshPose(const FVector2D& VisualOriginEN)
{
	SetActorLocation(Geo.ToUnreal(LatLon, VisualOriginEN, 0.0));
}

void AVILandmarkActor::BuildValencia()
{
	UStaticMesh* Cube = FVIShapeKit::Cube();
	UStaticMesh* Cyl = FVIShapeKit::Cylinder();
	if (!Cube)
	{
		return;
	}
	const FLinearColor Concrete(0.42f, 0.40f, 0.36f, 1);
	const FLinearColor City(0.55f, 0.48f, 0.40f, 1);
	const FLinearColor Crane(0.85f, 0.45f, 0.12f, 1);
	const FLinearColor Light(0.92f, 0.88f, 0.70f, 1);

	FVIShapeKit::Attach(this, Root, Cube, FVector(-800, 0, 80), FRotator::ZeroRotator, FVector(80, 8, 1.6f), Concrete, TEXT("Quay"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(-4000, 1800, 40), FRotator(0, 18, 0), FVector(90, 6, 0.8f), Concrete, TEXT("Breakwater"));
	for (int32 i = 0; i < 7; ++i)
	{
		const FName N(*FString::Printf(TEXT("Block_%d"), i));
		FVIShapeKit::Attach(this, Root, Cube,
			FVector(-2200.0f - i * 380.0f, 900.0f + (i % 3) * 220.0f, 220.0f + (i % 4) * 80.0f),
			FRotator::ZeroRotator,
			FVector(3.4f, 2.8f, 4.0f + (i % 3)),
			City, N);
	}
	if (Cyl)
	{
		FVIShapeKit::Attach(this, Root, Cyl, FVector(600, 400, 420), FRotator::ZeroRotator, FVector(0.5f, 0.5f, 8.0f), Crane, TEXT("CraneMast"));
		FVIShapeKit::Attach(this, Root, Cube, FVector(900, 400, 820), FRotator::ZeroRotator, FVector(8.0f, 0.35f, 0.35f), Crane, TEXT("CraneJib"));
		FVIShapeKit::Attach(this, Root, Cyl, FVector(2200, -200, 380), FRotator::ZeroRotator, FVector(0.7f, 0.7f, 7.2f), Light, TEXT("Lighthouse"));
	}
}

void AVILandmarkActor::BuildIbiza()
{
	UStaticMesh* Cube = FVIShapeKit::Cube();
	UStaticMesh* Sphere = FVIShapeKit::Sphere();
	if (!Cube)
	{
		return;
	}
	const FLinearColor Rock(0.38f, 0.32f, 0.24f, 1);
	const FLinearColor Hill(0.22f, 0.34f, 0.20f, 1);
	const FLinearColor White(0.93f, 0.91f, 0.86f, 1);
	const FLinearColor Wall(0.62f, 0.52f, 0.38f, 1);

	if (Sphere)
	{
		FVIShapeKit::Attach(this, Root, Sphere, FVector(-1800, 2200, 400), FRotator::ZeroRotator, FVector(28, 22, 9), Hill, TEXT("HillW"));
		FVIShapeKit::Attach(this, Root, Sphere, FVector(-3200, 3800, 700), FRotator::ZeroRotator, FVector(36, 26, 14), Hill, TEXT("HillN"));
	}
	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 70), FRotator::ZeroRotator, FVector(36, 7, 1.4f), Wall, TEXT("HarborWall"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(-900, 1400, 520), FRotator(0, -20, 0), FVector(8, 6, 6), Wall, TEXT("DaltVila"));
	for (int32 i = 0; i < 8; ++i)
	{
		FVIShapeKit::Attach(this, Root, Cube,
			FVector(-400.0f - i * 160.0f, 700.0f + (i % 2) * 180.0f, 160.0f),
			FRotator::ZeroRotator,
			FVector(1.4f, 1.2f, 1.6f + (i % 3) * 0.4f),
			White, *FString::Printf(TEXT("House_%d"), i));
	}

	if (UStaticMeshComponent* Found = FindComponentByClass<UStaticMeshComponent>())
	{
		FVIFalMedia::ApplyIfPresent(Found, TEXT("ibiza"));
	}
}

void AVILandmarkActor::BuildBuoy()
{
	UStaticMesh* Cyl = FVIShapeKit::Cylinder();
	UStaticMesh* Cone = FVIShapeKit::Cone();
	if (!Cyl)
	{
		return;
	}
	FVIShapeKit::Attach(this, Root, Cyl, FVector(0, 0, 90), FRotator::ZeroRotator, FVector(0.9f, 0.9f, 1.6f), FLinearColor(0.85f, 0.15f, 0.12f), TEXT("BuoyBody"));
	if (Cone)
	{
		FVIShapeKit::Attach(this, Root, Cone, FVector(0, 0, 200), FRotator::ZeroRotator, FVector(0.7f, 0.7f, 1.1f), FLinearColor(0.95f, 0.85f, 0.15f), TEXT("BuoyTop"));
	}
}

void AVILandmarkActor::BuildDistant()
{
	UStaticMesh* Sphere = FVIShapeKit::Sphere();
	if (!Sphere)
	{
		return;
	}
	FVIShapeKit::Attach(this, Root, Sphere, FVector(0, 0, 180), FRotator::ZeroRotator, FVector(80, 40, 8), FLinearColor(0.28f, 0.30f, 0.26f), TEXT("Silhouette"));
}
