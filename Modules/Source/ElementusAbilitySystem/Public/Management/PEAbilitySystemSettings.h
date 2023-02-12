// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PEAbilitySystemSettings.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS(Config = Plugins, DefaultConfig, Meta = (DisplayName = "Project Elementus: Ability System"))
class ELEMENTUSABILITYSYSTEM_API UPEAbilitySystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEAbilitySystemSettings(const FObjectInitializer& ObjectInitializer);
	static const UPEAbilitySystemSettings* Get();
	
	/* Global death effect used by GAS objects */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftClassPtr<UGameplayEffect> GlobalDeathEffect;

	/* Global stun effect used by GAS objects */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftClassPtr<UGameplayEffect> GlobalStunEffect;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftObjectPtr<UDataTable> BasicStatusAttributeMetadata;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftObjectPtr<UDataTable> CustomStatusAttributeMetadata;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftObjectPtr<UDataTable> LevelingAttributeMetadata;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "GAS | Effects")
	TSoftObjectPtr<UDataTable> LevelingBonusAttributeMetadata;
};
