// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/CWRGameplayAbility_FromEquipment.h"
#include "Equipment/CWREquipmentInstance.h"
#include "Inventory/CWRInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UCWRGameplayAbility_FromEquipment::UCWRGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UCWREquipmentInstance* UCWRGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UCWREquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UCWRInventoryItemInstance* UCWRGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UCWREquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UCWRInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UCWRGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
				{
					Context.AddError(NSLOCTEXT("CWR", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
					Result = EDataValidationResult::Invalid;
				}

	return Result;
}

#endif