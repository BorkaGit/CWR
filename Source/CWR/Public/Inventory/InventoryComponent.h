// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UENUM()
enum class EItemType: uint8
{
	PrimaryWeapon,
	
	SecondaryWeapon,
};

USTRUCT()
struct FInventoryItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType Type = EItemType::PrimaryWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PickupClass;

	
};

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CWR_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInventoryComponent();
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable", meta = (RowType ="InventoryItem"))
	FDataTableRowHandle InventoryItemRowHandle;

	UPROPERTY(EditAnywhere, Category = "DataTable", meta = (RequiredAssetDataTags = "RowStructure=InventoryItem"))
	TObjectPtr<UDataTable> InventoryDataTable;
	

	UFUNCTION(Blueprintable)
	void EquipeItem();
	
private:	

	int32 CurrentItemSelection = 0;
	
	TArray<int32> InventoryItems;
};
