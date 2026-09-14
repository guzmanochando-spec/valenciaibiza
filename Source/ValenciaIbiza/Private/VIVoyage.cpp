#include "VIVoyage.h"

FVIVoyage FVIVoyage::ValenciaToIbiza()
{
	FVIVoyage V;
	V.VesselName = TEXT("LEVANTE");
	V.Departure = TEXT("Valencia · Puerto");
	V.Arrival = TEXT("Ibiza · Eivissa");

	auto Add = [&V](const TCHAR* Id, const TCHAR* Name, double Lat, double Lon, double Kn, bool Harbor)
	{
		FVIWaypoint W;
		W.Id = Id;
		W.Name = Name;
		W.Pos = FVILatLon(Lat, Lon);
		W.SpeedHintKn = Kn;
		W.bHarbor = Harbor;
		V.Waypoints.Add(W);
	};

	// Passenger basin / Levante quay, Port of Valencia
	Add(TEXT("vlc-darsena"), TEXT("Valencia · dársena de pasaje"), 39.44650, -0.31980, 6.0, true);
	// South breakwater / channel
	Add(TEXT("vlc-bocana"), TEXT("Bocana de Valencia"), 39.43240, -0.29720, 9.0, true);
	// Clear of traffic separation, heading ESE
	Add(TEXT("vlc-largo"), TEXT("Aguas de Valencia"), 39.39000, -0.17000, 14.0, false);
	Add(TEXT("levante-1"), TEXT("Mar Balear · tramo 1"), 39.30000, 0.05000, 16.0, false);
	Add(TEXT("levante-2"), TEXT("Mar Balear · mitad"), 39.17000, 0.48000, 16.0, false);
	Add(TEXT("pitiusas"), TEXT("Aproximación a Pitiusas"), 39.03000, 0.98000, 15.0, false);
	Add(TEXT("ibz-oeste"), TEXT("Oeste de Ibiza"), 38.97500, 1.25000, 12.0, false);
	Add(TEXT("ibz-botafoc"), TEXT("Canal de Botafoc"), 38.92080, 1.40850, 8.0, true);
	// Ibiza commercial / passenger harbour
	Add(TEXT("ibz-puerto"), TEXT("Ibiza · puerto"), 38.90890, 1.43670, 5.0, true);

	return V;
}

int32 FVIVoyage::NextIndex(const FVILatLon& Here, int32 From, double ArriveM) const
{
	if (Waypoints.Num() == 0)
	{
		return 0;
	}
	int32 Idx = FMath::Clamp(From, 0, Waypoints.Num() - 1);
	while (Idx < Waypoints.Num() - 1 && FVIGeo::HaversineM(Here, Waypoints[Idx].Pos) < ArriveM)
	{
		++Idx;
	}
	return Idx;
}

FVIWaypoint FVIVoyage::CurrentTarget(int32 Index) const
{
	if (Waypoints.Num() == 0)
	{
		return FVIWaypoint();
	}
	return Waypoints[FMath::Clamp(Index, 0, Waypoints.Num() - 1)];
}

double FVIVoyage::RemainingM(const FVILatLon& Here, int32 NextIdx) const
{
	if (Waypoints.Num() == 0)
	{
		return 0.0;
	}
	const int32 Start = FMath::Clamp(NextIdx, 0, Waypoints.Num() - 1);
	double Acc = FVIGeo::HaversineM(Here, Waypoints[Start].Pos);
	for (int32 i = Start; i < Waypoints.Num() - 1; ++i)
	{
		Acc += FVIGeo::HaversineM(Waypoints[i].Pos, Waypoints[i + 1].Pos);
	}
	return Acc;
}

double FVIVoyage::TotalLengthM() const
{
	double Acc = 0.0;
	for (int32 i = 0; i < Waypoints.Num() - 1; ++i)
	{
		Acc += FVIGeo::HaversineM(Waypoints[i].Pos, Waypoints[i + 1].Pos);
	}
	return Acc;
}

double FVIVoyage::Progress01(const FVILatLon& Here, int32 NextIdx) const
{
	const double Total = TotalLengthM();
	if (Total <= 1.0)
	{
		return 1.0;
	}
	const double Left = RemainingM(Here, NextIdx);
	return FMath::Clamp(1.0 - Left / Total, 0.0, 1.0);
}
