// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include "PEEnemyData.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSACTORS_API UPEEnemyData final : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    explicit UPEEnemyData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("PE_EnemyData"), *("Enemy_" + FString::FromInt(EnemyId)));
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus", meta = (AssetBundles = "Data"))
    int32 EnemyId;
};
