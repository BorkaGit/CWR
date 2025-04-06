// Fill out your copyright notice in the Description page of Project Settings.


#include "System/CWRGameData.h"
#include "System/CWRAssetManager.h"


UCWRGameData::UCWRGameData()
{
}

const UCWRGameData& UCWRGameData::UCWRGameData::Get()
{
	return UCWRAssetManager::Get().GetGameData();
}

