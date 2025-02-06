// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CWRAbilityCost_ItemTagStack.h"
#include "NativeGameplayTags.h"
#include "Equipment/CWRGameplayAbility_FromEquipment.h"
#include "Inventory/CWRInventoryItemInstance.h"
#include "Weapons/CWRGameplayAbility_RangedWeapon.h"
#include "Weapons/CWRRangedWeaponInstance.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

UCWRAbilityCost_ItemTagStack::UCWRAbilityCost_ItemTagStack()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UCWRAbilityCost_ItemTagStack::CheckCost(const UCWRGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	
	if (const auto* EquipmentAbility = Cast<const UCWRGameplayAbility_FromEquipment>(Ability))
	{
		if (const UCWRInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// Inform other abilities why this cost cannot be applied
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);				
			}
			return bCanApplyCost;
		}
	}
	return false;

	/*if (const auto RangedWeaponAbility = Cast<const UCWRGameplayAbility_RangedWeapon>(Ability))
	{
		if (const UCWRRangedWeaponInstance* WeaponInstance = RangedWeaponAbility->GetWeaponInstance())
		{
			const bool bCanApplyCost = WeaponInstance->GetBulletsAmount() > 0;

			// Inform other abilities why this cost cannot be applied
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);				
			}
			return bCanApplyCost;
		}
	} 
	return false;*/
}

void UCWRAbilityCost_ItemTagStack::ApplyCost(const UCWRGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
	if (ActorInfo->IsNetAuthority())
	{
		if (const auto* EquipmentAbility = Cast<const UCWRGameplayAbility_FromEquipment>(Ability))
		{
			if (UCWRInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				ItemInstance->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}

	if (ActorInfo->IsNetAuthority())
	{
		if (const auto RangedWeaponAbility = Cast<const UCWRGameplayAbility_RangedWeapon>(Ability))
		{
			if (UCWRRangedWeaponInstance* WeaponInstance = RangedWeaponAbility->GetWeaponInstance())
			{
				WeaponInstance->DecrementBulletsAmount();
			}
		}
	}
}
