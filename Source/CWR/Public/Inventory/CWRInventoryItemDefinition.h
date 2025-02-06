// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CWRInventoryItemDefinition.generated.h"

class UCWRInventoryItemInstance;
struct FFrame;


// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UCWRInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UCWRInventoryItemInstance* Instance) const {}
};

/**
 * UCWRInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class CWR_API UCWRInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UCWRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UCWRInventoryItemFragment>> Fragments;

public:
	const UCWRInventoryItemFragment* FindFragmentByClass(TSubclassOf<UCWRInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UCWRInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UCWRInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, TSubclassOf<UCWRInventoryItemFragment> FragmentClass);
};
