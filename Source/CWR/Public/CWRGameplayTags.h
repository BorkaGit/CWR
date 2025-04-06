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

	FGameplayTag CWR_Player;
	FGameplayTag CWR_Elimination_Message;
	FGameplayTag CWR_Damage_Message;
	FGameplayTag CWR_Damage_Taken_Message;
	FGameplayTag CWR_Assist_Message;
	FGameplayTag CWR_AddNotification_KillFeed;

	FGameplayTag Gameplay_Message_Aiming;

	FGameplayTag Gameplay_Zone_Head;

	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;
	
	FGameplayTag GameplayEvent_Death;
	FGameplayTag GameplayEvent_Reset;
	FGameplayTag GameplayEvent_RequestReset;

	FGameplayTag GameplayEffect_DamageType_Basic;
	FGameplayTag GameplayEffect_DamageType_Grenade;
	FGameplayTag GameplayEffect_DamageType_Melee;
	FGameplayTag GameplayEffect_DamageType_Pistol;
	FGameplayTag GameplayEffect_DamageType_Rifle;
	FGameplayTag GameplayEffect_DamageType_Shotgun;
	FGameplayTag GameplayEffect_DamageTrait_Instant;
	FGameplayTag GameplayEffect_DamageTrait_Periodic;

	FGameplayTag GameplayCue_UserMessage_MatchDecided;
	FGameplayTag GameplayCue_UserMessage_WaitingForPlayers;
	FGameplayTag GameplayCue_Weapon_Rifle_Fire;
	FGameplayTag GameplayCue_Character_DamageTaken;
	
	FGameplayTag Attributes_Base_Health;
	FGameplayTag Attributes_Base_MaxHealth;
	FGameplayTag Attributes_Base_Armor;
	FGameplayTag Attributes_Base_MaxArmor;
	FGameplayTag Attributes_Base_Stamina;
	FGameplayTag Attributes_Base_MaxStamina;
	
	FGameplayTag Attributes_Meta_Damage;

	FGameplayTag Gameplay_Damage;
	FGameplayTag Gameplay_Damage_Immunity;
	FGameplayTag Gameplay_Damage_SelfDestruct;
	FGameplayTag Gameplay_Damage_FellOutOfWorld;

	FGameplayTag Gameplay_AbilityInputBlocked;
	
	FGameplayTag SetByCaller_Damage;
	FGameplayTag SetByCaller_Heal;
	
	FGameplayTag Ability_Behavior_SurvivesDeath;
	FGameplayTag Ability_Respawn_Completed_Message;
	FGameplayTag Ability_Respawn_Duration_Message;
	
	FGameplayTag Ability_Type;
	FGameplayTag Ability_Type_Action;
	FGameplayTag Ability_Type_Action_DropItem;
	FGameplayTag Ability_Type_Action_Grenade;
	FGameplayTag Ability_Type_Action_Jump;
	FGameplayTag Ability_Type_Action_Crouch;
	FGameplayTag Ability_Type_Action_Walk;
	FGameplayTag Ability_Type_Action_Sprint;
	FGameplayTag Ability_Type_Action_Melee;
	FGameplayTag Ability_Type_Action_Reload;
	FGameplayTag Ability_Type_Action_Aim;
	FGameplayTag Ability_Type_Action_WeaponFire;
	FGameplayTag Ability_Type_Info;
	FGameplayTag Ability_Type_Info_ShowLeaderboard;
	
	FGameplayTag Ability_Type_Passive;
	FGameplayTag Ability_Type_Passive_AutoReload;
	FGameplayTag Ability_Type_Passive_AutoRespawn;
	FGameplayTag Ability_Type_Passive_ChangeQuickbarSlot;
	
	FGameplayTag Ability_Type_StatusChange;
	FGameplayTag Ability_Type_StatusChange_Death;
	FGameplayTag Ability_Type_StatusChange_Spawning;

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
	FGameplayTag InputTag_Weapon_Fire;
	FGameplayTag InputTag_Weapon_FireAuto;
	FGameplayTag InputTag_Weapon_ChangeFireMode;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_Weapon_Reload;
	FGameplayTag InputTag_Shift;
	FGameplayTag InputTag_Space;
	FGameplayTag InputTag_LAlt;
	FGameplayTag InputTag_LCtrl;
	FGameplayTag InputTag_G;
	FGameplayTag InputTag_QuickSlot_SelectSlot;
	FGameplayTag InputTag_QuickSlot_CycleForward;
	FGameplayTag InputTag_QuickSlot_CycleBackward;
	FGameplayTag InputTag_E;
	FGameplayTag InputTag_Ability_ShowLeaderboard;

	FGameplayTag Status_Crouching;
	FGameplayTag Status_AutoRunning;
	FGameplayTag Status_Death;
	FGameplayTag Status_Death_Dying;
	FGameplayTag Status_Death_Dead;
	FGameplayTag Status_SpawningIn;
	
	FGameplayTag Weapon_MagazineSize;
	FGameplayTag Weapon_MagazineAmmo;
	FGameplayTag Weapon_SpareAmmo;

	FGameplayTag Weapon_FireMode_Single;
	FGameplayTag Weapon_FireMode_Auto;

	FGameplayTag UI_Layer_Game;
	FGameplayTag UI_Layer_GameMenu;
	FGameplayTag UI_Layer_Menu;
	FGameplayTag UI_Layer_Modal;
	
	FGameplayTag HUD_Slot_PerfStats_Graph;
	FGameplayTag HUD_Slot_PerfStats_Text;
	FGameplayTag HUD_Slot_LeftSideTouchRegion;
	FGameplayTag HUD_Slot_RightSideTouchRegion;
	FGameplayTag HUD_Slot_InfrequentAbilities;
	FGameplayTag HUD_Slot_Reticle;
	FGameplayTag HUD_Slot_TopAccolades;
	FGameplayTag HUD_Slot_ExtraEquipment;
	FGameplayTag HUD_Slot_Equipment;
	FGameplayTag HUD_Slot_LeftSideTouchInputs;
	FGameplayTag HUD_Slot_RightSideTouchInputs;
	FGameplayTag HUD_Slot_ModeStatus;
	FGameplayTag HUD_Slot_TeamScore;
	FGameplayTag HUD_Slot_EliminationFeed;

	FGameplayTag Cheat_GodMode;
	FGameplayTag Cheat_UnlimitedHealth;

	FGameplayTag Score_Assists;
	FGameplayTag Score_Deaths;
	FGameplayTag Score_Eliminations;

	FGameplayTag GamePhase_Playing;
	FGameplayTag GamePhase_PostGame;
	FGameplayTag GamePhase_Warmup;
	FGameplayTag GamePhase_MatchBeginCountdown;

	FGameplayTag TODO_GamemodeDamageImmunity;
	
private:
	static FCWRGameplayTags GameplayTags;
};