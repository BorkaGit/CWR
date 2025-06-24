// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/CWREquipmentInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Equipment/CWREquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "AbilitySystem/CWRGameplayAbility.h"
#include "CWR/CWRCharacter_Player.h"
#include "Inventory/CWRInventoryItemInstance.h"
#include "Weapons/CWRRangedWeaponInstance.h"
#include "Weapons/CWRWeaponActor.h"
#include "Weapons/InventoryFragment_SkeletalMesh.h"


class FLifetimeProperty;
class UClass;
class USceneComponent;

UCWREquipmentInstance::UCWREquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UCWREquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void UCWREquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

#if UE_WITH_IRIS
void UCWREquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

APawn* UCWREquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

ACWRCharacter_Base* UCWREquipmentInstance::GetCWRCharacter() const
{
	return Cast<ACWRCharacter_Base>(GetOuter());
}

APawn* UCWREquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UCWREquipmentInstance::SpawnEquipmentActors(const TArray<FCWREquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		if (ACWRCharacter_Base* CWRCharacter = Cast<ACWRCharacter_Base>(OwningPawn))
		{
			for (const FCWREquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
			{
				FActorSpawnParameters SpawnParameters;
				AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, OwningPawn->GetActorTransform(), OwningPawn);

				const auto WeaponActor = Cast<ACWRWeaponActor>(NewActor);
				
				if (const auto RangedWeaponInstance = Cast<UCWRRangedWeaponInstance>(this) )
				{
					if (WeaponActor)
					{
						if ( const auto CWRInventoryItemInstance = Cast<UCWRInventoryItemInstance>(GetInstigator()) )
						{
							if (const auto SkeletalMeshFragment = CWRInventoryItemInstance->FindFragmentByClass<UInventoryFragment_SkeletalMesh>() )
							{
								WeaponActor->GetFPMesh()->SetSkeletalMeshAsset(SkeletalMeshFragment->SkeletalMesh);
								WeaponActor->GetTPMesh()->SetSkeletalMeshAsset(SkeletalMeshFragment->SkeletalMesh);
							}

							WeaponActor->SetAttachments(CWRInventoryItemInstance->GetAttachments());
						}
					}
				}
				
				NewActor->FinishSpawning(OwningPawn->GetActorTransform(), true);

				if ( WeaponActor )
				{

					if (const ACWRCharacter_Player* PlayerCharacter = Cast<ACWRCharacter_Player>(CWRCharacter))
					{
						FAttachmentTransformRules FPAttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true );
						WeaponActor->GetFPMesh()->AttachToComponent(PlayerCharacter->GetMesh1P(), FPAttachmentRules,WeaponActor->GetFPSocketAttach());
					}

					FAttachmentTransformRules TPAttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true );
					WeaponActor->GetTPMesh()->AttachToComponent(CWRCharacter->GetMesh(), TPAttachmentRules, WeaponActor->GetTPSocketAttach());


					CWRCharacter->UpdateRecoilParameters(WeaponActor);
					CWRCharacter->SVR_UpdateDT(WeaponActor->GetAnimationSlot());

					WeaponActor->CalculateSightTransform();
					CWRCharacter->SetLengthOfWeapon(WeaponActor->GetLengthOfWeapon());
					CWRCharacter->SetTimeFromAim(WeaponActor->GetAimOutSpeed());
					CWRCharacter->SetTimeToAim(WeaponActor->GetAimInSpeed());
					CWRCharacter->SetCycleSightSpeed(WeaponActor->GetCycleSightSpeed());
					CWRCharacter->SetADSFov(WeaponActor->GetADSFov());
					
					CWRCharacter->SetCurrentWeapon(WeaponActor);
					CWRCharacter->AttachWeapon(NewActor);
				}
				
				SpawnedActors.Add(NewActor);
			}
		}
	}
}

void UCWREquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UCWREquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UCWREquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void UCWREquipmentInstance::RemoveAbilitiesFromHolder()
{
	if (const auto HolderCharacter = Cast<ACWRCharacter_Player>(GetPawn()) )
	{
		const auto AbilitiesComponent = HolderCharacter->GetAbilitySystemComponent();
		for (auto const& AbilityHandle : GrantedAbilityHandles)
		{
			FGameplayAbilitySpec* AbilitySpec = AbilitiesComponent->FindAbilitySpecFromHandle(AbilityHandle);
			if (const auto CWRAbility = Cast<UCWRGameplayAbility>(AbilitySpec->Ability))
			{
			//	AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(CWRAbility->StartupInputTag);
				AbilitiesComponent->ClearAbility(AbilityHandle);
			}
		}
	}
}

void UCWREquipmentInstance::GiveAbilitiesToHolder(UCWRAbilitySystemComponent* ASC)
{
	for (auto const& GrantAbility : GrantAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(GrantAbility, 1);
		if (const auto CWRAbility = Cast<UCWRGameplayAbility>(AbilitySpec.Ability))
		{
			//AbilitySpec.GetDynamicSpecSourceTags().AddTag(CWRAbility->StartupInputTag);
			GrantedAbilityHandles.Add(ASC->GiveAbility(AbilitySpec));
		}
		
	}
}

void UCWREquipmentInstance::OnRep_Instigator()
{
}



