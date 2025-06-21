// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CWRAttachmentDetails.h"
#include "Inventory/CWRInventoryItemDefinition.h"
#include "InventoryFragment_CustomizationTree.generated.h"

USTRUCT(BlueprintType)
struct FCWRCustomizationLevel : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(RowType="/Script/CWR.CWRAttachmentDetails"))
	TArray<FDataTableRowHandle> Attachments;
};

UCLASS()
class CWR_API UInventoryFragment_CustomizationTree : public UCWRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta=(RequiredAssetDataTags = "RowStructure=/Script/CWR.CWRCustomizationLevel"))
	TObjectPtr<UDataTable> CustomizationTree;
	
	UFUNCTION(BlueprintCallable)
	TArray<FCWRCustomizationLevel>  GetCustomizationLevels() const;

	UFUNCTION(BlueprintCallable)
	static FCWRAttachmentDetails GetAttachmentDetails(const FDataTableRowHandle Attachment);
};
