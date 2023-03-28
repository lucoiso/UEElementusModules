// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Actors/ElementusInventoryPackage.h>
#include <Interfaces/PEInteractable.h>
#include "PEInventoryPackage.generated.h"

/**
 * 
 */
UCLASS(Category = "Project Elementus | Classes")
class ELEMENTUSINVENTORYSYSTEM_API APEInventoryPackage : public AElementusInventoryPackage, public IPEInteractable
{
	GENERATED_BODY()

public:
	explicit APEInventoryPackage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "Project Elementus | Properties")
	TObjectPtr<UStaticMeshComponent> PackageMesh;

protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetIsCurrentlyFocusedByActor_Implementation(const bool bIsFocused, AActor* ActorFocusing, const FHitResult& HitResult) override;

	virtual bool IsInteractEnabled_Implementation() const override;
	virtual void DoInteractionBehavior_Implementation(class ACharacter* CharacterInteracting, const FHitResult& HitResult) override;

	TArray<uint32> FocusIDs;
};
