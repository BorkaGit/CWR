// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CWRGameplayAbility_Jump.h"

#include "AbilitySystem/CWRGameplayAbility.h"
#include "Character/CWRCharacter_Base.h"

struct FGameplayTagContainer;


UCWRGameplayAbility_Jump::UCWRGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UCWRGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ACWRCharacter_Base* CWRCharacter = Cast<ACWRCharacter_Base>(ActorInfo->AvatarActor.Get());
	if (!CWRCharacter || !CWRCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UCWRGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCWRGameplayAbility_Jump::CharacterJumpStart()
{
	if (ACWRCharacter_Base* CWRCharacter = GetCWRCharacterFromActorInfo())
	{
		if (CWRCharacter->IsLocallyControlled() && !CWRCharacter->bPressedJump)
		{
			CWRCharacter->UnCrouch();
			CWRCharacter->Jump();
		}
	}
}

void UCWRGameplayAbility_Jump::CharacterJumpStop()
{
	if (ACWRCharacter_Base* CWRCharacter = GetCWRCharacterFromActorInfo())
	{
		if (CWRCharacter->IsLocallyControlled() && CWRCharacter->bPressedJump)
		{
			CWRCharacter->StopJumping();
		}
	}
}


