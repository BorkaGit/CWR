// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CWRCasingActor.generated.h"

UCLASS()
class CWR_API ACWRCasingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRCasingActor();

	virtual void PostInitializeComponents() override;
	
	FORCEINLINE void SetCasingMesh(UStaticMesh* InCasingMesh) { CasingMesh = InCasingMesh; }
	FORCEINLINE void SetCasingSound(USoundBase* InCasingSound) { CasingSound = InCasingSound; }
	FORCEINLINE void SetCasingSoundAttenuation(USoundAttenuation* InCasingSoundAttenuation) { CasingSoundAttenuation = InCasingSoundAttenuation; }

	FORCEINLINE UStaticMeshComponent* GetFP() const { return FP; }
	FORCEINLINE UStaticMeshComponent* GetTP() const { return TP; }
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CasingMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USoundBase> CasingSound = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<USoundAttenuation> CasingSoundAttenuation = nullptr;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void StopSimulatingPhysics();
	
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> FP = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TP = nullptr;

	bool bWasHit = false;
};
