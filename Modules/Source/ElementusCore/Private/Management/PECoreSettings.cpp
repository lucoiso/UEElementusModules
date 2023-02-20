// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PECoreSettings.h"
#include "LogElementusCore.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECoreSettings)

UPECoreSettings::UPECoreSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bEnableInternalLogs(false), bEnableBaseControllerLogs(false), bEnableAxisControllerLogs(false), PlayerColor(FLinearColor::Blue), BotColor(FLinearColor::Red), DefaultGravityScale(1.25f), DefaultWalkSpeed(500.f), DefaultCrouchSpeedPercentage(0.6f), DefaultJumpZVelocity(500.f), DefaultAirControl(0.375f)
{
	CategoryName = TEXT("Plugins");
}

const UPECoreSettings* UPECoreSettings::Get()
{
    static const UPECoreSettings* const Instance = GetDefault<UPECoreSettings>();
    return Instance;
}

#if WITH_EDITOR
void UPECoreSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPECoreSettings, bEnableInternalLogs) ||
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPECoreSettings, bEnableBaseControllerLogs) ||
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPECoreSettings, bEnableAxisControllerLogs))
	{
		ToggleInternalLogs();
	}
}
#endif

void UPECoreSettings::PostInitProperties()
{
	Super::PostInitProperties();
	ToggleInternalLogs();
}

void UPECoreSettings::ToggleInternalLogs()
{
#if !UE_BUILD_SHIPPING
	LogElementusCore_Internal.SetVerbosity(bEnableInternalLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
	LogController_Base.SetVerbosity(bEnableBaseControllerLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
	LogController_Axis.SetVerbosity(bEnableAxisControllerLogs ? ELogVerbosity::Display : ELogVerbosity::NoLogging);
#endif
}
