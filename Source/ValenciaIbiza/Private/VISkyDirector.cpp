#include "VISkyDirector.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/VolumetricCloudComponent.h"

AVISkyDirector::AVISkyDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVISkyDirector::BeginPlay()
{
	Super::BeginPlay();
	BuildMediterraneanSky();
}

void AVISkyDirector::BuildMediterraneanSky()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Sun = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(0, 0, 4000), FRotator(-38.0f, -48.0f, 0.0f), P);
	if (Sun)
	{
		if (UDirectionalLightComponent* C = Sun->GetComponent())
		{
			C->SetIntensity(9.0f);
			C->SetLightColor(FLinearColor(1.0f, 0.93f, 0.78f));
			C->SetAtmosphereSunLight(true);
			C->DynamicShadowDistanceMovableLight = 50000.0f;
		}
	}

	Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	if (Sky)
	{
		if (USkyLightComponent* C = Sky->GetLightComponent())
		{
			C->SourceType = ESkyLightSourceType::SLS_CapturedScene;
			C->SetIntensity(1.15f);
			C->RecaptureSky();
		}
	}

	Atmosphere = World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	Clouds = World->SpawnActor<AVolumetricCloud>(AVolumetricCloud::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);

	Fog = World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FVector(0, 0, 200), FRotator::ZeroRotator, P);
	if (Fog)
	{
		if (UExponentialHeightFogComponent* C = Fog->GetComponent())
		{
			C->SetFogDensity(0.012f);
			C->SetFogHeightFalloff(0.12f);
			C->SetFogInscatteringColor(FLinearColor(0.45f, 0.62f, 0.72f));
			C->SetVolumetricFog(true);
		}
	}

	Post = World->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, P);
	if (Post)
	{
		Post->bUnbound = true;
		FPostProcessSettings& S = Post->Settings;
		S.bOverride_AutoExposureBias = true;
		S.AutoExposureBias = 0.15f;
		S.bOverride_ColorSaturation = true;
		S.ColorSaturation = FVector4(1.02f, 1.04f, 1.08f, 1.0f);
		S.bOverride_ColorContrast = true;
		S.ColorContrast = FVector4(1.04f, 1.04f, 1.05f, 1.0f);
		S.bOverride_BloomIntensity = true;
		S.BloomIntensity = 0.35f;
		S.bOverride_VignetteIntensity = true;
		S.VignetteIntensity = 0.28f;
	}
}
