// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTree/Tasks/CWRBTTask_GetClosestTeamSpawnPoint.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CWRCharacter_Base.h"
#include "Teams/CWRTeamSpawnPoint.h"
#include "Teams/CWRTeamSubsystem.h"


UCWRBTTask_GetClosestTeamSpawnPoint::UCWRBTTask_GetClosestTeamSpawnPoint()
{
	NodeName = TEXT("GetClosestTeamSpawnPoint");
}

EBTNodeResult::Type UCWRBTTask_GetClosestTeamSpawnPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const AAIController* AIController = OwnerComp.GetAIOwner( );
	if( !AIController )
	{
		return EBTNodeResult::Failed;
	}

	const auto CWRCharacter = Cast<ACWRCharacter_Base>(AIController->GetPawn());
	if ( !CWRCharacter )
	{
		return EBTNodeResult::Failed;
	}
		
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if ( !BlackBoard )
	{
		return EBTNodeResult::Failed;
	}

	const FVector OwnerLocation = CWRCharacter->GetActorLocation();

	TArray<FVector> FilteredSpawnPointLocations;

	if (const UWorld* World = GetWorld())
	{
		if (const UCWRTeamSubsystem* TeamSubsystem = World->GetSubsystem<UCWRTeamSubsystem>())
		{
			const int32 OwnerTeamId = TeamSubsystem->FindTeamFromObject(CWRCharacter);
		
			for (TActorIterator<ACWRTeamSpawnPoint> It(World); It; ++It)
			{
				if (const ACWRTeamSpawnPoint* TeamSpawnPoint = *It)
				{
					if ( !TeamSpawnPoint->IsCaptured() || TeamSpawnPoint->GetCurrentTeamId() != OwnerTeamId )
					{
						FilteredSpawnPointLocations.Add(TeamSpawnPoint->GetActorLocation());
					}
				}
			}
		}
	}

	FVector ClosestLocation = FVector::ZeroVector;
	FVector SecondClosestLocation = FVector::ZeroVector;
	double ClosestDistance = DBL_MAX;
	double SecondClosestDistance = DBL_MAX;
	
	for ( FVector Location  : FilteredSpawnPointLocations )
	{
		const double Distance = (OwnerLocation - Location).SizeSquared();
		
		if ( Distance <  ClosestDistance )
		{
			SecondClosestDistance = ClosestDistance;
			ClosestDistance = Distance;

			SecondClosestLocation = ClosestLocation;
			ClosestLocation = Location;
		}
		else if ( Distance < SecondClosestDistance && Distance != ClosestDistance )
		{
			SecondClosestDistance = Distance;

			SecondClosestLocation = Location;
		}
	}

	if ( !FilteredSpawnPointLocations.IsEmpty() )
	{
		BlackBoard->SetValueAsVector(BlackboardKey.SelectedKeyName, FilteredSpawnPointLocations.Num() > 1 ? FMath::RandBool()  ? ClosestLocation : SecondClosestLocation : ClosestLocation);
		return EBTNodeResult::Succeeded;
	}
	
	return  EBTNodeResult::Failed;
	
}
