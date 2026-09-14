#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VIMariaPresence.generated.h"

class AVIShipPawn;
class UTextRenderComponent;

/**
 * María on the helm — physical instructor, not a HUD chatbot.
 * Speaks local voyage cues now; mah-hub /maria/chat is the later brain.
 * She never writes physics, throttle, or rudder.
 */
UCLASS()
class VALENCIAIBIZA_API AVIMariaPresence : public AActor
{
	GENERATED_BODY()

public:
	AVIMariaPresence();

	void AttachToHelm(AVIShipPawn* Ship);
	void Speak(const FString& Line, float HoldSeconds = 8.0f);
	void CueVoyage(const FString& EventId);

	const FString& GetLastLine() const { return LastLine; }
	bool IsSpeaking() const { return SpeakTimeLeft > 0.0f; }

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> SpeechCard;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void BuildFigure();
	void LookAtHelmsman(float DeltaSeconds);

	UPROPERTY()
	TObjectPtr<AVIShipPawn> HelmShip;

	FString LastLine;
	float SpeakTimeLeft = 0.0f;
	bool bWelcomed = false;
};
