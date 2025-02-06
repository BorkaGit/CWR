// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRCharacterMovementComponent.h"


void UCWRCharacterMovementComponent::SetWalkingType(ECWRWalkingTypes const InWalkingType)
{
	if ( WalkingType == InWalkingType)
		return;

	const ECWRWalkingTypes OldWalkingType = WalkingType;
	WalkingType = InWalkingType;
	MaxWalkSpeed = GetWalkingSpeedByType(InWalkingType);

	OnWalkingTypeChanged.Broadcast(this, OldWalkingType, WalkingType);
}

ECWRWalkingTypes UCWRCharacterMovementComponent::GetWalkingType() const
{
	return WalkingType;
}

float UCWRCharacterMovementComponent::GetWalkingSpeedByType(ECWRWalkingTypes const InWalkingType) const
{
	return MaxWalkingSpeedByType[static_cast<uint8>(InWalkingType)];
}
