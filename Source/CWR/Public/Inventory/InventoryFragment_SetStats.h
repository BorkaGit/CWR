// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/CWRInventoryItemDefinition.h"

#include "InventoryFragment_SetStats.generated.h"

class UCWRInventoryItemInstance;
class UObject;
struct FGameplayTag;

UCLASS()
class CWR_API UInventoryFragment_SetStats : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UCWRInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
	
	
};
