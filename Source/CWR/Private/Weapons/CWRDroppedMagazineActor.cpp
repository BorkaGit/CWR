// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRDroppedMagazineActor.h"

#include "Kismet/GameplayStatics.h"


ACWRDroppedMagazineActor::ACWRDroppedMagazineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
}

void ACWRDroppedMagazineActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SkeletalMeshComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ACWRDroppedMagazineActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bWasHit) return;
	
	bWasHit = true;

	UGameplayStatics::PlaySoundAtLocation(this, MagazineDropSound,Hit.ImpactPoint,FRotationMatrix::MakeFromX(Hit.ImpactNormal).Rotator(), 1.f, 1.f, 0.f,MagazineDropSoundAttenuation);
}

void ACWRDroppedMagazineActor::BeginPlay()
{
	Super::BeginPlay();

	SkeletalMeshComponent->SetSkeletalMeshAsset(MagazineMesh);
	SkeletalMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->AddForce(GetActorTransform().TransformVectorNoScale(FVector::ForwardVector * 5000.f),NAME_None, true);
}


