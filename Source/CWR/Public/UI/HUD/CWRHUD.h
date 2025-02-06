// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CWRHUD.generated.h"


class UAttributeSet;
class UAbilitySystemComponent;
class UCWRUserWidget;
class UOverlayWidgetController;
struct FWidgetControllerParams;

UCLASS()
class CWR_API ACWRHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	
	UFUNCTION(BlueprintPure)
	UCWRUserWidget* GetOverlayWidget() const { return OverlayWidget; }

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PlayerController, APlayerState* PlayerState, UAbilitySystemComponent* AbilitySystemComponent, UAttributeSet* AttributeSet);
	
private:

	UPROPERTY(Transient)
	TObjectPtr<UCWRUserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCWRUserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
