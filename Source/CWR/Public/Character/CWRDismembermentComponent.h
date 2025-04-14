// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CWRDismembermentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class CWR_API UCWRDismembermentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCWRDismembermentComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HitLimb(int32 DamageToLimb, FName HitBoneName, AActor* DamageCauser, AActor* HitPawn, FVector Impulse, FVector Location);
	
protected:
	virtual void BeginPlay() override;
	
private:
	void AnyDamage(int32 Damage, FName HitBoneName, AActor* DamageCauser, AActor* HitPawn, FVector Impulse, FVector Location);
	
};
