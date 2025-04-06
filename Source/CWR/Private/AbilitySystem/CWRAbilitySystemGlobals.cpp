// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CWRAbilitySystemGlobals.h"

#include "AbilitySystem/CWRGameplayEffectContext.h"

struct FGameplayEffectContext;

UCWRAbilitySystemGlobals::UCWRAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UCWRAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FCWRGameplayEffectContext();
}

