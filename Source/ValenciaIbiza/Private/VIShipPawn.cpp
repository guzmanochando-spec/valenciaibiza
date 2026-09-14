#include "VIShipPawn.h"
#include "VIShapeKit.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"

AVIShipPawn::AVIShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 4.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 6.0f;
	SpringArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->SetFieldOfView(70.0f);
	CameraIndex = 0;

	Voyage = FVIVoyage::ValenciaToIbiza();
	LatLon = Voyage.Waypoints.Num() ? Voyage.Waypoints[0].Pos : FVILatLon(39.4465, -0.3198);
}

void AVIShipPawn::BeginPlay()
{
	Super::BeginPlay();
	BuildHull();
	SetCameraMode(0);
	UpdateVisualPose();
}

void AVIShipPawn::BuildHull()
{
	UStaticMesh* Cube = FVIShapeKit::Cube();
	UStaticMesh* Cone = FVIShapeKit::Cone();
	UStaticMesh* Cyl = FVIShapeKit::Cylinder();
	UStaticMesh* Plane = FVIShapeKit::Plane();
	if (!Cube)
	{
		return;
	}

	const FLinearColor Hull(0.07f, 0.10f, 0.14f, 1.0f);
	const FLinearColor Boot(0.55f, 0.12f, 0.10f, 1.0f);
	const FLinearColor Deck(0.78f, 0.74f, 0.66f, 1.0f);
	const FLinearColor Super(0.93f, 0.94f, 0.96f, 1.0f);
	const FLinearColor Glass(0.15f, 0.28f, 0.38f, 1.0f);
	const FLinearColor Teak(0.42f, 0.28f, 0.16f, 1.0f);
	const FLinearColor Chrome(0.75f, 0.78f, 0.82f, 1.0f);

	// Engine cube is 100cm. Scale is in component units.
	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 90), FRotator::ZeroRotator, FVector(22.0f, 5.4f, 1.8f), Hull, TEXT("Hull"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(0, 0, 40), FRotator::ZeroRotator, FVector(22.2f, 5.5f, 0.18f), Boot, TEXT("BootTop"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(80, 0, 175), FRotator::ZeroRotator, FVector(16.0f, 5.0f, 0.16f), Deck, TEXT("Deck"));
	if (Cone)
	{
		FVIShapeKit::Attach(this, Root, Cone, FVector(1180, 0, 90), FRotator(0, 0, -90), FVector(3.4f, 5.2f, 6.5f), Hull, TEXT("Bow"));
	}
	FVIShapeKit::Attach(this, Root, Cube, FVector(-200, 0, 280), FRotator::ZeroRotator, FVector(7.2f, 4.4f, 2.1f), Super, TEXT("Salon"));
	// Open bridge: roof + pillars so the interior camera sees bow, deck and sea.
	FVIShapeKit::Attach(this, Root, Cube, FVector(-40, 0, 470), FRotator::ZeroRotator, FVector(4.6f, 4.0f, 0.12f), Super, TEXT("BridgeRoof"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(160, 170, 400), FRotator::ZeroRotator, FVector(0.16f, 0.16f, 1.6f), Super, TEXT("PillarStbd"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(160, -170, 400), FRotator::ZeroRotator, FVector(0.16f, 0.16f, 1.6f), Super, TEXT("PillarPort"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(-40, 0, 330), FRotator::ZeroRotator, FVector(2.8f, 3.4f, 0.28f), FLinearColor(0.10f, 0.12f, 0.14f), TEXT("HelmDash"));
	FVIShapeKit::Attach(this, Root, Cube, FVector(70, 0, 355), FRotator(-18, 0, 0), FVector(1.1f, 2.2f, 0.06f), Glass, TEXT("MfdGlass"));
	if (Cyl)
	{
		FVIShapeKit::Attach(this, Root, Cyl, FVector(20, 0, 365), FRotator(80, 0, 0), FVector(0.55f, 0.55f, 0.08f), Chrome, TEXT("Wheel"));
	}
	FVIShapeKit::Attach(this, Root, Cube, FVector(-780, 0, 230), FRotator::ZeroRotator, FVector(3.6f, 4.6f, 0.12f), Teak, TEXT("AftDeck"));
	if (Cyl)
	{
		FVIShapeKit::Attach(this, Root, Cyl, FVector(-120, 0, 620), FRotator::ZeroRotator, FVector(0.18f, 0.18f, 4.2f), Chrome, TEXT("Mast"));
		FVIShapeKit::Attach(this, Root, Cyl, FVector(-980, 160, 70), FRotator(0, 0, 90), FVector(0.35f, 0.35f, 0.8f), Hull, TEXT("PropL"));
		FVIShapeKit::Attach(this, Root, Cyl, FVector(-980, -160, 70), FRotator(0, 0, 90), FVector(0.35f, 0.35f, 0.8f), Hull, TEXT("PropR"));
	}
	if (Plane)
	{
		const FLinearColor Wake(0.75f, 0.88f, 0.92f, 0.35f);
		FVIShapeKit::Attach(this, Root, Plane, FVector(-1600, 0, 8), FRotator::ZeroRotator, FVector(18.0f, 4.0f, 1.0f), Wake, TEXT("Wake"), true);
	}
}

void AVIShipPawn::SetVoyage(const FVIVoyage& InVoyage, const FVIGeo& InGeo)
{
	Voyage = InVoyage;
	Geo = InGeo;
}

void AVIShipPawn::SetVisualOrigin(const FVector2D& EastNorthM)
{
	VisualOriginEN = EastNorthM;
	UpdateVisualPose();
}

void AVIShipPawn::TeleportToLatLon(const FVILatLon& Pos, double Heading)
{
	LatLon = Pos;
	HeadingDeg = FVIGeo::NormalizeHeading(Heading);
	CogDeg = HeadingDeg;
	SogKn = 0.0;
	Throttle = 0.15;
	bArrived = false;
	WaypointIndex = Voyage.NextIndex(LatLon, 1);
	UpdateVisualPose();
}

void AVIShipPawn::SetThrottleInput(float Value)
{
	if (bAutopilot)
	{
		return;
	}
	ThrottleCmd = FMath::Clamp(Value, -0.35f, 1.0f);
}

void AVIShipPawn::SetSteerInput(float Value)
{
	SteerInput = FMath::Clamp(Value, -1.0f, 1.0f);
	if (!bAutopilot)
	{
		RudderCmd = SteerInput * 32.0f;
	}
}

void AVIShipPawn::SetLookInput(float Yaw, float Pitch)
{
	LookYaw = FMath::Clamp(LookYaw + Yaw, -160.0f, 160.0f);
	LookPitch = FMath::Clamp(LookPitch + Pitch, -40.0f, 10.0f);
}

void AVIShipPawn::ToggleAutopilot()
{
	bAutopilot = !bAutopilot;
	if (!bAutopilot)
	{
		RudderCmd = 0.0;
	}
}

void AVIShipPawn::CycleCamera()
{
	SetCameraMode((CameraIndex + 1) % 3);
}

void AVIShipPawn::SetCameraMode(int32 Mode)
{
	CameraIndex = ((Mode % 3) + 3) % 3;
	LookYaw = 0.0f;
	LookPitch = 0.0f;
	ApplyCameraRig();
}

FString AVIShipPawn::GetCameraName() const
{
	switch (CameraIndex)
	{
	case 0: return TEXT("Puente · interior");
	case 1: return TEXT("Exterior · 3/4");
	default: return TEXT("Vista de pájaro");
	}
}

void AVIShipPawn::ApplyCameraRig()
{
	if (!SpringArm || !Camera)
	{
		return;
	}

	switch (CameraIndex)
	{
	case 0: // Interior: seated at helm, bow and deck through the open windshield
		SpringArm->TargetArmLength = 0.0f;
		SpringArm->bEnableCameraLag = false;
		SpringArm->SetRelativeLocation(FVector(-90.0f, 0.0f, 368.0f));
		SpringArm->SetRelativeRotation(FRotator(-4.0f + LookPitch * 0.15f, LookYaw * 0.35f, 0.0f));
		Camera->SetFieldOfView(68.0f);
		break;
	case 1: // Exterior chase: whole hull in frame
		SpringArm->TargetArmLength = 3200.0f;
		SpringArm->bEnableCameraLag = true;
		SpringArm->SetRelativeLocation(FVector(-200.0f, 0.0f, 520.0f));
		SpringArm->SetRelativeRotation(FRotator(-16.0f + LookPitch, LookYaw, 0.0f));
		Camera->SetFieldOfView(70.0f);
		break;
	default: // Bird's eye: overhead, boat + wake + sea
		SpringArm->TargetArmLength = 0.0f;
		SpringArm->bEnableCameraLag = true;
		SpringArm->SetRelativeLocation(FVector(-200.0f, 0.0f, 9800.0f));
		SpringArm->SetRelativeRotation(FRotator(-88.0f, LookYaw * 0.15f, 0.0f));
		Camera->SetFieldOfView(52.0f);
		break;
	}
}

void AVIShipPawn::ResetVoyage()
{
	if (Voyage.Waypoints.Num() == 0)
	{
		return;
	}
	const FVIWaypoint& Start = Voyage.Waypoints[0];
	const double Brg = Voyage.Waypoints.Num() > 1
		? FVIGeo::BearingDeg(Start.Pos, Voyage.Waypoints[1].Pos)
		: 118.0;
	Throttle = 0.2;
	ThrottleCmd = 0.2;
	RudderDeg = 0.0;
	RudderCmd = 0.0;
	YawRateDps = 0.0;
	bAutopilot = true;
	TeleportToLatLon(Start.Pos, Brg);
}

void AVIShipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AVIShipPawn::ApplyAutopilot(float Dt)
{
	if (!bAutopilot || Voyage.Waypoints.Num() == 0)
	{
		return;
	}

	WaypointIndex = Voyage.NextIndex(LatLon, WaypointIndex, bArrived ? 180.0 : 700.0);
	const FVIWaypoint Target = Voyage.CurrentTarget(WaypointIndex);
	const double Dist = FVIGeo::HaversineM(LatLon, Target.Pos);
	const double Brg = FVIGeo::BearingDeg(LatLon, Target.Pos);
	const double Err = FVIGeo::DeltaHeadingDeg(HeadingDeg, Brg);

	RudderCmd = FMath::Clamp(Err * 1.15, -32.0, 32.0);

	double DesiredKn = Target.SpeedHintKn;
	if (Dist < 1200.0)
	{
		DesiredKn = FMath::Lerp(4.5, DesiredKn, Dist / 1200.0);
	}
	if (Target.bHarbor)
	{
		DesiredKn = FMath::Min(DesiredKn, 8.0);
	}
	if (bArrived)
	{
		DesiredKn = 0.0;
	}

	const double MaxKn = 22.0;
	ThrottleCmd = FMath::Clamp(DesiredKn / MaxKn, 0.0, 1.0);
}

void AVIShipPawn::Integrate(float Dt)
{
	Dt = FMath::Clamp(Dt, 0.0f, 0.05f);

	Throttle = FMath::FInterpTo(Throttle, ThrottleCmd, Dt, 0.7f);
	RudderDeg = FMath::FInterpTo(RudderDeg, RudderCmd, Dt, 2.4f);

	const double MaxKn = 22.0;
	const double TargetKn = Throttle * MaxKn;
	SogKn = FMath::FInterpTo(SogKn, TargetKn, Dt, 0.55f);

	const double SpeedFac = FMath::Clamp(SogKn / 10.0, 0.15, 1.6);
	YawRateDps = FMath::FInterpTo(YawRateDps, RudderDeg * 0.22 * SpeedFac, Dt, 2.8f);
	HeadingDeg = FVIGeo::NormalizeHeading(HeadingDeg + YawRateDps * Dt);

	// Slight sideslip so COG is not identical to heading
	const double Drift = RudderDeg * 0.08;
	CogDeg = FVIGeo::NormalizeHeading(HeadingDeg + Drift);

	const double Ms = SogKn * 1852.0 / 3600.0;
	if (Ms > 0.02)
	{
		LatLon = FVIGeo::RhumbDestination(LatLon, CogDeg, Ms * Dt);
	}

	const FVIWaypoint Last = Voyage.CurrentTarget(Voyage.Waypoints.Num() - 1);
	if (FVIGeo::HaversineM(LatLon, Last.Pos) < 220.0)
	{
		bArrived = true;
	}
}

void AVIShipPawn::UpdateVisualPose()
{
	const FVector Loc = Geo.ToUnreal(LatLon, VisualOriginEN, 0.0);
	SetActorLocation(Loc);
	SetActorRotation(FRotator(0.0, HeadingDeg, 0.0));
}

void AVIShipPawn::UpdateWaveMotion(float Dt)
{
	BobTime += Dt;
	const float Heave = FMath::Sin(BobTime * 1.15f) * 18.0f + FMath::Sin(BobTime * 0.37f) * 10.0f;
	const float Pitch = FMath::Sin(BobTime * 0.85f) * 0.55f;
	const float Roll = FMath::Sin(BobTime * 0.62f + 0.4f) * 0.7f;
	const FVector Base = Geo.ToUnreal(LatLon, VisualOriginEN, 0.0);
	SetActorLocation(Base + FVector(0, 0, Heave));
	SetActorRotation(FRotator(Pitch, HeadingDeg, Roll));

	ApplyCameraRig();
}

void AVIShipPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ApplyAutopilot(DeltaSeconds);
	Integrate(DeltaSeconds);
	UpdateWaveMotion(DeltaSeconds);
}

FString AVIShipPawn::GetStatusLine() const
{
	if (bArrived)
	{
		return TEXT("Llegada a Ibiza · amarre en puerto");
	}
	if (bAutopilot)
	{
		const FVIWaypoint T = Voyage.CurrentTarget(WaypointIndex);
		return FString::Printf(TEXT("Piloto automático · rumbo a %s"), *T.Name);
	}
	return TEXT("Gobierno manual · caña y máquinas");
}
