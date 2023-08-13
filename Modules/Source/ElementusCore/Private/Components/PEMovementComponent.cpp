// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Components/PEMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEMovementComponent)

UPEMovementComponent::UPEMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}