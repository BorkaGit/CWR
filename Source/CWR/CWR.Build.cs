// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CWR : ModuleRules
{
	public CWR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreOnline",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"PhysicsCore", 
				"GameplayTags", 
				"GameplayTasks", 
				"GameplayAbilities", 
				"AIModule",
				"ModularGameplay", 
				"ModularGameplayActors",
				"DataRegistry",
				"ReplicationGraph",
				"GameFeatures",
				"SignificanceManager",
				"Hotfix",
				"CommonLoadingScreen",
				"Niagara",
				"AsyncMixin",
				"ControlFlows",
				"PropertyPath"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore", 
				"Slate", 
				"SlateCore", 
				"RenderCore",
				"DeveloperSettings",
				"EnhancedInput", 
				"NetCore",
				"RHI",
				"Projects",
				"Gauntlet",
				"UMG",
				"CommonUI",
				"CommonInput",
				"GameSettings",
				"CommonGame",
				"CommonUser",
				"GameplayMessageRuntime",
				"AudioMixer",
				"NetworkReplayStreaming",
				"UIExtension",
				"ClientPilot",
				"AudioModulation",
				"EngineSettings",
				"DTLSHandlerComponent",
				"Json",
				"NavigationSystem", 
				"PocketWorlds", 
			});
		
		SetupGameplayDebuggerSupport(Target);
		SetupIrisSupport(Target);
	}
}
