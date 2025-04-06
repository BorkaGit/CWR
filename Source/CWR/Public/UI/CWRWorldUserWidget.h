// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CWRWorldUserWidget.generated.h"

class USizeBox;

UCLASS()
class CWR_API UCWRWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	AActor* GetAttachedActor() const { return AttachedActor; }
	FVector GetWorldOffset() const { return WorldOffset; }
	
	void SetAttachedActor(AActor* NewActor) { AttachedActor = NewActor; }
	void SetWorldOffset(const FVector& NewOffset) { WorldOffset = NewOffset; }

	
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn = true))
	AActor* AttachedActor;
	
	
};
