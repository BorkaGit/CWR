// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CWRVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FCWRVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;

UCLASS()
class CWR_API UCWRVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "CWR")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "CWR")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "CWR")
	static FGameplayCueParameters VerbMessageToCueParameters(const FCWRVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "CWR")
	static FCWRVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
