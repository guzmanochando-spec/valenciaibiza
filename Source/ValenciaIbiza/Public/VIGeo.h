#pragma once

#include "CoreMinimal.h"

/** WGS-84 helpers. Unreal world uses centimeters; geo math stays in meters. */
struct FVILatLon
{
	double Lat = 0.0;
	double Lon = 0.0;

	FVILatLon() = default;
	FVILatLon(double InLat, double InLon) : Lat(InLat), Lon(InLon) {}

	bool Equals(const FVILatLon& Other, double Eps = 1e-7) const
	{
		return FMath::Abs(Lat - Other.Lat) < Eps && FMath::Abs(Lon - Other.Lon) < Eps;
	}
};

struct FVIGeo
{
	/** Projection origin: midpoint of the Valencia–Ibiza rhumb. */
	FVILatLon Origin = FVILatLon(39.1777, 0.55845);

	static constexpr double EarthRadiusM = 6371008.8;

	static double DegToRad(double Deg) { return Deg * PI / 180.0; }
	static double RadToDeg(double Rad) { return Rad * 180.0 / PI; }

	static double MetersPerDegLat(double LatDeg)
	{
		const double Lat = DegToRad(LatDeg);
		return 111132.92 - 559.82 * FMath::Cos(2.0 * Lat) + 1.175 * FMath::Cos(4.0 * Lat);
	}

	static double MetersPerDegLon(double LatDeg)
	{
		const double Lat = DegToRad(LatDeg);
		return 111412.84 * FMath::Cos(Lat) - 93.5 * FMath::Cos(3.0 * Lat);
	}

	/** East-north meters from the current origin. */
	FVector2D ToEastNorthM(const FVILatLon& P) const
	{
		const double Lat0 = Origin.Lat;
		const double North = (P.Lat - Lat0) * MetersPerDegLat(Lat0);
		const double East = (P.Lon - Origin.Lon) * MetersPerDegLon(Lat0);
		return FVector2D(East, North);
	}

	FVILatLon FromEastNorthM(const FVector2D& EN) const
	{
		const double Lat0 = Origin.Lat;
		return FVILatLon(
			Lat0 + EN.Y / MetersPerDegLat(Lat0),
			Origin.Lon + EN.X / MetersPerDegLon(Lat0));
	}

	/** Unreal location (cm) relative to a visual origin expressed in east-north meters. */
	FVector ToUnreal(const FVILatLon& P, const FVector2D& VisualOriginEN, double AltM = 0.0) const
	{
		const FVector2D EN = ToEastNorthM(P) - VisualOriginEN;
		return FVector(EN.X * 100.0, EN.Y * 100.0, AltM * 100.0);
	}

	FVILatLon FromUnreal(const FVector& WorldCm, const FVector2D& VisualOriginEN) const
	{
		const FVector2D EN(WorldCm.X / 100.0 + VisualOriginEN.X, WorldCm.Y / 100.0 + VisualOriginEN.Y);
		return FromEastNorthM(EN);
	}

	static double HaversineM(const FVILatLon& A, const FVILatLon& B)
	{
		const double DLat = DegToRad(B.Lat - A.Lat);
		const double DLon = DegToRad(B.Lon - A.Lon);
		const double La1 = DegToRad(A.Lat);
		const double La2 = DegToRad(B.Lat);
		const double H = FMath::Sin(DLat * 0.5) * FMath::Sin(DLat * 0.5)
			+ FMath::Cos(La1) * FMath::Cos(La2) * FMath::Sin(DLon * 0.5) * FMath::Sin(DLon * 0.5);
		return 2.0 * EarthRadiusM * FMath::Asin(FMath::Clamp(FMath::Sqrt(H), 0.0, 1.0));
	}

	static double NauticalMiles(double Meters) { return Meters / 1852.0; }

	/** Initial true bearing A → B, degrees [0, 360). */
	static double BearingDeg(const FVILatLon& A, const FVILatLon& B)
	{
		const double La1 = DegToRad(A.Lat);
		const double La2 = DegToRad(B.Lat);
		const double DLon = DegToRad(B.Lon - A.Lon);
		const double Y = FMath::Sin(DLon) * FMath::Cos(La2);
		const double X = FMath::Cos(La1) * FMath::Sin(La2) - FMath::Sin(La1) * FMath::Cos(La2) * FMath::Cos(DLon);
		double Brg = RadToDeg(FMath::Atan2(Y, X));
		if (Brg < 0.0)
		{
			Brg += 360.0;
		}
		return Brg;
	}

	static FVILatLon RhumbDestination(const FVILatLon& Start, double BearingDegValue, double DistanceM)
	{
		const double Lat1 = DegToRad(Start.Lat);
		const double Lon1 = DegToRad(Start.Lon);
		const double Brg = DegToRad(BearingDegValue);
		const double D = DistanceM / EarthRadiusM;
		const double Lat2 = Lat1 + D * FMath::Cos(Brg);
		const double DPhi = FMath::Loge(FMath::Tan(Lat2 * 0.5 + PI * 0.25) / FMath::Tan(Lat1 * 0.5 + PI * 0.25));
		const double Q = FMath::Abs(DPhi) > 1e-12 ? (Lat2 - Lat1) / DPhi : FMath::Cos(Lat1);
		const double Lon2 = Lon1 + D * FMath::Sin(Brg) / Q;
		return FVILatLon(RadToDeg(Lat2), RadToDeg(Lon2));
	}

	static double DeltaHeadingDeg(double From, double To)
	{
		double D = FMath::Fmod(To - From + 540.0, 360.0) - 180.0;
		return D;
	}

	static double NormalizeHeading(double Deg)
	{
		double H = FMath::Fmod(Deg, 360.0);
		if (H < 0.0)
		{
			H += 360.0;
		}
		return H;
	}
};
