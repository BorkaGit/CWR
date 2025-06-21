// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/CWRInventoryItemDefinition.h"
#include "Inventory/CWRInventoryItemInstance.h"

#include "InventoryFragment_Attachments.generated.h"



UCLASS()
class CWR_API UInventoryFragment_Attachments : public UCWRInventoryItemFragment
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	TArray<FCWRAttachmentInfo> Attachments;
};
