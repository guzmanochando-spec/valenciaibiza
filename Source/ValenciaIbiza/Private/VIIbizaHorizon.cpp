#include "VIIbizaHorizon.h"
#include "VIContentPolicy.h"
#include "VIShapeKit.h"

AVIIbizaHorizon::AVIIbizaHorizon()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	SetActorHiddenInGame(true);
}

void AVIIbizaHorizon::BeginPlay()
{
	Super::BeginPlay();
}

void AVIIbizaHorizon::Configure(const FVIGeo& InGeo, const FVILatLon& InMassif)
{
	Geo = InGeo;
	Massif = InMassif;
	if (!bBuilt)
	{
		BuildMassif();
		bBuilt = true;
	}
}

void AVIIbizaHorizon::BuildMassif()
{
	UStaticMesh* Sphere = FVIShapeKit::Sphere();
	if (!Sphere)
	{
		return;
	}

	const FLinearColor Ridge(0.28f, 0.30f, 0.24f, 1.0f);
	const FLinearColor Peak(0.34f, 0.32f, 0.26f, 1.0f);
	const FLinearColor Shore(0.42f, 0.38f, 0.30f, 1.0f);

	auto Hill = [&](const FVector& Loc, const FVector& Scale, const FLinearColor& Color, const TCHAR* Name)
	{
		if (UStaticMeshComponent* C = FVIShapeKit::Attach(this, Root, Sphere, Loc, FRotator::ZeroRotator, Scale, Color, Name))
		{
			Meshes.Add(C);
		}
	};

	Hill(FVector(0, 0, 900), FVector(180, 70, 22), Ridge, TEXT("SaTalaia"));
	Hill(FVector(-4200, 2800, 620), FVector(110, 55, 14), Peak, TEXT("WestRidge"));
	Hill(FVector(3800, -1800, 480), FVector(90, 48, 11), Shore, TEXT("SouthCape"));
	Hill(FVector(1800, 4200, 380), FVector(70, 40, 8), Shore, TEXT("NorthHint"));
}

void AVIIbizaHorizon::ApplyOpacity(float Opacity)
{
	const bool bShow = Opacity > 0.02f;
	SetActorHiddenInGame(!bShow);
	const float Scale = FMath::Lerp(0.55f, 1.0f, Opacity);
	SetActorScale3D(FVector(Scale, Scale, FMath::Lerp(0.35f, 1.0f, Opacity)));
	for (UStaticMeshComponent* C : Meshes)
	{
		if (C)
		{
			C->SetVisibility(bShow);
		}
	}
}

void AVIIbizaHorizon::UpdateFromObserver(const FVILatLon& Observer, const FVector2D& VisualOriginEN)
{
	const double Dist = FVIGeo::HaversineM(Observer, Massif);
	const float Opacity = FVIIbizaVisibility::OpacityForDistanceM(Dist);
	SetActorLocation(Geo.ToUnreal(Massif, VisualOriginEN, 0.0));
	ApplyOpacity(Opacity);
}
