// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CWRCharacterMovementComponent.generated.h"

UENUM( BlueprintType )
enum class ECWRWalkingTypes : uint8
{
	Walk,
	Jog,
	Sprint,	
	Count			UMETA( Hidden )
};

UCLASS()
class CWR_API UCWRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWalkingTypeChange, UCWRCharacterMovementComponent*, MovementComponent, ECWRWalkingTypes, OldWalkingType, ECWRWalkingTypes, NewWalkingType);
	UPROPERTY(BlueprintAssignable, Category = "Character Movement ")
	FWalkingTypeChange	OnWalkingTypeChanged;
	
	UFUNCTION(BlueprintCallable)
	void SetWalkingType ( ECWRWalkingTypes const InWalkingType );

	UFUNCTION(BlueprintPure)
	ECWRWalkingTypes GetWalkingType	( ) const;

	UFUNCTION(BlueprintPure)
	float GetWalkingSpeedByType	( ECWRWalkingTypes const InWalkingType ) const;

protected:

	UPROPERTY( EditAnywhere, Category = "Movement", Meta = ( ClampMin = 0.f, UIMin = 0.f ) )
	float MaxWalkingSpeedByType[(int)ECWRWalkingTypes::Count] = {10.f, 200.f, 600.f };
	
private:

	UPROPERTY(Transient)
	bool bIsSprinting = false;
	
	UPROPERTY(Transient)
	ECWRWalkingTypes WalkingType = ECWRWalkingTypes::Jog;
	
};
