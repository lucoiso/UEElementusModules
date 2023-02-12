// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Attributes/PECustomStatusAS.h"
#include "Management/PEAbilitySystemSettings.h"
#include <GameplayEffectExtension.h>
#include <GameFramework/PlayerState.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECustomStatusAS)

UPECustomStatusAS::UPECustomStatusAS(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), DefaultCharJumpRate(1.f), DefaultCharWalkSpeedRate(1.f), DefaultCharCrouchSpeedRate(1.f), AttackRate(1.f), DefenseRate(1.f), SpeedRate(1.f), JumpRate(1.f), Gold(0.f)
{
	if (const UPEAbilitySystemSettings* const ProjectSettings = UPEAbilitySystemSettings::Get(); !ProjectSettings->CustomStatusAttributeMetadata.IsNull())
	{
		UAttributeSet::InitFromMetaDataTable(ProjectSettings->CustomStatusAttributeMetadata.LoadSynchronous());
	}
}

void UPECustomStatusAS::PostAttributeChange(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetSpeedRateAttribute() || Attribute == GetJumpRateAttribute())
	{
		if (UCharacterMovementComponent* const MovComp = GetOwningCharacterMovementComponent())
		{
			if (Attribute == GetSpeedRateAttribute())
			{
				MovComp->MaxWalkSpeed = DefaultCharWalkSpeedRate * NewValue;
				MovComp->MaxWalkSpeedCrouched = DefaultCharCrouchSpeedRate * NewValue;
			}
			else if (Attribute == GetJumpRateAttribute())
			{
				MovComp->JumpZVelocity = DefaultCharJumpRate * NewValue;
			}
		}
	}
}

void UPECustomStatusAS::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);

	if (UCharacterMovementComponent* const MovComp = GetOwningCharacterMovementComponent())
	{
		DefaultCharJumpRate = MovComp->JumpZVelocity;
		DefaultCharWalkSpeedRate = MovComp->MaxWalkSpeed;
		DefaultCharCrouchSpeedRate = MovComp->MaxWalkSpeedCrouched;

		MARK_PROPERTY_DIRTY_FROM_NAME(UPECustomStatusAS, DefaultCharJumpRate, this);
		MARK_PROPERTY_DIRTY_FROM_NAME(UPECustomStatusAS, DefaultCharWalkSpeedRate, this);
		MARK_PROPERTY_DIRTY_FROM_NAME(UPECustomStatusAS, DefaultCharCrouchSpeedRate, this);
	}
}

UCharacterMovementComponent* UPECustomStatusAS::GetOwningCharacterMovementComponent() const
{
	if (const APlayerState* const State = Cast<APlayerState>(GetOwningActor()))
	{
		if (const ACharacter* const Character = State->GetPawn<ACharacter>())
		{
			return Character->GetCharacterMovement();
		}
	}

	return nullptr;
}

#pragma region Attribute Replication
void UPECustomStatusAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, AttackRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, DefenseRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, SpeedRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, JumpRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPECustomStatusAS, Gold, COND_None, REPNOTIFY_Always);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(UPECustomStatusAS, DefaultCharJumpRate, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UPECustomStatusAS, DefaultCharWalkSpeedRate, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UPECustomStatusAS, DefaultCharCrouchSpeedRate, SharedParams);
}

void UPECustomStatusAS::OnRep_AttackRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, AttackRate, OldValue);
}

void UPECustomStatusAS::OnRep_DefenseRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, DefenseRate, OldValue);
}

void UPECustomStatusAS::OnRep_SpeedRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, SpeedRate, OldValue);
}

void UPECustomStatusAS::OnRep_JumpRate(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, JumpRate, OldValue);
}

void UPECustomStatusAS::OnRep_Gold(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPECustomStatusAS, Gold, OldValue);
}
#pragma endregion Attribute Replication
