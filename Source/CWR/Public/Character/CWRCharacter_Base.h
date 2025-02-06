// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CWRCharacter_Base.generated.h"

class UCWRPreCMCTick;
class UCWRInventoryItemDefinition;
class UCWREquipmentInstance;
class UCWREquipmentManagerComponent;
class UCWRAttributeSet;
class UCWRAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponStatChanged, int32)

UCLASS()
class CWR_API ACWRCharacter_Base : public ACharacter
{
	GENERATED_BODY()

public:

	ACWRCharacter_Base(FObjectInitializer const& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintNativeEvent)
	void PreCMCTick();
	
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void InitAbilityActorInfo();

	virtual void InitializeDefaultAttributes() const;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	virtual void AddStartupAbilities() const;

	UFUNCTION(BlueprintCallable)
	virtual void AddInitialInventory();
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 100.f;

public:

	UFUNCTION(BlueprintCallable)
	void SetSprinting( bool IsSprinting ) const;

	UFUNCTION(BlueprintPure)
	bool IsSprinting() const;
	
	UFUNCTION(BlueprintCallable)
	void SetWalking( bool IsWalking ) const;

	UFUNCTION(BlueprintPure)
	bool IsWalking() const;
	
	virtual void DropItem();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAimingStarted();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAimingEnded();
	
	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandSocketTransform() const;

	UFUNCTION(BlueprintCallable)
	FRotator GetAimRotation() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsAiming(const bool InbIsAiming)  { bIsAiming = InbIsAiming; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool HasWeaponInHands() const { return bHasWeaponInHands; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetHasWeaponInHands(const bool InbHasWeaponInHands ) { bHasWeaponInHands = InbHasWeaponInHands; }

	UFUNCTION(BlueprintCallable)
	UCWREquipmentInstance* GetItemInHands() const;

	FOnWeaponStatChanged OnBulletAmountChanged;
	FOnWeaponStatChanged OnBackpackBulletAmountChanged;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets")
	FName RightHandSocketName = "hand_r";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets")
	FName WeaponSocketName = "WeaponSocket";

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bIsFiring = false;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bIsFirstPerson = false;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bHasWeaponInHands = false;

	UPROPERTY(Transient, BlueprintReadWrite)
	bool bIsAiming = false;                  

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bCanAim = true;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bCanFire = true;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bCanReload = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCWREquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCWRPreCMCTick> CWRPreCMCTick;
	
	UPROPERTY()
	TObjectPtr<UCWRAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UCWRAttributeSet> AttributeSet;

	// Set GameplayEffect with Max values of Health, Armor and Stamina 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultBaseAttributesMax;

	// Set GameplayEffect with Health, Armor and Stamina 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultBaseAttributes;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<TSubclassOf<UCWRInventoryItemDefinition>> InitialInventoryItems;
	
};
