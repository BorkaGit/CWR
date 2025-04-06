// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/CWRWidgetController.h"


void UCWRWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UCWRWidgetController::BroadcastInitialValues()
{
	
}

void UCWRWidgetController::BindCallbacksToDependencies()
{
	
}
