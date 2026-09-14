#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VISkyDirector.generated.h"

class ADirectionalLight;
class ASkyLight;
class ASkyAtmosphere;
class AExponentialHeightFog;
class APostProcessVolume;
class AVolumetricCloud;

UCLASS()
class VALENCIAIBIZA_API AVISkyDirector : public AActor
{
	GENERATED_BODY()

public:
	AVISkyDirector();
	void BuildMediterraneanSky();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ADirectionalLight> Sun;

	UPROPERTY()
	TObjectPtr<ASkyLight> Sky;

	UPROPERTY()
	TObjectPtr<ASkyAtmosphere> Atmosphere;

	UPROPERTY()
	TObjectPtr<AExponentialHeightFog> Fog;

	UPROPERTY()
	TObjectPtr<APostProcessVolume> Post;

	UPROPERTY()
	TObjectPtr<AVolumetricCloud> Clouds;
};
