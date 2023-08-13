// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "Effects/PEEffectData.h"
#include "PEAbilityProjectile.generated.h"

class UAbilitySystemComponent;
class USphereComponent;
class UProjectileMovementComponent;
/**
 *
 */
UCLASS(Abstract, Blueprintable, Category = "Project Elementus | Classes")
class ELEMENTUSABILITYSYSTEM_API APEAbilityProjectile : public AActor
{
    GENERATED_BODY()

public:
    explicit APEAbilityProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Project Elementus | Properties")
    float ImpulseMultiplier = 1.f;

    virtual void BeginPlay() override;

public:
    /* Gameplay Effects and SetByCaller parameters that will be applied to target */
    UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn = true), Category = "Project Elementus | Properties", Meta = (TitleProperty = "{EffectClass}"))
    TArray<FGameplayEffectGroupedData> ProjectileEffects;

    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
    void FireInDirection(const FVector& Direction);

protected:
    UFUNCTION(BlueprintNativeEvent, Category = "Project Elementus | Functions")
    void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    void ApplyProjectileEffect(UAbilitySystemComponent* TargetABSC);
};
