#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VIPlayerController.generated.h"

class AVIShipPawn;
class AVIMariaPresence;

UCLASS()
class VALENCIAIBIZA_API AVIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVIPlayerController();

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	AVIShipPawn* Ship() const;
	AVIMariaPresence* Maria() const;

	void Throttle(float V);
	void Steer(float V);
	void LookYaw(float V);
	void LookPitch(float V);
	void ToggleAutopilot();
	void CycleCamera();
	void TimeX1();
	void TimeX10();
	void TimeX30();
	void TimeX60();
	void ResetVoyage();
	void AskMariaChart();
	void AskMariaRadar();
	void AskMariaRadio();
};
