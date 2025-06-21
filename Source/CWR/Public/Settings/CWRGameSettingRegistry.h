// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DataSource/GameSettingDataSourceDynamic.h" // IWYU pragma: keep
#include "GameSettingRegistry.h"
#include "Settings/CWRSettingsLocal.h" // IWYU pragma: keep

#include "CWRGameSettingRegistry.generated.h"

class ULocalPlayer;
class UObject;

//--------------------------------------
// UCWRGameSettingRegistry
//--------------------------------------

class UGameSettingCollection;
class UCWRLocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogCWRGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UCWRLocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UCWRSettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UCWRLocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UCWRSettingsLocal, FunctionOrPropertyName)			\
	}))

/**
 * 
 */
UCLASS()
class UCWRGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()
	
public:
	UCWRGameSettingRegistry();

	static UCWRGameSettingRegistry* Get(UCWRLocalPlayer* InLocalPlayer);
	
	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	UGameSettingCollection* InitializeVideoSettings(UCWRLocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, UCWRLocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, UCWRLocalPlayer* InLocalPlayer);

	//UGameSettingCollection* InitializeAudioSettings(UCWRLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(UCWRLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(UCWRLocalPlayer* InLocalPlayer);
	
	UGameSettingCollection* InitializeGamepadSettings(UCWRLocalPlayer* InLocalPlayer);

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
