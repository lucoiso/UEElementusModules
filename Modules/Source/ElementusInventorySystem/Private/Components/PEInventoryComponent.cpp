// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Components/PEInventoryComponent.h"
#include "Interfaces/PEEquipment.h"
#include "LogElementusInventorySystem.h"
#include <Core/PEAbilitySystemComponent.h>
#include <Core/PEAbilityFunctions.h>
#include <Effects/PEEffectData.h>
#include <Management/ElementusInventoryFunctions.h>
#include <GameFramework/Character.h>
#include <AbilitySystemGlobals.h>
#include <PEAbilityTags.h>
#include <MFEA_Settings.h>
#include <GameplayEffect.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInventoryComponent)

UPEInventoryComponent::UPEInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UPEInventoryComponent::CanGiveItem(const FElementusItemInfo InItemInfo) const
{
    // We cannot give the item if it is currently equiped
    return Super::CanGiveItem(InItemInfo) && !InItemInfo.Tags.HasTag(FGameplayTag::RequestGameplayTag(GlobalTag_EquipSlot_Base));
}

void UPEInventoryComponent::EquipItem(const FElementusItemInfo& InItem)
{
    if (TryEquipItem_Internal(InItem))
    {
        NotifyInventoryChange();
    }
}

void UPEInventoryComponent::UnequipItem(FElementusItemInfo& InItem)
{
    if (TryUnequipItem_Internal(InItem))
    {
        NotifyInventoryChange();
    }
}

void UPEInventoryComponent::UnnequipAll(UAbilitySystemComponent* OwnerABSC)
{
    if (EquipmentMap.IsEmpty())
    {
        return;
    }

    TArray<FElementusItemInfo> ItemsToUnequip;
    for (auto& [SlotTag, ItemInfo] : EquipmentMap)
    {
        ItemsToUnequip.Add(ItemInfo);
    }

    for (FElementusItemInfo& ItemInfo : ItemsToUnequip)
    {
        TryUnequipItem_Internal(ItemInfo);
    }
}

void UPEInventoryComponent::ApplyItemEffect(const TSubclassOf<UGameplayEffect> EffectClass)
{
    ApplyItemEffect_Server(EffectClass);
}

UPEEquipment* UPEInventoryComponent::LoadEquipamentAsset(const FPrimaryElementusItemId& ItemId)
{
    if (const UElementusItemData* const ItemData = UElementusInventoryFunctions::GetSingleItemDataById(ItemId, { "SoftData" }, false))
    {
        return Cast<UPEEquipment>(ItemData->ItemClass.LoadSynchronous()->GetDefaultObject());
    }

    UE_LOG(LogElementusInventorySystem_Internal, Error, TEXT("%s - Failed to load item %s"), *FString(__func__), *ItemId.ToString())

        return nullptr;
}

bool UPEInventoryComponent::CheckInventoryAndItem(const FElementusItemInfo& InItem) const
{
    if (!IsValid(GetOwner()))
    {
        UE_LOG(LogElementusInventorySystem_Internal, Error, TEXT("%s - Invalid owning actor"), *FString(__func__));
        return false;
    }

    if (!ContainsItem(InItem, true))
    {
        UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - The item %s is not in the %s's inventory"), *FString(__func__), *InItem.ItemId.ToString(), *GetOwner()->GetName());
        return false;
    }

    if (!GetOwner()->GetClass()->IsChildOf<ACharacter>())
    {
        UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Owning actor isn't a Character"), *FString(__func__));
        return false;
    }

    return true;
}

UPEAbilitySystemComponent* UPEInventoryComponent::GetCharacterPEABSC(ACharacter* Character) const
{
    if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
    {
        return Cast<UPEAbilitySystemComponent>(TargetABSC);
    }

    return nullptr;
}

bool UPEInventoryComponent::TryEquipItem_Internal(const FElementusItemInfo& InItem)
{
    if (!CheckInventoryAndItem(InItem))
    {
        return false;
    }

    if (UPEEquipment* const EquipedItem = LoadEquipamentAsset(InItem.ItemId))
    {
        const FGameplayTagContainer EquipmentSlotTags = EquipedItem->EquipmentSlotTags;
        if (int32 FoundTagIndex; FindFirstItemIndexWithTags(EquipmentSlotTags, FoundTagIndex, FGameplayTagContainer::EmptyContainer))
        {
            // Already equipped
            UE_LOG(LogElementusInventorySystem_Internal, Display, TEXT("%s - Actor %s has already unequipped item %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());

            UnequipItem(GetItemReferenceAt(FoundTagIndex));
        }
        else if (int32 FoundInfoIndex; FindFirstItemIndexWithInfo(InItem, FoundInfoIndex, FGameplayTagContainer::EmptyContainer))
        {
            FElementusItemInfo& ItemRef = GetItemReferenceAt(FoundInfoIndex);
            ItemRef.Tags.AppendTags(EquipmentSlotTags);

            ProcessEquipmentAddition_Internal(Cast<ACharacter>(GetOwner()), EquipedItem);

            for (const FGameplayTag& Iterator : EquipmentSlotTags)
            {
                EquipmentMap.Add(Iterator, InItem);
            }

            UE_LOG(LogElementusInventorySystem, Display, TEXT("%s - Actor %s equipped %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());
        }
        else
        {
            UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Failed to find item %s in %s's inventory"), *FString(__func__), *InItem.ItemId.ToString(), *GetOwner()->GetName());
            UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
            return false;
        }

        UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
        return true;
    }

    return false;
}

bool UPEInventoryComponent::TryUnequipItem_Internal(FElementusItemInfo& InItem)
{
    if (!CheckInventoryAndItem(InItem))
    {
        return false;
    }

    if (UPEEquipment* const EquipedItem = LoadEquipamentAsset(InItem.ItemId))
    {
        ProcessEquipmentRemoval_Internal(Cast<ACharacter>(GetOwner()), EquipedItem);

        const FGameplayTagContainer EquipmentSlotTags = EquipedItem->EquipmentSlotTags;
        for (const FGameplayTag& Iterator : EquipmentSlotTags)
        {
            EquipmentMap.Remove(Iterator);
        }

        InItem.Tags.RemoveTags(EquipmentSlotTags);

        UE_LOG(LogElementusInventorySystem, Display, TEXT("%s - Actor %s unequipped %s"), *FString(__func__), *GetOwner()->GetName(), *InItem.ItemId.ToString());

        UElementusInventoryFunctions::UnloadElementusItem(InItem.ItemId);
        return true;
    }

    return false;
}

void UPEInventoryComponent::ProcessEquipmentAddition_Internal(ACharacter* OwningCharacter, UPEEquipment* Equipment)
{
    if (UPEAbilitySystemComponent* const TargetABSC = GetCharacterPEABSC(OwningCharacter))
    {
        AddEquipmentGASData_Server(TargetABSC, Equipment);
        TargetABSC->AddLooseGameplayTags(Equipment->EquipmentSlotTags);
    }

    if (GetOwnerRole() == ROLE_Authority)
    {
        ProcessEquipmentAttachment_Multicast(OwningCharacter->GetMesh(), Equipment);
    }
    else
    {
        ProcessEquipmentAttachment_Server(OwningCharacter->GetMesh(), Equipment);
    }
}

void UPEInventoryComponent::ProcessEquipmentRemoval_Internal(ACharacter* OwningCharacter, UPEEquipment* Equipment)
{
    if (UPEAbilitySystemComponent* const TargetABSC = GetCharacterPEABSC(OwningCharacter))
    {
        RemoveEquipmentGASData_Server(TargetABSC, Equipment);
        TargetABSC->RemoveLooseGameplayTags(Equipment->EquipmentSlotTags);
    }

    if (GetOwnerRole() == ROLE_Authority)
    {
        ProcessEquipmentDettachment_Multicast(Equipment);
    }
    else
    {
        ProcessEquipmentDettachment_Server(Equipment);
    }
}

void UPEInventoryComponent::AddEquipmentGASData_Server_Implementation(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment)
{
    // Add equipment effects
    for (const FGameplayEffectGroupedData& Effect : Equipment->EquipmentEffects)
    {
        TargetABSC->ApplyEffectGroupedDataToSelf(Effect);
    }

    UEnum* const InputIdEnum = UMFEA_Settings::Get()->InputIDEnumeration.LoadSynchronous();

    // Add equipment abilities
    for (const auto& [InInputID_Name, InAbilityClass] : Equipment->EquipmentAbilities)
    {
        if (InInputID_Name.IsNone())
        {
            UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Invalid InputID"), *FString(__func__));
            continue;
        }

        if (!IsValid(InAbilityClass))
        {
            UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Invalid Ability Class"), *FString(__func__));
            continue;
        }

        UE_LOG(LogElementusInventorySystem_Internal, Display, TEXT("%s - Binding ability %s with InputId %s"), *FString(__func__), *InAbilityClass->GetName(), *InInputID_Name.ToString());
        UPEAbilityFunctions::GiveAbility(TargetABSC, InAbilityClass, InInputID_Name, InputIdEnum, false, true);
    }
}

void UPEInventoryComponent::RemoveEquipmentGASData_Server_Implementation(UPEAbilitySystemComponent* TargetABSC, UPEEquipment* Equipment)
{
    // Remove equipment effects
    for (const FGameplayEffectGroupedData& Effect : Equipment->EquipmentEffects)
    {
        TargetABSC->RemoveEffectGroupedDataFromSelf(Effect, TargetABSC, 1);
    }

    // Remove equipment abilities
    for (const auto& [InInputID_Name, InAbilityClass] : Equipment->EquipmentAbilities)
    {
        if (InInputID_Name.IsNone())
        {
            UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Invalid InputID"), *FString(__func__));
            continue;
        }

        if (!IsValid(InAbilityClass))
        {
            UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - Invalid Ability Class"), *FString(__func__));
            continue;
        }

        UE_LOG(LogElementusInventorySystem_Internal, Display, TEXT("%s - Removing ability %s with InputId %s"), *FString(__func__), *InAbilityClass->GetName(), *InInputID_Name.ToString());
        UPEAbilityFunctions::RemoveAbility(TargetABSC, InAbilityClass);
    }
}

void UPEInventoryComponent::ProcessEquipmentAttachment_Server_Implementation(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment)
{
    ProcessEquipmentAttachment_Multicast(TargetMesh, Equipment);
}

void UPEInventoryComponent::ProcessEquipmentAttachment_Multicast_Implementation(USkeletalMeshComponent* TargetMesh, UPEEquipment* Equipment)
{
    USkeletalMeshComponent* const InMesh = NewObject<USkeletalMeshComponent>(GetOwner(), USkeletalMeshComponent::StaticClass(), *Equipment->GetName());
    if (!IsValid(InMesh))
    {
        UE_LOG(LogElementusInventorySystem_Internal, Error, TEXT("%s - Failed to create skeletal mesh"), *FString(__func__));
        return;
    }

    InMesh->SetIsReplicated(true);
    InMesh->SetSkeletalMesh(Equipment->EquipmentMesh.LoadSynchronous());
    InMesh->ComponentTags.Add(*FString::Printf(TEXT("ElementusEquipment_%s"), *Equipment->GetName()));

    GetOwner()->AddOwnedComponent(InMesh);

    if (!InMesh->AttachToComponent(TargetMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Equipment->SocketToAttach))
    {
        UE_LOG(LogElementusInventorySystem_Internal, Error, TEXT("%s - Failed to attach mesh to character"), *FString(__func__));
    }

    GetOwner()->FinishAndRegisterComponent(InMesh);
}

void UPEInventoryComponent::ProcessEquipmentDettachment_Server_Implementation(UPEEquipment* Equipment)
{
    ProcessEquipmentDettachment_Multicast(Equipment);
}

void UPEInventoryComponent::ProcessEquipmentDettachment_Multicast_Implementation(UPEEquipment* Equipment)
{
    const TArray<UActorComponent*> CompArr = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), *FString::Printf(TEXT("ElementusEquipment_%s"), *Equipment->GetName()));

    if (CompArr.IsEmpty())
    {
        UE_LOG(LogElementusInventorySystem_Internal, Warning, TEXT("%s - %s have no equipment attached"), *FString(__func__), *GetOwner()->GetName());
        return;
    }

    for (UActorComponent* const& Iterator : CompArr)
    {
        Iterator->UnregisterComponent();
        Iterator->RemoveFromRoot();
        Iterator->DestroyComponent();
    }
}

void UPEInventoryComponent::ApplyItemEffect_Server_Implementation(const TSubclassOf<UGameplayEffect> EffectClass)
{
    if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
    {
        TargetABSC->ApplyGameplayEffectToSelf(EffectClass.GetDefaultObject(), 1.f, TargetABSC->MakeEffectContext());
    }
}