// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/CWRTeamCheats.h"

#include "Teams/CWRTeamSubsystem.h"


//////////////////////////////////////////////////////////////////////
// UCWRTeamCheats

void UCWRTeamCheats::CycleTeam()
{
	if (UCWRTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UCWRTeamSubsystem>(GetWorld()))
	{
		APlayerController* PC = GetPlayerController();

		const int32 OldTeamId = TeamSubsystem->FindTeamFromObject(PC);
		const TArray<int32> TeamIds = TeamSubsystem->GetTeamIDs();
		
		if (TeamIds.Num())
		{
			const int32 IndexOfOldTeam = TeamIds.Find(OldTeamId);
			const int32 IndexToUse = (IndexOfOldTeam + 1) % TeamIds.Num();

			const int32 NewTeamId = TeamIds[IndexToUse];

			TeamSubsystem->ChangeTeamForActor(PC, NewTeamId);
		}

		const int32 ActualNewTeamId = TeamSubsystem->FindTeamFromObject(PC);

		UE_LOG(LogConsoleResponse, Log, TEXT("Changed to team %d (from team %d)"), ActualNewTeamId, OldTeamId);
	}
}

void UCWRTeamCheats::SetTeam(int32 TeamID)
{
	if (UCWRTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UCWRTeamSubsystem>(GetWorld()))
	{
		if (TeamSubsystem->DoesTeamExist(TeamID))
		{
			APlayerController* PC = GetPlayerController();

			TeamSubsystem->ChangeTeamForActor(PC, TeamID);
		}
	}
}

void UCWRTeamCheats::ListTeams()
{
	if (UCWRTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UCWRTeamSubsystem>(GetWorld()))
	{
		const TArray<int32> TeamIDs = TeamSubsystem->GetTeamIDs();

		for (const int32 TeamID : TeamIDs)
		{
			UE_LOG(LogConsoleResponse, Log, TEXT("Team ID %d"), TeamID);
		}
	}
}

