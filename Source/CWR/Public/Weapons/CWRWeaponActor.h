// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWRWeaponActor.generated.h"

class UCWRWeaponCustomizationItem;
class UNiagaraSystem;
class UAkAudioEvent;
class UAkComponent;
class UNiagaraComponent;
class USphereComponent;

UCLASS(Blueprintable)
class CWR_API ACWRWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRWeaponActor();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAkComponent* GetAkComponent() const { return AkComponent; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USceneComponent* GetAimOffset() const { return AimOffset; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UNiagaraComponent* GetNiagaraComponent() const { return NiagaraComponent; }

	UFUNCTION(BlueprintCallable)
	void TriggerFireAudio() const;

	UFUNCTION(BlueprintCallable)
	void TriggerMuzzleFireEffect() const;

	UFUNCTION(BlueprintCallable)
	void SpawnAttachment(UCWRWeaponCustomizationItem* CustomizationItem);
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> RootSphereComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MuzzleSocketName = "MuzzleSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  BarrelSocketName = "BarrelSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  ScopeSocketName = "ScopeSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MagazineSocketName = "MagazineSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAkAudioEvent> FireSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> MuzzleFireNiagara;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAkComponent> AkComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> AimOffset = nullptr;
};
