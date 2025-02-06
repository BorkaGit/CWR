// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "CWRPlayerState.generated.h"


class UCWRAttributeSet;
class UCWRAbilitySystemComponent;

UCLASS()
class CWR_API ACWRPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	ACWRPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UCWRAttributeSet* GetAttributeSet() const{ return AttributeSet; }

protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCWRAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCWRAttributeSet> AttributeSet;

	
	
};
