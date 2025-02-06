// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CWRPreCMCTick.h"

#include "Character/CWRCharacter_Base.h"


UCWRPreCMCTick::UCWRPreCMCTick()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCWRPreCMCTick::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACWRCharacter_Base>(GetOwner());
	
}


void UCWRPreCMCTick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OwnerCharacter->PreCMCTick();
}

