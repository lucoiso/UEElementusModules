// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PEInventorySystemSettings.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS(Config = Plugins, DefaultConfig, Meta = (DisplayName = "Project Elementus: Inventory System"))
class ELEMENTUSINVENTORYSYSTEM_API UPEInventorySystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPEInventorySystemSettings(const FObjectInitializer& ObjectInitializer);
	static const UPEInventorySystemSettings* Get();

	/* A Blueprint Widget class to use as Inventory UI */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "Main Inventory UMG Class"))
	TSoftClassPtr<UUserWidget> MainInventoryWidget;

	/* A Blueprint Widget class to use as Trade UI */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "Trade Inventory UMG Class"))
	TSoftClassPtr<UUserWidget> TradeInventoryWidget;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Settings")
	TSoftObjectPtr<UStaticMesh> InventoryPackageMesh;
};
