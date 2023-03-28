// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

using UnrealBuildTool;

public class ElementusAbilitySystem : ModuleRules
{
	public ElementusAbilitySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"ModelViewViewModel"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Engine",
			"NetCore",
			"CoreUObject",
			"DeveloperSettings"
		});

		SetupIrisSupport(Target);
	}
}