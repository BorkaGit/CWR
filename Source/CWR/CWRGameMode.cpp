// Copyright Epic Games, Inc. All Rights Reserved.

#include "CWRGameMode.h"
#include "CWRCharacter_Player.h"
#include "UObject/ConstructorHelpers.h"

ACWRGameMode::ACWRGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
