// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/CWRAttributeSet.h"
#include "CWR/CWRCharacter_Player.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	if (const auto CWRAttributeSet = Cast<UCWRAttributeSet>(AttributeSet) )
	{
		OnHealthChanged.Broadcast(CWRAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(CWRAttributeSet->GetMaxHealth());
		OnArmorChanged.Broadcast(CWRAttributeSet->GetArmor());
		OnMaxArmorChanged.Broadcast(CWRAttributeSet->GetMaxArmor());
		OnStaminaChanged.Broadcast(CWRAttributeSet->GetStamina());
		OnMaxStaminaChanged.Broadcast(CWRAttributeSet->GetMaxStamina());
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (const auto CWRAttributeSet = Cast<UCWRAttributeSet>(AttributeSet) )
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetArmorAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnArmorChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetMaxArmorAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxArmorChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnStaminaChanged.Broadcast(Data.NewValue);
		});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CWRAttributeSet->GetMaxStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxStaminaChanged.Broadcast(Data.NewValue);
		});
	}

	if ( PlayerController )
	{
		if (const auto CWRCharacter = PlayerController->GetPawn<ACWRCharacter_Player>())
		{
			CWRCharacter->OnBulletAmountChanged.AddLambda([this](const int32 NewBulletAmount)
			{
				OnBulletAmountChanged.Broadcast(NewBulletAmount);
			});
		}
		if (const auto CWRCharacter = PlayerController->GetPawn<ACWRCharacter_Player>())
		{
			CWRCharacter->OnBackpackBulletAmountChanged.AddLambda([this](const int32 NewBulletAmount)
			{
				OnBackpackBulletAmountChanged.Broadcast(NewBulletAmount);
			});
		}
	}
}
