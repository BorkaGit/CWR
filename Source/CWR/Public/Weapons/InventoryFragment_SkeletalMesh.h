// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/CWRInventoryItemDefinition.h"

#include "InventoryFragment_SkeletalMesh.generated.h"


UCLASS()
class CWR_API UInventoryFragment_SkeletalMesh : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;
};
