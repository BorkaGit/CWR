// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include <string>


UInventoryComponent::UInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	
}



void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInventoryComponent::EquipeItem()
{
	
	if ( InventoryItems.Contains(CurrentItemSelection) )
	{
		// Convert int32 to FString
		FString NumberAsString = FString::FromInt(CurrentItemSelection);

		// Convert FString to FName
		FName NameFromInt = FName(*NumberAsString);
		FInventoryItem* Row = InventoryDataTable->FindRow<FInventoryItem>(NameFromInt, TEXT(""));
		if (Row)
		{
			
		}
	}
}


