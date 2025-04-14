// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CWRDroppedMagazineActor.generated.h"

UCLASS(Blueprintable)
class CWR_API ACWRDroppedMagazineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRDroppedMagazineActor();

	FORCEINLINE void SetMagazineMesh(USkeletalMesh* InMagazineMesh) { MagazineMesh = InMagazineMesh; }
	FORCEINLINE void SetMagazineDropSound(USoundBase* InMagazineDropSound) { MagazineDropSound = InMagazineDropSound; }
	FORCEINLINE void SetMagazineDropSoundAttenuation(USoundAttenuation* InMagazineDropSoundAttenuation) { MagazineDropSoundAttenuation = InMagazineDropSoundAttenuation; }
	
protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(Transient, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> MagazineMesh = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USoundBase> MagazineDropSound = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USoundAttenuation> MagazineDropSoundAttenuation = nullptr;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	bool bWasHit = false;
};
