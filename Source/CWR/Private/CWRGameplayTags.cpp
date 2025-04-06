// Fill out your copyright notice in the Description page of Project Settings.


#include "CWRGameplayTags.h"
#include "GameplayTagsManager.h"

FCWRGameplayTags FCWRGameplayTags::GameplayTags;

void FCWRGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.CWR_Player									= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.Player"),									FString("Controlled by Player"));
	GameplayTags.CWR_Elimination_Message					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.Elimination.Message"),					FString("CWR Elimination Message"));
	GameplayTags.CWR_Damage_Message							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.Damage.Message"),							FString("CWR Damage Message"));
	GameplayTags.CWR_Damage_Taken_Message					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.Damage.Taken.Message"),					FString("CWR Damage Taken Message"));
	GameplayTags.CWR_Assist_Message							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.Assist.Message"),							FString("CWR Assist Message"));
	GameplayTags.CWR_AddNotification_KillFeed				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CWR.AddNotification.KillFeed"),				FString("SendKillFeedInfo to UI"));

	GameplayTags.Gameplay_Message_Aiming					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Message.Aiming"),					FString("Message to UI, Reticle"));

	GameplayTags.Gameplay_Zone_Head							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Zone.Head"),							FString("Gameplay Zone Head"));

	GameplayTags.InitState_Spawned							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InitState.Spawned"),							FString("Actor/component has initially spawned and can be extended"));
	GameplayTags.InitState_DataAvailable					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InitState.DataAvailable"),					FString("All required data has been loaded/replicated and is ready for initialization"));
	GameplayTags.InitState_DataInitialized					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InitState.DataInitialized"),					FString("The available data has been initialized for this actor/component, but it is not ready for full gameplay"));
	GameplayTags.InitState_GameplayReady					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InitState.GameplayReady"),					FString("The actor/component is fully ready for active gameplay"));
	
	GameplayTags.GameplayEvent_Death						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEvent.Death"),						FString("Event that fires on death. This event only fires on the server"));
	GameplayTags.GameplayEvent_Reset						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEvent.Reset"),						FString("Event that fires once a player reset is executed"));
	GameplayTags.GameplayEvent_RequestReset					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEvent.RequestReset"),					FString("Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location"));
	
	GameplayTags.GameplayEffect_DamageType_Basic			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Basic"),			FString("GameplayEffect DamageType Basic"));
	GameplayTags.GameplayEffect_DamageType_Grenade			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Grenade"),			FString("GameplayEffect DamageType Grenade"));
	GameplayTags.GameplayEffect_DamageType_Melee			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Melee"),			FString("GameplayEffect DamageType Melee"));
	GameplayTags.GameplayEffect_DamageType_Pistol			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Pistol"),			FString("GameplayEffect DamageType Pistol"));
	GameplayTags.GameplayEffect_DamageType_Rifle			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Rifle"),			FString("GameplayEffect DamageType Rifle"));
	GameplayTags.GameplayEffect_DamageType_Shotgun			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageType.Shotgun"),			FString("GameplayEffect DamageType Shotgun"));
	GameplayTags.GameplayEffect_DamageTrait_Instant			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageTrait.Instant"),			FString("GameplayEffect DamageTrait Instant"));
	GameplayTags.GameplayEffect_DamageTrait_Periodic		= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayEffect.DamageTrait.Periodic"),		FString("GameplayEffect DamageTrait Periodic"));

	GameplayTags.GameplayCue_UserMessage_MatchDecided		= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.UserMessage.MatchDecided"),		FString("GameplayCue UserMessage MatchDecided"));
	GameplayTags.GameplayCue_UserMessage_WaitingForPlayers	= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.UserMessage.WaitingForPlayers"),	FString("GameplayCue UserMessage WaitingForPlayers"));
	GameplayTags.GameplayCue_Weapon_Rifle_Fire				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Weapon.Rifle.Fire"),				FString("Gameplay Cue for Rifle"));
	GameplayTags.GameplayCue_Character_DamageTaken			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Character.DamageTaken"),			FString("GameplayCue Character DamageTaken"));
	
	GameplayTags.SetByCaller_Damage							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SetByCaller.Damage"),							FString("SetByCaller tag used by damage gameplay effects"));
	GameplayTags.SetByCaller_Heal							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SetByCaller.Heal"),							FString("SetByCaller tag used by healing gameplay effects"));
	
	GameplayTags.Attributes_Base_Health						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Health"),						FString("Player's Health"));
	GameplayTags.Attributes_Base_MaxHealth					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxHealth"),					FString("Player's Max Health"));
	GameplayTags.Attributes_Base_Armor						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Armor"),						FString("Player's Armor"));
	GameplayTags.Attributes_Base_MaxArmor					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxArmor"),					FString("Player's Max Armor"));
	GameplayTags.Attributes_Base_Stamina					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.Stamina"),					FString("Player's Stamina"));
	GameplayTags.Attributes_Base_MaxStamina					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Base.MaxStamina"),					FString("Player's Max Stamina"));
	
	GameplayTags.Attributes_Meta_Damage						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Meta.Damage"),						FString("Player's Meta Damage"));
	
	GameplayTags.Gameplay_Damage							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Damage"),							FString("Gameplay Damage"));
	GameplayTags.Gameplay_Damage_Immunity					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Damage.Immunity"),					FString("Gameplay Damage Immunity"));
	GameplayTags.Gameplay_Damage_SelfDestruct				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Damage.SelfDestruct"),				FString("Gameplay Damage SelfDestruct"));
	GameplayTags.Gameplay_Damage_FellOutOfWorld				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.Damage.FellOutOfWorld"),				FString("Gameplay Damage FellOutOfWorld"));

	GameplayTags.Gameplay_AbilityInputBlocked				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Gameplay.AbilityInputBlocked"),				FString("Gameplay AbilityInputBlocked"));
	
	GameplayTags.Ability_Behavior_SurvivesDeath				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Behavior.SurvivesDeath"),				FString("An ability with this type tag should not be canceled due to death"));
	GameplayTags.Ability_Respawn_Completed_Message			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Respawn.Completed.Message"),			FString("Message tag broadcast when a respawn process is completed"));
	GameplayTags.Ability_Respawn_Duration_Message			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Respawn.Duration.Message"),			FString("Message tag broadcast when a respawn is in process"));
	
	GameplayTags.Ability_Type								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type"),								FString("Applied to Gameplay Abilities to indicate a functional category"));
	GameplayTags.Ability_Type_Action						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action"),						FString("Action type abilities cause the avatar to perform an action in the game world, possibly involving equipment such as a weapon"));
	GameplayTags.Ability_Type_Action_DropItem				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.DropItem"),				FString(""));
	GameplayTags.Ability_Type_Action_Grenade				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Grenade"),				FString(""));
	GameplayTags.Ability_Type_Action_Jump					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Jump"),					FString(""));
	GameplayTags.Ability_Type_Action_Crouch					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Crouch"),					FString(""));
	GameplayTags.Ability_Type_Action_Walk					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Walk"),					FString(""));
	GameplayTags.Ability_Type_Action_Sprint					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Sprint"),					FString(""));
	GameplayTags.Ability_Type_Action_Melee					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Melee"),					FString(""));
	GameplayTags.Ability_Type_Action_Reload					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Reload"),					FString(""));
	GameplayTags.Ability_Type_Action_Aim					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.Aim"),					FString(""));
	GameplayTags.Ability_Type_Action_WeaponFire				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Action.WeaponFire"),				FString(""));
	GameplayTags.Ability_Type_Info							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Info"),							FString("Info abilities are used to summon information for the players, and do not directly affect gameplay"));
	GameplayTags.Ability_Type_Info_ShowLeaderboard			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Info.ShowLeaderboard"),			FString(""));

	GameplayTags.Ability_Type_Passive						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive"),						FString("Passive abilities are always active on the owner"));
	GameplayTags.Ability_Type_Passive_AutoReload			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive.AutoReload"),			FString("Polls for active weapon to be empty, triggers reload input if enough time has elapsed"));
	GameplayTags.Ability_Type_Passive_AutoRespawn			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive.AutoRespawn"),			FString("Passive ability that listens for a player\'s pawn to die or otherwise be removed from play, and automatically respawns"));
	GameplayTags.Ability_Type_Passive_ChangeQuickbarSlot	= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive.ChangeQuickbarSlot"),	FString("Handles requests for any type of slot change, whether directly indexed or cycling forward/backward"));
	
	GameplayTags.Ability_Type_StatusChange					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.StatusChange"),					FString("Abilities of this type are used when a status if imposed on the avatar or owner, such as death or spawning"));
	GameplayTags.Ability_Type_StatusChange_Death			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.StatusChange.Death"),			FString("Covers the transition period when health goes to 0 until the avatar is removed from the world"));
	GameplayTags.Ability_Type_StatusChange_Spawning			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.StatusChange.Spawning"),			FString("Covers the time when a new hero is created, spawning into the world, and when it\'s actively controllable"));

	GameplayTags.Restriction_Jumping						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Jumping"),						FString("Restriction Jump Ability"));
	GameplayTags.Restriction_Crouching						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Crouching"),						FString("Restriction Crouch Ability"));
	GameplayTags.Restriction_Walking						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Walking"),						FString("Restriction Walk Ability"));
	GameplayTags.Restriction_Sprinting						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Sprinting"),						FString("Restriction Sprint Ability"));
	GameplayTags.Restriction_DroppingItem					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.DroppingItem"),					FString("Restriction DroppingItem Ability"));
	GameplayTags.Restriction_Firing							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Firing"),							FString("Restriction Fire Ability"));
	GameplayTags.Restriction_Reloading						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Reloading"),						FString("Restriction Reload Ability"));
	GameplayTags.Restriction_Aiming							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Restriction.Aiming"),								FString("Restriction Aim Ability"));
	
	GameplayTags.Event_Jumping								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Jumping"),								FString("Jump Ability activated"));
	GameplayTags.Event_Crouching							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Crouching"),							FString("Crouch Ability activated"));
	GameplayTags.Event_Walking								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Walking"),								FString("Walk Ability activated"));
	GameplayTags.Event_Sprinting							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Sprinting"),							FString("Sprint Ability activated"));
	GameplayTags.Event_DroppingItem							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.DroppingItem"),							FString("DroppingItem Ability activated"));
	GameplayTags.Event_Firing								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Firing"),								FString("Fire Ability activated"));
	GameplayTags.Event_Reloading							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Reloading"),							FString("Reload Ability activated"));
	GameplayTags.Event_Aiming								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Aiming"),								FString("Aim Ability activated"));
	
	GameplayTags.InputTag_Move								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Move"),								FString("Input Tag Move"));
	GameplayTags.InputTag_Look_Mouse						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Look.Mouse"),						FString("Input Tag Look Mouse"));
	GameplayTags.InputTag_Weapon_Fire						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Fire"),						FString("Input Tag Fire"));
	GameplayTags.InputTag_Weapon_FireAuto					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.FireAuto"),					FString("Input Tag FireAuto"));
	GameplayTags.InputTag_Weapon_ChangeFireMode				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.ChangeFireMode"),				FString("InputTag Weapon ChangeFireMode"));
	GameplayTags.InputTag_RMB								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"),								FString("Input Tag RMB"));
	GameplayTags.InputTag_Weapon_Reload						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Reload"),						FString("InputTag Weapon Reload"));
	GameplayTags.InputTag_Shift								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Shift"),								FString("Input Tag Shift"));
	GameplayTags.InputTag_Space								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Space"),								FString("Input Tag Space"));
	GameplayTags.InputTag_LAlt								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LAlt"),								FString("Input Tag LAlt"));
	GameplayTags.InputTag_LCtrl								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LCtrl"),								FString("Input Tag LCtrl"));
	GameplayTags.InputTag_G									= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.G"),									FString("Input Tag G"));
	GameplayTags.InputTag_QuickSlot_SelectSlot				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.SelectSlot"),				FString("Input Tag QuickSlot SelectSlot"));
	GameplayTags.InputTag_QuickSlot_CycleForward			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.CycleForward"),			FString("Input Tag QuickSlot CycleForward"));
	GameplayTags.InputTag_QuickSlot_CycleBackward			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.QuickSlot.CycleBackward"),			FString("Input Tag QuickSlot CycleBackward"));
	GameplayTags.InputTag_E									= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.E"),									FString("Input Tag QuickSlot CycleBackward"));
	GameplayTags.InputTag_Ability_ShowLeaderboard			= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Ability.ShowLeaderboard"),			FString("InputTag Ability ShowLeaderboard"));
	
	GameplayTags.Status_Crouching							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.Crouching"),							FString("Target is crouching"));
	GameplayTags.Status_AutoRunning							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.AutoRunning"),							FString("Target is auto-running"));
	GameplayTags.Status_Death								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.Death"),								FString("Target has the death status"));
	GameplayTags.Status_Death_Dying							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.Death.Dying"),							FString("Target has begun the death process."));
	GameplayTags.Status_Death_Dead							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.Death.Dead"),							FString("Target has finished the death process"));
	GameplayTags.Status_SpawningIn							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Status.SpawningIn"),							FString("Covers the time when the pawn is created until it is controlled and unlocked for play"));

	GameplayTags.Weapon_MagazineSize						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.MagazineSize"),						FString("Weapon Magazine Size"));
	GameplayTags.Weapon_MagazineAmmo						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.MagazineAmmo"),						FString("Weapon Magazine Ammo"));
	GameplayTags.Weapon_SpareAmmo							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.SpareAmmo"),							FString("Weapon Spare Ammo"));

	GameplayTags.Weapon_FireMode_Single		   				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.FireMode.Single"),						FString("Weapon FireMode Single"));
	GameplayTags.Weapon_FireMode_Auto		   				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Weapon.FireMode.Auto"),						FString("Weapon FireMode Auto"));
	
	GameplayTags.UI_Layer_Game								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Game"),								FString("UI Layer Game"));
	GameplayTags.UI_Layer_GameMenu							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameMenu"),							FString("UI Layer GameMenu"));
	GameplayTags.UI_Layer_Menu								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Menu"),								FString("UI Layer Menu"));
	GameplayTags.UI_Layer_Modal								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Modal"),								FString("UI Layer Modal"));
	
	GameplayTags.HUD_Slot_PerfStats_Graph					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.PerfStats.Graph"),					FString("HUD Slot PerfStats Graph"));
	GameplayTags.HUD_Slot_PerfStats_Text					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.PerfStats.Text"),					FString("HUD Slot PerfStats Text"));
	GameplayTags.HUD_Slot_LeftSideTouchRegion				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.LeftSideTouchRegion"),				FString("HUD Slot LeftSideTouchRegion"));
	GameplayTags.HUD_Slot_RightSideTouchRegion				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.RightSideTouchRegion"),				FString("HUD Slot RightSideTouchRegion"));
	GameplayTags.HUD_Slot_InfrequentAbilities				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.InfrequentAbilities"),				FString("HUD Slot InfrequentAbilities"));
	GameplayTags.HUD_Slot_Reticle							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.Reticle"),							FString("HUD Slot Reticle"));
	GameplayTags.HUD_Slot_TopAccolades						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.TopAccolades"),						FString("HUD Slot TopAccolades"));
	GameplayTags.HUD_Slot_ExtraEquipment					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.ExtraEquipment"),					FString("HUD Slot ExtraEquipment"));
	GameplayTags.HUD_Slot_Equipment			 				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.Equipment"),							FString("HUD Slot Equipment"));
	GameplayTags.HUD_Slot_LeftSideTouchInputs				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.LeftSideTouchInputs"),				FString("HUD Slot LeftSideTouchInputs"));
	GameplayTags.HUD_Slot_RightSideTouchInputs				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.RightSideTouchInputs"),				FString("HUD Slot RightSideTouchInputs"));
	GameplayTags.HUD_Slot_ModeStatus						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.ModeStatus"),						FString("HUD Slot ModeStatus"));
	GameplayTags.HUD_Slot_TeamScore							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.TeamScore"),							FString("HUD Slot TeamScore"));
	GameplayTags.HUD_Slot_EliminationFeed					= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("HUD.Slot.EliminationFeed"),					FString("HUD Slot EliminationFeed"));
	
	GameplayTags.Cheat_GodMode								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cheat.GodMode"),								FString("GodMode cheat is active on the owner"));
	GameplayTags.Cheat_UnlimitedHealth						= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cheat.UnlimitedHealth"),						FString("UnlimitedHealth cheat is active on the owner"));
	
	GameplayTags.Score_Assists								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Score.Assists"),								FString("Score Assists"));
	GameplayTags.Score_Deaths								= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Score.Deaths"),								FString("Score Deaths"));
	GameplayTags.Score_Eliminations							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Score.Eliminations"),							FString("Score Eliminations"));
	
	GameplayTags.GamePhase_Playing							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GamePhase.Playing"),							FString("GamePhase Playing"));
	GameplayTags.GamePhase_PostGame							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GamePhase.PostGame"),							FString("GamePhase PostGame"));
	GameplayTags.GamePhase_Warmup							= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GamePhase.Warmup"),							FString("GamePhase Warmup"));
	GameplayTags.GamePhase_MatchBeginCountdown				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GamePhase.MatchBeginCountdown"),				FString("When this tag is used in a gameplay message, an expected duration is included in the payload"));

	GameplayTags.TODO_GamemodeDamageImmunity				= UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TODO.GamemodeDamageImmunity"),				FString("TODO GamemodeDamageImmunity"));
	
}
