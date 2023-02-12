// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Attributes/PELevelingAS.h"
#include "Attributes/PEBasicStatusAS.h"
#include "Attributes/PECustomStatusAS.h"
#include "Attributes/PEAttributeData.h"
#include "Management/PEAbilitySystemSettings.h"
#include <AbilitySystemComponent.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PELevelingAS)

UPELevelingAS::UPELevelingAS(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentLevel(0.f), CurrentExperience(0.f), RequiredExperience(1.f)
{
	if (const UPEAbilitySystemSettings* const ProjectSettings = UPEAbilitySystemSettings::Get(); !ProjectSettings->LevelingAttributeMetadata.IsNull())
	{
		UAttributeSet::InitFromMetaDataTable(ProjectSettings->CustomStatusAttributeMetadata.LoadSynchronous());
	}
}

void UPELevelingAS::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetCurrentExperienceAttribute() && NewValue >= GetRequiredExperience())
	{
		UAbilitySystemComponent* const AbilityComp = GetOwningAbilitySystemComponentChecked();

		// This block will verify if the leveling bonus table exists and modify
		// the attributes accordingly the current (new) level
		if (const UPEAbilitySystemSettings* const ProjectSettings = UPEAbilitySystemSettings::Get(); !ProjectSettings->LevelingBonusAttributeMetadata.IsNull())
		{
			const UDataTable* const LevelingBonus_Table = ProjectSettings->LevelingBonusAttributeMetadata.LoadSynchronous();

			const FPELevelingData* LevelingInfo = LevelingBonus_Table->FindRow<FPELevelingData>(*FString::FromInt(GetCurrentLevel() + 1), FString());
			if (!LevelingInfo)
			{
				return;
			}

			AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxHealthAttribute(), EGameplayModOp::Additive, LevelingInfo->BonusMaxHealth);
			AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxStaminaAttribute(), EGameplayModOp::Additive, LevelingInfo->BonusMaxStamina);
			AbilityComp->ApplyModToAttribute(UPEBasicStatusAS::GetMaxManaAttribute(), EGameplayModOp::Additive, LevelingInfo->BonusMaxMana);

			AbilityComp->ApplyModToAttribute(UPECustomStatusAS::GetAttackRateAttribute(), EGameplayModOp::Additive, LevelingInfo->BonusAttackRate);
			AbilityComp->ApplyModToAttribute(UPECustomStatusAS::GetDefenseRateAttribute(), EGameplayModOp::Additive, LevelingInfo->BonusDefenseRate);

			const float NewExperience = GetCurrentExperience() - GetRequiredExperience();

			SetRequiredExperience(LevelingInfo->RequiredExp);
			SetCurrentLevel(GetCurrentLevel() + 1);
			SetCurrentExperience(NewExperience);
		}
	}
}

#pragma region Attribute Replication
void UPELevelingAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, CurrentLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, CurrentExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPELevelingAS, RequiredExperience, COND_None, REPNOTIFY_Always);
}

void UPELevelingAS::OnRep_CurrentLevel(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, CurrentLevel, OldValue);
}

void UPELevelingAS::OnRep_CurrentExperience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, CurrentExperience, OldValue);
}

void UPELevelingAS::OnRep_RequiredExperience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPELevelingAS, RequiredExperience, OldValue);
}
#pragma endregion Attribute Replication
