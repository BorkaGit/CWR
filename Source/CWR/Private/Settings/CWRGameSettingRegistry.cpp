// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/CWRGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "Settings/CWRSettingsLocal.h"
#include "Settings/CWRSettingsShared.h"
#include "Player/CWRLocalPlayer.h"

DEFINE_LOG_CATEGORY(LogCWRGameSettingRegistry);

#define LOCTEXT_NAMESPACE "CWR"

//--------------------------------------
// UCWRGameSettingRegistry
//--------------------------------------

UCWRGameSettingRegistry::UCWRGameSettingRegistry()
{
}

UCWRGameSettingRegistry* UCWRGameSettingRegistry::Get(UCWRLocalPlayer* InLocalPlayer)
{
	UCWRGameSettingRegistry* Registry = FindObject<UCWRGameSettingRegistry>(InLocalPlayer, TEXT("CWRGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UCWRGameSettingRegistry>(InLocalPlayer, TEXT("CWRGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UCWRGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UCWRLocalPlayer* LocalPlayer = Cast<UCWRLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UCWRGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UCWRLocalPlayer* CWRLocalPlayer = Cast<UCWRLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(CWRLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, CWRLocalPlayer);
	RegisterSetting(VideoSettings);

/*	AudioSettings = InitializeAudioSettings(CWRLocalPlayer);
	RegisterSetting(AudioSettings);*/

	GameplaySettings = InitializeGameplaySettings(CWRLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(CWRLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(CWRLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UCWRGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (UCWRLocalPlayer* LocalPlayer = Cast<UCWRLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE


