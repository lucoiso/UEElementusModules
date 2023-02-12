// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Attributes/PEAttributeBase.h"
#include "Core/PEAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAttributeBase)

UPEAttributeBase::UPEAttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{	
}

void UPEAttributeBase::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, const float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	if (const float CurrentMaxValue = MaxAttribute.GetCurrentValue(); !FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue))
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = CurrentMaxValue > 0.f ? CurrentValue * NewMaxValue / CurrentMaxValue - CurrentValue : NewMaxValue;

		GetOwningAbilitySystemComponentChecked()->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UPEAttributeBase::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);

	if (UPEAbilitySystemComponent* const AbilityComp = GetCastedAbilitySystemComponent<UPEAbilitySystemComponent>())
	{
		AbilityComp->InitializeAttributeViewModel(this);
	}
}
