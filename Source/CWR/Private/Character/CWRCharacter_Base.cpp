// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRCharacter_Base.h"

#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "Animation/CWRPreCMCTick.h"
#include "Camera/CameraComponent.h"
#include "Character/CWRCharacterMovementComponent.h"
#include "Character/CWRHealthComponent.h"
#include "Character/CWRPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/CWREquipmentManagerComponent.h"
#include "Equipment/CWRQuickBarComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/CWRInventoryItemDefinition.h"
#include "Inventory/CWRInventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Player/CWRPlayerController.h"
#include "Player/CWRPlayerState.h"
#include "Weapons/CWRRangedWeaponInstance.h"

static FName NAME_CWRCharacterCollisionProfile_Capsule(TEXT("CWRPawnCapsule"));
static FName NAME_CWRCharacterCollisionProfile_Mesh(TEXT("CWRPawnMesh"));
static FName NAME_CWRCharacterCollisionProfile_Ragdoll(TEXT("Ragdoll"));
static FName NAME_RagdollImpulseBone(TEXT("Pelvis"));

ACWRCharacter_Base::ACWRCharacter_Base(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCWRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetNetCullDistanceSquared(900000000.0f);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionProfileName(NAME_CWRCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetCollisionProfileName(NAME_CWRCharacterCollisionProfile_Mesh);

	FP_Base = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Base"));
	FP_Base->SetupAttachment(CapsuleComp);

	CB_MeshRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("CB_MeshRoot"));
	CB_MeshRoot->TargetArmLength = 0.f;
	CB_MeshRoot->bDoCollisionTest = false;
	CB_MeshRoot->bUsePawnControlRotation = true;
	CB_MeshRoot->bInheritRoll = false;
	CB_MeshRoot->SetupAttachment(FP_Base);

	FP_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Offset"));
	FP_Offset->SetupAttachment(CB_MeshRoot);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetCastShadow(false);
	Mesh1P->SetupAttachment(FP_Offset);

	CB_Camera = CreateDefaultSubobject<USpringArmComponent>(TEXT("CB_Camera"));
	CB_Camera->TargetArmLength = 0.f;
	CB_Camera->bDoCollisionTest = false;
	CB_Camera->bUsePawnControlRotation = true;
	CB_Camera->bInheritRoll = false;
	CB_Camera->SetupAttachment(Mesh1P, Socket1P);

	Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera1P"));
	Camera1P->bUsePawnControlRotation = true;
	Camera1P->SetupAttachment(CB_Camera);
	
	auto CWRMovementComponent = CastChecked<UCWRCharacterMovementComponent>(GetCharacterMovement());
	CWRMovementComponent->GravityScale = 1.0f;
	CWRMovementComponent->MaxAcceleration = 2400.0f;
	CWRMovementComponent->BrakingFrictionFactor = 1.0f;
	CWRMovementComponent->BrakingFriction = 6.0f;
	CWRMovementComponent->GroundFriction = 8.0f;
	CWRMovementComponent->BrakingDecelerationWalking = 1400.0f;
	CWRMovementComponent->bUseControllerDesiredRotation = false;
	CWRMovementComponent->bOrientRotationToMovement = false;
	CWRMovementComponent->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	CWRMovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = false;
	CWRMovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
	CWRMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
	CWRMovementComponent->SetCrouchedHalfHeight(65.0f);
	
	PawnExtensionComponent = CreateDefaultSubobject<UCWRPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtensionComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtensionComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));
	AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	AIPerceptionStimuliSourceComponent->RegisterForSense(UAISense_Hearing::StaticClass());
	
	HealthComponent = CreateDefaultSubobject<UCWRHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	
	CWRPreCMCTick = CreateDefaultSubobject<UCWRPreCMCTick>(TEXT("CWRPreCMCTick"));
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void ACWRCharacter_Base::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ACWRCharacter_Base::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void ACWRCharacter_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACWRCharacter_Base::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ACWRCharacter_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME(ThisClass, MyTeamID)
}

void ACWRCharacter_Base::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void ACWRCharacter_Base::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (ICWRTeamAgentInterface* ControllerWithTeam = Cast<ICWRTeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}

ACWRPlayerController* ACWRCharacter_Base::GetCWRPlayerController() const
{
	return CastChecked<ACWRPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ACWRPlayerState* ACWRCharacter_Base::GetCWRPlayerState() const
{
	return CastChecked<ACWRPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UCWRAbilitySystemComponent* ACWRCharacter_Base::GetCWRAbilitySystemComponent() const
{
	return Cast<UCWRAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ACWRCharacter_Base::GetAbilitySystemComponent() const
{
	if ( !PawnExtensionComponent )
	{
		return nullptr;
	}

	return PawnExtensionComponent->GetCWRAbilitySystemComponent();
}

void ACWRCharacter_Base::OnAbilitySystemInitialized()
{
	UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent();
	check(CWRAbilitySystemComponent);

	HealthComponent->InitializeWithAbilitySystem(CWRAbilitySystemComponent);
	
	InitializeGameplayTags();
}

void ACWRCharacter_Base::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ACWRCharacter_Base::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtensionComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (ICWRTeamAgentInterface* ControllerAsTeamProvider = Cast<ICWRTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);

		if ( bIsWaitingForTeamSet )
		{
			bIsWaitingForTeamSet = false;
			OnTeamSetDelegate.Broadcast();
		}
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ACWRCharacter_Base::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (ICWRTeamAgentInterface* ControllerAsTeamProvider = Cast<ICWRTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	PawnExtensionComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ACWRCharacter_Base::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtensionComponent->HandleControllerChanged();
}

void ACWRCharacter_Base::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtensionComponent->HandlePlayerStateReplicated();
}

void ACWRCharacter_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtensionComponent->SetupPlayerInputComponent();
}

void ACWRCharacter_Base::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	/*
	if (UCWRAbilitySystemComponent* CWRAbilitySystemComponent = UCWRAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : CWRGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				CWRASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : CWRGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				CWRASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}*/

		UCWRCharacterMovementComponent* CWRMoveComp = CastChecked<UCWRCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(CWRMoveComp->MovementMode, CWRMoveComp->CustomMovementMode, true);
}

void ACWRCharacter_Base::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent())
	{
		CWRAbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	}
}

bool ACWRCharacter_Base::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent())
	{
		return CWRAbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ACWRCharacter_Base::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent())
	{
		return CWRAbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ACWRCharacter_Base::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent())
	{
		return CWRAbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ACWRCharacter_Base::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void ACWRCharacter_Base::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
	
	
	UAnimMontage* RandomDeathMontage = DeathMontages[FMath::RandRange(0,DeathMontages.Num() - 1)];
	PlayAnimMontage(RandomDeathMontage);

	AIPerceptionStimuliSourceComponent->UnregisterFromSense(UAISense_Sight::StaticClass());
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
{
	Ragdoll();
	HideEquippedWeapons();
	Death();	
		
}, FMath::FRandRange(0.1f, 0.6f), false);
	
}

void ACWRCharacter_Base::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ACWRCharacter_Base::Ragdoll() const
{
	GetMesh()->SetCollisionProfileName(NAME_CWRCharacterCollisionProfile_Ragdoll);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_RagdollImpulseBone, true);
	
	FVector LastUpdateVelocity = GetCharacterMovement()->GetLastUpdateVelocity();
	
	GetMesh()->AddImpulse(LastUpdateVelocity + LastUpdateVelocity.Normalize(0.0001) * RagdollImpulseStrength, NAME_RagdollImpulseBone, true);
}

void ACWRCharacter_Base::HideEquippedWeapons() const
{
	const auto EquipmentManager = GetComponentByClass<UCWREquipmentManagerComponent>();
	if ( IsValid(EquipmentManager) )
	{
		for (const auto WeaponInstance: EquipmentManager->GetEquipmentInstancesOfType(UCWRWeaponInstance::StaticClass()) )
		{
			for (const auto SpawnedActor : WeaponInstance->GetSpawnedActors() )
			{
				SpawnedActor->SetActorHiddenInGame(true);
			}
		}
	}
}

void ACWRCharacter_Base::Death()
{
	
}

void ACWRCharacter_Base::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCWRCharacterMovementComponent* CWRMovementComponent = CastChecked<UCWRCharacterMovementComponent>(GetCharacterMovement());
	CWRMovementComponent->StopMovementImmediately();
	CWRMovementComponent->DisableMovement();
}

void ACWRCharacter_Base::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void ACWRCharacter_Base::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UCWRAbilitySystemComponent* CWRAbilitySystemComponent = GetCWRAbilitySystemComponent())
	{
		if (CWRAbilitySystemComponent->GetAvatarActor() == this)
		{
			PawnExtensionComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ACWRCharacter_Base::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UCWRCharacterMovementComponent* CWRMoveComp = CastChecked<UCWRCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(CWRMoveComp->MovementMode, CWRMoveComp->CustomMovementMode, true);
}

void ACWRCharacter_Base::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	/*
	if (UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = CWRGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = CWRGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			CWRASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}*/
}

bool ACWRCharacter_Base::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void ACWRCharacter_Base::OnRep_ReplicatedAcceleration()
{
	if (UCWRCharacterMovementComponent* CWRMovementComponent = Cast<UCWRCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel         = CWRMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		CWRMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void ACWRCharacter_Base::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId ACWRCharacter_Base::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnCWRTeamIndexChangedDelegate* ACWRCharacter_Base::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ACWRCharacter_Base::OnRep_SightTransform()
{
	SightTransformChanged();
}

void ACWRCharacter_Base::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void ACWRCharacter_Base::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

bool ACWRCharacter_Base::UpdateSharedReplication()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FSharedRepMovement SharedMovement;
		if (SharedMovement.FillForCharacter(this))
		{
			// Only call FastSharedReplication if data has changed since the last frame.
			// Skipping this call will cause replication to reuse the same bunch that we previously
			// produced, but not send it to clients that already received. (But a new client who has not received
			// it, will get it this frame)
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				LastSharedReplication = SharedMovement;
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				FastSharedReplication(SharedMovement);
			}
			return true;
		}
	}

	// We cannot fastrep right now. Don't send anything.
	return false;
}

void ACWRCharacter_Base::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// Timestamp is checked to reject old moves.
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Timestamp
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// Movement mode
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// Location, Rotation, Velocity, etc.
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// This also sets LastRepMovement
		OnRep_ReplicatedMovement();

		// Jump force
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// Crouch
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}

void ACWRCharacter_Base::InitializeDefaultAttributes() const
{
	//ApplyEffectToSelf(DefaultBaseAttributesMax, 1.f);
	//ApplyEffectToSelf(DefaultBaseAttributes, 1.f);
}


void ACWRCharacter_Base::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, GetAbilitySystemComponent());
}

void ACWRCharacter_Base::AddInitialInventory()
{
	if ( !HasAuthority() || !IsValid(GetController()) ) return;

	const auto InventoryManager = GetController()->GetComponentByClass<UCWRInventoryManagerComponent>();
	const auto QuickBar = GetController()->GetComponentByClass<UCWRQuickBarComponent>();

	if ( !IsValid(InventoryManager) || !IsValid(QuickBar) ) return;

	for ( int32 i = 0; i < InitialInventoryItems.Num(); ++i )
	{
		UCWRInventoryItemInstance* ItemInstance = InventoryManager->AddItemDefinition(InitialInventoryItems[i], 1);
		const int32 ItemSlot = QuickBar->FindSlotIndex(InitialInventoryItems[i]);
		QuickBar->AddItemToSlot(ItemSlot, ItemInstance);
	}

	if ( !InitialInventoryItems.IsEmpty() )
		QuickBar->SetActiveSlotIndex(QuickBar->FindBestPrioritySlotIndex());
}

void ACWRCharacter_Base::PickUpItem(TSubclassOf<UCWRInventoryItemDefinition> ItemDefinition)
{
	if ( !HasAuthority() || !IsValid(GetController()) ) return;

	const auto InventoryManager = GetController()->GetComponentByClass<UCWRInventoryManagerComponent>();
	const auto QuickBar = GetController()->GetComponentByClass<UCWRQuickBarComponent>();

	if ( !IsValid(InventoryManager) || !IsValid(QuickBar) ) return;

	const int32 ItemSlot = QuickBar->FindSlotIndex(ItemDefinition);
	
	if (const UCWRInventoryItemInstance* PrevInventoryItem = QuickBar->GetItemAtSlot(ItemSlot))
	{
		if ( ItemDefinition == PrevInventoryItem->GetItemDef() )
		{
			return;
		}
	}
	
	UCWRInventoryItemInstance* ItemInstance = InventoryManager->AddItemDefinition(ItemDefinition, 1);

	
	QuickBar->AddItemToSlot(ItemSlot, ItemInstance);
	QuickBar->SetActiveSlotIndex(QuickBar->FindBestPrioritySlotIndex());
}

void ACWRCharacter_Base::PreCMCTick_Implementation()
{
	
}

void ACWRCharacter_Base::SetSprinting(bool IsSprinting) const
{
	if ( GetCharacterMovement() )
	{
		Cast<UCWRCharacterMovementComponent>(GetCharacterMovement() )->SetWalkingType( IsSprinting ? ECWRWalkingTypes::Sprint : ECWRWalkingTypes::Jog );
	}
}

bool ACWRCharacter_Base::IsSprinting() const
{
	if ( GetCharacterMovement() && !GetCharacterMovement()->IsFalling() )
		return Cast<UCWRCharacterMovementComponent>(GetCharacterMovement())->GetWalkingType() == ECWRWalkingTypes::Sprint
				 && GetCharacterMovement()->GetLastUpdateVelocity().Size() > Cast<UCWRCharacterMovementComponent>(GetCharacterMovement())->GetWalkingSpeedByType(ECWRWalkingTypes::Jog) ;
	
	return false;
}

void ACWRCharacter_Base::SetWalking(bool IsWalking) const
{
	if ( GetCharacterMovement() )
	{
		Cast<UCWRCharacterMovementComponent>(GetCharacterMovement() )->SetWalkingType( IsWalking ? ECWRWalkingTypes::Walk : ECWRWalkingTypes::Jog );
	}
}

bool ACWRCharacter_Base::IsWalking() const
{
	if ( GetCharacterMovement() && !GetCharacterMovement()->IsFalling() )
		return Cast<UCWRCharacterMovementComponent>(GetCharacterMovement())->GetWalkingType() == ECWRWalkingTypes::Walk;
	
	return false;
}

void ACWRCharacter_Base::DropItem()
{
	if (const auto RangedWeapon = Cast<UCWRRangedWeaponInstance>(GetItemInHands()) )
	{
		RangedWeapon->RemoveAbilitiesFromHolder();
	
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, true);
		//Weapon->GetWeaponMesh1P()->DetachFromComponent(DetachmentTransformRules);
		//Weapon->GetWeaponMesh3P()->DetachFromComponent(DetachmentTransformRules);
		//Weapon = nullptr;
	}
}

void ACWRCharacter_Base::OnAimingStarted_Implementation()
{
}

void ACWRCharacter_Base::OnAimingEnded_Implementation()
{
}

FTransform ACWRCharacter_Base::GetLeftHandSocketTransform() const
{
	if (const auto RangedWeapon = Cast<UCWRRangedWeaponInstance>(GetItemInHands()) )
	{
		FVector OutLocation = FVector::ZeroVector;
		FRotator OutRotation = FRotator::ZeroRotator;
		
		const FTransform WeaponTransform = RangedWeapon->GetLeftHandGripTransform();

		const USkeletalMeshComponent* MeshToTransform = GetMesh();
		
		MeshToTransform->TransformToBoneSpace(RightHandSocketName,WeaponTransform.GetLocation(),WeaponTransform.Rotator(),OutLocation, OutRotation);
		
		return FTransform(OutRotation, OutLocation, FVector::OneVector);
	}
	
	UE_LOG(LogTemp, Error, TEXT("Weapon is not valid while trying to get its socket"))
	return FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
}

FRotator ACWRCharacter_Base::GetAimRotation() const
{
	const double ControlPitch = GetControlRotation().Pitch;
	const double A = ControlPitch > 180.0 ? 360.0 - ControlPitch : ControlPitch * -1.0;
	const double NewRoll = A / 5.0;
	return FRotator(0.0, 0.0, NewRoll);
	/*
	const double ControlPitch = GetControlRotation().Pitch;
	//const double AdjustedPitch = FMath::Fmod(ControlPitch + 180.0, 360.0) - 180.0;
	const double AdjustedPitch = FMath::Clamp(ControlPitch, -90.0, 90.0);
	const double NewRoll = -AdjustedPitch / 3.0;
	return FRotator(NewRoll,0.0,0.0);*/
}

UCWREquipmentInstance* ACWRCharacter_Base::GetItemInHands() const
{
	if (const auto PlayerController = GetController())
	{
		if (const auto QuickBarComponent = PlayerController->GetComponentByClass<UCWRQuickBarComponent>())
		{
			return QuickBarComponent->GetActiveEquipmentInstance();
		}
	}
	return nullptr;
}

