// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/CWRInventoryItemDefinition.h"
#include "Styling/SlateBrush.h"

#include "InventoryFragment_MenuIcon.generated.h"


UCLASS()
class CWR_API UInventoryFragment_MenuIcon : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush Brush;
};
