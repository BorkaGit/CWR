// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TDM_PlayerSpawnManagerComponent.h"

#include "GameFramework/PlayerState.h"
#include "GameModes/CWRGameState.h"
#include "Player/CWRPlayerStart.h"
#include "Teams/CWRTeamSubsystem.h"


UTDM_PlayerSpawnManagerComponent::UTDM_PlayerSpawnManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AActor* UTDM_PlayerSpawnManagerComponent::OnChoosePlayerStart(AController* Player, TArray<ACWRPlayerStart*>& PlayerStarts)
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

	ACWRGameState* GameState = GetGameStateChecked<ACWRGameState>();

	ACWRPlayerStart* BestPlayerStart = nullptr;
	double MaxDistance = 0;
	ACWRPlayerStart* FallbackPlayerStart = nullptr;
	double FallbackMaxDistance = 0;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		const int32 TeamId = TeamSubsystem->FindTeamFromObject(PlayerState);
		
		// We should have a TeamId by now...
		if (PlayerState->IsOnlyASpectator() || !ensure(TeamId != INDEX_NONE))
		{
			continue;
		}

		// If the other player isn't on the same team, lets find the furthest spawn from them.
		if (TeamId != PlayerTeamId)
		{
			for (ACWRPlayerStart* PlayerStart : PlayerStarts)
			{
				if (APawn* Pawn = PlayerState->GetPawn())
				{
					const double Distance = PlayerStart->GetDistanceTo(Pawn);

					if (PlayerStart->IsClaimed())
					{
						if (FallbackPlayerStart == nullptr || Distance > FallbackMaxDistance)
						{
							FallbackPlayerStart = PlayerStart;
							FallbackMaxDistance = Distance;
						}
					}
					else if (PlayerStart->GetLocationOccupancy(Player) < ECWRPlayerStartLocationOccupancy::Full)
					{
						if (BestPlayerStart == nullptr || Distance > MaxDistance)
						{
							BestPlayerStart = PlayerStart;
							MaxDistance = Distance;
						}
					}
				}
			}
		}
	}

	if (BestPlayerStart)
	{
		return BestPlayerStart;
	}

	return FallbackPlayerStart;
}

void UTDM_PlayerSpawnManagerComponent::OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation)
{
	
}
