// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CWRAssetManager.generated.h"


UCLASS()
class CWR_API UCWRAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UCWRAssetManager& Get();

protected:

	virtual void  StartInitialLoading() override;
	
	
};
