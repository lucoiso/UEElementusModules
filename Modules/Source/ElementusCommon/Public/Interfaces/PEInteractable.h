// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <UObject/Interface.h>
#include "PEInteractable.generated.h"

/**
 *
 */
UINTERFACE(MinimalAPI, Blueprintable, Category = "Project Elementus | Interfaces")
class UPEInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ELEMENTUSCOMMON_API IPEInteractable
{
	GENERATED_BODY()

public:
	/* This function will perform the interaction behavior */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Project Elementus | Functions")
	void DoInteractionBehavior(class ACharacter* CharacterInteracting, const FHitResult& HitResult);

	/* Set if the actor is focusing or not the interactable object */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Project Elementus | Functions")
	void SetIsCurrentlyFocusedByActor(const bool bIsFocused, AActor* ActorFocusing, const FHitResult& HitResult);

	/* Is Interact enabled in the current state? */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Project Elementus | Functions")
	bool IsInteractEnabled() const;
};
