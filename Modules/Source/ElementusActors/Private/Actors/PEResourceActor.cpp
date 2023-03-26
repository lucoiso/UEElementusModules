// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEResourceActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEResourceActor)

APEResourceActor::APEResourceActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = false;
	AActor::SetReplicateMovement(false);
	NetUpdateFrequency = 100.f;
	NetPriority = 1.f;

	// Not implemented yet
}
