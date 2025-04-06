// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CWRGameplayAbility.h"
#include "Equipment/CWRGameplayAbility_FromEquipment.h"
#include "CWRGameplayAbility_RangedWeapon.generated.h"


class UCWRRangedWeaponInstance;
class ACWRCharacter_Player;

UCLASS()
class CWR_API UCWRGameplayAbility_RangedWeapon : public UCWRGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:

	
	UFUNCTION(BlueprintCallable)
	UCWRRangedWeaponInstance* GetWeaponInstance() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACWRCharacter_Player* GetWeaponHolder() const { return WeaponHolder; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAnimInstance* GetWeaponAnimInstance1P() const { return WeaponAnimInstance1P; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAnimInstance* GetWeaponAnimInstance3P() const { return WeaponAnimInstance3P; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAnimInstance* GetHolderAnimInstance1P() const { return HolderAnimInstance1P; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAnimInstance* GetHolderAnimInstance3P() const { return HolderAnimInstance3P; }

	//~UGameplayAbility interface
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface
	

protected:

	UPROPERTY(BlueprintReadOnly)
	ACWRCharacter_Player* WeaponHolder;

	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* WeaponAnimInstance1P;

	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* WeaponAnimInstance3P;
	
	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* HolderAnimInstance1P;

	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* HolderAnimInstance3P;
	
	
};
