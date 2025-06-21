// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/CWRCustomizationPreview.h"


ACWRCustomizationPreview::ACWRCustomizationPreview()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	PivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComponent"));
	PivotComponent->SetupAttachment(RootComponent);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(PivotComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ACWRCustomizationPreview::BeginPlay()
{
	Super::BeginPlay();
}

void ACWRCustomizationPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( bInteracting )
	{
		TargetRotate.Yaw += DeltaX * -RotationSpeed;
		TargetRotate.Roll = FMath::ClampAngle(DeltaY * -RotationSpeed + TargetRotate.Roll, -89.f, 89.f);
	}

	if ( bCanRotate )
	{
		PivotComponent->SetRelativeRotation(FMath::RInterpTo(PivotComponent->GetRelativeRotation(), FRotator(0.0, TargetRotate.Yaw, TargetRotate.Roll), DeltaTime, RotationInterp));
	}

	if ( bCenterWeapon )
	{
		//SpringArmComponent->SocketOffset = FMath::VInterpTo(SpringArmComponent->SocketOffset, FVector::ZeroVector, DeltaTime, CenterSpeed);
	}
}

