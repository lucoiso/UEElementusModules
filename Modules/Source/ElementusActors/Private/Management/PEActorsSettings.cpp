// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEActorsSettings.h"
#include "LogElementusActors.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEActorsSettings)

UPEActorsSettings::UPEActorsSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
	CategoryName = TEXT("Plugins");
}

const UPEActorsSettings* UPEActorsSettings::Get()
{
	static const UPEActorsSettings* const Instance = GetDefault<UPEActorsSettings>();
	return Instance;
}

#if WITH_EDITOR
void UPEActorsSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPEActorsSettings, bEnableInternalLogs))
	{
		ToggleInternalLogs();
	}
}
#endif

void UPEActorsSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ToggleInternalLogs();
}

void UPEActorsSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
	LogElementusActors_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}
