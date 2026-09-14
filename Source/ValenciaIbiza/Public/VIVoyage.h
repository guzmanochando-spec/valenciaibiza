#pragma once

#include "CoreMinimal.h"
#include "VIGeo.h"

struct FVIWaypoint
{
	FString Id;
	FString Name;
	FVILatLon Pos;
	double SpeedHintKn = 14.0;
	bool bHarbor = false;
};

struct FVIVoyage
{
	FString VesselName = TEXT("LEVANTE");
	FString Departure = TEXT("Valencia · Puerto");
	FString Arrival = TEXT("Ibiza · Eivissa");
	TArray<FVIWaypoint> Waypoints;

	static FVIVoyage ValenciaToIbiza();

	int32 NextIndex(const FVILatLon& Here, int32 From = 0, double ArriveM = 650.0) const;
	FVIWaypoint CurrentTarget(int32 Index) const;
	double RemainingM(const FVILatLon& Here, int32 NextIdx) const;
	double TotalLengthM() const;
	double Progress01(const FVILatLon& Here, int32 NextIdx) const;
};
