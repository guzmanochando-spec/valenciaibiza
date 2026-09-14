#include "VIMariaPresence.h"
#include "VIShipPawn.h"
#include "VIShapeKit.h"
#include "Components/TextRenderComponent.h"
#include "ValenciaIbiza.h"

AVIMariaPresence::AVIMariaPresence()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpeechCard = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SpeechCard"));
	SpeechCard->SetupAttachment(Root);
	SpeechCard->SetRelativeLocation(FVector(40.0f, 0.0f, 210.0f));
	SpeechCard->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	SpeechCard->SetHorizontalAlignment(EHTA_Center);
	SpeechCard->SetVerticalAlignment(EVRTA_TextBottom);
	SpeechCard->SetWorldSize(18.0f);
	SpeechCard->SetTextRenderColor(FColor(236, 244, 248));
	SpeechCard->SetText(FText::GetEmpty());
}

void AVIMariaPresence::BeginPlay()
{
	Super::BeginPlay();
	BuildFigure();
}

void AVIMariaPresence::BuildFigure()
{
	UStaticMesh* Cube = FVIShapeKit::Cube();
	UStaticMesh* Sphere = FVIShapeKit::Sphere();
	UStaticMesh* Cyl = FVIShapeKit::Cylinder();
	if (!Cube)
	{
		return;
	}

	const FLinearColor Skin(0.72f, 0.54f, 0.42f, 1.0f);
	const FLinearColor Hair(0.12f, 0.09f, 0.08f, 1.0f);
	const FLinearColor Polo(0.08f, 0.28f, 0.38f, 1.0f);
	const FLinearColor Vest(0.92f, 0.55f, 0.12f, 1.0f);
	const FLinearColor Trouser(0.12f, 0.14f, 0.16f, 1.0f);

	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 55), FRotator::ZeroRotator, FVector(0.55f, 0.38f, 1.05f), Trouser, TEXT("Legs"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 125), FRotator::ZeroRotator, FVector(0.72f, 0.48f, 0.85f), Polo, TEXT("Torso"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 145), FRotator::ZeroRotator, FVector(0.82f, 0.58f, 0.38f), Vest, TEXT("LifeVest"));
	if (Sphere)
	{
		FVIShapeKit::Attach(this, Root, Sphere, FVector(0, 0, 188), FRotator::ZeroRotator, FVector(0.42f, 0.38f, 0.42f), Skin, TEXT("Head"));
		FVIShapeKit::Attach(this, Root, Sphere, FVector(-6, 0, 198), FRotator::ZeroRotator, FVector(0.44f, 0.40f, 0.22f), Hair, TEXT("Hair"));
	}
	if (Cyl)
	{
		FVIShapeKit::Attach(this, Root, Cyl, FVector(8, 32, 128), FRotator(0, 0, 18), FVector(0.12f, 0.12f, 0.55f), Polo, TEXT("ArmL"));
		FVIShapeKit::Attach(this, Root, Cyl, FVector(8, -32, 128), FRotator(0, 0, -12), FVector(0.12f, 0.12f, 0.55f), Polo, TEXT("ArmR"));
	}
}

void AVIMariaPresence::AttachToHelm(AVIShipPawn* Ship)
{
	HelmShip = Ship;
	if (!Ship)
	{
		return;
	}
	AttachToActor(Ship, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	// Starboard of the helm, facing the instruments / helmsman.
	SetActorRelativeLocation(FVector(-60.0f, 210.0f, 175.0f));
	SetActorRelativeRotation(FRotator(0.0f, -110.0f, 0.0f));
}

void AVIMariaPresence::Speak(const FString& Line, float HoldSeconds)
{
	LastLine = Line;
	SpeakTimeLeft = HoldSeconds;
	if (SpeechCard)
	{
		SpeechCard->SetText(FText::FromString(Line));
		SpeechCard->SetVisibility(true);
	}
	UE_LOG(LogValenciaIbiza, Display, TEXT("María: %s"), *Line);
}

void AVIMariaPresence::CueVoyage(const FString& EventId)
{
	if (EventId == TEXT("depart"))
	{
		Speak(TEXT("María · Estamos en Valencia. Establece rumbo a Ibiza. Yo vigilo carta, radar y VHF."));
	}
	else if (EventId == TEXT("autopilot_on"))
	{
		Speak(TEXT("María · Piloto automático al rumbo de derrota. Si cambia el tráfico, suéltalo y gobierna."));
	}
	else if (EventId == TEXT("open_chart"))
	{
		Speak(TEXT("María · Abre Carta en la pantalla Riomares. Necesito tu posición por dos demoras."));
	}
	else if (EventId == TEXT("open_radar"))
	{
		Speak(TEXT("María · Tráfico a estribor. Mira el radar: rumbo, distancia, CPA y TCPA."));
	}
	else if (EventId == TEXT("open_vhf"))
	{
		Speak(TEXT("María · Canal 16. Escucha y responde con fraseología correcta."));
	}
	else if (EventId == TEXT("arrive"))
	{
		Speak(TEXT("María · Ibiza a la vista. Reduce, entra por Botafoc y prepárate a amarrar."));
	}
}

void AVIMariaPresence::LookAtHelmsman(float DeltaSeconds)
{
	if (!HelmShip)
	{
		return;
	}
	const FVector Target = HelmShip->GetActorLocation() + HelmShip->GetActorForwardVector() * 80.0f + FVector(0, 0, 380);
	const FVector To = Target - (GetActorLocation() + FVector(0, 0, 180));
	if (!To.IsNearlyZero())
	{
		const FRotator Want = To.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(0.0f, Want.Yaw, 0.0f), DeltaSeconds, 1.6f));
	}
}

void AVIMariaPresence::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LookAtHelmsman(DeltaSeconds);

	if (!bWelcomed)
	{
		bWelcomed = true;
		CueVoyage(TEXT("depart"));
	}

	if (HelmShip && HelmShip->HasArrived() && LastLine.IsEmpty())
	{
		CueVoyage(TEXT("arrive"));
	}

	if (SpeakTimeLeft > 0.0f)
	{
		SpeakTimeLeft -= DeltaSeconds;
		if (SpeakTimeLeft <= 0.0f && SpeechCard)
		{
			SpeechCard->SetVisibility(false);
		}
	}
}
