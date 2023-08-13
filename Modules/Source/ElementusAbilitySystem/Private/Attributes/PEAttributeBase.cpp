// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Attributes/PEAttributeBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAttributeBase)

UPEAttributeBase::UPEAttributeBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPEAttributeBase::InitFromMetaDataTable(const UDataTable* DataTable)
{
    Super::InitFromMetaDataTable(DataTable);
}