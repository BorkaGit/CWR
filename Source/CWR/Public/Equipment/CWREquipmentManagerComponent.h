// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/CWRAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "CWREquipmentManagerComponent.generated.h"

class UActorComponent;
class UCWRAbilitySystemComponent;
class UCWREquipmentDefinition;
class UCWREquipmentInstance;
class UCWREquipmentManagerComponent;
class UObject;
struct FFrame;
struct FCWREquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FCWRAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FCWRAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FCWREquipmentList;
	friend UCWREquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UCWREquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UCWREquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FCWRAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FCWREquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FCWREquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FCWREquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FCWRAppliedEquipmentEntry, FCWREquipmentList>(Entries, DeltaParms, *this);
	}

	UCWREquipmentInstance* AddEntry(TSubclassOf<UCWREquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UCWREquipmentInstance* Instance);

private:
	UCWRAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UCWREquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FCWRAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FCWREquipmentList> : public TStructOpsTypeTraitsBase2<FCWREquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class CWR_API UCWREquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UCWREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UCWREquipmentInstance* EquipItem(TSubclassOf<UCWREquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UCWREquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCWREquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UCWREquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UCWREquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UCWREquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FCWREquipmentList EquipmentList;
	
	
};
