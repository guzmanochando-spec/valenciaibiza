#include "VIFalMedia.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Misc/Paths.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

FString FVIFalMedia::GeneratedDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Content/FalGenerated"));
}

UTexture2D* FVIFalMedia::LoadPng(const TCHAR* Stem)
{
	const FString Path = GeneratedDir() / FString::Printf(TEXT("%s.png"), Stem);
	if (!FPaths::FileExists(Path))
	{
		return nullptr;
	}
	return FImageUtils::ImportFileAsTexture2D(Path);
}

void FVIFalMedia::ApplyIfPresent(UStaticMeshComponent* Comp, const TCHAR* Stem)
{
	if (!Comp)
	{
		return;
	}
	UTexture2D* Tex = LoadPng(Stem);
	if (!Tex)
	{
		return;
	}

	UMaterialInterface* Base = Comp->GetMaterial(0);
	if (!Base)
	{
		return;
	}
	UMaterialInstanceDynamic* Mid = Cast<UMaterialInstanceDynamic>(Base);
	if (!Mid)
	{
		Mid = UMaterialInstanceDynamic::Create(Base, Comp);
		Comp->SetMaterial(0, Mid);
	}
	Mid->SetTextureParameterValue(TEXT("Texture"), Tex);
	Mid->SetTextureParameterValue(TEXT("BaseColor"), Tex);
	Mid->SetTextureParameterValue(TEXT("Diffuse"), Tex);
}
