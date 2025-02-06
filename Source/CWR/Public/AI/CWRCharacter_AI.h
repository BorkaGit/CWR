// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CWRCharacter_Base.h"
#include "CWRCharacter_AI.generated.h"


class UBehaviorTree;
class ACWRAIController;
class UBlackboardComponent;

UCLASS()
class CWR_API ACWRCharacter_AI : public ACWRCharacter_Base
{
	GENERATED_BODY()

public:

	ACWRCharacter_AI(FObjectInitializer const& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

protected:

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ACWRAIController> CWRAIController = nullptr;
	

private:

	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent = nullptr;
};
