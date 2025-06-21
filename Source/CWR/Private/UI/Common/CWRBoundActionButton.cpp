// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/CWRBoundActionButton.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"

class UCommonButtonStyle;

void UCWRBoundActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (UCommonInputSubsystem* InputSubsystem = GetInputSubsystem())
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);
		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

void UCWRBoundActionButton::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	TSubclassOf<UCommonButtonStyle> NewStyle = nullptr;

	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (NewInputMethod == ECommonInputType::Touch)
	{
		NewStyle = TouchStyle;
	}
	else
	{
		NewStyle = KeyboardStyle;
	}

	if (NewStyle)
	{
		SetStyle(NewStyle);
	}
}

