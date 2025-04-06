// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Teams/CWRTeamInfoBase.h"
#include "CWRTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class ACWRTeamPrivateInfo : public ACWRTeamInfoBase
{
	GENERATED_BODY()
	
public:
	ACWRTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
