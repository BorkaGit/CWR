// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CWRWeaponCustomizationItem.generated.h"

UENUM(BlueprintType)
enum class ECWRWeaponItemCategory : uint8
{
	None,
   Scope,
   Barrel,
   Magazine
};


UCLASS(Blueprintable)
class CWR_API UCWRWeaponCustomizationItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Item")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Item")
	ECWRWeaponItemCategory ItemCategory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Item")
	FTransform RelativeTransform;
	
};
