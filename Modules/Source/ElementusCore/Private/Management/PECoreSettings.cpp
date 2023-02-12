// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PECoreSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECoreSettings)

UPECoreSettings::UPECoreSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), PlayerColor(FLinearColor::Blue), BotColor(FLinearColor::Red), DefaultGravityScale(1.25f), DefaultWalkSpeed(500.f), DefaultCrouchSpeedPercentage(0.6f), DefaultJumpZVelocity(500.f), DefaultAirControl(0.375f)
{
	CategoryName = TEXT("Plugins");
}

const UPECoreSettings* UPECoreSettings::Get()
{
    static const UPECoreSettings* const Instance = GetDefault<UPECoreSettings>();
    return Instance;
}
