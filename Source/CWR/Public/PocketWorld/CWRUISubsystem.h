// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CWRUISubsystem.generated.h"

class ACWRPocketStage;
class UPocketCapture;


UCLASS()
class CWR_API UCWRUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
	void SetPocketCapture(UPocketCapture* NewPocketCapture);

	UFUNCTION(BlueprintCallable)
	UPocketCapture* GetPocketCapture();

	UFUNCTION(BlueprintCallable)
	void SetPocketStage(ACWRPocketStage* NewPocketStage);

	UFUNCTION(BlueprintCallable)
	ACWRPocketStage* GetPocketStage();
	
	UFUNCTION(BlueprintCallable)
	void SpawnPocketLevel(FSoftObjectPath PocketLevelAssetPath);

private:
	TObjectPtr<UPocketCapture> PocketCapture;
	TObjectPtr<ACWRPocketStage> PocketStage;
	
};
