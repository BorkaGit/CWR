// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/CWRInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
//#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Inventory/CWRInventoryItemDefinition.h"
#include "Inventory/CWRInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(CWRInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_CWR_Inventory_Message_StackChanged, "CWR.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FCWRInventoryEntry

FString FCWRInventoryEntry::GetDebugString() const
{
	TSubclassOf<UCWRInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FCWRInventoryList

void FCWRInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FCWRInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FCWRInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FCWRInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FCWRInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FCWRInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FCWRInventoryList::BroadcastChangeMessage(FCWRInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FCWRInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	//UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	//MessageSystem.BroadcastMessage(TAG_CWR_Inventory_Message_StackChanged, Message);
}

UCWRInventoryItemInstance* FCWRInventoryList::AddEntry(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UCWRInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FCWRInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UCWRInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UCWRInventoryItemFragment* Fragment : GetDefault<UCWRInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UCWRInventoryItemDefinition* ItemCDO = GetDefault<UCWRInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FCWRInventoryList::AddEntry(UCWRInventoryItemInstance* Instance)
{
	unimplemented();
}

void FCWRInventoryList::RemoveEntry(UCWRInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FCWRInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UCWRInventoryItemInstance*> FCWRInventoryList::GetAllItems() const
{
	TArray<UCWRInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FCWRInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UCWRInventoryManagerComponent

UCWRInventoryManagerComponent::UCWRInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UCWRInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UCWRInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UCWRInventoryItemInstance* UCWRInventoryManagerComponent::AddItemDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UCWRInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UCWRInventoryManagerComponent::AddItemInstance(UCWRInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UCWRInventoryManagerComponent::RemoveItemInstance(UCWRInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UCWRInventoryItemInstance*> UCWRInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UCWRInventoryItemInstance* UCWRInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef) const
{
	for (const FCWRInventoryEntry& Entry : InventoryList.Entries)
	{
		UCWRInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UCWRInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FCWRInventoryEntry& Entry : InventoryList.Entries)
	{
		UCWRInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UCWRInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UCWRInventoryItemInstance* Instance = UCWRInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UCWRInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UCWRInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FCWRInventoryEntry& Entry : InventoryList.Entries)
		{
			UCWRInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UCWRInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FCWRInventoryEntry& Entry : InventoryList.Entries)
	{
		UCWRInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UCWRInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UCWRInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UCWRInventoryFilter_HasTag : public UCWRInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UCWRInventoryItemInstance* Instance) const { return true; }
// };

