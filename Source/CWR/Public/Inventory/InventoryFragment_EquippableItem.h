// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/CWRInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UCWREquipmentDefinition;
class UObject;

UCLASS()
class CWR_API UInventoryFragment_EquippableItem : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=CWR)
	TSubclassOf<UCWREquipmentDefinition> EquipmentDefinition;
	
	
};
