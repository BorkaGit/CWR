// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRAttachmentActor.h"


ACWRAttachmentActor::ACWRAttachmentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ACWRAttachmentActor::BeginPlay()
{
	Super::BeginPlay();
	
}


