// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/CWRGameplayAbility.h"

#include "CWRGameplayAbility_FromEquipment.generated.h"

class UCWREquipmentInstance;
class UCWRInventoryItemInstance;

/**
 * UCWRGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class CWR_API UCWRGameplayAbility_FromEquipment : public UCWRGameplayAbility
{
	GENERATED_BODY()
	
public:

	UCWRGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="CWR|Ability")
	UCWREquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "CWR|Ability")
	UCWRInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
	
};
