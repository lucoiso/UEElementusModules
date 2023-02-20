// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PEEpicOnlineServicesSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Plugins, DefaultConfig, Meta = (DisplayName = "Elementus Module: Epic Online Services"))
class ELEMENTUSEPICONLINESERVICES_API UPEEpicOnlineServicesSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEEpicOnlineServicesSettings(const FObjectInitializer& ObjectInitializer);
	static const UPEEpicOnlineServicesSettings* Get();

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
