// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "Teams/CWRTeamAgentInterface.h"
#include "CWRCharacter_Base.generated.h"

class UCWRWeaponInstance;
class UCWRInventoryItemInstance;
class USplineComponent;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateDT, FName, WeaponSlot);


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

UENUM(BlueprintType)
enum class ECWRMovementMode : uint8
{
	Walking,
	Sprinting,
	Aiming,
	Slidings
};

UCLASS()
class CWR_API ACWRCharacter_Base : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface, public ICWRTeamAgentInterface
{
	GENERATED_BODY()

public:

	ACWRCharacter_Base(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintImplementableEvent)
	void AttachWeapon(AActor* Weapon);
	
	static void PlayMontage(const USkeletalMeshComponent* MeshCompToPlayOn, UAnimMontage* MontageToPlay);
	
	void PlayMontage3P(UAnimMontage* MontageToPlay) const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetRestIdleAnim(UAnimSequence* InRestIdleAnim) { RestIdleAnim = InRestIdleAnim; }
	
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
	virtual void Tick(float DeltaSeconds) override;
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateRecoilParameters(ACWRWeaponActor* TargetWeapon);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_UpdateDT(FName WeaponSlot);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_EnterAiming();

	UFUNCTION(NetMulticast, Reliable)
	void MC_EnterAiming();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_ExitAiming();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_PlayUnequipWeaponMontageTP();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_UpdateCurrentWeapon(ACWRWeaponActor* NewCurrentWeapon);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SVR_Lean(float InLean);
	
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SVR_ArmsRotateLean(float InLeanHands);
	
	UFUNCTION(NetMulticast, Reliable)
	void MC_ExitAiming();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MC_UpdateDT(FName WeaponSlot);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void MC_PlayUnequipWeaponMontage();

	UPROPERTY(BlueprintAssignable)
	FOnUpdateDT OnUpdateDT;

	UFUNCTION(BlueprintCallable)
	void SetFiringTP(bool bInFiring);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ControllerPitchYawRecoil();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ApplyRecoil();
	
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
	//void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual bool CanJumpInternal_Implementation() const;

	virtual void InitializeDefaultAttributes() const;

	virtual void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	UFUNCTION(BlueprintCallable)
	virtual void AddInitialInventory();

	UFUNCTION(BlueprintCallable)
	void EnterAiming();

	UFUNCTION(BlueprintCallable)
	void ExitAiming();

	UFUNCTION(BlueprintImplementableEvent)
	void EnterAimingVFX();

	UFUNCTION(BlueprintImplementableEvent)
	void ExitAimingVFX();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ragdoll")
	float RagdollImpulseStrength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	TObjectPtr<UCurveFloat> PitchRecoilCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	TObjectPtr<UCurveFloat> YawRecoilCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float HipFireCurveRecoilMultiplier = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilRecoverSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float HipFireRecoilLocationMultiplier = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float HipFireRecoilRotationMultiplier = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float LengthOfWeapon = 55.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_bFiring, Category = "Weapon")
	bool bFiring = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bChangingWeapon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsLeaning = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float TimeFromAim = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float TimeToAim = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float CycleSightSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float ADSFov = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> WeaponMoveSound;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<ACWRWeaponActor> CurrentWeapon;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACWRWeaponActor> PrimaryWeapon;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ACWRWeaponActor> SecondaryWeapon;
	
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

	FORCEINLINE void SetLengthOfWeapon(const float InLengthOfWeapon) { LengthOfWeapon = InLengthOfWeapon; }
	FORCEINLINE void SetTimeFromAim(const float InTimeFromAim) { TimeFromAim = InTimeFromAim; }
	FORCEINLINE void SetTimeToAim(const float InTimeToAim) { TimeToAim = InTimeToAim; }
	FORCEINLINE void SetCycleSightSpeed(const float InCycleSightSpeed) { CycleSightSpeed = InCycleSightSpeed; }
	FORCEINLINE void SetADSFov(const float InADSFov) { ADSFov = InADSFov; }

	FORCEINLINE void SetCurrentWeapon(const TObjectPtr<ACWRWeaponActor>& InCurrentWeapon) { CurrentWeapon = InCurrentWeapon; }
	
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

	UPROPERTY()
	TArray<TObjectPtr<UCWRInventoryItemInstance>> InitialInventoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimSequence> RestIdleAnim = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseWalkSpeed = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardAxisValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	ECWRMovementMode MovementMode = ECWRMovementMode::Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWantsToRun = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float WeaponMoveDownAlpha = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float Lean = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float LeanHands = 0.f;

	void LowerWeapon();

	void UpdateWeaponParameters();
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> SupressedRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> ProjectilePredictionSpline;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCWRPawnExtensionComponent> PawnExtensionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CWR|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCWRHealthComponent> HealthComponent;
	
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

	void LocalEnterAiming();	
	void LocalExitAiming();	
	
	UFUNCTION(Client, Unreliable)
	void ROC_ADSFX();
	
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	UFUNCTION()
	void OnRep_bFiring();
	
	UFUNCTION()
	void OnRep_CurrentWeapon();

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
