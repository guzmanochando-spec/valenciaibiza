#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VIGameMode.generated.h"

class AVIShipPawn;
class AVIWorldDirector;
class AVISkyDirector;
class AVIMariaPresence;
class AVITrafficDirector;

UCLASS()
class VALENCIAIBIZA_API AVIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVIGameMode();

	AVIMariaPresence* GetMaria() const { return Maria; }
	AVITrafficDirector* GetTraffic() const { return Traffic; }
	AVIShipPawn* GetShip() const { return Ship; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY() TObjectPtr<AVIShipPawn> Ship;
	UPROPERTY() TObjectPtr<AVIWorldDirector> Director;
	UPROPERTY() TObjectPtr<AVISkyDirector> Sky;
	UPROPERTY() TObjectPtr<AVIMariaPresence> Maria;
	UPROPERTY() TObjectPtr<AVITrafficDirector> Traffic;
};
