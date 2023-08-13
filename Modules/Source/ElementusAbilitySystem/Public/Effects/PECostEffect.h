// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameplayEffect.h>
#include "PECostEffect.generated.h"

/**
 *
 */
UCLASS(Category = "Project Elementus | Classes")
class ELEMENTUSABILITYSYSTEM_API UPECostEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
    explicit UPECostEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
