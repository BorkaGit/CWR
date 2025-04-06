// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/CWRTeamEliminationSpawnManager.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/CWRGameState.h"
#include "Player/CWRPlayerStart.h"
#include "Teams/CWRTeamSpawnPoint.h"
#include "Teams/CWRTeamSubsystem.h"

UCWRTeamEliminationSpawnManager::UCWRTeamEliminationSpawnManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

AActor* UCWRTeamEliminationSpawnManager::OnChoosePlayerStart(AController* Player, TArray<ACWRPlayerStart*>& PlayerStarts)
{
	UCWRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UCWRTeamSubsystem>();
	if (!ensure(TeamSubsystem))
	{
		return nullptr;
	}

	const int32 PlayerTeamId = TeamSubsystem->FindTeamFromObject(Player);
	

	// We should have a TeamId by now, but early login stuff before post login can try to do stuff, ignore it.
	if (!ensure(PlayerTeamId != INDEX_NONE))
	{
		return nullptr;
	}

	CachedTeamSpawnPoints.Empty();
	
	if (const UWorld* World = GetWorld())
	{
		for (TActorIterator<ACWRTeamSpawnPoint> It(World); It; ++It)
		{
			if (ACWRTeamSpawnPoint* TeamSpawnPoint = *It)
			{
				if ( TeamSpawnPoint->WasEverCaptured())
				{
					if ( TeamSpawnPoint->GetCurrentTeamId() == PlayerTeamId )
					{
						CachedTeamSpawnPoints.Add(TeamSpawnPoint);
					}
				}
				else
				{
					if ( TeamSpawnPoint->IsStartingPoint() )
					{
						TeamSpawnPoint->CapturePoint(PlayerTeamId);
						return TeamSpawnPoint->FindUnclaimedPlayerStart();
					}
				}
			}
		}
		
		if ( !CachedTeamSpawnPoints.IsEmpty() )
		{
			const int32 RandomIndex = FMath::RandRange(0, CachedTeamSpawnPoints.Num() - 1);
			return CachedTeamSpawnPoints[RandomIndex]->FindUnclaimedPlayerStart();
		}
	}

	return nullptr;
}

void UCWRTeamEliminationSpawnManager::OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation)
{
	
}
