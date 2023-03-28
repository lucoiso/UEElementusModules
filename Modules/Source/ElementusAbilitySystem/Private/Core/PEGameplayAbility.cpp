// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Core/PEGameplayAbility.h"
#include "Core/PEAbilitySystemComponent.h"
#include "Core/PEAbilityData.h"
#include "Targeting/PETrace.h"
#include "Effects/PECooldownEffect.h"
#include "Effects/PECostEffect.h"
#include "Tasks/PESpawnProjectile_Task.h"
#include "Actors/PEAbilityProjectile.h"
#include "PEAbilityTags.h"

#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include <Abilities/Tasks/AbilityTask_WaitConfirmCancel.h>
#include <Abilities/Tasks/AbilityTask_WaitCancel.h>
#include <Abilities/Tasks/AbilityTask_WaitGameplayTag.h>
#include <Abilities/Tasks/AbilityTask_WaitTargetData.h>
#include <Abilities/Tasks/AbilityTask_SpawnActor.h>
#include <Abilities/Tasks/AbilityTask_NetworkSyncPoint.h>
#include <Abilities/GameplayAbilityTargetActor_SingleLineTrace.h>
#include <Abilities/GameplayAbilityTargetActor_GroundTrace.h>
#include <GameplayEffect.h>
#include <AbilitySystemGlobals.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystemLog.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEGameplayAbility)

UPEGameplayAbility::UPEGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), AbilityMaxRange(0), bIgnoreCost(false), bIgnoreCooldown(false), bWaitCancel(true), AbilityActiveTime(0), bEndAbilityAfterActiveTime(false)
{
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_DeadState));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_StunState));

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bIsCancelable = true;

	CostGameplayEffectClass = UPECostEffect::StaticClass();
	AbilityCostSetByCallerData.Add(TPair<FGameplayTag, float>(FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerDuration), 0.f));
	AbilityCostSetByCallerData.Add(TPair<FGameplayTag, float>(FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerHealth), 0.f));
	AbilityCostSetByCallerData.Add(TPair<FGameplayTag, float>(FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerStamina), 0.f));
	AbilityCostSetByCallerData.Add(TPair<FGameplayTag, float>(FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerMana), 0.f));
		
	CooldownGameplayEffectClass = UPECooldownEffect::StaticClass();
	AbilityCooldownSetByCallerData.Add(TPair<FGameplayTag, float>(FGameplayTag::RequestGameplayTag(GlobalTag_SetByCallerDuration), 0.f));

	SetByCallerCooldownTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_GenericCooldown));
}

void UPEGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		OwningActorName = *ActorInfo->AvatarActor->GetName();
	}

	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Ability %s given to %s."), *GetName(), *OwningActorName.ToString());
	
	// If the ability failed to activate on granting, will notify the ability system component
	if (bAutoActivateOnGrant && !ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle))
	{
		const TArray<FGameplayTag> FailureTags{
			FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.OnGive"),
			FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.TryActivate")
		};

		ActorInfo->AbilitySystemComponent->NotifyAbilityFailed(Spec.Handle, this, FGameplayTagContainer::CreateFromArray(FailureTags));
	}
}

void UPEGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	RemoveAbilityEffectsFromSelf(ActorInfo);
}

void UPEGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (!ShouldActivateAbility(ActorInfo->AvatarActor->GetLocalRole()))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Actor %s is trying to pre-activate ability %s."), *OwningActorName.ToString(), *GetName());

	// Cancel the ability if can't commit cost or cooldown. Also cancel if has not auth or is not predicted
	const bool bCanCommitAbility = CommitCheck(Handle, ActorInfo, ActivationInfo);
	const bool bFailsWithAuthority = GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalOnly || HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo);
	
	if (!bCanCommitAbility || !bFailsWithAuthority)
	{
		FGameplayTagContainer FailureTags = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{ FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.PreActivate") });

		if (!bCanCommitAbility)
		{
			FailureTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.CommitCheck"));
		}
		if (!bFailsWithAuthority)
		{
			FailureTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.HasAuthorityOrPredictionKey"));
		}
		if (GetCooldownTimeRemaining() > 0.f)
		{
			FailureTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.Cooldown"));
		}

		FGameplayTagContainer CharacterTags;
		ActorInfo->AbilitySystemComponent->GetOwnedGameplayTags(CharacterTags);

		if (CharacterTags.HasAny(ActivationBlockedTags))
		{
			FailureTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Fail.ActivationBlocked"));
		}

		ActorInfo->AbilitySystemComponent->NotifyAbilityFailed(Handle, this, FailureTags);

		PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Actor %s failed to pre-activate ability %s."), *OwningActorName.ToString(), *GetName());

		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	ActivationBlockedTags.AppendTags(AbilityTags);

	// Auto cancel can only be called on instantiated abilities. Non-Instantiated abilities can't handle tasks
	if (IsInstantiated())
	{
		UAbilityTask_WaitGameplayTagAdded* const WaitDeadTagAddedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, FGameplayTag::RequestGameplayTag(GlobalTag_DeadState));

		WaitDeadTagAddedTask->Added.AddDynamic(this, &UPEGameplayAbility::K2_EndAbility);
		WaitDeadTagAddedTask->ReadyForActivation();

		UAbilityTask_WaitGameplayTagAdded* const WaitStunTagAddedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, FGameplayTag::RequestGameplayTag(GlobalTag_StunState));

		WaitStunTagAddedTask->Added.AddDynamic(this, &UPEGameplayAbility::K2_EndAbility);
		WaitStunTagAddedTask->ReadyForActivation();

		if (CanBeCanceled() && bWaitCancel)
		{
			ActivateWaitCancelInputTask();
		}
	}

	// If auto cancel by time is active (by some undef reason), try to clear the timer and invalidate the handle
	if (CancelationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CancelationTimerHandle);
		CancelationTimerHandle.Invalidate();
	}

	// If the ability is time based, will cancel after active time
	if (bEndAbilityAfterActiveTime)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda(
			[=]() -> void
			{
				if (IsValid(this) && IsActive())
				{
					EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
				}
			}
		);

		GetWorld()->GetTimerManager().SetTimer(CancelationTimerHandle, TimerDelegate, AbilityActiveTime, false);
	}
}

void UPEGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Actor %s activated ability %s."), *OwningActorName.ToString(), *GetName());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPEGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool bReplicateEndAbility, const bool bWasCancelled)
{
	if (!IsEndAbilityValid(Handle, ActorInfo))
	{
		return;
	}

	if (!ActorInfo && IsInstantiated())
	{
		ActorInfo = GetCurrentActorInfo();
	}

	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Actor %s' ability %s has been finalized."), *OwningActorName.ToString(), *GetName());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// If auto cancel by time is active, try to clear the timer and invalidate the handle
	if (CancelationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CancelationTimerHandle);
		CancelationTimerHandle.Invalidate();
	}

	UAbilitySystemComponent* const OwningComp = ActorInfo->AbilitySystemComponent.Get();

	if (!IsValid(OwningComp))
	{
		return;
	}

	// Remove active time based cost effects
	if (IsValid(GetCostGameplayEffect()) && GetCostGameplayEffect()->DurationPolicy == EGameplayEffectDurationType::Infinite)
	{
		OwningComp->RemoveActiveGameplayEffectBySourceEffect(GetCostGameplayEffect()->GetClass(), OwningComp);
	}

	// Remove active time based buff/debuff effects from self
	if (AbilityActiveTime <= 0.f)
	{
		if (UPEAbilitySystemComponent* const TargetABSC = Cast<UPEAbilitySystemComponent>(OwningComp))
		{
			for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
			{
				TargetABSC->RemoveEffectGroupedDataFromSelf(EffectGroup, TargetABSC, 1);
			}
		}
	}

	// Remove extra tags that were given by this ability
	if (OwningComp->HasAnyMatchingGameplayTags(AbilityExtraTags))
	{
		OwningComp->RemoveLooseGameplayTags(AbilityExtraTags);
	}

	// If the 'Ignore Cooldown' failed, force remove the cooldown tag
	if (bIgnoreCooldown && GetCooldownTimeRemaining() != 0.f)
	{
		RemoveCooldownEffect(OwningComp);
	}
}

bool UPEGameplayAbility::CommitAbilityCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool ForceCooldown, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	return bIgnoreCooldown ? true : Super::CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, ForceCooldown, OptionalRelevantTags);
}

bool UPEGameplayAbility::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	return bIgnoreCost ? true : Super::CommitAbilityCost(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UPEGameplayAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!bIgnoreCooldown)
	{
		ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}

	if (!bIgnoreCost)
	{
		ApplyCost(Handle, ActorInfo, ActivationInfo);
	}
}

void UPEGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (AbilityCooldownSetByCallerData.IsEmpty() || !CooldownGameplayEffectClass)
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}
	else if (const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo)); SpecHandle.IsValid())
	{
		FGameplayTagContainer CooldownTags_Copy = *GetCooldownTags();
		CooldownTags_Copy.RemoveTag(FGameplayTag::RequestGameplayTag(GlobalTag_GenericCooldown));	

		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags_Copy);
		
		ApplySetByCallerParamsToEffectSpec(AbilityCooldownSetByCallerData, SpecHandle.Data);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UPEGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AbilityCooldownSetByCallerData.IsEmpty() || !CooldownGameplayEffectClass)
	{
		return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
	}

	FGameplayTagContainer CooldownTags_Copy = *GetCooldownTags();
	CooldownTags_Copy.RemoveTag(FGameplayTag::RequestGameplayTag(GlobalTag_GenericCooldown));

	if (CooldownTags_Copy.IsEmpty())
	{
		return true;
	}

	if (const UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		return !AbilitySystemComponent->HasAnyMatchingGameplayTags(CooldownTags_Copy);
	}

	return true;
}

const FGameplayTagContainer* UPEGameplayAbility::GetCooldownTags() const
{
	if (AbilityCooldownSetByCallerData.IsEmpty() || SetByCallerCooldownTags.IsEmpty())
	{
		return Super::GetCooldownTags();
	}

	return &SetByCallerCooldownTags;
}

void UPEGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (AbilityCostSetByCallerData.IsEmpty() || !CostGameplayEffectClass)
	{
		Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	}
	else if (const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo)); SpecHandle.IsValid())
	{
		ApplySetByCallerParamsToEffectSpec(AbilityCostSetByCallerData, SpecHandle.Data);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UPEGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AbilityCostSetByCallerData.IsEmpty() || !CostGameplayEffectClass)
	{
		return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	}
	
	else if (const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, FGameplayAbilityActivationInfo(), CostGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo)); SpecHandle.IsValid())
	{
		ApplySetByCallerParamsToEffectSpec(AbilityCostSetByCallerData, SpecHandle.Data);

		UAbilitySystemComponent* const TargetABSC = ActorInfo->AbilitySystemComponent.Get();
		if (!IsValid(TargetABSC))
		{
			return false;
		}

		SpecHandle.Data->CalculateModifierMagnitudes();
		for (int32 ModIdx = 0; ModIdx < SpecHandle.Data->Modifiers.Num(); ++ModIdx)
		{
			const FGameplayModifierInfo& ModDef = SpecHandle.Data->Def->Modifiers[ModIdx];
			const FModifierSpec& ModSpec = SpecHandle.Data->Modifiers[ModIdx];

			if (ModDef.ModifierOp == EGameplayModOp::Additive)
			{
				if (!ModDef.Attribute.IsValid())
				{
					continue;
				}

				const UAttributeSet* const Set = TargetABSC->GetAttributeSet(ModDef.Attribute.GetAttributeSetClass());
				if (!IsValid(Set))
				{
					return false;
				}

				const float CurrentValue = ModDef.Attribute.GetNumericValueChecked(Set);
				const float CostValue = ModSpec.GetEvaluatedMagnitude();

				if (CurrentValue + CostValue < 0.f)
				{
					return false;
				}
			}
		}

		return true;
	}
	
	return false;
}

void UPEGameplayAbility::ApplySetByCallerParamsToEffectSpec(const TMap<FGameplayTag, float>& SetByCallerData, const TSharedPtr<FGameplayEffectSpec>& EffectSpec) const
{
	for (const TPair<FGameplayTag, float>& StackedData : SetByCallerData)
	{
		EffectSpec.Get()->SetSetByCallerMagnitude(StackedData.Key, StackedData.Value);
	}
}

void UPEGameplayAbility::ActivateGameplayCues(const FGameplayTag GameplayCueTag, FGameplayCueParameters Parameters, UAbilitySystemComponent* SourceAbilitySystem)
{
	if (SourceAbilitySystem == nullptr)
	{
		SourceAbilitySystem = GetAbilitySystemComponentFromActorInfo_Checked();
	}

	if (GameplayCueTag.IsValid())
	{
		PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Activating %s ability associated Gameplay Cues with Tag %s for actor %s."), *GetName(), *GameplayCueTag.ToString(), *OwningActorName.ToString());

		Parameters.AbilityLevel = GetAbilityLevel();
		Parameters.Instigator = SourceAbilitySystem->GetAvatarActor();
		Parameters.EffectContext = SourceAbilitySystem->MakeEffectContext();
		Parameters.SourceObject = SourceAbilitySystem;
		
		SourceAbilitySystem->GetOwnedGameplayTags(Parameters.AggregatedSourceTags);

		SourceAbilitySystem->AddGameplayCue(GameplayCueTag, Parameters);
		TrackedGameplayCues.Add(GameplayCueTag);
	}
	else
	{
		PEABILITY_VLOG_INTERNAL(this, Warning, TEXT("Ability %s failed to activate Gameplay Cue for actor %s."), *GetName(), *OwningActorName.ToString());
	}
}

void UPEGameplayAbility::BP_ApplyAbilityEffectsToSelf()
{
	check(CurrentActorInfo);

	ApplyAbilityEffectsToSelf(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UPEGameplayAbility::ApplyAbilityEffectsToSelf(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Applying %s ability effects to actor %s."), *GetName(), *OwningActorName.ToString());

	for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
	{		
		if (const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, EffectGroup.EffectClass, GetAbilityLevel(Handle, ActorInfo));
			SpecHandle.IsValid())
		{
			ApplySetByCallerParamsToEffectSpec(EffectGroup.SetByCallerStackedData, SpecHandle.Data);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void UPEGameplayAbility::BP_RemoveAbilityEffectsFromSelf()
{
	check(CurrentActorInfo);

	RemoveAbilityEffectsFromSelf(CurrentActorInfo);
}

void UPEGameplayAbility::RemoveAbilityEffectsFromSelf(const FGameplayAbilityActorInfo* ActorInfo)
{
	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Removing %s ability effects from actor %s."), *GetName(), *OwningActorName.ToString());

	for (const FGameplayEffectGroupedData& EffectGroup : SelfAbilityEffects)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = EffectGroup.EffectClass;

		ActorInfo->AbilitySystemComponent.Get()->RemoveActiveEffects(Query);
	}
}

void UPEGameplayAbility::BP_ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle)
{
	check(CurrentActorInfo);

	ApplyAbilityEffectsToTarget(TargetDataHandle, CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UPEGameplayAbility::ApplyAbilityEffectsToTarget(const FGameplayAbilityTargetDataHandle TargetDataHandle, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Actor %s is applying %s ability effects to targets."), *OwningActorName.ToString(), *GetName());

	for (const FGameplayEffectGroupedData& EffectGroup : TargetAbilityEffects)
	{
		if (const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, EffectGroup.EffectClass, GetAbilityLevel(Handle, ActorInfo));
			SpecHandle.IsValid())
		{
			ApplySetByCallerParamsToEffectSpec(EffectGroup.SetByCallerStackedData, SpecHandle.Data);
			ApplyGameplayEffectSpecToTarget(Handle, ActorInfo, ActivationInfo, SpecHandle, TargetDataHandle);
		}
	}
}

void UPEGameplayAbility::BP_SpawnProjectileWithTargetEffects(const TSubclassOf<APEAbilityProjectile> ProjectileClass, const FTransform ProjectileTransform, const FVector ProjectileFireDirection)
{
	check(CurrentActorInfo);

	SpawnProjectileWithTargetEffects(ProjectileClass, ProjectileTransform, ProjectileFireDirection, CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
}

void UPEGameplayAbility::SpawnProjectileWithTargetEffects(const TSubclassOf<APEAbilityProjectile> ProjectileClass, const FTransform ProjectileTransform, const FVector ProjectileFireDirection, [[maybe_unused]] const FGameplayAbilitySpecHandle, [[maybe_unused]] const FGameplayAbilityActorInfo*, [[maybe_unused]] const FGameplayAbilityActivationInfo)
{
	UPESpawnProjectile_Task* const PESpawnProjectile_Task = UPESpawnProjectile_Task::SpawnProjectile(this, TEXT("SpawnProjectileTask"), ProjectileClass, ProjectileTransform, ProjectileFireDirection, TargetAbilityEffects);

	PESpawnProjectile_Task->OnProjectileSpawn.AddDynamic(this, &UPEGameplayAbility::SpawnProjectile_Callback);
	PESpawnProjectile_Task->OnSpawnFailed.AddDynamic(this, &UPEGameplayAbility::SpawnProjectile_Callback);

	PESpawnProjectile_Task->ReadyForActivation();
}

void UPEGameplayAbility::RemoveCooldownEffect(UAbilitySystemComponent* SourceAbilitySystem) const
{
	if (IsValid(GetCooldownGameplayEffect()))
	{
		PEABILITY_VLOG_INTERNAL(this, Display, TEXT("Removing %s ability cooldown from actor %s."), *GetName(), *OwningActorName.ToString());
		SourceAbilitySystem->RemoveActiveGameplayEffectBySourceEffect(CooldownGameplayEffectClass, SourceAbilitySystem);
	}
}

void UPEGameplayAbility::PlayAbilitySoundAttached(USceneComponent* InComponent, const FName SocketToAttach, const FVector InLocation)
{
	if (!IsValid(AbilitySoundData.AbilitySoundFX))
	{
		PEABILITY_VLOG_INTERNAL(this, Error, TEXT("Actor %s tried to play ability %s sound with a null sound object."), *OwningActorName.ToString(), *GetName());
		return;
	}

	if (!IsValid(InComponent))
	{
		PEABILITY_VLOG_INTERNAL(this, Error, TEXT("Actor %s tried to play ability %s sound with a null scene component target."), *OwningActorName.ToString(), *GetName());
		return;
	}

	UGameplayStatics::SpawnSoundAttached(AbilitySoundData.AbilitySoundFX, InComponent, SocketToAttach, InLocation, EAttachLocation::KeepRelativeOffset, false, AbilitySoundData.VolumeMultiplier, AbilitySoundData.PitchMultiplier, AbilitySoundData.StartTime);
}

void UPEGameplayAbility::PlayAbilitySoundAtLocation(const UObject* WorldContext, const FVector InLocation)
{
	if (!IsValid(AbilitySoundData.AbilitySoundFX))
	{
		PEABILITY_VLOG_INTERNAL(this, Error, TEXT("Actor %s tried to play ability %s sound with a null sound object."), *OwningActorName.ToString(), *GetName());
		return;
	}
	
	UGameplayStatics::SpawnSoundAtLocation(WorldContext, AbilitySoundData.AbilitySoundFX, InLocation, FRotator::ZeroRotator, AbilitySoundData.VolumeMultiplier, AbilitySoundData.PitchMultiplier, AbilitySoundData.StartTime);
}

void UPEGameplayAbility::ActivateWaitMontageTask(const FName MontageSection, const float Rate, const bool bRandomSection, const bool bStopsWhenAbilityEnds)
{
	FName MontageSectionName = MontageSection;

	if (bRandomSection)
	{
		MontageSectionName = AbilityAnimation->GetSectionName(FMath::RandRange(0, AbilityAnimation->CompositeSections.Num()));
	}

	UAbilityTask_PlayMontageAndWait* const AbilityTask_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "WaitMontageTask", AbilityAnimation, Rate, MontageSectionName, bStopsWhenAbilityEnds);

	AbilityTask_PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UPEGameplayAbility::WaitMontage_Callback);
	AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UPEGameplayAbility::K2_EndAbility);
	AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this, &UPEGameplayAbility::K2_CancelAbility);

	AbilityTask_PlayMontageAndWait->ReadyForActivation();
}

void UPEGameplayAbility::ActivateWaitTargetDataTask(const TEnumAsByte<EGameplayTargetingConfirmation::Type> TargetingConfirmation, const TSubclassOf<AGameplayAbilityTargetActor_Trace> TargetActorClass, FPETargetActorSpawnParams TargetParameters)
{
	if constexpr (&TargetParameters.StartLocation == nullptr)
	{
		TargetParameters.StartLocation = MakeTargetLocationInfoFromOwnerActor();
	}

	TargetParameters.Range = AbilityMaxRange;

	UAbilityTask_WaitTargetData* const AbilityTask_WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetDataTask", TargetingConfirmation, TargetActorClass);

	AbilityTask_WaitTargetData->Cancelled.AddDynamic(this, &UPEGameplayAbility::WaitTargetData_Callback);
	AbilityTask_WaitTargetData->ValidData.AddDynamic(this, &UPEGameplayAbility::WaitTargetData_Callback);

	// Initialize the spawning task with the TargetActor
	if (AGameplayAbilityTargetActor* TargetActor = nullptr;
		AbilityTask_WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor))
	{
		TargetActor->StartLocation = TargetParameters.StartLocation;
		TargetActor->ReticleClass = TargetParameters.ReticleClass;
		TargetActor->ReticleParams = TargetParameters.ReticleParams;
		TargetActor->bDebug = TargetParameters.bDebug;

		FGameplayTargetDataFilterHandle FilterHandle;
		FilterHandle.Filter = MakeShared<FGameplayTargetDataFilter>(TargetParameters.TargetFilter);
		TargetActor->Filter = FilterHandle;

		// Check if TargetActorClass is child of AGameplayAbilityTargetActor_Trace and add values to class params
		if (TargetActorClass.Get()->IsChildOf<AGameplayAbilityTargetActor_Trace>())
		{
			AGameplayAbilityTargetActor_Trace* const TraceObj = Cast<AGameplayAbilityTargetActor_Trace>(TargetActor);

			TraceObj->MaxRange = TargetParameters.Range;
			TraceObj->bTraceAffectsAimPitch = TargetParameters.bTraceAffectsAimPitch;

			// Check if TargetActorClass is child of AGameplayAbilityTargetActor_GroundTrace
			// and add values to class params
			if (TargetActorClass.Get()->IsChildOf<AGameplayAbilityTargetActor_GroundTrace>())
			{
				AGameplayAbilityTargetActor_GroundTrace* const GroundTraceObj = Cast<AGameplayAbilityTargetActor_GroundTrace>(TargetActor);

				GroundTraceObj->CollisionRadius = TargetParameters.Radius;
				GroundTraceObj->CollisionHeight = TargetParameters.Height;
			}
		}

		AbilityTask_WaitTargetData->FinishSpawningActor(this, TargetActor);
		TargetActor->bDestroyOnConfirmation = TargetParameters.bDestroyOnConfirmation;
		AbilityTask_WaitTargetData->ReadyForActivation();
	}

	// If Targeting is different than Instant, add the aiming tag and start waiting for confirmation
	if (AbilityTask_WaitTargetData->IsActive() && TargetingConfirmation != EGameplayTargetingConfirmation::Instant)
	{
		UAbilitySystemComponent* const Comp = GetAbilitySystemComponentFromActorInfo_Checked();

		Comp->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(GlobalTag_AimingState));
		Comp->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingConfirmationState));

		AbilityExtraTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_AimingState));
		AbilityExtraTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingConfirmationState));
	}
}

void UPEGameplayAbility::ActivateWaitConfirmInputTask()
{
	// Add extra tag to the ability system component to tell that we are waiting for confirm input
	UAbilitySystemComponent* const Comp = GetAbilitySystemComponentFromActorInfo_Checked();
	if (!AbilityExtraTags.HasTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingConfirmationState)))
	{
		Comp->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingConfirmationState));
		AbilityExtraTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingConfirmationState));
	}

	UAbilityTask_WaitConfirmCancel* const AbilityTask_WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);

	AbilityTask_WaitConfirm->OnConfirm.AddDynamic(this, &UPEGameplayAbility::WaitConfirmInput_Callback);

	// Canceling is already binded by ActivateWaitCancelInputTask()
	// We will only use it to re-activate this task if bWaitCancel is false
	// Because this WaitConfirmCancel task ends independly if Cancel or Confirm is pressed
	if (!bWaitCancel)
	{
		AbilityTask_WaitConfirm->OnCancel.AddDynamic(this, &UPEGameplayAbility::ActivateWaitConfirmInputTask);
	}
	AbilityTask_WaitConfirm->ReadyForActivation();
}

void UPEGameplayAbility::ActivateWaitCancelInputTask()
{
	// Add extra tag to the ability system component to tell that we are waiting for cancel input
	UAbilitySystemComponent* const Comp = GetAbilitySystemComponentFromActorInfo_Checked();
	if (!AbilityExtraTags.HasTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingCancelationState)))
	{
		Comp->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingCancelationState));
		AbilityExtraTags.AddTag(FGameplayTag::RequestGameplayTag(GlobalTag_WaitingCancelationState));
	}

	UAbilityTask_WaitCancel* const AbilityTask_WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);

	AbilityTask_WaitCancel->OnCancel.AddDynamic(this, &UPEGameplayAbility::WaitCancelInput_Callback);
	AbilityTask_WaitCancel->ReadyForActivation();
}

void UPEGameplayAbility::ActivateWaitAddedTagTask(const FGameplayTag Tag)
{
	UAbilityTask_WaitGameplayTagAdded* const AbilityTask_WaitGameplayTagAdded = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, Tag);

	AbilityTask_WaitGameplayTagAdded->Added.AddDynamic(this, &UPEGameplayAbility::WaitAddedTag_Callback);
	AbilityTask_WaitGameplayTagAdded->ReadyForActivation();
}

void UPEGameplayAbility::ActivateWaitRemovedTagTask(const FGameplayTag Tag)
{
	UAbilityTask_WaitGameplayTagRemoved* const AbilityTask_WaitGameplayTagRemoved = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, Tag);

	AbilityTask_WaitGameplayTagRemoved->Removed.AddDynamic(this, &UPEGameplayAbility::WaitRemovedTag_Callback);
	AbilityTask_WaitGameplayTagRemoved->ReadyForActivation();
}

void UPEGameplayAbility::ActivateWaitGameplayEventTask(const FGameplayTag EventTag)
{
	UAbilityTask_WaitGameplayEvent* const AbilityTask_WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);

	AbilityTask_WaitGameplayEvent->EventReceived.AddDynamic(this, &UPEGameplayAbility::WaitGameplayEvent_Callback);
	AbilityTask_WaitGameplayEvent->ReadyForActivation();
}

void UPEGameplayAbility::ActivateSpawnActorTask(const FGameplayAbilityTargetDataHandle TargetDataHandle, const TSubclassOf<AActor> ActorClass)
{
	UAbilityTask_SpawnActor* const AbilityTask_SpawnActor = UAbilityTask_SpawnActor::SpawnActor(this, TargetDataHandle, ActorClass);

	AbilityTask_SpawnActor->DidNotSpawn.AddDynamic(this, &UPEGameplayAbility::SpawnActor_Callback);
	AbilityTask_SpawnActor->Success.AddDynamic(this, &UPEGameplayAbility::SpawnActor_Callback);
	AbilityTask_SpawnActor->ReadyForActivation();
}

void UPEGameplayAbility::WaitCancelInput_Callback()
{
	if (CanBeCanceled())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}
