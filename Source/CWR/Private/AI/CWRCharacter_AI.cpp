// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CWRCharacter_AI.h"

#include "AI/CWRAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


ACWRCharacter_AI::ACWRCharacter_AI(FObjectInitializer const& ObjectInitializer): Super(
	ObjectInitializer )
{
	
}

void ACWRCharacter_AI::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CWRAIController = Cast<ACWRAIController>(NewController);

	BlackboardComponent = CWRAIController->GetBlackboardComponent();
	BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	CWRAIController->RunBehaviorTree(BehaviorTree);
}
