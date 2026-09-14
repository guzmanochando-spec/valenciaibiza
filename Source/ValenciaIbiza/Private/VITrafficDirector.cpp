#include "VITrafficDirector.h"

AVITrafficDirector::AVITrafficDirector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVITrafficDirector::BeginPlay()
{
	Super::BeginPlay();
	SeedRoute();
}

void AVITrafficDirector::SeedRoute()
{
	Contacts.Reset();
	auto Add = [this](const TCHAR* Mmsi, const TCHAR* Name, double Lat, double Lon, double Cog, double Kn, double Len)
	{
		FVIContact C;
		C.Mmsi = Mmsi;
		C.Name = Name;
		C.Pos = FVILatLon(Lat, Lon);
		C.CogDeg = Cog;
		C.SogKn = Kn;
		C.LengthM = Len;
		Contacts.Add(C);
	};
	Add(TEXT("224123000"), TEXT("BALEARIA ABBA"), 39.410, -0.210, 118.0, 19.0, 165.0);
	Add(TEXT("224888111"), TEXT("TRASMED"), 39.250, 0.120, 300.0, 17.5, 180.0);
	Add(TEXT("224055432"), TEXT("PESCA LEVANTE"), 39.360, -0.080, 40.0, 7.0, 22.0);
	Add(TEXT("225901002"), TEXT("PILOT VLC"), 39.438, -0.305, 200.0, 9.0, 16.0);
	Add(TEXT("224777010"), TEXT("YACHT IBZ"), 38.980, 1.180, 250.0, 12.0, 28.0);
}

void AVITrafficDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Dt = FMath::Clamp(DeltaSeconds, 0.f, 0.1f);
	for (FVIContact& C : Contacts)
	{
		const double Ms = C.SogKn * 1852.0 / 3600.0;
		C.Pos = FVIGeo::RhumbDestination(C.Pos, C.CogDeg, Ms * Dt);
	}
}
