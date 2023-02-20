// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEEpicOnlineServicesSettings.h"
#include "LogElementusEpicOnlineServices.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEEpicOnlineServicesSettings)

UPEEpicOnlineServicesSettings::UPEEpicOnlineServicesSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
	CategoryName = TEXT("Plugins");
}

const UPEEpicOnlineServicesSettings* UPEEpicOnlineServicesSettings::Get()
{
	static const UPEEpicOnlineServicesSettings* const Instance = GetDefault<UPEEpicOnlineServicesSettings>();
	return Instance;
}

#if WITH_EDITOR
void UPEEpicOnlineServicesSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPEEpicOnlineServicesSettings, bEnableInternalLogs))
	{
		ToggleInternalLogs();
	}
}
#endif

void UPEEpicOnlineServicesSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ToggleInternalLogs();
}

void UPEEpicOnlineServicesSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
	LogElementusEpicOnlineServices_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}
