// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRWeaponActor.h"

#include "AkComponent.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/CWRCharacter_Base.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "UI/CWRWorldUserWidget.h"
#include "Weapons/CWRWeaponCustomizationItem.h"


ACWRWeaponActor::ACWRWeaponActor()
{
 
	PrimaryActorTick.bCanEverTick = false;

	bNetUseOwnerRelevancy = true;
	SetNetUpdateFrequency(30.f);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TPMesh"));
	TPMesh->SetOwnerNoSee(true);
	TPMesh->bUseAttachParentBound = true;
	TPMesh->SetupAttachment(RootComponent);

	TPShellEject = CreateDefaultSubobject<UArrowComponent>(TEXT("TPShellEject"));
	TPShellEject->ArrowSize = 0.25f;
	TPShellEject->SetupAttachment(TPMesh);
	
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetCastShadow(false);
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->bUseAsOccluder = false;
	FPMesh->SetupAttachment(RootComponent);

	FPShellEject = CreateDefaultSubobject<UArrowComponent>(TEXT("FPShellEject"));
	FPShellEject->ArrowSize = 0.25f;
	FPShellEject->SetupAttachment(FPMesh);

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);


}

void ACWRWeaponActor::TriggerFireAudio() const
{
	if ( IsValid(AkComponent) )
	{
		AkComponent->PostAkEvent(FireSound);
	}
}

ACWRCharacter_Base* ACWRWeaponActor::GetOwningCharacter() const
{
	return Cast<ACWRCharacter_Base>(Owner);
}

void ACWRWeaponActor::TriggerMuzzleFireEffect() 
{
	/*
	if ( IsValid(NiagaraComponent) )
	{
		NiagaraComponent->Activate();
	}*/
}

void ACWRWeaponActor::DisableMuzzleFireEffect()
{
	
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
	
/*	AttachmentMeshComponent->SetupAttachment(GetWeaponMesh(), AttachmentSocketName);
	AttachmentMeshComponent->SetStaticMesh(CustomizationItem->Mesh);
	AttachmentMeshComponent->CanCharacterStepUpOn = ECB_No;
	AttachmentMeshComponent->SetCanEverAffectNavigation(false);
	AttachmentMeshComponent->SetGenerateOverlapEvents(false);
	AttachmentMeshComponent->SetOnlyOwnerSee(true);
	AttachmentMeshComponent->SetRelativeTransform(CustomizationItem->RelativeTransform);
	AttachmentMeshComponent->RegisterComponent();
	AttachmentMeshComponent->SetSimulatePhysics(false);
	AttachmentMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	
}

void ACWRWeaponActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if (const auto CWRCharacter = Cast<ACWRCharacter_Base>(InstigatorPawn) )
	{
		CWRCharacter->PickUpItem(WeaponDefinition);
	}
}

FText ACWRWeaponActor::GetInteractText_Implementation(APawn* InstigatorPawn) const
{
	return FText::GetEmpty();
}




