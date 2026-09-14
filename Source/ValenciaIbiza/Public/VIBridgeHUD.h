#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VIBridgeHUD.generated.h"

class AVIShipPawn;
class AVITrafficDirector;

UCLASS()
class VALENCIAIBIZA_API AVIBridgeHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVIBridgeHUD();
	virtual void DrawHUD() override;

	void ToggleChart();
	void ToggleRadar();
	void ToggleVhf();
	void CycleRadarRange();
	void CycleVhfChannel();
	void TransmitVhf();

	bool bChart = true;
	bool bRadar = true;
	bool bVhf = true;
	int32 RadarRangeNm = 12;
	int32 VhfChannel = 16;
	FString LastVhf;

protected:
	virtual void BeginPlay() override;

private:
	void DrawConning(AVIShipPawn* Ship, float X, float Y);
	void DrawChart(AVIShipPawn* Ship, float X, float Y, float W, float H);
	void DrawRadar(AVIShipPawn* Ship, AVITrafficDirector* Traffic, float X, float Y, float S);
	void DrawVhf(AVIShipPawn* Ship, float X, float Y, float W, float H);
	void DrawHelp(float Y);

	float DumpTimer = 0.0f;
};
