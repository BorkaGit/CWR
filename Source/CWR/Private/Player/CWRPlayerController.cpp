// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CWRPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CWRGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "CWR/CWRCharacter_Player.h"
#include "GameFramework/Character.h"
#include "Input/CWRInputComponent.h"
#include "Inventory/CWRInventoryManagerComponent.h"
#include "Weapons/CWRWeaponStateComponent.h"


ACWRPlayerController::ACWRPlayerController()
{
	bReplicates = true;

	InventoryManagerComponent	= CreateDefaultSubobject<UCWRInventoryManagerComponent>(TEXT("InventoryManagerComponent"));
	WeaponStateComponent		= CreateDefaultSubobject<UCWRWeaponStateComponent>(TEXT("WeaponStateComponent"));
}

void ACWRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ACWRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(CWRContext);

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedSubsystem->AddMappingContext(CWRContext, 0);
	}
}

void ACWRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	CWRInputComponent = CastChecked<UCWRInputComponent>(InputComponent);

	CWRInputComponent->BindNativeAction(InputConfig,FCWRGameplayTags::Get().InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move, false );
	CWRInputComponent->BindNativeAction(InputConfig,FCWRGameplayTags::Get().InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::LookMouse, false );
	
	CWRInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void ACWRPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), MovementVector.Y);
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), MovementVector.X);
	}
}

void ACWRPlayerController::LookMouse(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACWRPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	
}

void ACWRPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void ACWRPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

UCWRAbilitySystemComponent* ACWRPlayerController::GetAbilitySystemComponent()
{
	if (!CWRAbilitySystemComponent)
	{
	//	CWRAbilitySystemComponent = Cast<UCWRAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		if (const auto CWRCharacter = Cast<ACWRCharacter_Player>(GetCharacter()))
		{
			CWRAbilitySystemComponent = Cast<UCWRAbilitySystemComponent>(CWRCharacter->GetAbilitySystemComponent());
		}
	}
	return CWRAbilitySystemComponent;
}
