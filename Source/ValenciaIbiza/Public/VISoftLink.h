#pragma once

#include "CoreMinimal.h"
#include "VIGeo.h"
#include "VIContact.h"
#include "VIVoyage.h"
#include "VIAcademiaConfig.h"

struct FVISoftLink
{
	static FString NmeaChecksum(const FString& Body);
	static FString Rmc(const FVILatLon& P, double SogKn, double CogDeg);
	static FString Gga(const FVILatLon& P);
	static FString OwnshipJson(const FVILatLon& P, double Hdg, double Cog, double Sog,
		int32 Wp, const FString& Status, const TArray<FVIContact>& Contacts);
	static void WriteSnapshot(const FString& Json);
	static FString CartaUrl() { return VIAcademia::CartaUrl(); }
	static FString RadarUrl() { return VIAcademia::RadarUrl(); }
	static FString RadioUrl() { return VIAcademia::RadioUrl(); }
};
