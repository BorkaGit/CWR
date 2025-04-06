// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/CWRInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CWRInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UCWRInventoryItemDefinition

UCWRInventoryItemDefinition::UCWRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UCWRInventoryItemFragment* UCWRInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UCWRInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UCWRInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UCWRInventoryItemDefinition

const UCWRInventoryItemFragment* UCWRInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UCWRInventoryItemDefinition> ItemDef, TSubclassOf<UCWRInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UCWRInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
