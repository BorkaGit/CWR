// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CWRAttributeSet.h"

#include "CWRGameplayTags.h"
#include "Net/UnrealNetwork.h"


UCWRAttributeSet::UCWRAttributeSet()
{
	const FCWRGameplayTags& GameplayTags = FCWRGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Base_Health,		GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxHealth,	GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_Armor,		GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxArmor,		GetMaxArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_Stamina,		GetStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxStamina,	GetMaxStaminaAttribute);
}


void UCWRAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if ( Attribute == GetHealthAttribute() )
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if ( Attribute == GetArmorAttribute() )
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArmor());
	}
	else if ( Attribute == GetStaminaAttribute() )
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
}

void UCWRAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if ( Attribute == GetMaxHealthAttribute() )
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if ( Attribute == GetMaxArmorAttribute() )
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if ( Attribute == GetMaxStaminaAttribute() )
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UCWRAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	
}


void UCWRAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Health,					COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxHealth,					COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Armor,						COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxArmor,					COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Stamina,					COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxStamina,				COND_None, REPNOTIFY_Always);
}

void UCWRAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, Health, OldHealth);
}

void UCWRAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, MaxHealth, OldMaxHealth);
}

void UCWRAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, Armor, OldArmor);
}

void UCWRAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, MaxArmor, OldMaxArmor);
}

void UCWRAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, Stamina, OldStamina);
}

void UCWRAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, MaxStamina, OldMaxStamina);
}

