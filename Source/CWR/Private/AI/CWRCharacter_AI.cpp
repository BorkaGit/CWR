// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CWRCharacter_AI.h"

#include "AI/CWRAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CWRCharacterMovementComponent.h"
#include "Inventory/CWRInventoryManagerComponent.h"


ACWRCharacter_AI::ACWRCharacter_AI(FObjectInitializer const& ObjectInitializer): Super(
	ObjectInitializer )
{
	auto CWRMovementComponent = CastChecked<UCWRCharacterMovementComponent>(GetCharacterMovement());
	DefaultWalkSpeed = CWRMovementComponent->MaxWalkSpeed;
}

void ACWRCharacter_AI::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CWRAIController = Cast<ACWRAIController>(NewController);
/*
	BlackboardComponent = CWRAIController->GetBlackboardComponent();
	BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	CWRAIController->RunBehaviorTree(BehaviorTree);*/
}

void ACWRCharacter_AI::AddInitialInventory()
{
	if ( !HasAuthority() || !IsValid(GetController()) ) return;

	const auto InventoryManager = GetController()->GetComponentByClass<UCWRInventoryManagerComponent>();

	if ( !IsValid(InventoryManager)) return;

	Algo::Transform(InitialInventoryItemsClasses, InitialInventoryItems, [&InventoryManager](const TSubclassOf<UCWRInventoryItemDefinition>& ItemClass)
	{
		return InventoryManager->AddItemDefinition(ItemClass);
	});
	
	
	Super::AddInitialInventory();
}
