// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameUserSettings.h>
#include "PEGameSettings.generated.h"

/**
 *
 */
UCLASS(Config = GameUserSettings, configdonotcheckdefaults, Category = "Project Elementus | Classes")
class ELEMENTUSCORE_API UPEGameSettings : public UGameUserSettings
{
    GENERATED_BODY()

public:
    explicit UPEGameSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
    virtual void ApplyPECustomSettings();

public:
    // Start of UGameUserSettings
    virtual void ApplyNonResolutionSettings() override;
    virtual void SetToDefaults() override;
    // End of UGameUserSettings

    UFUNCTION(BlueprintCallable, Category = Settings)
    static UPEGameSettings* GetCustomGameUserSettings();

    UFUNCTION(BlueprintCallable, Category = Settings)
    void SetAntiAliasingMode(const int32 InMode);

    UFUNCTION(BlueprintPure, Category = Settings)
    int32 GetAntiAliasingMode() const;

    UFUNCTION(BlueprintCallable, Category = Settings, meta = (DisplayName = "Set FSR Enabled"))
    void SetFSREnabled(const bool bEnable);

    UFUNCTION(BlueprintPure, Category = Settings, meta = (DisplayName = "Get FSR Enabled"))
    bool GetFSREnabled() const;

    UFUNCTION(BlueprintCallable, Category = Settings, meta = (DisplayName = "Set FSR Mode"))
    void SetFSRMode(const int32 InMode);

    UFUNCTION(BlueprintPure, Category = Settings, meta = (DisplayName = "Get FSR Mode"))
    int32 GetFSRMode() const;

    UFUNCTION(BlueprintCallable, Category = Settings)
    void SetTemporalUpscalingEnabled(const bool bEnable);

    UFUNCTION(BlueprintPure, Category = Settings)
    bool GetTemporalUpscalingEnabled() const;

    UFUNCTION(BlueprintCallable, Category = Settings)
    void SetLumenEnabled(const bool bEnable);

    UFUNCTION(BlueprintPure, Category = Settings)
    bool GetLumenEnabled() const;

private:
    UPROPERTY(config, meta = (Setter = "SetAntiAliasingMode", Getter = "GetAntiAliasingMode"))
    int32 AntiAliasingMode;

    UPROPERTY(config, meta = (Setter = "SetFSREnabled", Getter = "GetFSREnabled"))
    bool bFSREnabled;

    UPROPERTY(config, meta = (Setter = "SetFSRMode", Getter = "GetFSRMode"))
    int32 FSRMode;

    UPROPERTY(config, meta = (Setter = "SetTemporalUpscalingEnabled", Getter = "GetTemporalUpscalingEnabled"))
    bool bEnableTemporalUpscaling;

    UPROPERTY(config, meta = (Setter = "SetLumenEnabled", Getter = "GetLumenEnabled"))
    bool bEnableLumen;

    bool IsFSREnabled() const;
    void ApplyFSRSettings() const;
};
