// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "Teams/CWRTeamAgentInterface.h"
#include "CWRCharacter_Base.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACWRWeaponActor;
class UAIPerceptionStimuliSourceComponent;
class UCWRHealthComponent;
class ACWRPlayerState;
class ACWRPlayerController;
class UCWRPawnExtensionComponent;
class UCWRPreCMCTick;
class UCWRInventoryItemDefinition;
class UCWREquipmentInstance;
class UCWRAttributeSet;
class UCWRAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponStatChanged, int32)
DECLARE_MULTICAST_DELEGATE(FOnTeamSet);

/**
 * FCWRReplicatedAcceleration: Compressed representation of acceleration
 */
USTRUCT()
struct FCWRReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

/** The type we use to send FastShared movement updates. */
USTRUCT()
struct FSharedRepMovement
{
	GENERATED_BODY()

	FSharedRepMovement();

	bool FillForCharacter(ACharacter* Character);
	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY(Transient)
	FRepMovement RepMovement;

	UPROPERTY(Transient)
	float RepTimeStamp = 0.0f;

	UPROPERTY(Transient)
	uint8 RepMovementMode = 0;

	UPROPERTY(Transient)
	bool bProxyIsJumpForceApplied = false;

	UPROPERTY(Transient)
	bool bIsCrouched = false;
};

template<>
struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
{
	enum
	{
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};

UCLASS()
class CWR_API ACWRCharacter_Base : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface, public ICWRTeamAgentInterface
{
	GENERATED_BODY()

public:

	ACWRCharacter_Base(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent)
	void AttachWeapon(AActor* Weapon);
	
	UFUNCTION(BlueprintCallable, Category = "CWR|Character")
	ACWRPlayerController* GetCWRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "CWR|Character")
	ACWRPlayerState* GetCWRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "CWR|Character")
	UCWRAbilitySystemComponent* GetCWRAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

	//~ICWRTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnCWRTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ICWRTeamAgentInterface interface

	/** RPCs that is called on frames when default property replication is skipped. This replicates a single movement update to everyone. */
	UFUNCTION(NetMulticast, unreliable)
	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);

	// Last FSharedRepMovement we sent, to avoid sending repeatedly.
	FSharedRepMovement LastSharedReplication;

	virtual bool UpdateSharedReplication();

	UFUNCTION(BlueprintNativeEvent)
	void PreCMCTick();

	virtual void PickUpItem(TSubclassOf<UCWRInventoryItemDefinition> ItemDefinition);

	FOnTeamSet OnTeamSetDelegate;

	UFUNCTION(BlueprintPure)
	bool IsWaitingForTeamSet() const { return bIsWaitingForTeamSet; }
	
protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();
	
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void Ragdoll() const;
	void HideEquippedWeapons() const;
	void Death();
	
	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual bool CanJumpInternal_Implementation() const;

	virtual void InitializeDefaultAttributes() const;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	UFUNCTION(BlueprintCallable)
	virtual void AddInitialInventory();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ragdoll")
	float RagdollImpulseStrength = 500.f;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<TSubclassOf<UCWRInventoryItemDefinition>> InitialInventoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_SightTransform)
	FTransform SightTransform = FTransform::Identity;

	UFUNCTION()
	void OnRep_SightTransform();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SightTransformChanged();
	
private:
	
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
	TObjectPtr<UCWRPawnExtensionComponent> PawnExtensionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCWRHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	FName Socket1P = "Camera";
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FCWRReplicatedAcceleration ReplicatedAcceleration;

	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FOnCWRTeamIndexChangedDelegate OnTeamChangedDelegate;

	bool bIsWaitingForTeamSet = true;

protected:
	// Called to determine what happens to the team ID when possession ends
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}

private:
	
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
