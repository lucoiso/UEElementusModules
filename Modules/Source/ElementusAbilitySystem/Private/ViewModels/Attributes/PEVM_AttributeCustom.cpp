// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "ViewModels/Attributes/PEVM_AttributeCustom.h"
#include "Attributes/PECustomStatusAS.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEVM_AttributeCustom)

UPEVM_AttributeCustom::UPEVM_AttributeCustom(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), AttackRate(-1.f), DefenseRate(-1.f), SpeedRate(-1.f), JumpRate(-1.f), Gold(-1.f)
{
}

void UPEVM_AttributeCustom::NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue)
{
    Super::NotifyAttributeChange(Attribute, NewValue);

    CHECK_ATTRIBUTE_AND_SET_VALUE(UPECustomStatusAS, AttackRate);
    CHECK_ATTRIBUTE_AND_SET_VALUE(UPECustomStatusAS, DefenseRate);
    CHECK_ATTRIBUTE_AND_SET_VALUE(UPECustomStatusAS, SpeedRate);
    CHECK_ATTRIBUTE_AND_SET_VALUE(UPECustomStatusAS, JumpRate);
    CHECK_ATTRIBUTE_AND_SET_VALUE(UPECustomStatusAS, Gold);
}

void UPEVM_AttributeCustom::SetAttackRate(const float Value)
{
    UPDATE_MVVM_PROPERTY_VALUE(AttackRate, Value);
}

float UPEVM_AttributeCustom::GetAttackRate() const
{
    return AttackRate;
}

void UPEVM_AttributeCustom::SetDefenseRate(const float Value)
{
    UPDATE_MVVM_PROPERTY_VALUE(DefenseRate, Value);
}

float UPEVM_AttributeCustom::GetDefenseRate() const
{
    return DefenseRate;
}

void UPEVM_AttributeCustom::SetSpeedRate(const float Value)
{
    UPDATE_MVVM_PROPERTY_VALUE(SpeedRate, Value);
}

float UPEVM_AttributeCustom::GetSpeedRate() const
{
    return SpeedRate;
}

void UPEVM_AttributeCustom::SetJumpRate(const float Value)
{
    UPDATE_MVVM_PROPERTY_VALUE(JumpRate, Value);
}

float UPEVM_AttributeCustom::GetJumpRate() const
{
    return JumpRate;
}

void UPEVM_AttributeCustom::SetGold(const float Value)
{
    UPDATE_MVVM_PROPERTY_VALUE(Gold, Value);
}

float UPEVM_AttributeCustom::GetGold() const
{
    return Gold;
}