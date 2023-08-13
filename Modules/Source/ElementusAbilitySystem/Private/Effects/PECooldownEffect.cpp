// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Effects/PECooldownEffect.h"
#include "PEAbilityTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECooldownEffect)

UPECooldownEffect::UPECooldownEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;

    FSetByCallerFloat SetByCallerDuration;
    SetByCallerDuration.DataTag = FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerDuration);

    DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCallerDuration);
}