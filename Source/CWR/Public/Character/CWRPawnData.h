// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "CWRPawnData.generated.h"


class APawn;
class UCWRAbilitySet;
class UCWRAbilityTagRelationshipMapping;
class UCWRInputConfig;
class UObject;

/**
 * UCWRPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "CWR Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class CWR_API UCWRPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UCWRPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ACWRPawn or ACWRCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CWR|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CWR|Abilities")
	TArray<TObjectPtr<UCWRAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CWR|Abilities")
	TObjectPtr<UCWRAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CWR|Input")
	TObjectPtr<UCWRInputConfig> InputConfig;
	
};
