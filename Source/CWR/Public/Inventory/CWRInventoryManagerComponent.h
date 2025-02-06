// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "CWRInventoryManagerComponent.generated.h"

class UCWRInventoryItemDefinition;
class UCWRInventoryItemInstance;
class UCWRInventoryManagerComponent;
class UObject;
struct FFrame;
struct FCWRInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FCWRInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UCWRInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FCWRInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FCWRInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FCWRInventoryList;
	friend UCWRInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UCWRInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FCWRInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FCWRInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FCWRInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UCWRInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FCWRInventoryEntry, FCWRInventoryList>(Entries, DeltaParms, *this);
	}

	UCWRInventoryItemInstance* AddEntry(TSubclassOf<UCWRInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UCWRInventoryItemInstance* Instance);

	void RemoveEntry(UCWRInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FCWRInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UCWRInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FCWRInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FCWRInventoryList> : public TStructOpsTypeTraitsBase2<FCWRInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS( BlueprintType )
class CWR_API UCWRInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCWRInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UCWRInventoryItemInstance* AddItemDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UCWRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UCWRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UCWRInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UCWRInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FCWRInventoryList InventoryList;
};
