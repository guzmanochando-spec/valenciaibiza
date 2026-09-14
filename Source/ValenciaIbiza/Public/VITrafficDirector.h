#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIContact.h"
#include "VITrafficDirector.generated.h"

UCLASS()
class VALENCIAIBIZA_API AVITrafficDirector : public AActor
{
	GENERATED_BODY()

public:
	AVITrafficDirector();
	void SeedRoute();
	virtual void Tick(float DeltaSeconds) override;
	const TArray<FVIContact>& GetContacts() const { return Contacts; }

protected:
	virtual void BeginPlay() override;

private:
	TArray<FVIContact> Contacts;
};
