// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Tasks/PESpawnProjectile_Task.h"
#include "Actors/PEAbilityProjectile.h"
#include "Core/PEAbilityData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PESpawnProjectile_Task)

UPESpawnProjectile_Task::UPESpawnProjectile_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
}

UPESpawnProjectile_Task* UPESpawnProjectile_Task::SpawnProjectile(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const TSubclassOf<APEAbilityProjectile> ClassToSpawn, const FTransform SpawnTransform, const FVector DirectionToFire, const TArray<FGameplayEffectGroupedData> EffectDataArray)
{
	UPESpawnProjectile_Task* const MyObj = NewAbilityTask<UPESpawnProjectile_Task>(OwningAbility, TaskInstanceName);
	MyObj->ProjectileClass = ClassToSpawn;
	MyObj->ProjectileTransform = SpawnTransform;
	MyObj->ProjectileFireDirection = DirectionToFire;
	MyObj->ProjectileEffectArr = EffectDataArray;

	return MyObj;
}

void UPESpawnProjectile_Task::Activate()
{
	Super::Activate();

	check(Ability);

	// Only the server can spawn actors!
	if (!Ability->GetActorInfo().IsNetAuthority())
	{
		EndTask();
	}

	if (ensureAlwaysMsgf(ProjectileClass != nullptr, TEXT("%s - Task %s failed to activate because projectile class is null"), *FString(__func__), *GetName()))
	{
		APEAbilityProjectile* const SpawnedProjectile = GetWorld()->SpawnActorDeferred<APEAbilityProjectile>(ProjectileClass, ProjectileTransform, GetOwnerActor(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		SpawnedProjectile->ProjectileEffects = ProjectileEffectArr;
		SpawnedProjectile->FinishSpawning(ProjectileTransform);

		if (IsValid(SpawnedProjectile))
		{
			SpawnedProjectile->FireInDirection(ProjectileFireDirection);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnProjectileSpawn.Broadcast(SpawnedProjectile);
			}
		}
		else if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnSpawnFailed.Broadcast(nullptr);
		}
	}
	else if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnSpawnFailed.Broadcast(nullptr);
	}	

	UE_LOG(LogElementusAbilitySystem_Internal, Display, TEXT("%s - Task %s ended"), *FString(__func__), *GetName());
	EndTask();
}
