// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Game/PECoreGameInstance.h"
#include "LogElementusCore.h"
#include <MoviePlayer.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECoreGameInstance)

UPECoreGameInstance::UPECoreGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPECoreGameInstance::Init()
{
	Super::Init();
	
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPECoreGameInstance::OnMapLoadingStart);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UPECoreGameInstance::OnMapLoadingComplete);
}

void UPECoreGameInstance::OnMapLoadingStart(const FString& MapName)
{
	if (IsRunningDedicatedServer())
	{
		return;
	}
	
	UE_LOG(LogElementusCore, Display, TEXT("%s - Loading map: %s"), *FString(__func__), *MapName);

	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAllowEngineTick = false;
	LoadingScreen.bAllowInEarlyStartup = true;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.bMoviesAreSkippable = false;
	LoadingScreen.bWaitForManualStop = false;
	LoadingScreen.MinimumLoadingScreenDisplayTime = -1.f;
	LoadingScreen.PlaybackType = EMoviePlaybackType::MT_Normal;
	LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UPECoreGameInstance::OnMapLoadingComplete(UWorld* InLoadedWorld)
{
	UE_LOG(LogElementusCore, Display, TEXT("%s - Map Loaded: %s"), *FString(__func__), *InLoadedWorld->GetMapName());
}