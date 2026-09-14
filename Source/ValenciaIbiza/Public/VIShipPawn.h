#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VIGeo.h"
#include "VIVoyage.h"
#include "VIShipPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class VALENCIAIBIZA_API AVIShipPawn : public APawn
{
	GENERATED_BODY()

public:
	AVIShipPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetVoyage(const FVIVoyage& InVoyage, const FVIGeo& InGeo);
	void SetVisualOrigin(const FVector2D& EastNorthM);
	void TeleportToLatLon(const FVILatLon& Pos, double HeadingDeg);

	void SetThrottleInput(float Value);
	void SetSteerInput(float Value);
	void SetLookInput(float Yaw, float Pitch);
	void ToggleAutopilot();
	void CycleCamera();
	void SetCameraMode(int32 Mode);
	FString GetCameraName() const;
	void ResetVoyage();

	FVILatLon GetLatLon() const { return LatLon; }
	double GetHeadingDeg() const { return HeadingDeg; }
	double GetCogDeg() const { return CogDeg; }
	double GetSogKn() const { return SogKn; }
	double GetThrottle01() const { return Throttle; }
	double GetRudderDeg() const { return RudderDeg; }
	bool IsAutopilot() const { return bAutopilot; }
	int32 GetWaypointIndex() const { return WaypointIndex; }
	const FVIVoyage& GetVoyage() const { return Voyage; }
	FVIGeo& GetGeo() { return Geo; }
	FVector2D GetVisualOriginEN() const { return VisualOriginEN; }
	int32 GetCameraIndex() const { return CameraIndex; }
	bool HasArrived() const { return bArrived; }
	FString GetStatusLine() const;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

protected:
	virtual void BeginPlay() override;

private:
	void BuildHull();
	void ApplyCameraRig();
	void ApplyAutopilot(float Dt);
	void Integrate(float Dt);
	void UpdateVisualPose();
	void UpdateWaveMotion(float Dt);

	FVIVoyage Voyage;
	FVIGeo Geo;
	FVector2D VisualOriginEN = FVector2D::ZeroVector;

	FVILatLon LatLon;
	double HeadingDeg = 118.0;
	double CogDeg = 118.0;
	double SogKn = 0.0;
	double Throttle = 0.0;
	double ThrottleCmd = 0.0;
	double RudderDeg = 0.0;
	double RudderCmd = 0.0;
	double YawRateDps = 0.0;
	float SteerInput = 0.0f;
	float LookYaw = 0.0f;
	float LookPitch = -12.0f;
	int32 WaypointIndex = 1;
	int32 CameraIndex = 0;
	bool bAutopilot = true;
	bool bArrived = false;
	float BobTime = 0.0f;
};
