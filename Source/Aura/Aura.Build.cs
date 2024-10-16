// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Aura : ModuleRules
{
	public Aura(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{	"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
		});


        PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"GameplayTags",
			"GameplayTasks",
			"NavigationSystem",
			"MotionWarping",
			"Niagara",
			"AIModule",
		});

        PublicIncludePaths.AddRange(new string[]
        {
            "Aura/Public/AbilitySystem",
			"Aura/Public/AbilitySystem/Ability",
			"Aura/Public/AbilitySystem/AbilityTask",
			"Aura/Public/AbilitySystem/Data",
			"Aura/Public/AbilitySystem/ExecCalc",
			"Aura/Public/AbilitySystem/ModMagCalc",
			"Aura/Public/Actor",
			"Aura/Public/AI",
			"Aura/Public/Character",
            "Aura/Public/Game",
			"Aura/Public/Input",
			"Aura/Public/Interaction",
            "Aura/Public/Player",
			"Aura/Public/UI/HUD",
			"Aura/Public/UI/Widget",
			"Aura/Public/UI/WidgetController",
		});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
