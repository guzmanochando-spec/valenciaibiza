#include "VIGameMode.h"
#include "VIShipPawn.h"
#include "VIWorldDirector.h"
#include "VISkyDirector.h"
#include "VIMariaPresence.h"
#include "VIPlayerController.h"
#include "VITrafficDirector.h"
#include "VIBridgeHUD.h"
#include "Kismet/GameplayStatics.h"

AVIGameMode::AVIGameMode()
{
	DefaultPawnClass = AVIShipPawn::StaticClass();
	PlayerControllerClass = AVIPlayerController::StaticClass();
	HUDClass = AVIBridgeHUD::StaticClass();
}

void AVIGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Sky = GetWorld()->SpawnActor<AVISkyDirector>(AVISkyDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	Director = GetWorld()->SpawnActor<AVIWorldDirector>(AVIWorldDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	Traffic = GetWorld()->SpawnActor<AVITrafficDirector>(AVITrafficDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);

	APawn* Existing = UGameplayStatics::GetPlayerPawn(this, 0);
	Ship = Cast<AVIShipPawn>(Existing);
	if (!Ship)
	{
		Ship = GetWorld()->SpawnActor<AVIShipPawn>(AVIShipPawn::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->Possess(Ship);
		}
	}

	if (Director && Ship) Director->InitializeVoyage(Ship);

	Maria = GetWorld()->SpawnActor<AVIMariaPresence>(AVIMariaPresence::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	if (Maria && Ship) Maria->AttachToHelm(Ship);
}
