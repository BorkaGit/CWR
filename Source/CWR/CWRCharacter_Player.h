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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SightTransformChanged();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSightTransform(const FTransform InSightTransform ) {  SightTransform = InSightTransform; }

	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void InitializeDefaultAttributes() const override;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SightTransform)
	FTransform SightTransform = FTransform::Identity;
	
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ROC_SpawnMag();

	UFUNCTION()
	void OnRep_SightTransform();
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FP_Legs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> FP_Base;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CB_MeshRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> FP_Offset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CB_Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	FName Socket1P = "Camera";
};

