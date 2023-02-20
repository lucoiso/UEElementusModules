// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Tasks/PEMoveCamera_Task.h"
#include <GameFramework/Character.h>
#include <Components/TimelineComponent.h>
#include <Camera/CameraComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEMoveCamera_Task)

UPEMoveCamera_Task::UPEMoveCamera_Task(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bTickingTask = false;
}

UPEMoveCamera_Task* UPEMoveCamera_Task::MoveCamera(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const FVector CameraRelativeTargetPosition, const float CameraLerpTime)
{
	UPEMoveCamera_Task* const MyObj = NewAbilityTask<UPEMoveCamera_Task>(OwningAbility, TaskInstanceName);	
	MyObj->CameraTargetPosition = CameraRelativeTargetPosition;
	MyObj->CameraLerpTime = CameraLerpTime;
	
	return MyObj;
}

void UPEMoveCamera_Task::Activate()
{
	Super::Activate();
	check(Ability);

	TaskTimeline = NewObject<UTimelineComponent>(GetAvatarActor(), UTimelineComponent::StaticClass(), TEXT("MoveCameraTimeline"));
	if (!TaskTimeline.IsValid())
	{
		UE_LOG(LogElementusAbilitySystem_Internal, Error, TEXT("%s - Task %s ended on activation due to invalid timeline"), *FString(__func__), *GetName());
		EndTask();
		return;
	}	

	if (ACharacter* const OwningCharacter = Cast<ACharacter>(Ability->GetAvatarActorFromActorInfo());
		ensureAlwaysMsgf(IsValid(OwningCharacter), TEXT("%s - Task %s failed to activate because have a invalid owner"), *FString(__func__), *GetName()))
	{
		UActorComponent* const CameraComp = OwningCharacter->GetComponentByClass(UCameraComponent::StaticClass());
		if (!IsValid(CameraComp))
		{
			UE_LOG(LogElementusAbilitySystem_Internal, Error, TEXT("%s - Task %s ended on activation due to invalid camera target"), *FString(__func__), *GetName());			
			EndTask();
			return;
		}

		TargetCamera = Cast<UCameraComponent>(CameraComp);
		CameraInitialPosition = TargetCamera->GetRelativeLocation();
		TaskTimeline->SetTimelineLength(CameraLerpTime);

		UCurveFloat* const TimelineCurve = NewObject<UCurveFloat>();
		TimelineCurve->FloatCurve.AddKey(0.f, 0.f);
		TimelineCurve->FloatCurve.AddKey(CameraLerpTime, 1.f);

		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, TEXT("TimelineProgress"));
		TaskTimeline->AddInterpFloat(TimelineCurve, TimelineUpdate);
		
		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, TEXT("TimelineFinished"));
		TaskTimeline->SetTimelineFinishedFunc(TimelineFinished);
		
		TaskTimeline->RegisterComponentWithWorld(GetWorld());
		TaskTimeline->Play();
	}
}

void UPEMoveCamera_Task::RevertCameraPosition()
{
	if (!TaskTimeline.IsValid())
	{
		UE_LOG(LogElementusAbilitySystem_Internal, Error, TEXT("%s - Task %s failed while trying to revert camera position due to invalid timeline"), *FString(__func__), *GetName());
		
		OnFailed.Broadcast();
		EndTask();

		return;
	}
	
	TaskTimeline->Reverse();
}

void UPEMoveCamera_Task::OnDestroy(const bool AbilityIsEnding)
{
	UE_LOG(LogElementusAbilitySystem_Internal, Display, TEXT("%s - Task %s ended"), *FString(__func__), *GetName());

	if (TaskTimeline.IsValid() && TaskTimeline->IsPlaying())
	{
		UE_LOG(LogElementusAbilitySystem_Internal, Warning, TEXT("%s - Task %s ended while the timeline is playing!"), *FString(__func__), *GetName());
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UPEMoveCamera_Task::TimelineProgress(const float InValue)
{
	CurrentValue = InValue;
	
	if (!TargetCamera.IsValid())
	{
		UE_LOG(LogElementusAbilitySystem_Internal, Error, TEXT("%s - Task %s failed while trying to change camera position due to invalid target camera"), *FString(__func__), *GetName());
		
		OnFailed.Broadcast();
		
		TaskTimeline->Stop();
		EndTask();
		
		return;
	}

	TargetCamera->SetRelativeLocation(FMath::Lerp(CameraInitialPosition, CameraTargetPosition, InValue));
}

void UPEMoveCamera_Task::TimelineFinished()
{
	if (CurrentValue == 0.f)
	{
		OnReversionCompleted.Broadcast();
	}
	else if (CurrentValue == CameraLerpTime)
	{
		OnMoveCompleted.Broadcast();		
	}	
}
