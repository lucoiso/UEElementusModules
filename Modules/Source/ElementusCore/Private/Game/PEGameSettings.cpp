// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Game/PEGameSettings.h"
#include "LogElementusCore.h"
#include <Interfaces/IPluginManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEGameSettings)

UPEGameSettings::UPEGameSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UPEGameSettings::SetToDefaults();
}

void UPEGameSettings::ApplyPECustomSettings()
{
	if (IConsoleVariable* const AntiAliasingCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod")))
	{
		AntiAliasingCVar->Set(AntiAliasingMode);
	}
		
	if (IConsoleVariable* const TemporalUpsamplingCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upsampling")); TemporalUpsamplingCVar && AntiAliasingMode == 2)
	{
		TemporalUpsamplingCVar->Set(bEnableTemporalUpscaling);
	}

	if (IConsoleVariable* const DynamicGICvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod")))
	{
		// 0 = None | 1 = Lumen
		DynamicGICvar->Set(bEnableLumen);
	}

	if (IConsoleVariable* const DynamicGRCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ReflectionMethod")))
	{
		// 1 = Lumen | 2 = Screen Space
		DynamicGRCvar->Set(bEnableLumen ? 1 : 2);
	}

	if (IConsoleVariable* const VirtualShadowCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Enable")))
	{
		// Enable Virtual Shadow Map if Lumen is enabled
		VirtualShadowCvar->Set(bEnableLumen);
	}

	ApplyFSRSettings();
}

void UPEGameSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	ApplyPECustomSettings();
}

void UPEGameSettings::SetToDefaults()
{
	AntiAliasingMode = 1;
	bEnableTemporalUpscaling = true;
	bEnableLumen = false;

	FSRMode = 2;
	bFSREnabled = IsFSREnabled();

	Super::SetToDefaults();

	ApplyPECustomSettings();
}

void UPEGameSettings::SetAntiAliasingMode(const int32 InMode)
{
	AntiAliasingMode = InMode;
}

int32 UPEGameSettings::GetAntiAliasingMode() const
{
	return AntiAliasingMode;
}

void UPEGameSettings::SetFSREnabled(const bool bEnable)
{
	if (!IsFSREnabled())
	{
		UE_LOG(LogElementusCore_Internal, Error, TEXT("%s - FSR 1.0 plugin not found."), *FString(__func__));
		return;
	}

	bFSREnabled = bEnable;
}

bool UPEGameSettings::GetFSREnabled() const
{
	return bFSREnabled;
}

void UPEGameSettings::SetFSRMode(const int32 InMode)
{
	if (!IsFSREnabled())
	{
		UE_LOG(LogElementusCore_Internal, Error, TEXT("%s - FSR 1.0 plugin not found."), *FString(__func__));
		return;
	}

	FSRMode = InMode;
}

int32 UPEGameSettings::GetFSRMode() const
{
	return FSRMode;
}

void UPEGameSettings::SetTemporalUpscalingEnabled(const bool bEnable)
{
	bEnableTemporalUpscaling = bEnable;
}

bool UPEGameSettings::GetTemporalUpscalingEnabled() const
{
	return bEnableTemporalUpscaling;
}

void UPEGameSettings::SetLumenEnabled(const bool bEnable)
{
	bEnableLumen = bEnable;
}

bool UPEGameSettings::GetLumenEnabled() const
{
	return bEnableLumen;
}

bool UPEGameSettings::IsFSREnabled() const
{
	const TSharedPtr<IPlugin> FSRPlugin = IPluginManager::Get().FindPlugin("FSR");
	return FSRPlugin.IsValid() && FSRPlugin->IsEnabled();
}

void UPEGameSettings::ApplyFSRSettings() const
{
	if (!IsFSREnabled())
	{
		return;
	}

	if (IConsoleVariable* const FSREnabledCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enabled")))
	{
		FSREnabledCVar->Set(bFSREnabled);
	}

	if (IConsoleVariable* const ScreenPercentageCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")); ScreenPercentageCVar && bFSREnabled)
	{
		switch (FSRMode)
		{
			case 0: // Performance
				ScreenPercentageCVar->Set(50);
				break;

			case 1: // Balanced
				ScreenPercentageCVar->Set(59);
				break;

			case 2: // Quality
				ScreenPercentageCVar->Set(67);
				break;

			case 3: // Ultra Quality
				ScreenPercentageCVar->Set(77);
				break;

			default: 
				break;
		}
	}
}

UPEGameSettings* UPEGameSettings::GetCustomGameUserSettings()
{
	return Cast<UPEGameSettings>(GetGameUserSettings());
}
