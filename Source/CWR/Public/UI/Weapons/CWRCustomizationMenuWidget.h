// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CWRCustomizationMenuWidget.generated.h"


UCLASS()
class CWR_API UCWRCustomizationMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
	void HandleBackAction();
	void HandleApplyAction();
	void HandleCancelAction();
	
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackInputAction;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle ApplyInputAction;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle CancelInputAction;
	
	FUIActionBindingHandle BackBindingHandle;
	FUIActionBindingHandle ApplyBindingHandle;
	FUIActionBindingHandle CancelBindingHandle;
	
	
	
};
