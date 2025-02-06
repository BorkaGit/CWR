// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/CWREquipmentManagerComponent.h"

#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Equipment/CWREquipmentDefinition.h"
#include "Equipment/CWREquipmentInstance.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FCWRAppliedEquipmentEntry

FString FCWRAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FCWREquipmentList

void FCWREquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
 	for (int32 Index : RemovedIndices)
 	{
 		const FCWRAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
 	}
}

void FCWREquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FCWRAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FCWREquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
// 	for (int32 Index : ChangedIndices)
// 	{
// 		const FGameplayTagStack& Stack = Stacks[Index];
// 		TagToCountMap[Stack.Tag] = Stack.StackCount;
// 	}
}

UCWRAbilitySystemComponent* FCWREquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	const APawn* OwningPawn = OwnerComponent->GetOwner<APawn>();
	
	return Cast<UCWRAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(	OwningPawn->GetPlayerState()));
}

UCWREquipmentInstance* FCWREquipmentList::AddEntry(TSubclassOf<UCWREquipmentDefinition> EquipmentDefinition)
{
	UCWREquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UCWREquipmentDefinition* EquipmentCDO = GetDefault<UCWREquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UCWREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UCWREquipmentInstance::StaticClass();
	}
	
	FCWRAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UCWREquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (UCWRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		Result->GiveAbilitiesToHolder(ASC);
		for (const TObjectPtr<const UCWRAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void FCWREquipmentList::RemoveEntry(UCWREquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FCWRAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UCWRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
				Entry.Instance->RemoveAbilitiesFromHolder();
			}

			Instance->DestroyEquipmentActors();
			

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UCWREquipmentManagerComponent

UCWREquipmentManagerComponent::UCWREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UCWREquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UCWREquipmentInstance* UCWREquipmentManagerComponent::EquipItem(TSubclassOf<UCWREquipmentDefinition> EquipmentClass)
{
	UCWREquipmentInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UCWREquipmentManagerComponent::UnequipItem(UCWREquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

bool UCWREquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FCWRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UCWREquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UCWREquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UCWREquipmentManagerComponent::UninitializeComponent()
{
	TArray<UCWREquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FCWRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UCWREquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UCWREquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing CWREquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FCWRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UCWREquipmentInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UCWREquipmentInstance* UCWREquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UCWREquipmentInstance> InstanceType)
{
	for (FCWRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UCWREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UCWREquipmentInstance*> UCWREquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UCWREquipmentInstance> InstanceType) const
{
	TArray<UCWREquipmentInstance*> Results;
	for (const FCWRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UCWREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}


