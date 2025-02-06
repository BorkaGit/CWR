// Fill out your copyright notice in the Description page of Project Settings.


#include "CWRGameplayTags.h"
#include "GameplayTagsManager.h"

FCWRGameplayTags FCWRGameplayTags::GameplayTags;

void FCWRGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_Base_Health			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Health"),			FString("Player's Health"));
	GameplayTags.Attributes_Base_MaxHealth		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxHealth"),		FString("Player's Max Health"));
	GameplayTags.Attributes_Base_Armor			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Armor"),			FString("Player's Armor"));
	GameplayTags.Attributes_Base_MaxArmor		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxArmor"),			FString("Player's Max Armor"));
	GameplayTags.Attributes_Base_Stamina		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Stamina"),			FString("Player's Stamina"));
	GameplayTags.Attributes_Base_MaxStamina		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxStamina"),		FString("Player's Max Stamina"));
	
	GameplayTags.Abilities_Character_Jump		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Character.Jump"),			FString("Jump Character Ability"));
	GameplayTags.Abilities_Character_Crouch		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Character.Crouch"),		FString("Crouch Character Ability"));
	GameplayTags.Abilities_Character_Walk		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Character.Walk"),			FString("Walk Character Ability"));
	GameplayTags.Abilities_Character_Sprint		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Character.Sprint"),		FString("Sprint Character Ability"));
	GameplayTags.Abilities_Character_DropItem	  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Character.DropItem"),		FString("DropItem Character Ability"));
	
	GameplayTags.Abilities_RangedWeapon_Fire	  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.RangedWeapon.Fire"),		FString("Ranged Weapon Fire Ability"));
	GameplayTags.Abilities_RangedWeapon_Reload	  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.RangedWeapon.Reload"),	FString("Ranged Weapon Reload Ability"));
	GameplayTags.Abilities_RangedWeapon_Aim		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.RangedWeapon.Aim"),		FString("Ranged Weapon Aim Ability"));

	GameplayTags.Restriction_Jumping			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Jumping"),				FString("Restriction Jump Ability"));
	GameplayTags.Restriction_Crouching			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Crouching"),			FString("Restriction Crouch Ability"));
	GameplayTags.Restriction_Walking			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Walking"),				FString("Restriction Walk Ability"));
	GameplayTags.Restriction_Sprinting			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Sprinting"),			FString("Restriction Sprint Ability"));
	GameplayTags.Restriction_DroppingItem		  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.DroppingItem"),			FString("Restriction DroppingItem Ability"));
	GameplayTags.Restriction_Firing				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Firing"),				FString("Restriction Fire Ability"));
	GameplayTags.Restriction_Reloading			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Reloading"),			FString("Restriction Reload Ability"));
	GameplayTags.Restriction_Aiming				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Aiming"),				FString("Restriction Aim Ability"));
	
	GameplayTags.Event_Jumping					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Jumping"),					FString("Jump Ability activated"));
	GameplayTags.Event_Crouching				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Crouching"),					FString("Crouch Ability activated"));
	GameplayTags.Event_Walking					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Walking"),					FString("Walk Ability activated"));
	GameplayTags.Event_Sprinting				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Sprinting"),					FString("Sprint Ability activated"));
	GameplayTags.Event_DroppingItem				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.DroppingItem"),				FString("DroppingItem Ability activated"));
	GameplayTags.Event_Firing					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Firing"),						FString("Fire Ability activated"));
	GameplayTags.Event_Reloading				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Reloading"),					FString("Reload Ability activated"));
	GameplayTags.Event_Aiming					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Aiming"),						FString("Aim Ability activated"));

	GameplayTags.InputTag_Move					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Move"),					FString("Input Tag Move"));
	GameplayTags.InputTag_Look_Mouse			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Look.Mouse"),				FString("Input Tag Look Mouse"));
	GameplayTags.InputTag_LMB					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"),						FString("Input Tag LMB"));
	GameplayTags.InputTag_RMB					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"),						FString("Input Tag RMB"));
	GameplayTags.InputTag_R						  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.R"),						FString("Input Tag R"));
	GameplayTags.InputTag_Shift					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Shift"),					FString("Input Tag Shift"));
	GameplayTags.InputTag_Space					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Space"),					FString("Input Tag Space"));
	GameplayTags.InputTag_LAlt					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LAlt"),					FString("Input Tag LAlt"));
	GameplayTags.InputTag_LCtrl					  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LCtrl"),					FString("Input Tag LCtrl"));
	GameplayTags.InputTag_G						  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.G"),						FString("Input Tag G"));
	GameplayTags.InputTag_QuickSlot_SelectSlot	  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.SelectSlot"),	FString("Input Tag QuickSlot SelectSlot"));
	GameplayTags.InputTag_QuickSlot_CycleForward  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.CycleForward"),	FString("Input Tag QuickSlot CycleForward"));
	GameplayTags.InputTag_QuickSlot_CycleBackward = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.CycleBackward"),	FString("Input Tag QuickSlot CycleBackward"));

	GameplayTags.Weapon_MagazineSize			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.MagazineSize"),				FString("Weapon Magazine Size"));
	GameplayTags.Weapon_MagazineAmmo			  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.MagazineAmmo"),				FString("Weapon Magazine Ammo"));
	GameplayTags.Weapon_SpareAmmo				  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.SpareAmmo"),					FString("Weapon Spare Ammo"));
	
	GameplayTags.GameplayCue_Weapon_Rifle_Fire	  = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Weapon.Rifle.Fire"),	FString("Gameplay Cue for Rifle"));
	
}
