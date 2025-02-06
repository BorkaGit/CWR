// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CWR : ModuleRules
{
	public CWR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "AkAudio", "ModularGameplay", "AIModule" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "GameplayTasks", "NavigationSystem", "Niagara","NetCore", "Slate", "SlateCore", "PocketWorlds" });
		
		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
