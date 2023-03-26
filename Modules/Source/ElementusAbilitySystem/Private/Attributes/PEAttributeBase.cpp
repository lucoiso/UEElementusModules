// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Attributes/PEAttributeBase.h"
#include "Core/PEAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAttributeBase)

UPEAttributeBase::UPEAttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{	
}

void UPEAttributeBase::InitFromMetaDataTable(const UDataTable* DataTable)
{
	if (UPEAbilitySystemComponent* const AbilityComp = GetCastedAbilitySystemComponent<UPEAbilitySystemComponent>())
	{
		AbilityComp->InitializeAttributeViewModel(this);
	}

	Super::InitFromMetaDataTable(DataTable);
}
