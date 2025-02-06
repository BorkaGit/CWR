// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRGameplayAbility_RangedWeapon.h"

#include "CWR/CWRCharacter_Player.h"
#include "Weapons/CWRRangedWeaponInstance.h"

UCWRRangedWeaponInstance* UCWRGameplayAbility_RangedWeapon::GetWeaponInstance() const
{
	return Cast<UCWRRangedWeaponInstance>(GetAssociatedEquipment());
}

void UCWRGameplayAbility_RangedWeapon::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	WeaponHolder = Cast<ACWRCharacter_Player>(GetAvatarActorFromActorInfo());
}

void UCWRGameplayAbility_RangedWeapon::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UCWRGameplayAbility_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCWRGameplayAbility_RangedWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


