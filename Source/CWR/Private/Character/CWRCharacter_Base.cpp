// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRCharacter_Base.h"

#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "Animation/CWRPreCMCTick.h"
#include "Character/CWRCharacterMovementComponent.h"
#include "Equipment/CWREquipmentManagerComponent.h"
#include "Equipment/CWRQuickBarComponent.h"
#include "Inventory/CWRInventoryManagerComponent.h"
#include "Weapons/CWRRangedWeaponInstance.h"


ACWRCharacter_Base::ACWRCharacter_Base(FObjectInitializer const& ObjectInitializer) : Super(
	ObjectInitializer
		.SetDefaultSubobjectClass<UCWRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	)
{

	PrimaryActorTick.bCanEverTick = true;

	
	EquipmentManagerComponent	= CreateDefaultSubobject<UCWREquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	CWRPreCMCTick = CreateDefaultSubobject<UCWRPreCMCTick>(TEXT("CWRPreCMCTick"));
}

UAbilitySystemComponent* ACWRCharacter_Base::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACWRCharacter_Base::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	
}

void ACWRCharacter_Base::InitAbilityActorInfo()
{
	
}

void ACWRCharacter_Base::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultBaseAttributesMax, 1.f);
	ApplyEffectToSelf(DefaultBaseAttributes, 1.f);
}


void ACWRCharacter_Base::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, GetAbilitySystemComponent());
}

void ACWRCharacter_Base::AddStartupAbilities() const
{
	AbilitySystemComponent->AddStartupAbilities(StartupAbilities);
}

void ACWRCharacter_Base::AddInitialInventory()
{
	if ( !HasAuthority() || !IsValid(GetController()) ) return;

	const auto InventoryManager = GetController()->GetComponentByClass<UCWRInventoryManagerComponent>();
	const auto QuickBar = GetController()->GetComponentByClass<UCWRQuickBarComponent>();

	if ( !IsValid(GetController()) || !IsValid(InventoryManager) || !IsValid(QuickBar) ) return;

	for ( int32 i = 0; i < InitialInventoryItems.Num(); ++i )
	{
		UCWRInventoryItemInstance* ItemInstance = InventoryManager->AddItemDefinition(InitialInventoryItems[i], 1);
		QuickBar->AddItemToSlot(i, ItemInstance);
	}

	if ( !InitialInventoryItems.IsEmpty() )
		QuickBar->SetActiveSlotIndex(0);
}

void ACWRCharacter_Base::PreCMCTick_Implementation()
{
	
}

void ACWRCharacter_Base::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}


void ACWRCharacter_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	//const double AdjustedPitch = FMath::Fmod(ControlPitch + 180.0, 360.0) - 180.0;
	const double AdjustedPitch = FMath::Clamp(ControlPitch, -90.0, 90.0);
	const double NewRoll = -AdjustedPitch / 3.0;
	return FRotator(NewRoll,0.0,0.0);
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

