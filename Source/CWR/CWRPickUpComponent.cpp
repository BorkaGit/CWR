// Copyright Epic Games, Inc. All Rights Reserved.

#include "CWRPickUpComponent.h"

UCWRPickUpComponent::UCWRPickUpComponent()
{

	SphereRadius = 32.f;
}

void UCWRPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	
	OnComponentBeginOverlap.AddDynamic(this, &UCWRPickUpComponent::OnSphereBeginOverlap);
}

void UCWRPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	ACWRCharacter_Player* Character = Cast<ACWRCharacter_Player>(OtherActor);
	if(Character)
	{
		
		OnPickUp.Broadcast(Character);

		//if (const auto Weapon = Cast<ACWRWeaponBase>(GetOwner()))
		//{
		//	Weapon->SetOwner(Character);
		//	Character->SetWeapon(Weapon);
		//}
		OnComponentBeginOverlap.RemoveAll(this);
		//GetOwner()->Destroy();
	}
}
