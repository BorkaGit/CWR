// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/CWRTeamInfoBase.h"

#include "Net/UnrealNetwork.h"
#include "Teams/CWRTeamSubsystem.h"

class FLifetimeProperty;

ACWRTeamInfoBase::ACWRTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void ACWRTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void ACWRTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void ACWRTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		UCWRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UCWRTeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ACWRTeamInfoBase::RegisterWithTeamSubsystem(UCWRTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void ACWRTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		UCWRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UCWRTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void ACWRTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void ACWRTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}




