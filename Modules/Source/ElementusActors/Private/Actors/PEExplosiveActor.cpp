// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEExplosiveActor.h"
#include <Core/PEAbilitySystemComponent.h>
#include <Core/PEAbilityData.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraSystem.h>
#include <GameFramework/Character.h>
#include <AbilitySystemGlobals.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEExplosiveActor)

APEExplosiveActor::APEExplosiveActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), ExplosionRadius(150.f), ExplosionMagnitude(1000.f), bDestroyAfterExplosion(true)
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APEExplosiveActor::PerformExplosion()
{
	// Only call SetReplicates if has authority
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// Only replicates while exploding
	SetReplicates(true);

	TArray<FHitResult> HitOut;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.MobilityType = EQueryMobilityType::Dynamic;

#if UE_ENABLE_DEBUG_DRAWING
	const FName TraceTag("SphereTraceDebugTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	QueryParams.TraceTag = TraceTag;
#endif

	GetWorld()->SweepMultiByObjectType(HitOut, GetActorLocation(), GetActorLocation(), FQuat(FRotator(0.f)), FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(ExplosionRadius), QueryParams);

	for (UNiagaraSystem* const& NiagaraSystem : ExplosionVFXs)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, GetActorLocation());
	}

	for (const FHitResult& Hit : HitOut)
	{
		if (!IsValid(Hit.GetActor()))
		{
			continue;
		}

		const FVector Velocity = ExplosionMagnitude * (Hit.GetActor()->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		if (Hit.GetActor()->GetClass()->IsChildOf<ACharacter>())
		{
			if (ACharacter* const Player = Cast<ACharacter>(Hit.GetActor()))
			{
				Player->LaunchCharacter(Velocity, true, true);
			}
		}
		else if (Hit.GetActor()->IsRootComponentMovable())
		{
			Hit.GetComponent()->AddForce(Velocity);
			Hit.GetComponent()->AddImpulse(Velocity);
		}

		if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Hit.GetActor()))
		{
			ApplyExplosibleEffect(TargetABSC);
		}
	}

	if (bDestroyAfterExplosion)
	{
		Destroy();
		return;
	}

	// Only replicates while exploding
	SetReplicates(false);
}

void APEExplosiveActor::ApplyExplosibleEffect(UAbilitySystemComponent* TargetABSC)
{
	if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
	{
		for (const FGameplayEffectGroupedData& Effect : ExplosionEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}
	}
}
