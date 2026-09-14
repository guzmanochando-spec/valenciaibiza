#include "VIBridgeHUD.h"
#include "VIShipPawn.h"
#include "VITrafficDirector.h"
#include "VIMariaPresence.h"
#include "VIGameMode.h"
#include "VISoftLink.h"
#include "VIVoyage.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"

AVIBridgeHUD::AVIBridgeHUD() { LastVhf = TEXT("16 · Guardia — LEVANTE listo."); }
void AVIBridgeHUD::BeginPlay() { Super::BeginPlay(); }
void AVIBridgeHUD::ToggleChart() { bChart = !bChart; }
void AVIBridgeHUD::ToggleRadar() { bRadar = !bRadar; }
void AVIBridgeHUD::ToggleVhf() { bVhf = !bVhf; }
void AVIBridgeHUD::CycleRadarRange()
{
	static const int32 R[] = {1,3,6,12,24,48};
	int32 i=0; for (; i<6; ++i) if (R[i]==RadarRangeNm) break;
	RadarRangeNm = R[(i+1)%6];
}
void AVIBridgeHUD::CycleVhfChannel() { VhfChannel = (VhfChannel==16)?13:(VhfChannel==13)?10:16; }
void AVIBridgeHUD::TransmitVhf()
{
	AVIShipPawn* Ship = Cast<AVIShipPawn>(GetOwningPawn());
	if (!Ship) return;
	const FVILatLon P = Ship->GetLatLon();
	LastVhf = FString::Printf(TEXT("%d · LEVANTE, Valencia-Ibiza, pos %.3f %.3f, rumbo %.0f, %.1f kn"), VhfChannel, P.Lat, P.Lon, Ship->GetHeadingDeg(), Ship->GetSogKn());
	if (AVIGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AVIGameMode>() : nullptr)
	{
		if (AVIMariaPresence* M = GM->GetMaria()) M->Speak(TEXT("María · Transmisión VHF anotada. Espera acuse en 16."));
	}
}
void AVIBridgeHUD::DrawHUD()
{
	Super::DrawHUD();
	AVIShipPawn* Ship = Cast<AVIShipPawn>(GetOwningPawn());
	if (!Ship || !Canvas) return;
	AVITrafficDirector* Traffic = nullptr;
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVITrafficDirector::StaticClass(), Found);
	if (Found.Num()) Traffic = Cast<AVITrafficDirector>(Found[0]);
	const float W = Canvas->SizeX; const float H = Canvas->SizeY;
	DrawConning(Ship, 24.f, 24.f);
	if (bChart) DrawChart(Ship, 24.f, H*0.42f, W*0.38f, H*0.48f);
	if (bRadar) DrawRadar(Ship, Traffic, W*0.42f, H*0.42f, FMath::Min(H*0.48f, W*0.28f));
	if (bVhf) DrawVhf(Ship, W*0.72f, H*0.42f, W*0.26f, H*0.28f);
	DrawHelp(H-28.f);
	DumpTimer += GetWorld()->GetDeltaSeconds();
	if (DumpTimer > 0.5f)
	{
		DumpTimer = 0.f;
		TArray<FVIContact> C; if (Traffic) C = Traffic->GetContacts();
		FVISoftLink::WriteSnapshot(FVISoftLink::OwnshipJson(Ship->GetLatLon(), Ship->GetHeadingDeg(), Ship->GetCogDeg(), Ship->GetSogKn(), Ship->GetWaypointIndex(), Ship->GetStatusLine(), C));
	}
}
void AVIBridgeHUD::DrawConning(AVIShipPawn* Ship, float X, float Y)
{
	const FVILatLon P = Ship->GetLatLon();
	DrawText(FString::Printf(TEXT("LEVANTE  HDG %05.1f  COG %05.1f  SOG %04.1f kn\n%.5f N  %.5f E  %s  AP:%s"),
		Ship->GetHeadingDeg(), Ship->GetCogDeg(), Ship->GetSogKn(), P.Lat, P.Lon, *Ship->GetStatusLine(), Ship->IsAutopilot()?TEXT("ON"):TEXT("OFF")),
		FLinearColor(0.82f,0.93f,1.f), X, Y, nullptr, 1.15f);
}
void AVIBridgeHUD::DrawChart(AVIShipPawn* Ship, float X, float Y, float W, float H)
{
	DrawRect(FLinearColor(0.02f,0.07f,0.12f,0.78f), X, Y, W, H);
	DrawText(TEXT("CARTA · Valencia → Ibiza  (Riomares)"), FLinearColor(0.7f,0.9f,1.f), X+8, Y+6, nullptr, 1.f);
	const FVIVoyage& V = Ship->GetVoyage();
	if (V.Waypoints.Num()<2) return;
	double MinLat=90,MaxLat=-90,MinLon=180,MaxLon=-180;
	for (const FVIWaypoint& Wp : V.Waypoints) { MinLat=FMath::Min(MinLat,Wp.Pos.Lat); MaxLat=FMath::Max(MaxLat,Wp.Pos.Lat); MinLon=FMath::Min(MinLon,Wp.Pos.Lon); MaxLon=FMath::Max(MaxLon,Wp.Pos.Lon); }
	MinLat-=0.08; MaxLat+=0.08; MinLon-=0.12; MaxLon+=0.12;
	auto ToPx=[&](const FVILatLon& G){ return FVector2D(X+16.f+float((G.Lon-MinLon)/(MaxLon-MinLon))*(W-32.f), Y+H-24.f-float((G.Lat-MinLat)/(MaxLat-MinLat))*(H-48.f)); };
	for (int32 i=0;i<V.Waypoints.Num()-1;++i){ const FVector2D A=ToPx(V.Waypoints[i].Pos), B=ToPx(V.Waypoints[i+1].Pos); DrawLine(A.X,A.Y,B.X,B.Y,FLinearColor(0.95f,0.75f,0.15f,0.9f),2.f); }
	for (int32 i=0;i<V.Waypoints.Num();++i){ const FVector2D A=ToPx(V.Waypoints[i].Pos); DrawRect(i==Ship->GetWaypointIndex()?FLinearColor(1.f,0.4f,0.1f):FLinearColor(0.9f,0.9f,0.4f), A.X-3,A.Y-3,6,6); }
	const FVector2D Me=ToPx(Ship->GetLatLon()); DrawRect(FLinearColor(0.2f,1.f,0.45f), Me.X-4,Me.Y-4,8,8);
	const float Rad=FVIGeo::DegToRad(Ship->GetHeadingDeg()); DrawLine(Me.X,Me.Y,Me.X+FMath::Sin(Rad)*18.f,Me.Y-FMath::Cos(Rad)*18.f,FLinearColor(0.2f,1.f,0.45f),2.f);
}
void AVIBridgeHUD::DrawRadar(AVIShipPawn* Ship, AVITrafficDirector* Traffic, float X, float Y, float S)
{
	DrawRect(FLinearColor(0.01f,0.06f,0.03f,0.82f), X, Y, S, S);
	const FVector2D C(X+S*0.5f, Y+S*0.5f); const float R=S*0.42f;
	for (int32 k=1;k<=4;++k){ const float rr=R*k/4.f; FVector2D Prev(C.X,C.Y-rr); for(int32 s=1;s<=48;++s){ const float A=(2.f*PI*s)/48.f; const FVector2D Cur(C.X+FMath::Sin(A)*rr,C.Y-FMath::Cos(A)*rr); DrawLine(Prev.X,Prev.Y,Cur.X,Cur.Y,FLinearColor(0.15f,0.55f,0.25f,0.55f),1.f); Prev=Cur; } }
	DrawLine(C.X,C.Y-R,C.X,C.Y+R,FLinearColor(0.15f,0.55f,0.25f,0.4f),1.f);
	DrawLine(C.X-R,C.Y,C.X+R,C.Y,FLinearColor(0.15f,0.55f,0.25f,0.4f),1.f);
	DrawText(FString::Printf(TEXT("RADAR  %d NM  HM-up"), RadarRangeNm), FLinearColor(0.4f,1.f,0.5f), X+8, Y+6, nullptr, 1.f);
	const double RangeM = RadarRangeNm * 1852.0; const FVILatLon Own = Ship->GetLatLon();
	if (Traffic) for (const FVIContact& Ct : Traffic->GetContacts()) {
		const double Dist=Ct.RangeM(Own); if (Dist>RangeM) continue;
		const float Ang=FVIGeo::DegToRad(FVIGeo::DeltaHeadingDeg(Ship->GetHeadingDeg(), Ct.BearingDeg(Own)));
		const float U=float(Dist/RangeM)*R; const FVector2D P(C.X+FMath::Sin(Ang)*U, C.Y-FMath::Cos(Ang)*U);
		double Cpa=0,Tcpa=0; Ct.CpaTcpa(Own, Ship->GetCogDeg(), Ship->GetSogKn(), Cpa, Tcpa);
		DrawRect((Cpa<800.0 && Tcpa>0 && Tcpa<900.0)?FLinearColor(1.f,0.25f,0.1f):FLinearColor(0.3f,1.f,0.4f), P.X-3,P.Y-3,6,6);
	}
	DrawRect(FLinearColor(1.f,1.f,0.3f), C.X-3,C.Y-3,6,6);
}
void AVIBridgeHUD::DrawVhf(AVIShipPawn* Ship, float X, float Y, float W, float H)
{
	DrawRect(FLinearColor(0.08f,0.08f,0.06f,0.84f), X, Y, W, H);
	DrawText(FString::Printf(TEXT("VHF  CH %d   DSC listo"), VhfChannel), FLinearColor(0.95f,0.85f,0.3f), X+8, Y+6, nullptr, 1.1f);
	DrawText(TEXT("16 guardia · 13 puente-puente · 10 interbarco"), FLinearColor(0.7f,0.7f,0.55f), X+8, Y+28, nullptr, 0.9f);
	DrawText(LastVhf, FLinearColor(0.9f,0.95f,0.75f), X+8, Y+56, nullptr, 0.95f);
}
void AVIBridgeHUD::DrawHelp(float Y)
{
	DrawText(TEXT("J carta  K radar  L VHF  M panel  V PTT  N canal  Espacio AP  C cam  1-4 tiempo"), FLinearColor(0.65f,0.75f,0.8f), 24.f, Y, nullptr, 0.95f);
}
