// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Management/PEInventorySystemSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInventorySystemSettings)

UPEInventorySystemSettings::UPEInventorySystemSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CategoryName = TEXT("Plugins");
}

const UPEInventorySystemSettings* UPEInventorySystemSettings::Get()
{
	static const UPEInventorySystemSettings* const Instance = GetDefault<UPEInventorySystemSettings>();
	return Instance;
}
