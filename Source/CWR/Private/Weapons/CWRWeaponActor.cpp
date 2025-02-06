// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRWeaponActor.h"

#include "AkComponent.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Weapons/CWRWeaponCustomizationItem.h"


ACWRWeaponActor::ACWRWeaponActor()
{
 
	PrimaryActorTick.bCanEverTick = false;

	RootSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphereComponent"));
	SetRootComponent(RootSphereComponent);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->CanCharacterStepUpOn = ECB_No;
	WeaponMesh->SetCanEverAffectNavigation(false);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetSimulatePhysics(false);

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(WeaponMesh);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(WeaponMesh, MuzzleSocketName);
	
	AimOffset = CreateDefaultSubobject<USceneComponent>(TEXT("AimOffset"));
	AimOffset->SetupAttachment(RootSphereComponent);
}

void ACWRWeaponActor::TriggerFireAudio() const
{
	if ( IsValid(AkComponent) )
	{
		AkComponent->PostAkEvent(FireSound);
	}
}

void ACWRWeaponActor::TriggerMuzzleFireEffect() const
{
	if ( IsValid(NiagaraComponent) )
	{
		NiagaraComponent->Activate();
	}
}

void ACWRWeaponActor::SpawnAttachment(UCWRWeaponCustomizationItem* CustomizationItem)
{
	UStaticMeshComponent* AttachmentMeshComponent = NewObject<UStaticMeshComponent>(this);
	FName AttachmentSocketName;
	switch (CustomizationItem->ItemCategory)
	{
	case ECWRWeaponItemCategory::Scope:
		
		AttachmentSocketName = ScopeSocketName;
		break;
		
	case ECWRWeaponItemCategory::Barrel:
		
		AttachmentSocketName = BarrelSocketName;
		break;
		
	case ECWRWeaponItemCategory::Magazine:

		AttachmentSocketName = MagazineSocketName;
		break;

		default:
			AttachmentSocketName = "";
			break;
	}
	
	AttachmentMeshComponent->SetupAttachment(GetWeaponMesh(), AttachmentSocketName);
	AttachmentMeshComponent->SetStaticMesh(CustomizationItem->Mesh);
	//AttachmentMeshComponent->SetupAttachment(GetWeaponMesh3P(), AttachmentSocketName);
	AttachmentMeshComponent->CanCharacterStepUpOn = ECB_No;
	AttachmentMeshComponent->SetCanEverAffectNavigation(false);
	AttachmentMeshComponent->SetGenerateOverlapEvents(false);
	AttachmentMeshComponent->SetOnlyOwnerSee(true);
	AttachmentMeshComponent->SetRelativeTransform(CustomizationItem->RelativeTransform);
	AttachmentMeshComponent->RegisterComponent();
	AttachmentMeshComponent->SetSimulatePhysics(false);
	AttachmentMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}




