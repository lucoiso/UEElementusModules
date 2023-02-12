// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

using UnrealBuildTool;

public class ElementusCore : ModuleRules
{
    public ElementusCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "ElementusInventory",
            "GameplayTags",
            "ElementusEpicOnlineServices"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Engine",
            "NetCore",
            "CoreUObject",
            "InputCore",
            "EnhancedInput",
            "ModularGameplay",
            "GameFeatures",
            "ModularFeatures_ExtraActions",
            "GameplayAbilities",
            "AIModule",
            "UMG",
            "Niagara",
            "DeveloperSettings",
            "MoviePlayer",
            "ElementusAbilitySystem",
            "ElementusInventorySystem",
            "ElementusCommon"
        });

        SetupIrisSupport(Target);
    }
}