// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatteryCollector2 : ModuleRules
{
	public BatteryCollector2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" }); // Additions for UI

		PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore" });
	}
}
