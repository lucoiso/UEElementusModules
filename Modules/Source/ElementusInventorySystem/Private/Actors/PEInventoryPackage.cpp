// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEInventoryPackage.h"
#include "Management/PEInventorySystemSettings.h"
#include <GameFramework/Character.h>
#include <Blueprint/UserWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEInventoryPackage)

APEInventoryPackage::APEInventoryPackage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    AActor::SetActorTickInterval(0.1f);
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    bReplicates = true;
    bOnlyRelevantToOwner = false;
    bAlwaysRelevant = false;
    AActor::SetReplicateMovement(false);
    NetUpdateFrequency = 30.f;
    NetPriority = 1.f;
    NetDormancy = ENetDormancy::DORM_Initial;

    PackageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackageMesh"));
    PackageMesh->SetupAttachment(RootComponent);
    PackageMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    PackageMesh->SetRelativeScale3D(FVector(0.5f));
    PackageMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    PackageMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    if (const UPEInventorySystemSettings* const ProjectSettings = UPEInventorySystemSettings::Get(); !ProjectSettings->InventoryPackageMesh.IsNull())
    {
        PackageMesh->SetStaticMesh(ProjectSettings->InventoryPackageMesh.LoadSynchronous());
    }
}

void APEInventoryPackage::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    PackageMesh->AddRelativeRotation(FRotator(0.f, DeltaSeconds * 15.f, 0.f));
}

void APEInventoryPackage::SetIsCurrentlyFocusedByActor_Implementation(const bool bIsFocused, AActor* ActorFocusing, const FHitResult& HitResult)
{
    if (bIsFocused)
    {
        FocusIDs.Add(ActorFocusing->GetUniqueID());
    }
    else
    {
        FocusIDs.Remove(ActorFocusing->GetUniqueID());

        if (FocusIDs.IsEmpty())
        {
            SetNetDormancy(ENetDormancy::DORM_DormantAll);
        }
    }
}

bool APEInventoryPackage::IsInteractEnabled_Implementation() const
{
    return true;
}

void APEInventoryPackage::DoInteractionBehavior_Implementation(ACharacter* CharacterInteracting, const FHitResult& HitResult)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        SetNetDormancy(ENetDormancy::DORM_Awake);
    }

    if (!CharacterInteracting->IsLocallyControlled())
    {
        return;
    }

    if (APlayerController* const TargetController = CharacterInteracting->GetController<APlayerController>())
    {
        const UPEInventorySystemSettings* const ProjectSettings = UPEInventorySystemSettings::Get();
        const TSubclassOf<UUserWidget> TradeUIClass = ProjectSettings->TradeInventoryWidget.IsNull() ? nullptr : ProjectSettings->TradeInventoryWidget.LoadSynchronous();

        if (UUserWidget* const TradeWidget = CreateWidget(TargetController, TradeUIClass))
        {
            if (const FObjectProperty* const PackageRef = FindFProperty<FObjectProperty>(TradeUIClass, TEXT("PackageRef")))
            {
                PackageRef->SetPropertyValue_InContainer(TradeWidget, this);
            }

            if (const FObjectProperty* const PlayerRef = FindFProperty<FObjectProperty>(TradeUIClass, TEXT("PlayerRef")))
            {
                PlayerRef->SetPropertyValue_InContainer(TradeWidget, CharacterInteracting);
            }

            TradeWidget->AddToPlayerScreen();
        }
    }
}