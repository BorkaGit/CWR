// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Teams/CWRTeamInfoBase.h"
#include "CWRTeamPublicInfo.generated.h"

class ACWRTeamSpawnPoint;
class UCWRTeamCreationComponent;
class UCWRTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class ACWRTeamPublicInfo : public ACWRTeamInfoBase
{
	GENERATED_BODY()
	
	friend UCWRTeamCreationComponent;

public:
	ACWRTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UCWRTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

	FORCEINLINE TArray<TObjectPtr<ACWRTeamSpawnPoint>> GetCapturedPoints() const { return CapturedPoints; }
	
private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UCWRTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<UCWRTeamDisplayAsset> TeamDisplayAsset;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACWRTeamSpawnPoint>> CapturedPoints;
	
};
