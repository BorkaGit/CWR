// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRRangedWeaponInstance.h"
#include "CWR/CWRCharacter_Player.h"
#include "Weapons/CWRWeaponActor.h"

UCWRRangedWeaponInstance::UCWRRangedWeaponInstance()
{
	CurrentBulletsAmount = MagazineSize;
	BackpackBulletsAmount = (MagazinesAmount - 1) * MagazineSize;
}

void UCWRRangedWeaponInstance::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

#if WITH_EDITOR
void UCWRRangedWeaponInstance::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDebugVisualization();
}

void UCWRRangedWeaponInstance::UpdateDebugVisualization()
{
	
}
#endif

void UCWRRangedWeaponInstance::Tick(float DeltaSeconds)
{
}

void UCWRRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
	GetCWRCharacter()->SetHasWeaponInHands(true);
}

void UCWRRangedWeaponInstance::OnUnequipped()
{
	GetCWRCharacter()->SetHasWeaponInHands(false);
	Super::OnUnequipped();
}

void UCWRRangedWeaponInstance::SpawnEquipmentActors(const TArray<FCWREquipmentActorToSpawn>& ActorsToSpawn)
{
	Super::SpawnEquipmentActors(ActorsToSpawn);

	for (AActor* SpawnedActor : GetSpawnedActors())
	{
		if (const auto RangedWeaponActor =  Cast<ACWRWeaponActor>(SpawnedActor))
		{
			for (const auto Attachment : Attachments)
			{	
				RangedWeaponActor->SpawnAttachment(Attachment);	
			}
		}
	}
}

void UCWRRangedWeaponInstance::AddSpread()
{
	SetRecoilTransform();

	GetCWRCharacter()->ControllerRecoil(RecoilAmount);
	
}

void UCWRRangedWeaponInstance::DecrementBulletsAmount()
{
	--CurrentBulletsAmount;
	GetCWRCharacter()->OnBulletAmountChanged.Broadcast(CurrentBulletsAmount);
}

bool UCWRRangedWeaponInstance::CanReload() const
{
	return BackpackBulletsAmount > 0 && CurrentBulletsAmount < MagazineSize;
}

void UCWRRangedWeaponInstance::SetRecoilTransform()
{
	FRotator RecoilRotation;
	
	RecoilRotation.Roll = RecoilMultiplier * FMath::FRandRange(-2.5f, -5.0f);
	RecoilRotation.Pitch = RecoilMultiplier * FMath::FRandRange(-0.8f, 0.8f);
	RecoilRotation.Yaw = RecoilMultiplier * FMath::FRandRange(-1.6f, 1.6f);

	
	FVector RecoilLocation;

	RecoilLocation.X = RecoilMultiplier * FMath::FRandRange(-0.16f, 0.16f);
	RecoilLocation.Y = RecoilMultiplier * FMath::FRandRange(-1.1f, -2.1f);
	RecoilLocation.Z = RecoilMultiplier * FMath::FRandRange(0.0f, 0.0f);

	RecoilTransform = FTransform(RecoilRotation, RecoilLocation, FVector::OneVector);
}

void UCWRRangedWeaponInstance::Fire()
{
	UpdateFiringTime();
	/*	
	if ( FireSound )
		AkComponent->PostAkEvent(FireSound);
	
	if ( !bWithSilencer )
		NiagaraComponent->Activate();*/
}

void UCWRRangedWeaponInstance::RefillAmmo()
{
	const int32 BulletsFired = MagazineSize - CurrentBulletsAmount;
	if ( BulletsFired > BackpackBulletsAmount)
	{
		CurrentBulletsAmount += BackpackBulletsAmount;
		BackpackBulletsAmount = 0;
	}
	else
	{
		CurrentBulletsAmount = MagazineSize;
		BackpackBulletsAmount -= BulletsFired;
	}
		
	GetCWRCharacter()->OnBulletAmountChanged.Broadcast(CurrentBulletsAmount);
	GetCWRCharacter()->OnBackpackBulletAmountChanged.Broadcast(BackpackBulletsAmount);
}

FTransform UCWRRangedWeaponInstance::GetLeftHandGripTransform() const
{
	if ( !GetSpawnedActors().IsEmpty() )
	{
		if (const auto WeaponActor = Cast<ACWRWeaponActor>(GetSpawnedActors()[0]) )
		{
			return WeaponActor->GetWeaponMesh()->GetSocketTransform(LeftHandGripSocketName);
		}
	}
	return FTransform();
}

FVector UCWRRangedWeaponInstance::GetAimOffsetLocation() const
{
	if ( !GetSpawnedActors().IsEmpty() )
	{
		if (const auto WeaponActor = Cast<ACWRWeaponActor>(GetSpawnedActors()[0]) )
		{
			return WeaponActor->GetAimOffset()->GetRelativeLocation();
		}
	}
	return FVector::ZeroVector;
}






