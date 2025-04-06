// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CWRTeamSpawnPoint.generated.h"

class USphereComponent;
class ACWRPlayerStart;

UCLASS()
class CWR_API ACWRTeamSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRTeamSpawnPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ACWRPlayerStart* FindUnclaimedPlayerStart();

	void CapturePoint(const int32 TeamId);
	
	FORCEINLINE bool IsStartingPoint() const { return bStartingPoint; }
	FORCEINLINE bool WasEverCaptured() const { return bWasEverCaptured; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentTeamId() const { return CurrentTeamId; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCaptured() const { return CurrentTeamId != -1; }
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;
	
private:
	UPROPERTY(EditInstanceOnly)
	TArray<TSoftObjectPtr<ACWRPlayerStart>> AssociatedPlayerStarts;

	// Is point the first for a team at the match
	UPROPERTY(EditInstanceOnly)
	bool bStartingPoint = false;

	UPROPERTY(EditInstanceOnly)
	float TimeToUnCapture = 40.f;
	
	int32 CurrentTeamId = INDEX_NONE;

	/* { TeamId, PlayerNum } */
	TArray<TPair<int32,int32>> PlayerCountNearPoint;
	
	bool bWasEverCaptured = false;

	FTimerHandle UnCaptureTimerHandle;

	FColor TeamColor = FColor::White;
	
	void ProcessActorOverlaps(AActor* OtherActor, bool bIsInside);
	void RecalculateCapture(const int32 NewPlayerTeamId);

	void UnCapturePoint();
};
