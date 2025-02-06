// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CWRPlayerState.h"

#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystem/CWRAttributeSet.h"
#include "Net/UnrealNetwork.h"


ACWRPlayerState::ACWRPlayerState()
{
	AbilitySystemComponent	= CreateDefaultSubobject<UCWRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet			= CreateDefaultSubobject<UCWRAttributeSet>(TEXT("AttributeSet"));
	
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	SetNetUpdateFrequency(100.f);
}

void ACWRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

UAbilitySystemComponent* ACWRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


