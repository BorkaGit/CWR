// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRDismembermentComponent.h"


UCWRDismembermentComponent::UCWRDismembermentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCWRDismembermentComponent::BeginPlay()
{
	Super::BeginPlay();
}



void UCWRDismembermentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCWRDismembermentComponent::HitLimb(int32 DamageToLimb, FName HitBoneName, AActor* DamageCauser, AActor* HitPawn, FVector Impulse, FVector Location)
{
	AnyDamage(DamageToLimb, HitBoneName, DamageCauser, HitPawn, Impulse, Location);
}

void UCWRDismembermentComponent::AnyDamage(int32 Damage, FName HitBoneName, AActor* DamageCauser, AActor* HitPawn, FVector Impulse, FVector Location)
{
	
}