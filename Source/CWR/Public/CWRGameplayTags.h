// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * CWR Gameplay Tags
 *
 * Singleton containing native Gameplay Tags
 */

struct FCWRGameplayTags
{
	public:
	
	static const FCWRGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();


	FGameplayTag Attributes_Base_Health;
	FGameplayTag Attributes_Base_MaxHealth;
	FGameplayTag Attributes_Base_Armor;
	FGameplayTag Attributes_Base_MaxArmor;
	FGameplayTag Attributes_Base_Stamina;
	FGameplayTag Attributes_Base_MaxStamina;

	FGameplayTag Abilities_Character_Jump;
	FGameplayTag Abilities_Character_Crouch;
	FGameplayTag Abilities_Character_Walk;
	FGameplayTag Abilities_Character_Sprint;
	FGameplayTag Abilities_Character_DropItem;
	
	FGameplayTag Abilities_RangedWeapon_Fire;	
	FGameplayTag Abilities_RangedWeapon_Reload;
	FGameplayTag Abilities_RangedWeapon_Aim;

	FGameplayTag Restriction_Jumping;
	FGameplayTag Restriction_Crouching;
	FGameplayTag Restriction_Walking;
	FGameplayTag Restriction_Sprinting;
	FGameplayTag Restriction_DroppingItem;
	FGameplayTag Restriction_Firing;
	FGameplayTag Restriction_Reloading;
	FGameplayTag Restriction_Aiming;

	FGameplayTag Event_Jumping;
	FGameplayTag Event_Crouching;
	FGameplayTag Event_Walking;
	FGameplayTag Event_Sprinting;
	FGameplayTag Event_DroppingItem;
	FGameplayTag Event_Firing;
	FGameplayTag Event_Reloading;
	FGameplayTag Event_Aiming;

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_R;
	FGameplayTag InputTag_Shift;
	FGameplayTag InputTag_Space;
	FGameplayTag InputTag_LAlt;
	FGameplayTag InputTag_LCtrl;
	FGameplayTag InputTag_G;
	FGameplayTag InputTag_QuickSlot_SelectSlot;
	FGameplayTag InputTag_QuickSlot_CycleForward;
	FGameplayTag InputTag_QuickSlot_CycleBackward;

	FGameplayTag Weapon_MagazineSize;
	FGameplayTag Weapon_MagazineAmmo;
	FGameplayTag Weapon_SpareAmmo;
	
	FGameplayTag GameplayCue_Weapon_Rifle_Fire;
	
private:
	static FCWRGameplayTags GameplayTags;
};