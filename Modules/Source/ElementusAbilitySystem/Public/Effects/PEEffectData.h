// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>
#include "PEEffectData.generated.h"

class UGameplayEffect;

/**
 *
 */
USTRUCT(BlueprintType, Category = "Project Elementus | Structs")
struct FGameplayEffectGroupedData
{
    GENERATED_BODY()

    FGameplayEffectGroupedData() = default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
    TSubclassOf<UGameplayEffect> EffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
    TMap<FGameplayTag, float> SetByCallerStackedData;
};