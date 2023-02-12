// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>
#include <Management/ElementusInventoryData.h>
#include "PEElementusInventoryProcessor.generated.h"

class APlayerController;
class UGameplayEffect;
class UElementusInventoryComponent;
class UPEInventoryComponent;
/**
 *
 */
UINTERFACE(NotBlueprintable, Category = "Project Elementus | Interfaces")
class ELEMENTUSINVENTORYSYSTEM_API UPEElementusInventoryProcessor : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ELEMENTUSINVENTORYSYSTEM_API IPEElementusInventoryProcessor
{
	GENERATED_BODY()

public:
	/* This function will perform the interaction behavior */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual void ProcessTrade(const TArray<FElementusItemInfo>& TradeInfo, UElementusInventoryComponent* OtherComponent = nullptr, const bool bIsFromPlayer = false);

	/* Set if the actor is focusing or not the interactable object */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual void ProcessGameplayEffect(const TSubclassOf<UGameplayEffect>& EffectClass);

	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	virtual UPEInventoryComponent* GetCastedInventoryComponent() const;

private:
	virtual APlayerController* GetOwningController() const;
};
