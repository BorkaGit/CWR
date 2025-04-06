// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/CWRInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UCWRReticleWidgetBase;
class UObject;

UCLASS()
class CWR_API UInventoryFragment_ReticleConfig : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UCWRReticleWidgetBase>> ReticleWidgets;
	
};
