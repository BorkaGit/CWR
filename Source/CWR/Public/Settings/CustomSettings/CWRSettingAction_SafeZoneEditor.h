// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameSettingAction.h"
#include "GameSettingValueScalarDynamic.h"

#include "CWRSettingAction_SafeZoneEditor.generated.h"

class UGameSetting;
class UObject;

UCLASS()
class CWR_API UCWRSettingValueScalarDynamic_SafeZoneValue : public UGameSettingValueScalarDynamic
{
	GENERATED_BODY()
	
public:
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;
};

UCLASS()
class UCWRSettingAction_SafeZoneEditor : public UGameSettingAction
{
	GENERATED_BODY()
	
public:
	UCWRSettingAction_SafeZoneEditor();
	virtual TArray<UGameSetting*> GetChildSettings() override;

private:
	UPROPERTY()
	TObjectPtr<UCWRSettingValueScalarDynamic_SafeZoneValue> SafeZoneValueSetting;
};
