// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/SceneCapture2D.h"
#include "CWRSceneCapture2D.generated.h"


UCLASS()
class CWR_API ACWRSceneCapture2D : public ASceneCapture2D
{
	GENERATED_BODY()

public:

	ACWRSceneCapture2D();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void MouseWheelActivated(const float AxisValue);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float InitialFov = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float TargetFov = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float MinFov = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float MaxFov = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float ZoomInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values")
	float WheelAxisMultiply = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
	bool bCanZoom = true;
};
