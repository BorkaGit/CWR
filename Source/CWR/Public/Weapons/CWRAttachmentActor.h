// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Inventory/CWRInventoryItemInstance.h"

#include "CWRAttachmentActor.generated.h"

USTRUCT(BlueprintType)
struct FCWRActualAimInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent* MeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCurrentlyUsing = false;
};

UCLASS()
class CWR_API ACWRAttachmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACWRAttachmentActor();

	FORCEINLINE void SetVisibility	( const bool bInVisibility )	{ bVisibility = bInVisibility; }
	FORCEINLINE void SetTP			( const bool bInTP )			{ bTP = bInTP; }
	FORCEINLINE void SetFP			( const bool bInFP )			{ bFP = bInFP; }
	FORCEINLINE void SetEnabled     ( const bool bInEnabled )       { bEnabled = bInEnabled; }
	
	FORCEINLINE bool IsAttachmentToggleable() const { return AttachmentToggleable; }
	FORCEINLINE TArray<FCWRActualAimInfo> GetSight() const { return Sight; }
	FORCEINLINE TArray<FCWRActualAimInfo> GetFrontSight() const { return FrontSight; }
	FORCEINLINE TArray<FCWRActualAimInfo> GetRearSight() const { return RearSight; }
		
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	bool bVisibility = false;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	bool bTP = false;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	bool bFP = false;

	UPROPERTY(BlueprintReadWrite)
	bool bEnabled = false;

	UPROPERTY(BlueprintReadWrite)
	bool AttachmentToggleable = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCWRActualAimInfo> Sight;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCWRActualAimInfo> FrontSight;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCWRActualAimInfo> RearSight;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	FCWRAttachmentInfo AttachmentInfo;
	
};
