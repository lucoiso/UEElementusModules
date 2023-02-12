// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEAbilitySystemSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAbilitySystemSettings)

UPEAbilitySystemSettings::UPEAbilitySystemSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CategoryName = TEXT("Plugins");
}

const UPEAbilitySystemSettings* UPEAbilitySystemSettings::Get()
{
	static const UPEAbilitySystemSettings* const Instance = GetDefault<UPEAbilitySystemSettings>();
	return Instance;
}
