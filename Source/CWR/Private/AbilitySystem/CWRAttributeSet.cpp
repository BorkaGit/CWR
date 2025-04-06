// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CWRAttributeSet.h"

#include "CWRGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


UCWRAttributeSet::UCWRAttributeSet()
	: Health(100.f)
	, MaxHealth(100.f)
{
	const FCWRGameplayTags& GameplayTags = FCWRGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Base_Health,		GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxHealth,	GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_Armor,		GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxArmor,		GetMaxArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_Stamina,		GetStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Base_MaxStamina,	GetMaxStaminaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Meta_Damage,		GetDamageAttribute);
}

void UCWRAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Health,					COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxHealth,					COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Armor,						COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxArmor,					COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, Stamina,					COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, MaxStamina,				COND_None,		REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, BaseDamage,				COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCWRAttributeSet, BaseHeal,					COND_OwnerOnly, REPNOTIFY_Always);
}

UCWRAbilitySystemComponent* UCWRAttributeSet::GetCWRAbilitySystemComponent() const
{
	return Cast<UCWRAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UCWRAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, Health, OldHealth);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes
	
	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldHealth.GetCurrentValue();

	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldHealth.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldHealth.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UCWRAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, MaxHealth, OldMaxHealth);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldMaxHealth.GetCurrentValue(), OldMaxHealth.GetCurrentValue(), GetMaxHealth());
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

void UCWRAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, BaseDamage, OldBaseDamage);
}

void UCWRAttributeSet::OnRep_BaseHeal(const FGameplayAttributeData& OldBaseHeal) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCWRAttributeSet, BaseHeal, OldBaseHeal);
}

bool UCWRAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(FCWRGameplayTags::Get().Gameplay_Damage_SelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(FCWRGameplayTags::Get().Gameplay_Damage_Immunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			if (Data.Target.HasMatchingGameplayTag(FCWRGameplayTags::Get().Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UCWRAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(FCWRGameplayTags::Get().Gameplay_Damage_SelfDestruct);
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(FCWRGameplayTags::Get().Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(FCWRGameplayTags::Get().Cheat_UnlimitedHealth) ))
	{
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			/*
			FCWRVerbMessage Message;
			Message.Verb = TAG_CWR_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);*/
		}

		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
/*	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}*/
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?

		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UCWRAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UCWRAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UCWRAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent();
			check(CWRASC);

			CWRASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UCWRAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

