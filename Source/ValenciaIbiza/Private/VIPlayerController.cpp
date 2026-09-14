#include "VIPlayerController.h"
#include "VIShipPawn.h"
#include "VIMariaPresence.h"
#include "VIGameMode.h"
#include "Kismet/GameplayStatics.h"

AVIPlayerController::AVIPlayerController()
{
	bShowMouseCursor = false;
}

void AVIPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

AVIShipPawn* AVIPlayerController::Ship() const
{
	return Cast<AVIShipPawn>(GetPawn());
}

AVIMariaPresence* AVIPlayerController::Maria() const
{
	if (const AVIGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AVIGameMode>() : nullptr)
	{
		return GM->GetMaria();
	}
	return nullptr;
}

void AVIPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindAxis("Throttle", this, &AVIPlayerController::Throttle);
	InputComponent->BindAxis("Steer", this, &AVIPlayerController::Steer);
	InputComponent->BindAxis("LookYaw", this, &AVIPlayerController::LookYaw);
	InputComponent->BindAxis("LookPitch", this, &AVIPlayerController::LookPitch);
	InputComponent->BindAction("ToggleAutopilot", IE_Pressed, this, &AVIPlayerController::ToggleAutopilot);
	InputComponent->BindAction("CycleCamera", IE_Pressed, this, &AVIPlayerController::CycleCamera);
	InputComponent->BindAction("TimeX1", IE_Pressed, this, &AVIPlayerController::TimeX1);
	InputComponent->BindAction("TimeX10", IE_Pressed, this, &AVIPlayerController::TimeX10);
	InputComponent->BindAction("TimeX30", IE_Pressed, this, &AVIPlayerController::TimeX30);
	InputComponent->BindAction("TimeX60", IE_Pressed, this, &AVIPlayerController::TimeX60);
	InputComponent->BindAction("ResetVoyage", IE_Pressed, this, &AVIPlayerController::ResetVoyage);
	InputComponent->BindAction("AskMariaChart", IE_Pressed, this, &AVIPlayerController::AskMariaChart);
	InputComponent->BindAction("AskMariaRadar", IE_Pressed, this, &AVIPlayerController::AskMariaRadar);
	InputComponent->BindAction("AskMariaRadio", IE_Pressed, this, &AVIPlayerController::AskMariaRadio);
}

void AVIPlayerController::Throttle(float V)
{
	if (AVIShipPawn* S = Ship())
	{
		S->SetThrottleInput(V);
	}
}

void AVIPlayerController::Steer(float V)
{
	if (AVIShipPawn* S = Ship())
	{
		S->SetSteerInput(V);
	}
}

void AVIPlayerController::LookYaw(float V)
{
	if (AVIShipPawn* S = Ship())
	{
		S->SetLookInput(V, 0.0f);
	}
}

void AVIPlayerController::LookPitch(float V)
{
	if (AVIShipPawn* S = Ship())
	{
		S->SetLookInput(0.0f, V);
	}
}

void AVIPlayerController::ToggleAutopilot()
{
	if (AVIShipPawn* S = Ship())
	{
		S->ToggleAutopilot();
		if (AVIMariaPresence* M = Maria())
		{
			M->CueVoyage(S->IsAutopilot() ? TEXT("autopilot_on") : TEXT("depart"));
		}
	}
}

void AVIPlayerController::CycleCamera()
{
	if (AVIShipPawn* S = Ship())
	{
		S->CycleCamera();
	}
}

void AVIPlayerController::TimeX1() { UGameplayStatics::SetGlobalTimeDilation(this, 1.0f); }
void AVIPlayerController::TimeX10() { UGameplayStatics::SetGlobalTimeDilation(this, 10.0f); }
void AVIPlayerController::TimeX30() { UGameplayStatics::SetGlobalTimeDilation(this, 30.0f); }
void AVIPlayerController::TimeX60() { UGameplayStatics::SetGlobalTimeDilation(this, 60.0f); }

void AVIPlayerController::ResetVoyage()
{
	if (AVIShipPawn* S = Ship())
	{
		S->ResetVoyage();
	}
	if (AVIMariaPresence* M = Maria())
	{
		M->CueVoyage(TEXT("depart"));
	}
}

void AVIPlayerController::AskMariaChart()
{
	if (AVIMariaPresence* M = Maria())
	{
		M->CueVoyage(TEXT("open_chart"));
	}
}

void AVIPlayerController::AskMariaRadar()
{
	if (AVIMariaPresence* M = Maria())
	{
		M->CueVoyage(TEXT("open_radar"));
	}
}

void AVIPlayerController::AskMariaRadio()
{
	if (AVIMariaPresence* M = Maria())
	{
		M->CueVoyage(TEXT("open_vhf"));
	}
}
