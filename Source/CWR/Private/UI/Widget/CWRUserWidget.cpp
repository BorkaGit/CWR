// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/CWRUserWidget.h"


void UCWRUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
