// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CWRInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/CWRLocalPlayer.h"
#include "Settings/CWRSettingsLocal.h"

class UCWRInputConfig;

UCWRInputComponent::UCWRInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UCWRInputComponent::AddInputMappings(const UCWRInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UCWRInputComponent::RemoveInputMappings(const UCWRInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UCWRInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}


