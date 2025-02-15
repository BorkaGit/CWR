// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRWeaponInstance.h"

#include "GameFramework/InputDeviceSubsystem.h"

UCWRWeaponInstance::UCWRWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Listen for death of the owning pawn so that any device properties can be removed if we
	// die and can't unequip
	//if (APawn* Pawn = GetPawn())
	//{
		// We only need to do this for player controlled pawns, since AI and others won't have input devices on the client
	//	if (Pawn->IsPlayerControlled())
	//	{
	//		if (UCWRHealthComponent* HealthComponent = UCWRHealthComponent::FindHealthComponent(GetPawn()))
	//		{
	//			HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	//		}
	//	}
//	}
}

void UCWRWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	const UWorld* World = GetWorld();
	check(World);
	TimeLastEquipped = World->GetTimeSeconds();
	
	ApplyDeviceProperties();
}

void UCWRWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

	RemoveDeviceProperties();
}

void UCWRWeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World);
	TimeLastFired = World->GetTimeSeconds();
}

float UCWRWeaponInstance::GetTimeSinceLastInteractedWith() const
{
	UWorld* World = GetWorld();
	check(World);
	const double WorldTime = World->GetTimeSeconds();

	double Result = WorldTime - TimeLastEquipped;

	if (TimeLastFired > 0.0)
	{
		const double TimeSinceFired = WorldTime - TimeLastFired;
		Result = FMath::Min(Result, TimeSinceFired);
	}

	return Result;
}

TSubclassOf<UAnimInstance> UCWRWeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
	//const FCWRAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
	//return SetToQuery.SelectBestLayer(CosmeticTags);
	return UAnimInstance::StaticClass();
}

const FPlatformUserId UCWRWeaponInstance::GetOwningUserId() const
{
	if (const APawn* Pawn = GetPawn())
	{
		return Pawn->GetPlatformUserId();
	}
	return PLATFORMUSERID_NONE;
}

void UCWRWeaponInstance::ApplyDeviceProperties()
{
	const FPlatformUserId UserId = GetOwningUserId();

	if (UserId.IsValid())
	{
		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
		{
			for (TObjectPtr<UInputDeviceProperty>& DeviceProp : ApplicableDeviceProperties)
			{
				FActivateDevicePropertyParams Params = {};
				Params.UserId = UserId;

				// By default, the device property will be played on the Platform User's Primary Input Device.
				// If you want to override this and set a specific device, then you can set the DeviceId parameter.
				//Params.DeviceId = <some specific device id>;
				
				// Don't remove this property it was evaluated. We want the properties to be applied as long as we are holding the 
				// weapon, and will remove them manually in OnUnequipped
				Params.bLooping = true;
			
				DevicePropertyHandles.Emplace(InputDeviceSubsystem->ActivateDeviceProperty(DeviceProp, Params));
			}
		}	
	}
}

void UCWRWeaponInstance::RemoveDeviceProperties()
{
	const FPlatformUserId UserId = GetOwningUserId();
	
	if (UserId.IsValid() && !DevicePropertyHandles.IsEmpty())
	{
		// Remove any device properties that have been applied
		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
		{
			InputDeviceSubsystem->RemoveDevicePropertyHandles(DevicePropertyHandles);
			DevicePropertyHandles.Empty();
		}
	}
}

void UCWRWeaponInstance::OnDeathStarted(AActor* OwningActor)
{
	// Remove any possibly active device properties when we die to make sure that there aren't any lingering around
	RemoveDeviceProperties();
}



