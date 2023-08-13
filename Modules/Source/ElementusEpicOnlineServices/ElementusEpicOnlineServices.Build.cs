// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

using UnrealBuildTool;

public class ElementusEpicOnlineServices : ModuleRules
{
    public ElementusEpicOnlineServices(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "EOSSDKHandler",
            "EOSVoiceChat",
            "EOSShared",
            "OnlineSubsystemEOS",
            "OnlineSubsystem",
            "VoiceChat"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Engine",
            "NetCore",
            "CoreUObject",
            "JsonUtilities",
            "DeveloperSettings"
        });

        SetupIrisSupport(Target);
    }
}