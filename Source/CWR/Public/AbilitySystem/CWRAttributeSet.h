// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CWRAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UCWRAbilitySystemComponent;
/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FCWRAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

UCLASS()
class CWR_API UCWRAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCWRAttributeSet();

	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, MaxArmor);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, BaseHeal);
	ATTRIBUTE_ACCESSORS(UCWRAttributeSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FCWRAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FCWRAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FCWRAttributeEvent OnOutOfHealth;
	
	UCWRAbilitySystemComponent* GetCWRAbilitySystemComponent() const;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	
	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor) const;
	
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;
	
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) const;

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldBaseHeal) const;

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Base Attributes", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxArmor;
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Base Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
	
	// Used to track when the health reaches 0.
	bool bOutOfHealth = false;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange = 0.0f;
	float HealthBeforeAttributeChange = 0.0f;
	
	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes", Meta = (HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
	
};
