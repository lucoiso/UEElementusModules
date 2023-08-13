// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEAbilitySystemSettings.h"
#include "LogElementusAbilitySystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAbilitySystemSettings)

UPEAbilitySystemSettings::UPEAbilitySystemSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
    CategoryName = TEXT("Plugins");
}

const UPEAbilitySystemSettings* UPEAbilitySystemSettings::Get()
{
    static const UPEAbilitySystemSettings* const Instance = GetDefault<UPEAbilitySystemSettings>();
    return Instance;
}

#if WITH_EDITOR
void UPEAbilitySystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPEAbilitySystemSettings, bEnableInternalLogs))
    {
        ToggleInternalLogs();
    }
}
#endif

void UPEAbilitySystemSettings::PostInitProperties()
{
    Super::PostInitProperties();
    ToggleInternalLogs();
}

void UPEAbilitySystemSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
    LogElementusAbilitySystem_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}