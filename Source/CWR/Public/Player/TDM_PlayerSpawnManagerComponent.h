// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/CWRPlayerSpawnManagerComponent.h"

#include "TDM_PlayerSpawnManagerComponent.generated.h"


UCLASS()
class CWR_API UTDM_PlayerSpawnManagerComponent : public UCWRPlayerSpawnManagerComponent
{
	GENERATED_BODY()
	
public:

	UTDM_PlayerSpawnManagerComponent(const FObjectInitializer& ObjectInitializer);

	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<ACWRPlayerStart*>& PlayerStarts) override;
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) override;
};
