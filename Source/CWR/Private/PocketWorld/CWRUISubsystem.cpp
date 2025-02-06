// Fill out your copyright notice in the Description page of Project Settings.


#include "PocketWorld/CWRUISubsystem.h"

#include "PocketLevel.h"
#include "PocketLevelSystem.h"
#include "Kismet/GameplayStatics.h"


void UCWRUISubsystem::SetPocketCapture(UPocketCapture* NewPocketCapture)
{
	PocketCapture = NewPocketCapture;
}

UPocketCapture* UCWRUISubsystem::GetPocketCapture()
{
	return PocketCapture;
}

void UCWRUISubsystem::SetPocketStage(ACWRPocketStage* NewPocketStage)
{
	PocketStage = NewPocketStage;
}

ACWRPocketStage* UCWRUISubsystem::GetPocketStage()
{
	return PocketStage;
}

void UCWRUISubsystem::SpawnPocketLevel(FSoftObjectPath PocketLevelAssetPath)
{
	UPocketLevelSubsystem* PocketLevelSubsystem = GetWorld()->GetSubsystem<UPocketLevelSubsystem>();

	UPocketLevel* PocketLevel = Cast<UPocketLevel>(PocketLevelAssetPath.TryLoad());
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();

	FVector PocketLevelLocation = FVector(0, 0, -10000); //Default location in case the level has no Postpo
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundActors);
	if (!FoundActors.IsEmpty())
	{ 
		const APostProcessVolume* PPVolume = Cast<APostProcessVolume>(FoundActors[0]);
		const FBoxSphereBounds Bounds = PPVolume->GetBounds();

		PocketLevelLocation = PPVolume->GetActorLocation() + FVector(0, 0, -Bounds.BoxExtent.Z) + PocketLevelLocation;
	}

	PocketLevelSubsystem->GetOrCreatePocketLevelFor(LocalPlayer, PocketLevel, PocketLevelLocation);
}
