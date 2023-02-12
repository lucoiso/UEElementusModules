// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Interfaces/PEElementusInventoryProcessor.h"
#include "Components/PEInventoryComponent.h"
#include <Management/ElementusInventoryFunctions.h>
#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEElementusInventoryProcessor)

APlayerController* IPEElementusInventoryProcessor::GetOwningController() const
{
	return const_cast<APlayerController*>(Cast<APlayerController>(this));
}

UPEInventoryComponent* IPEElementusInventoryProcessor::GetCastedInventoryComponent() const
{
	APlayerController* OwningController = GetOwningController();
	if (!IsValid(OwningController->GetPawn()))
	{
		return nullptr;
	}

	if (UActorComponent* const MatchingComponent = OwningController->GetPawn()->GetComponentByClass(UElementusInventoryComponent::StaticClass()); !IsValid(MatchingComponent))
	{
		return nullptr;
	}
	else
	{
		return Cast<UPEInventoryComponent>(MatchingComponent);
	}
}

void IPEElementusInventoryProcessor::ProcessTrade(const TArray<FElementusItemInfo>& TradeInfo, UElementusInventoryComponent* OtherComponent, const bool bIsFromPlayer)
{
	if (UPEInventoryComponent* const InventoryComp = GetCastedInventoryComponent(); IsValid(InventoryComp))
	{
		if (bIsFromPlayer && OtherComponent == nullptr)
		{
			InventoryComp->DiscardItems(TradeInfo);
		}
		else
		{
			bIsFromPlayer ? InventoryComp->GiveItemsTo(OtherComponent, TradeInfo) : InventoryComp->GetItemsFrom(OtherComponent, TradeInfo);
		}
	}
}

void IPEElementusInventoryProcessor::ProcessGameplayEffect(const TSubclassOf<UGameplayEffect>& EffectClass)
{
	if (UPEInventoryComponent* const InventoryComp = GetCastedInventoryComponent(); IsValid(InventoryComp))
	{
		InventoryComp->ApplyItemEffect(EffectClass);
	}
}