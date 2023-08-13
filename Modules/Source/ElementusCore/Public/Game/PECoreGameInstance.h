// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Game/PEEOSGameInstance.h>
#include "PECoreGameInstance.generated.h"

/**
 *
 */
UCLASS(Category = "Project Elementus | Classes")
class ELEMENTUSCORE_API UPECoreGameInstance : public UPEEOSGameInstance
{
    GENERATED_BODY()

public:
    explicit UPECoreGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void Init() override;

    virtual void OnMapLoadingStart(const FString& MapName);
    virtual void OnMapLoadingComplete(UWorld* InLoadedWorld);
};
