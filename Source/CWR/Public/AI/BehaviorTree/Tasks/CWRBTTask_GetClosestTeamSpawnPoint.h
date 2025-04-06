// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CWRBTTask_GetClosestTeamSpawnPoint.generated.h"


UCLASS()
class CWR_API UCWRBTTask_GetClosestTeamSpawnPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	
	UCWRBTTask_GetClosestTeamSpawnPoint();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
