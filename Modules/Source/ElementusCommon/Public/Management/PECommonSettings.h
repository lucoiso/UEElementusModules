// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PECommonSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Plugins, DefaultConfig, Meta = (DisplayName = "Elementus Module: Common"))
class ELEMENTUSCOMMON_API UPECommonSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPECommonSettings(const FObjectInitializer& ObjectInitializer);
	static const UPECommonSettings* Get();

	/* Will print extra internal informations in log */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Settings", Meta = (DisplayName = "Enable Internal Logs"))
	bool bEnableInternalLogs;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitProperties() override;

private:
	void ToggleInternalLogs();
};
