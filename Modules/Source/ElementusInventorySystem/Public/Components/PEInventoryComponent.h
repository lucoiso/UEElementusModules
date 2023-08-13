// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Components/ElementusInventoryComponent.h>
#include "PEInventoryComponent.generated.h"

class UPEEquipment;
class ACharacter;
class UPEAbilitySystemComponent;
class UGameplayEffect;

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), Category = "Project Elementus | Classes", EditInlineNew, meta = (BlueprintSpawnableComponent))
class ELEMENTUSINVENTORYSYSTEM_API UPEInventoryComponent : public UElementusInventoryComponent
{
    GENERATED_BODY()

public:
    explicit UPEInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual bool CanGiveItem(const FElementusItemInfo InItemInfo) const override;

    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
    void EquipItem(const FElementusItemInfo& InItem);

    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
    void UnequipItem(FElementusItemInfo& InItem);

    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
    void UnnequipAll(UAbilitySystemComponent* OwnerABSC = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
    void ApplyItemEffect(const TSubclassOf<UGameplayEffect> EffectClass);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Project Elementus | Properties")
    TMap<FGameplayTag, FElementusItemInfo> EquipmentMap;

    UPEEquipment* LoadEquipamentAsset(const FPrimaryElementusItemId& ItemId);

    bool CheckInventoryAndItem(const FElementusItemInfo& InItem) const;
    class UPEAbilitySystemComponent* GetCharacterPEABSC(ACharacter* Character) const;

private:
    bool TryEquipItem_Internal(const FElementusItemInfo& InItem);
    bool TryUnequipItem_Internal(FElementusItemInfo& InItem);

    void ProcessEquipmentAddition_Internal(ACharacter* OwningCharacter, UPEEquipment* Equipment);
    void ProcessEquipmentRemoval_Internal(ACharacter* OwningCharacter, UPEEquipment* Equipment);

    UFUNCTION(Server, Reliable)
    void AddEquipmentGASData_Server(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment);

    UFUNCTION(Server, Reliable)
    void RemoveEquipmentGASData_Server(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment);

    UFUNCTION(Server, Reliable)
    void ProcessEquipmentAttachment_Server(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment);

    UFUNCTION(Server, Reliable)
    void ProcessEquipmentDettachment_Server(UPEEquipment* Equipment);

    UFUNCTION(NetMulticast, Reliable)
    void ProcessEquipmentAttachment_Multicast(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment);

    UFUNCTION(NetMulticast, Reliable)
    void ProcessEquipmentDettachment_Multicast(UPEEquipment* Equipment);

    UFUNCTION(Server, Reliable)
    void ApplyItemEffect_Server(TSubclassOf<UGameplayEffect> EffectClass);
};
