// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEInventorySystemSettings.h"
#include "LogElementusInventorySystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInventorySystemSettings)

UPEInventorySystemSettings::UPEInventorySystemSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false)
{
	CategoryName = TEXT("Plugins");
}

const UPEInventorySystemSettings* UPEInventorySystemSettings::Get()
{
	static const UPEInventorySystemSettings* const Instance = GetDefault<UPEInventorySystemSettings>();
	return Instance;
}

#if WITH_EDITOR
void UPEInventorySystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPEInventorySystemSettings, bEnableInternalLogs))
	{
		ToggleInternalLogs();
	}
}
#endif

void UPEInventorySystemSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ToggleInternalLogs();
}

void UPEInventorySystemSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
	LogElementusInventorySystem_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}
