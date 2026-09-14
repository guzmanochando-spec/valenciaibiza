#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"

/** Runtime primitives from Engine/BasicShapes — no cooked content required. */
struct FVIShapeKit
{
	static UStaticMesh* Mesh(const TCHAR* ShortName)
	{
		const FString Path = FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), ShortName, ShortName);
		return LoadObject<UStaticMesh>(nullptr, *Path);
	}

	static UStaticMesh* Cube() { return Mesh(TEXT("Cube")); }
	static UStaticMesh* Plane() { return Mesh(TEXT("Plane")); }
	static UStaticMesh* Cylinder() { return Mesh(TEXT("Cylinder")); }
	static UStaticMesh* Sphere() { return Mesh(TEXT("Sphere")); }
	static UStaticMesh* Cone() { return Mesh(TEXT("Cone")); }

	static UStaticMeshComponent* Attach(
		AActor* Owner,
		USceneComponent* Parent,
		UStaticMesh* MeshAsset,
		const FVector& RelLoc,
		const FRotator& RelRot,
		const FVector& Scale,
		const FLinearColor& Color,
		const FName& Name,
		bool bTranslucent = false)
	{
		if (!Owner || !MeshAsset)
		{
			return nullptr;
		}

		UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(Owner, Name);
		Comp->SetStaticMesh(MeshAsset);
		Comp->SetRelativeLocation(RelLoc);
		Comp->SetRelativeRotation(RelRot);
		Comp->SetRelativeScale3D(Scale);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetCastShadow(true);
		Comp->SetupAttachment(Parent);
		Comp->RegisterComponent();

		if (UMaterialInterface* Base = MeshAsset->GetMaterial(0))
		{
			if (UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Base, Owner))
			{
				Mid->SetVectorParameterValue(TEXT("Color"), Color);
				Mid->SetVectorParameterValue(TEXT("BaseColor"), Color);
				Comp->SetMaterial(0, Mid);
			}
		}

		Comp->SetCustomPrimitiveDataVector4(0, FVector4(Color.R, Color.G, Color.B, Color.A));
		if (bTranslucent)
		{
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		return Comp;
	}
};
