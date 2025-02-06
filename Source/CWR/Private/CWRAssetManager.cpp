// Fill out your copyright notice in the Description page of Project Settings.


#include "CWRAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "CWRGameplayTags.h"


UCWRAssetManager& UCWRAssetManager::Get()
{
	check(GEngine);
	
	return *Cast<UCWRAssetManager>(GEngine->AssetManager);
}

void UCWRAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FCWRGameplayTags::InitializeNativeGameplayTags();

	// This is required to use Target Data
	UAbilitySystemGlobals::Get().InitGlobalData();
}
