// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CWRSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/CWRLocalPlayer.h"
#include "Settings/CWRGameSettingRegistry.h"

class UGameSettingRegistry;

void UCWRSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UCWRSettingScreen::CreateRegistry()
{
	UCWRGameSettingRegistry* NewRegistry = NewObject<UCWRGameSettingRegistry>();

	if (UCWRLocalPlayer* LocalPlayer = CastChecked<UCWRLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void UCWRSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void UCWRSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UCWRSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UCWRSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
