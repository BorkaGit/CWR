// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/CWRLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

class UUserWidget;

UCWRLoadingScreenSubsystem::UCWRLoadingScreenSubsystem()
{
}

void UCWRLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UCWRLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}