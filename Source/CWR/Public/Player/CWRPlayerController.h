// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "CWRPlayerController.generated.h"


class UCWRWeaponStateComponent;
class UCWRInventoryManagerComponent;
class UCWRInputConfig;
class UCWRInputComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCWRAbilitySystemComponent;

UCLASS()
class CWR_API ACWRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ACWRPlayerController();

protected:

	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void Move(const FInputActionValue& Value);
	
	void LookMouse(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UCWRInventoryManagerComponent> InventoryManagerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCWRWeaponStateComponent> WeaponStateComponent;
	
private:

	UPROPERTY()
	UCWRInputComponent* CWRInputComponent = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CWRContext = nullptr;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UCWRInputConfig> InputConfig = nullptr;
	
	UCWRAbilitySystemComponent* GetAbilitySystemComponent();
	
	UPROPERTY()
	TObjectPtr<UCWRAbilitySystemComponent> CWRAbilitySystemComponent;

	
};
