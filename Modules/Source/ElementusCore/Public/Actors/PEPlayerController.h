// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <InputTriggers.h>
#include <VisualLogger/VisualLogger.h>
#include <Interfaces/MFEA_AbilityInputBinding.h>
#include <Interfaces/PEElementusInventoryProcessor.h>
#include <Management/ElementusInventoryData.h>
#include "LogElementusCore.h"
#include "PEPlayerController.generated.h"

/**
 *
 */

#define CONTROLLER_BASE_VLOG(Actor, Verbosity, Format, ...) \
{ \
	UE_LOG(LogController_Base, Verbosity, Format, ##__VA_ARGS__); \
	UE_VLOG(Actor, LogController_Base, Verbosity, Format, ##__VA_ARGS__); \
}

#define CONTROLLER_AXIS_VLOG(Actor, Verbosity, Format, ...) \
{ \
	UE_LOG(LogController_Axis, Verbosity, Format, ##__VA_ARGS__); \
	UE_VLOG(Actor, LogController_Axis, Verbosity, Format, ##__VA_ARGS__); \
}

class UElementusInventoryComponent;
class UGameplayEffect;
struct FPrimaryElementusItemId;

USTRUCT()
struct FPendingAbilityInputData
{
	GENERATED_BODY()

	FPendingAbilityInputData() = default;
	FPendingAbilityInputData(UInputAction* InAction, const int32 InInputID) : Action(InAction), InputID(InInputID) {}

	UPROPERTY()
	TObjectPtr<UInputAction> Action;

	UPROPERTY()
	int32 InputID = 0;
};

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSCORE_API APEPlayerController final : public APlayerController, public IMFEA_AbilityInputBinding, public IPEElementusInventoryProcessor
{
	GENERATED_BODY()

public:
	explicit APEPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region IMFEA_AbilityInputBinding
	/* This function came from IMFEA_AbilityInputBinding interface,
	 * provided by GameFeatures_ExtraActions plugin to manage ability bindings */
	virtual void SetupAbilityBindingByInput_Implementation(UInputAction* Action, const int32 InputID) override;

	UFUNCTION(Client, Reliable)
	virtual void ProcessAbilityInputAddition(UInputAction* Action, const int32 InputID);

	/* This function came from IMFEA_AbilityInputBinding interface,
	 * provided by GameFeatures_ExtraActions plugin to manage ability bindings */
	virtual void RemoveAbilityInputBinding_Implementation(const UInputAction* Action) override;
		
	UFUNCTION(Client, Reliable)
	virtual void ProcessAbilityInputRemoval(const UInputAction* Action);
#pragma endregion IMFEA_AbilityInputBinding

	/* Setup the spectating state on both client and server */
	UFUNCTION(NetMulticast, Unreliable)
	void SetupControllerSpectator();

	/* Will respawn the character if the player is in spectating state */
	UFUNCTION(BlueprintCallable, Category = "Project Elementus | Functions")
	void InitializeRespawn(const float InSeconds);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Project Elementus | Functions")
	void PerformAbilityInputQueueAdditions();
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Project Elementus | Functions")
	void PerformAbilityInputQueueRemovals();

protected:
	virtual void SetupInputComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Perform the respawn task on server */
	UFUNCTION(Server, Reliable)
	void RespawnAndPossess();

private:
	struct FAbilityInputData
	{
		uint32 OnPressedHandle = 0;
		uint32 OnReleasedHandle = 0;
		uint32 InputID = 0;
	};

	TWeakObjectPtr<UEnum> InputEnumHandle;
	TMap<TObjectPtr<UInputAction>, FAbilityInputData> AbilityActionBindings;

	UPROPERTY(Replicated)
	TArray<FPendingAbilityInputData> AbilityInputAddQueue;
	
	UPROPERTY(Replicated)
	TArray<TObjectPtr<const UInputAction>> AbilityInputRemoveQueue;

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void OnAbilityInputPressed(UInputAction* SourceAction);

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void OnAbilityInputReleased(UInputAction* SourceAction);

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void ChangeCameraAxis(const FInputActionValue& Value);

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void Move(const FInputActionValue& Value) const;

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void Jump(const FInputActionValue& Value) const;

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void SetVoiceChatEnabled(const FInputActionValue& Value) const;

	UFUNCTION(Category = "Project Elementus | Input Binding")
	void OpenInventory(const FInputActionValue& Value);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Project Elementus | Functions")
	void Client_OpenInventory();
};
