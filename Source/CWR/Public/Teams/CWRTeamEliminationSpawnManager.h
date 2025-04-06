// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/CWRPlayerSpawnManagerComponent.h"

#include "CWRTeamEliminationSpawnManager.generated.h"


class ACWRTeamSpawnPoint;

UCLASS()
class CWR_API UCWRTeamEliminationSpawnManager : public UCWRPlayerSpawnManagerComponent
{
	GENERATED_BODY()

public:
	
	UCWRTeamEliminationSpawnManager(const FObjectInitializer& ObjectInitializer);

	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<ACWRPlayerStart*>& PlayerStarts) override;
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) override;

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACWRTeamSpawnPoint>> CachedTeamSpawnPoints;

};
