// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NetWorkProject1 : ModuleRules
{
	public NetWorkProject1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
            "EnhancedInput" , "UMG", "Slate", "OnlineSubsystem", "OnlineSubsystemSteam" });
	}
}
