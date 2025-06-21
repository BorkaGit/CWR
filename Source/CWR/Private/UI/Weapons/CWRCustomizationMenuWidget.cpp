// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/CWRCustomizationMenuWidget.h"

#include "Input/CommonUIInputTypes.h"

void UCWRCustomizationMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackBindingHandle = RegisterUIActionBinding(
	FBindUIActionArgs(BackInputAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));

	ApplyBindingHandle = RegisterUIActionBinding(
		FBindUIActionArgs(ApplyInputAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));

	CancelBindingHandle = RegisterUIActionBinding(
		FBindUIActionArgs(CancelInputAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelAction)));
}

void UCWRCustomizationMenuWidget::HandleBackAction()
{
	DeactivateWidget();
}

void UCWRCustomizationMenuWidget::HandleApplyAction()
{
}

void UCWRCustomizationMenuWidget::HandleCancelAction()
{
}
