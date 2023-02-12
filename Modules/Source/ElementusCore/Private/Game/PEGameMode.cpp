// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Game/PEGameMode.h"
#include "Management/PECoreSettings.h"
#include "Actors/PEHUD.h"
#include "Actors/PEPlayerState.h"
#include "Actors/PEPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEGameMode)

APEGameMode::APEGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bCanRespawn(true)
{
	if (const UPECoreSettings* const ProjectSettings = UPECoreSettings::Get(); !ProjectSettings->CharacterPawn.IsNull())
	{
		DefaultPawnClass = ProjectSettings->CharacterPawn.LoadSynchronous();
	}

	PlayerStateClass = APEPlayerState::StaticClass();
	PlayerControllerClass = APEPlayerController::StaticClass();
	HUDClass = APEHUD::StaticClass();
}
