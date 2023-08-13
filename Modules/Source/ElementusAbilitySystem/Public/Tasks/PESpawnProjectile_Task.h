// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Abilities/Tasks/AbilityTask.h>
#include "Effects/PEEffectData.h"
#include "PESpawnProjectile_Task.generated.h"

class APEAbilityProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnProjectileDelegate, APEAbilityProjectile*, SpawnedProjectile);

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSABILITYSYSTEM_API UPESpawnProjectile_Task final : public UAbilityTask
{
    GENERATED_BODY()

public:
    explicit UPESpawnProjectile_Task(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /* Create a reference to manage this ability task */
    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UPESpawnProjectile_Task* SpawnProjectile(UGameplayAbility* OwningAbility, const FName TaskInstanceName, TSubclassOf<APEAbilityProjectile> ClassToSpawn, const FTransform SpawnTransform, const FVector DirectionToFire, const TArray<FGameplayEffectGroupedData> EffectDataArray);

    virtual void Activate() override;

    UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
    FSpawnProjectileDelegate OnProjectileSpawn;

    UPROPERTY(BlueprintAssignable, Category = "Project Elementus | Delegates")
    FSpawnProjectileDelegate OnSpawnFailed;

protected:
    FTransform ProjectileTransform;
    FVector ProjectileFireDirection;
    TSubclassOf<APEAbilityProjectile> ProjectileClass;
    TArray<FGameplayEffectGroupedData> ProjectileEffectArr;
};
