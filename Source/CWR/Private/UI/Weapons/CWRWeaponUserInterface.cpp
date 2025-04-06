// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/CWRWeaponUserInterface.h"

#include "Equipment/CWREquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Weapons/CWRWeaponInstance.h"

struct FGeometry;

UCWRWeaponUserInterface::UCWRWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCWRWeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCWRWeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCWRWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UCWREquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UCWREquipmentManagerComponent>())
		{
			if (UCWRWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UCWRWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					UCWRWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void UCWRWeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}


