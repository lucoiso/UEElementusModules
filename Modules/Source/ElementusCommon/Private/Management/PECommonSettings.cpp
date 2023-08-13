// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PECommonSettings.h"
#include "LogElementusCommon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECommonSettings)

UPECommonSettings::UPECommonSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
    CategoryName = TEXT("Plugins");
}

const UPECommonSettings* UPECommonSettings::Get()
{
    static const UPECommonSettings* const Instance = GetDefault<UPECommonSettings>();
    return Instance;
}

#if WITH_EDITOR
void UPECommonSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPECommonSettings, bEnableInternalLogs))
    {
        ToggleInternalLogs();
    }
}
#endif

void UPECommonSettings::PostInitProperties()
{
    Super::PostInitProperties();
    ToggleInternalLogs();
}

void UPECommonSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
    LogElementusCommon_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}