// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEConsumableActor.h"
#include "Data/PEConsumableData.h"
#include <Core/PEAbilitySystemComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/Character.h>
#include <NiagaraComponent.h>
#include <AbilitySystemGlobals.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEConsumableActor)

APEConsumableActor::APEConsumableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bDestroyAfterConsumption(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = false;
	AActor::SetReplicateMovement(false);
	NetUpdateFrequency = 30.f;
	NetPriority = 1.f;
	NetDormancy = ENetDormancy::DORM_Initial;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object Mesh"));
	ObjectMesh->SetCollisionProfileName("Consumable");
	ObjectMesh->SetupAttachment(RootComponent);

	ObjectVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Object VFX"));
	ObjectVFX->bAutoActivate = true;
	ObjectVFX->SetupAttachment(ObjectMesh);
}

void APEConsumableActor::PerformConsumption(UAbilitySystemComponent* TargetABSC)
{
	if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC);
		TargetGASC->HasAllMatchingGameplayTags(ConsumableData->RequirementsTags) || ConsumableData->RequirementsTags.IsEmpty())
	{
		for (const FGameplayEffectGroupedData& Effect : ConsumableData->ConsumableEffects)
		{
			TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
		}

		if (bDestroyAfterConsumption)
		{
			Destroy();
		}
	}
}

void APEConsumableActor::DoInteractionBehavior_Implementation(ACharacter* CharacterInteracting, const FHitResult& HitResult)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterInteracting))
	{
		SetNetDormancy(ENetDormancy::DORM_Awake);
		PerformConsumption(TargetABSC);
		SetNetDormancy(ENetDormancy::DORM_DormantAll);
	}
}

#if WITH_EDITOR
void APEConsumableActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(APEConsumableActor, ConsumableData))
	{
		// Update data with values in the given UDataAsset
		if (IsValid(ConsumableData))
		{
			!ConsumableData->ObjectMesh.IsNull() ? ObjectMesh->SetStaticMesh(ConsumableData->ObjectMesh.LoadSynchronous()) : ObjectMesh->SetStaticMesh(nullptr);

			!ConsumableData->ObjectVFX.IsNull() ? ObjectVFX->SetAsset(ConsumableData->ObjectVFX.LoadSynchronous()) : ObjectVFX->SetAsset(nullptr);
		}
	}
}
#endif WITH_EDITOR
