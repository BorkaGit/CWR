// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/InventoryFragment_CustomizationTree.h"


TArray<FCWRCustomizationLevel> UInventoryFragment_CustomizationTree::GetCustomizationLevels() const
{
	TArray<FCWRCustomizationLevel> Result;
	
	TArray<FCWRCustomizationLevel*> Rows;
	CustomizationTree->GetAllRows(TEXT("Context"), Rows);

	for (const FCWRCustomizationLevel* RowPtr : Rows)
	{
		if (RowPtr)
		{
			Result.Add(*RowPtr);
		}
	}
	return Result;
}

FCWRAttachmentDetails UInventoryFragment_CustomizationTree::GetAttachmentDetails(const FDataTableRowHandle Attachment)
{
	if ( FCWRAttachmentDetails* DataRow = Attachment.GetRow<FCWRAttachmentDetails>(TEXT("Attachment Context")) )
	{
		return *DataRow;
	}

	UE_LOG(LogTemp, Warning, TEXT("Attachment row not found in %s"), *FString(__FUNCTION__));

	return FCWRAttachmentDetails();
}
