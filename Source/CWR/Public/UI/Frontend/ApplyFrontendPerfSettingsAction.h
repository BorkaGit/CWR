// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFeatureAction.h"

#include "ApplyFrontendPerfSettingsAction.generated.h"

//////////////////////////////////////////////////////////////////////
// UApplyFrontendPerfSettingsAction

/**
 * GameFeatureAction responsible for telling the user settings to apply frontend/menu specific performance settings
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Use Frontend Perf Settings"))
class UApplyFrontendPerfSettingsAction final : public UGameFeatureAction
{
	GENERATED_BODY()
	
public:
	//~UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~End of UGameFeatureAction interface

private:
	static int32 ApplicationCounter;
};
