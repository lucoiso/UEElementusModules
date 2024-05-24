// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

using UnrealBuildTool;

public class ElementusActors : ModuleRules
{
    public ElementusActors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "GameplayTags"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Engine",
            "NetCore",
            "CoreUObject",
            "Niagara",
            "ElementusCommon",
            "ElementusAbilitySystem",
            "GameplayAbilities",
            "DeveloperSettings"
        });

        SetupIrisSupport(Target);
    }
}