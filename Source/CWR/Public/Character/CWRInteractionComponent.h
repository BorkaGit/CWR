// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PawnComponent.h"
#include "CWRInteractionComponent.generated.h"

class UCWRWorldUserWidget;

UCLASS()
class CWR_API UCWRInteractionComponent : public UPawnComponent
{
	GENERATED_BODY()
	
	UCWRInteractionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PrimaryInteract();
	
	void EndInteract();
protected:

	void FindBestInteractable();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCWRWorldUserWidget> DefaultWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius = 30.0f;
		
private:

	UPROPERTY(Transient)
	AActor* FocusedActor = nullptr;

	bool bCurrentlyInteracting = false;
	
	UPROPERTY(Transient)
	UCWRWorldUserWidget* DefaultWidgetInstance = nullptr;
};
