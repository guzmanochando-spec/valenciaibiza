#include "VISoftLink.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

FString FVISoftLink::NmeaChecksum(const FString& Body)
{
	uint8 X = 0;
	for (int32 i = 0; i < Body.Len(); ++i)
	{
		X ^= static_cast<uint8>(Body[i]);
	}
	return FString::Printf(TEXT("*%02X"), X);
}

static FString FmtLat(double Lat)
{
	const double A = FMath::Abs(Lat);
	const int32 D = static_cast<int32>(A);
	const double M = (A - D) * 60.0;
	return FString::Printf(TEXT("%02d%07.4f,%c"), D, M, Lat >= 0 ? TCHAR('N') : TCHAR('S'));
}

static FString FmtLon(double Lon)
{
	const double A = FMath::Abs(Lon);
	const int32 D = static_cast<int32>(A);
	const double M = (A - D) * 60.0;
	return FString::Printf(TEXT("%03d%07.4f,%c"), D, M, Lon >= 0 ? TCHAR('E') : TCHAR('W'));
}

FString FVISoftLink::Rmc(const FVILatLon& P, double SogKn, double CogDeg)
{
	const FString Body = FString::Printf(TEXT("GPRMC,000000,A,%s,%s,%.2f,%.1f,140926,,,A"),
		*FmtLat(P.Lat), *FmtLon(P.Lon), SogKn, CogDeg);
	return TEXT("$") + Body + NmeaChecksum(Body);
}

FString FVISoftLink::Gga(const FVILatLon& P)
{
	const FString Body = FString::Printf(TEXT("GPGGA,000000,%s,%s,1,08,1.0,3.0,M,0.0,M,,"),
		*FmtLat(P.Lat), *FmtLon(P.Lon));
	return TEXT("$") + Body + NmeaChecksum(Body);
}

FString FVISoftLink::OwnshipJson(const FVILatLon& P, double Hdg, double Cog, double Sog,
	int32 Wp, const FString& Status, const TArray<FVIContact>& Contacts)
{
	FString Ctxt;
	for (int32 i = 0; i < Contacts.Num(); ++i)
	{
		const FVIContact& C = Contacts[i];
		if (i) Ctxt += TEXT(",");
		Ctxt += FString::Printf(TEXT("{\"mmsi\":\"%s\",\"name\":\"%s\",\"lat\":%.6f,\"lon\":%.6f,\"cog\":%.1f,\"sog\":%.1f}"),
			*C.Mmsi, *C.Name, C.Pos.Lat, C.Pos.Lon, C.CogDeg, C.SogKn);
	}
	const FString Safe = Status.Replace(TEXT("\""), TEXT("'"));
	return FString::Printf(
		TEXT("{\"ownship\":{\"lat\":%.6f,\"lon\":%.6f,\"hdg\":%.1f,\"cog\":%.1f,\"sog_kn\":%.2f,\"wp\":%d,\"status\":\"%s\"},")
		TEXT("\"carta\":\"%s\",\"radar\":\"%s\",\"vhf\":\"%s\",\"contacts\":[%s]}"),
		P.Lat, P.Lon, Hdg, Cog, Sog, Wp, *Safe,
		VIAcademia::CartaUrl(), VIAcademia::RadarUrl(), VIAcademia::RadioUrl(), *Ctxt);
}

void FVISoftLink::WriteSnapshot(const FString& Json)
{
	const FString Path = FPaths::ProjectSavedDir() / TEXT("Bridge") / TEXT("ownship.json");
	FFileHelper::SaveStringToFile(Json, *Path);
}
