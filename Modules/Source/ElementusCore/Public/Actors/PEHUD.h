// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "PEHUD.generated.h"

/**
 *
 */
UCLASS(Blueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSCORE_API APEHUD : public AHUD
{
    GENERATED_BODY()

public:
    explicit APEHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;

protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
    TObjectPtr<class UPEVM_AttributeBase> BasicAttributes_VM;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
    TObjectPtr<class UPEVM_AttributeBase> CustomAttributes_VM;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Project Elementus | Properties")
    TObjectPtr<class UPEVM_AttributeBase> LevelingAttributes_VM;

    /* A Blueprint Widget class to use as HUD */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Project Elementus | Properties")
    TSubclassOf<UUserWidget> HUDClass;

    UFUNCTION()
    virtual void InitializeAttributeViewModels();

    virtual void OnViewModelAttributeChange(const struct FOnAttributeChangeData& AttributeChangeData);
    void NotifyAttributeChange(const struct FGameplayAttribute& Attribute, const float& NewValue);

private:
    TWeakObjectPtr<UUserWidget> HUDHandle;
};
