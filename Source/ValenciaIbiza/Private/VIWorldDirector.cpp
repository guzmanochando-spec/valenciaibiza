#include "VIWorldDirector.h"
#include "VIShipPawn.h"
#include "VIOceanActor.h"
#include "VILandmarkActor.h"
#include "VIIbizaHorizon.h"
#include "VIContentPolicy.h"
#include "ValenciaIbiza.h"

AVIWorldDirector::AVIWorldDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	Voyage = FVIVoyage::ValenciaToIbiza();
}

void AVIWorldDirector::BeginPlay()
{
	Super::BeginPlay();
}

void AVIWorldDirector::InitializeVoyage(AVIShipPawn* InShip)
{
	Ship = InShip;
	Voyage = FVIVoyage::ValenciaToIbiza();

	if (Voyage.Waypoints.Num() >= 2)
	{
		Geo.Origin = FVILatLon(
			(Voyage.Waypoints[0].Pos.Lat + Voyage.Waypoints.Last().Pos.Lat) * 0.5,
			(Voyage.Waypoints[0].Pos.Lon + Voyage.Waypoints.Last().Pos.Lon) * 0.5);
	}

	if (Ship)
	{
		VisualOriginEN = Geo.ToEastNorthM(Voyage.Waypoints[0].Pos);
		Ship->SetVoyage(Voyage, Geo);
		Ship->SetVisualOrigin(VisualOriginEN);
		Ship->ResetVoyage();
	}

	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Ocean = GetWorld()->SpawnActor<AVIOceanActor>(AVIOceanActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	IbizaHorizon = GetWorld()->SpawnActor<AVIIbizaHorizon>(AVIIbizaHorizon::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	if (IbizaHorizon)
	{
		FVIContentPolicy Policy;
		IbizaHorizon->Configure(Geo, Policy.IbizaMassif);
		if (Ship)
		{
			IbizaHorizon->UpdateFromObserver(Ship->GetLatLon(), VisualOriginEN);
		}
	}
	SpawnLandmarks();
	RefreshLandmarks();
	if (Ship && Ocean)
	{
		Ocean->Follow(Ship->GetActorLocation());
	}

	UE_LOG(LogValenciaIbiza, Display, TEXT("Voyage ready: %s → %s (%.1f nm)"),
		*Voyage.Departure, *Voyage.Arrival, FVIGeo::NauticalMiles(Voyage.TotalLengthM()));
}

void AVIWorldDirector::SpawnLandmarks()
{
	UWorld* World = GetWorld();
	if (!World || Voyage.Waypoints.Num() == 0)
	{
		return;
	}

	auto SpawnOne = [&](EVILandmarkKind Kind, const FString& Name, const FVILatLon& Pos)
	{
		FActorSpawnParameters P;
		P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AVILandmarkActor* A = World->SpawnActor<AVILandmarkActor>(AVILandmarkActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
		if (A)
		{
			A->Configure(Kind, Name, Pos, Geo);
			Landmarks.Add(A);
		}
	};

	SpawnOne(EVILandmarkKind::ValenciaPort, TEXT("Valencia"), Voyage.Waypoints[0].Pos);
	SpawnOne(EVILandmarkKind::IbizaPort, TEXT("Ibiza"), Voyage.Waypoints.Last().Pos);
	SpawnOne(EVILandmarkKind::DistantLand, TEXT("Formentera"), FVILatLon(38.697, 1.458));

	for (int32 i = 1; i < Voyage.Waypoints.Num() - 1; ++i)
	{
		SpawnOne(EVILandmarkKind::WaypointBuoy, Voyage.Waypoints[i].Name, Voyage.Waypoints[i].Pos);
	}
}

void AVIWorldDirector::RebaseIfNeeded()
{
	if (!Ship)
	{
		return;
	}
	const FVector2D ShipEN = Geo.ToEastNorthM(Ship->GetLatLon());
	const FVector2D Delta = ShipEN - VisualOriginEN;
	if (Delta.Size() < 1800.0)
	{
		return;
	}
	VisualOriginEN = ShipEN;
	Ship->SetVisualOrigin(VisualOriginEN);
	RefreshLandmarks();
}

void AVIWorldDirector::RefreshLandmarks()
{
	for (AVILandmarkActor* L : Landmarks)
	{
		if (L)
		{
			L->RefreshPose(VisualOriginEN);
		}
	}
}

void AVIWorldDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RebaseIfNeeded();
	if (Ship && Ocean)
	{
		Ocean->Follow(Ship->GetActorLocation());
	}
	if (Ship && IbizaHorizon)
	{
		IbizaHorizon->UpdateFromObserver(Ship->GetLatLon(), VisualOriginEN);
	}
}
