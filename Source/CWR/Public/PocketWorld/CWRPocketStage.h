// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWRPocketStage.generated.h"

UCLASS()
class CWR_API ACWRPocketStage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACWRPocketStage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PreCaptureDiffuse();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PostCaptureDiffuse();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PreCaptureAlphaMask();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PostCaptureAlphaMask();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DetectMouseInput(FVector2D NewMouseLocation);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetNewDisplayActor(TSubclassOf<AActor> NewActorClass);
};
