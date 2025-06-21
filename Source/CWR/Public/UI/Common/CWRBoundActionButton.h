// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/CommonBoundActionButton.h"

#define UE_API CWR_API

class UCommonButtonStyle;
class UObject;

#include "CWRBoundActionButton.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract, meta = (DisableNativeTick))
class UCWRBoundActionButton : public UCommonBoundActionButton
{
	GENERATED_BODY()
	
protected:
	UE_API virtual void NativeConstruct() override;

private:
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle;

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle;
};

#undef UE_API