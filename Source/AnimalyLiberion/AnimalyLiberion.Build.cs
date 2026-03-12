using UnrealBuildTool;

public class AnimalyLiberion : ModuleRules
{
	public AnimalyLiberion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"AIModule",
				"GameplayTasks",
				"NavigationSystem",
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

