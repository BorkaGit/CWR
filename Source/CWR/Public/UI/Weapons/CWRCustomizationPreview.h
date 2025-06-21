// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CWRCustomizationPreview.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CWR_API ACWRCustomizationPreview : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRCustomizationPreview();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float DeltaX = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float DeltaY = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float RotationSpeed = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float RotationInterp = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float CenterSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bCanRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bInteracting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bCenterWeapon = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotator")
	FRotator TargetRotate = FRotator::ZeroRotator;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DefaultSceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> PivotComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;
};
