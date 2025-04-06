// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CWRPlayerState.h"

#include "AbilitySystem/CWRAbilitySet.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystem/CWRAttributeSet.h"
#include "Character/CWRPawnData.h"
#include "Character/CWRPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/CWRExperienceManagerComponent.h"
#include "GameModes/CWRGameMode.h"
#include "Messages/CWRVerbMessage.h"
#include "Net/UnrealNetwork.h"
#include "Player/CWRPlayerController.h"

class AController;
class APlayerState;
class FLifetimeProperty;

const FName ACWRPlayerState::NAME_CWRAbilityReady("CWRAbilitiesReady");

ACWRPlayerState::ACWRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(ECWRPlayerConnectionType::Player)
{
	AbilitySystemComponent	= CreateDefaultSubobject<UCWRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet			= CreateDefaultSubobject<UCWRAttributeSet>(TEXT("AttributeSet"));
	
	SetNetUpdateFrequency(100.f);
	
	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

void ACWRPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ACWRPlayerState::Reset()
{
	Super::Reset();
}

void ACWRPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void ACWRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void ACWRPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
		case ECWRPlayerConnectionType::Player:
		case ECWRPlayerConnectionType::InactivePlayer:
			//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
			// (e.g., for long running servers where they might build up if lots of players cycle through)
			bDestroyDeactivatedPlayerState = true;
			break;
		default:
			bDestroyDeactivatedPlayerState = true;
			break;
	}
	
	SetPlayerConnectionType(ECWRPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void ACWRPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == ECWRPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(ECWRPlayerConnectionType::Player);
	}
}

void ACWRPlayerState::OnExperienceLoaded(const UCWRExperienceDefinition* /*CurrentExperience*/)
{
	if (ACWRGameMode* CWRGameMode = GetWorld()->GetAuthGameMode<ACWRGameMode>())
	{
		if (const UCWRPawnData* NewPawnData = CWRGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
	}
}

void ACWRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);	
}

FRotator ACWRPlayerState::GetReplicatedViewRotation() const
{
	// Could replace this with custom replication
	return ReplicatedViewRotation;
}

void ACWRPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

ACWRPlayerController* ACWRPlayerState::GetCWRPlayerController() const
{
	return Cast<ACWRPlayerController>(GetOwner());
}

UAbilitySystemComponent* ACWRPlayerState::GetAbilitySystemComponent() const
{
	return GetCWRAbilitySystemComponent();
}

void ACWRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UCWRExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UCWRExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnCWRExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void ACWRPlayerState::SetPawnData(const UCWRPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UCWRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_CWRAbilityReady);
	
	ForceNetUpdate();
}

void ACWRPlayerState::OnRep_PawnData()
{
}

void ACWRPlayerState::SetPlayerConnectionType(ECWRPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void ACWRPlayerState::SetSquadID(int32 NewSquadId)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadId;
	}
}

void ACWRPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
}

FGenericTeamId ACWRPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnCWRTeamIndexChangedDelegate* ACWRPlayerState::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ACWRPlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ACWRPlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}

void ACWRPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void ACWRPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 ACWRPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool ACWRPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void ACWRPlayerState::ClientBroadcastMessage_Implementation(const FCWRVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}


