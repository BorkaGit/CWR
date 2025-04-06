// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CWRPreCMCTick.generated.h"


class ACWRCharacter_Base;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CWR_API UCWRPreCMCTick : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UCWRPreCMCTick();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<ACWRCharacter_Base> OwnerCharacter;
};
