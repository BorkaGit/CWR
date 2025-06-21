// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/CustomSettings/CWRSettingAction_SafeZoneEditor.h"

#include "DataSource/GameSettingDataSource.h"
#include "Player/CWRLocalPlayer.h"
#include "Settings/CWRGameSettingRegistry.h"
#include "Widgets/Layout/SSafeZone.h"

class UGameSetting;

#define LOCTEXT_NAMESPACE "CWRSettings"

UCWRSettingAction_SafeZoneEditor::UCWRSettingAction_SafeZoneEditor()
{
	SafeZoneValueSetting = NewObject<UCWRSettingValueScalarDynamic_SafeZoneValue>();
	SafeZoneValueSetting->SetDevName(TEXT("SafeZoneValue"));
	SafeZoneValueSetting->SetDisplayName(LOCTEXT("SafeZoneValue_Name", "Safe Zone Value"));
	SafeZoneValueSetting->SetDescriptionRichText(LOCTEXT("SafeZoneValue_Description", "The safezone area percentage."));
	SafeZoneValueSetting->SetDefaultValue(0.0f);
	SafeZoneValueSetting->SetDynamicGetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(GetSafeZone));
	SafeZoneValueSetting->SetDynamicSetter(GET_LOCAL_SETTINGS_FUNCTION_PATH(SetSafeZone));
	SafeZoneValueSetting->SetDisplayFormat([](double SourceValue, double NormalizedValue){ return FText::AsNumber(SourceValue); });
	SafeZoneValueSetting->SetSettingParent(this);
}

TArray<UGameSetting*> UCWRSettingAction_SafeZoneEditor::GetChildSettings()
{
	return { SafeZoneValueSetting };
}

void UCWRSettingValueScalarDynamic_SafeZoneValue::ResetToDefault()
{
	Super::ResetToDefault();
	SSafeZone::SetGlobalSafeZoneScale(TOptional<float>(DefaultValue.Get(0.0f)));
}

void UCWRSettingValueScalarDynamic_SafeZoneValue::RestoreToInitial()
{
	Super::RestoreToInitial();
	SSafeZone::SetGlobalSafeZoneScale(TOptional<float>(InitialValue));
}

#undef LOCTEXT_NAMESPACE


