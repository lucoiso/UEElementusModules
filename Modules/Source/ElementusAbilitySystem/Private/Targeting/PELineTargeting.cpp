// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Targeting/PELineTargeting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PELineTargeting)

APELineTargeting::APELineTargeting(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    TraceProfile = FCollisionProfileName(TEXT("Target"));
}