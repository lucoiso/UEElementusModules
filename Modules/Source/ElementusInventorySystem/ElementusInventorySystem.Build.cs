// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

using UnrealBuildTool;

public class ElementusInventorySystem : ModuleRules
{
    public ElementusInventorySystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Engine",
            "NetCore",
            "CoreUObject",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "ElementusInventory",
            "ElementusAbilitySystem",
            "ElementusCommon",
            "DeveloperSettings",
            "ModularFeatures_ExtraActions",
            "UMG"
        });

        SetupIrisSupport(Target);
    }
}