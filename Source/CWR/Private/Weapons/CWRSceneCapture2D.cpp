// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRSceneCapture2D.h"

#include "Components/SceneCaptureComponent2D.h"


ACWRSceneCapture2D::ACWRSceneCapture2D()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ACWRSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();
	
	GetCaptureComponent2D()->FOVAngle = bCanZoom ? FMath::Clamp(InitialFov, MinFov, MaxFov) : InitialFov;
}

void ACWRSceneCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( bCanZoom )
	{
		GetCaptureComponent2D()->FOVAngle = FMath::FInterpTo(GetCaptureComponent2D()->FOVAngle, TargetFov, DeltaTime, ZoomInterpSpeed);
	}
}

void ACWRSceneCapture2D::MouseWheelActivated(const float AxisValue)
{
	if ( !IsActorTickEnabled() )
	{
		SetActorTickEnabled(true);
	}
	
	TargetFov = FMath::Clamp(TargetFov + AxisValue * -WheelAxisMultiply, MinFov, MaxFov);
}
