// Copyright Epic Games, Inc. All Rights Reserved.

#include "CWRCharacter_Player.h"

//#include "AkComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystem/CWRAttributeSet.h"
#include "Character/CWRCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Inventory/CWRInventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/CWRPlayerController.h"
#include "Player/CWRPlayerState.h"
#include "System/CWRGameInstance.h"
#include "UI/HUD/CWRHUD.h"
#include "Weapons/CWRDroppedMagazineActor.h"
#include "Weapons/CWRRangedWeaponInstance.h"
#include "Weapons/CWRWeaponActor.h"
#include "Weapons/CWRWeaponInstance.h"


ACWRCharacter_Player::ACWRCharacter_Player(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	
	FP_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Legs"));
	FP_Legs->SetupAttachment(GetCapsuleComponent());
	FP_Legs->SetCastShadow(false);
	FP_Legs->SetOnlyOwnerSee(true);

	FP_Base = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Base"));
	FP_Base->SetupAttachment(GetCapsuleComponent());

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
}

void ACWRCharacter_Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	//InitAbilityActorInfo();
}

void ACWRCharacter_Player::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACWRCharacter_Player, SightTransform);
}

void ACWRCharacter_Player::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	//InitAbilityActorInfo();
}
/*
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
}*/

void ACWRCharacter_Player::InitializeDefaultAttributes() const
{
	Super::InitializeDefaultAttributes();
}

void ACWRCharacter_Player::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	Super::ApplyEffectToSelf(GameplayEffectClass, Level);
}

void ACWRCharacter_Player::AddInitialInventory()
{
	InitialInventoryItems = Cast<UCWRGameInstance>(GetGameInstance())->GetInitialInventoryItems();

	const auto InventoryManager = GetController()->GetComponentByClass<UCWRInventoryManagerComponent>();

	if ( !IsValid(InventoryManager)) return;
	
	for ( TObjectPtr<UCWRInventoryItemInstance> InventoryItem : InitialInventoryItems)
	{
		InventoryManager->AddItemInstance(InventoryItem);
	}
	
	Super::AddInitialInventory();
}

void ACWRCharacter_Player::OnRep_SightTransform()
{
	SightTransformChanged();
}

void ACWRCharacter_Player::ROC_SpawnMag_Implementation()
{
	if ( CurrentWeapon )
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.bDeferConstruction = true;
	
		ACWRDroppedMagazineActor* DroppedMagazineActor = GetWorld()->SpawnActor<ACWRDroppedMagazineActor>(ACWRDroppedMagazineActor::StaticClass(),Mesh1P->GetSocketTransform("hand_l"),SpawnParameters );
		DroppedMagazineActor->SetMagazineMesh(CurrentWeapon->GetDroppedMagazineMesh());
		DroppedMagazineActor->SetMagazineDropSound(CurrentWeapon->GetMagazineDropSound());
		DroppedMagazineActor->SetMagazineDropSoundAttenuation(CurrentWeapon->GetMagazineDropSoundAttenuation());
		DroppedMagazineActor->FinishSpawning(Mesh1P->GetSocketTransform("hand_l"), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);;
	}
}

void ACWRCharacter_Player::PlayMontage1P(UAnimMontage* MontageToPlay) const
{
	PlayMontage(Mesh1P, MontageToPlay);
}

void ACWRCharacter_Player::BeginPlay()
{
	Super::BeginPlay();

	FP_Legs->HideBoneByName("spine_03",PBO_None);
}

void ACWRCharacter_Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}





