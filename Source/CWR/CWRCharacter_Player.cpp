// Copyright Epic Games, Inc. All Rights Reserved.

#include "CWRCharacter_Player.h"

#include "AkComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystem/CWRAttributeSet.h"
#include "Character/CWRCharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CWRPlayerController.h"
#include "Player/CWRPlayerState.h"
#include "UI/HUD/CWRHUD.h"
#include "Weapons/CWRRangedWeaponInstance.h"
#include "Weapons/CWRWeaponInstance.h"


ACWRCharacter_Player::ACWRCharacter_Player(FObjectInitializer const& ObjectInitializer) : Super(
	ObjectInitializer
		.SetDefaultSubobjectClass<UCWRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	)
{


	SpringArmFP = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmFP"));
	SpringArmFP->SetupAttachment(GetMesh(), FPSpringArmSocketName);
	SpringArmFP->TargetArmLength = 0.f;

	CameraFP = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFP"));
	CameraFP->SetupAttachment(SpringArmFP);
	
	/*
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(CameraComponent);
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->CastShadow = false;
	ArmsMesh->bCastHiddenShadow = false;*/
	
}

void ACWRCharacter_Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	AddStartupAbilities();
}

void ACWRCharacter_Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void ACWRCharacter_Player::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	if ( const auto CWRPlayerState = GetPlayerState<ACWRPlayerState>())
	{
		AbilitySystemComponent = Cast<UCWRAbilitySystemComponent>(CWRPlayerState->GetAbilitySystemComponent());
		
		AttributeSet = CWRPlayerState->GetAttributeSet();
		
		AbilitySystemComponent->InitAbilityActorInfo(CWRPlayerState, this);
		AbilitySystemComponent->AbilityActorInfoSet();

		InitializeDefaultAttributes();
		
		if ( ACWRPlayerController* CWRPlayerController =  Cast<ACWRPlayerController>(GetController()) )
		{
			if ( ACWRHUD* CWRHUD =  Cast<ACWRHUD>(CWRPlayerController->GetHUD()) )
			{
				CWRHUD->InitOverlay(CWRPlayerController, CWRPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}
}

void ACWRCharacter_Player::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
}

void ACWRCharacter_Player::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	Super::ApplyEffectToSelf(GameplayEffectClass, Level);
}

void ACWRCharacter_Player::AddStartupAbilities() const
{
	Super::AddStartupAbilities();
}

void ACWRCharacter_Player::BeginPlay()
{
	Super::BeginPlay();
}

void ACWRCharacter_Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/*
	if (MoveAction)
	{
		const FVector2d SideMovVector = EnhancedInputComponent->BindActionValue(MoveAction).GetValue().Get<FVector2d>();
		const double SideMovClamped = FMath::GetMappedRangeValueClamped(FVector2d(-1.f,1.f), FVector2d(-10.f, 10.f), SideMovVector.X);
		HandSwayRotator.Pitch = FMath::FInterpTo(HandSwayRotator.Pitch, SideMovClamped, DeltaSeconds, 5.f);
	}
	if (LookAction)
	{
		const FVector2d MouseVector = EnhancedInputComponent->BindActionValue(LookAction).GetValue().Get<FVector2d>();
		const double MouseVectorXClamped = FMath::GetMappedRangeValueClamped(FVector2d(-1.f,1.f), FVector2d(-10.f, 10.f), MouseVector.X);
		HandSwayRotator.Yaw = FMath::FInterpTo(HandSwayRotator.Yaw, MouseVectorXClamped, DeltaSeconds, 5.f);

		const double MouseVectorYClamped = FMath::GetMappedRangeValueClamped(FVector2d(1.f,-1.f), FVector2d(-10.f, 10.f), MouseVector.Y);
		HandSwayRotator.Roll = FMath::FInterpTo(HandSwayRotator.Roll, MouseVectorYClamped, DeltaSeconds, 5.f);
		
	}*/

	if (bIsAiming)
	{
		AimCurrent = FMath::FInterpTo(AimCurrent, AimTarget, DeltaSeconds, 14.f);
		const FVector NewLocation =  FMath::Lerp(FVector::ZeroVector,Cast<UCWRRangedWeaponInstance>(GetItemInHands())->GetAimOffsetLocation(), AimCurrent );
		
		SpringArmFP->SetRelativeLocation(NewLocation);
	}
}

void ACWRCharacter_Player::DropItem()
{
	Super::DropItem();
}

void ACWRCharacter_Player::ControllerRecoil_Implementation(float RecoilAmount)
{
	
}

FVector ACWRCharacter_Player::GetCameraLocation() const
{
	return CameraFP->GetComponentLocation();
}

FVector ACWRCharacter_Player::GetCameraEndTraceLocation(const float Distance) const
{
	return GetCameraLocation() + Distance * CameraFP->GetForwardVector();
}



