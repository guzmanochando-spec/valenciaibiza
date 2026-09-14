#pragma once

#include "CoreMinimal.h"
#include "VIGeo.h"

struct FVIContact
{
	FString Mmsi;
	FString Name;
	FVILatLon Pos;
	double CogDeg = 0.0;
	double SogKn = 0.0;
	double LengthM = 80.0;
	bool bAis = true;

	double RangeM(const FVILatLon& Own) const { return FVIGeo::HaversineM(Own, Pos); }
	double BearingDeg(const FVILatLon& Own) const { return FVIGeo::BearingDeg(Own, Pos); }

	void CpaTcpa(const FVILatLon& Own, double OwnCog, double OwnSogKn, double& OutCpaM, double& OutTcpaS) const
	{
		const double RelBrg = FVIGeo::DegToRad(BearingDeg(Own));
		const double R = RangeM(Own);
		const FVector2D Rel(R * FMath::Sin(RelBrg), R * FMath::Cos(RelBrg));
		auto Vel = [](double Cog, double Kn)
		{
			const double Ms = Kn * 1852.0 / 3600.0;
			const double A = FVIGeo::DegToRad(Cog);
			return FVector2D(Ms * FMath::Sin(A), Ms * FMath::Cos(A));
		};
		const FVector2D Vrel = Vel(CogDeg, SogKn) - Vel(OwnCog, OwnSogKn);
		const double V2 = FVector2D::DotProduct(Vrel, Vrel);
		if (V2 < 1e-6)
		{
			OutCpaM = R;
			OutTcpaS = 0.0;
			return;
		}
		OutTcpaS = -FVector2D::DotProduct(Rel, Vrel) / V2;
		const FVector2D Closest = Rel + Vrel * OutTcpaS;
		OutCpaM = Closest.Size();
	}
};
