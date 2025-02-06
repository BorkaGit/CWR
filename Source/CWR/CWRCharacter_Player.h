// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/CWRCharacter_Base.h"
#include "CWRCharacter_Player.generated.h"

class UCWREquipmentInstance;
class UCWREquipmentManagerComponent;
class UCWRWeaponInstance;
class UGameplayAbility;
struct FGameplayTag;
class UAbilitySystemComponent;
class UGameplayEffect;
class UCWRAttributeSet;
class UCWRAbilitySystemComponent;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class ACWRCharacter_Player : public ACWRCharacter_Base
{
	GENERATED_BODY()


public:
	ACWRCharacter_Player(FObjectInitializer const& ObjectInitializer);
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const override;
	
	virtual void AddStartupAbilities() const override;

public:
	
	virtual void DropItem() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void ControllerRecoil(float RecoilAmount);

	FVector GetCameraLocation() const;
	FVector GetCameraEndTraceLocation(float Distance) const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmFP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraFP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sockets")
	FName FPSpringArmSocketName = "head";
	
	UPROPERTY(Transient, BlueprintReadOnly)
	FRotator HandSwayRotator = FRotator::ZeroRotator;
	
private:
	
	float AimCurrent = 0.f;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AimTarget = 0.f;
};

