// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PECharacter.h"
#include "Actors/PEAIController.h"
#include "Actors/PEPlayerState.h"
#include "Components/PEMovementComponent.h"
#include "Management/PECoreSettings.h"
#include <Actors/PEInventoryPackage.h>
#include <Core/PEAbilitySystemComponent.h>
#include <Components/PEInventoryComponent.h>
#include <PEAbilityTags.h>
#include <Management/ElementusInventoryFunctions.h>
#include <Components/CapsuleComponent.h>
#include <Components/GameFrameworkComponentManager.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <AbilitySystemLog.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PECharacter)

const FName APECharacter::PEInventoryComponentName(TEXT("InventoryComponent"));
const FVector APECharacter::PECameraDefaultPosition(FVector(50.f, 50.f, 50.f));

APECharacter::APECharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UPEMovementComponent>(CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AIControllerClass = APEAIController::StaticClass();

    bOnlyRelevantToOwner = false;
    bAlwaysRelevant = false;
    AActor::SetReplicateMovement(true);
    NetUpdateFrequency = 100.f;
    NetPriority = 3.f;
    NetDormancy = ENetDormancy::DORM_Awake;

    GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    GetMesh()->SetMobility(EComponentMobility::Movable);

    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    if (const UPECoreSettings* const ProjectSettings = UPECoreSettings::Get())
    {
        if (!ProjectSettings->CharacterMesh.IsNull())
        {
            GetMesh()->SetSkeletalMesh(ProjectSettings->CharacterMesh.LoadSynchronous());
        }

        if (!ProjectSettings->CharacterAnimBP.IsNull())
        {
            GetMesh()->SetAnimInstanceClass(ProjectSettings->CharacterAnimBP.LoadSynchronous());
        }

        GetCharacterMovement()->MaxWalkSpeed = ProjectSettings->DefaultWalkSpeed;
        GetCharacterMovement()->MaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeed * ProjectSettings->DefaultCrouchSpeedPercentage;
        GetCharacterMovement()->JumpZVelocity = ProjectSettings->DefaultJumpZVelocity;
        GetCharacterMovement()->AirControl = ProjectSettings->DefaultAirControl;
        GetCharacterMovement()->GravityScale = ProjectSettings->DefaultGravityScale;
    }

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 500.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = true;
    FollowCamera->SetRelativeLocation(PECameraDefaultPosition);

    InventoryComponent = CreateDefaultSubobject<UPEInventoryComponent>(APECharacter::PEInventoryComponentName);
    InventoryComponent->SetIsReplicated(true);
}

// Called on server when the character is possessed by a controller
void APECharacter::PossessedBy(AController* InController)
{
    Super::PossessedBy(InController);

    // Check if this character is controlled by a player or AI
    if (InController->IsPlayerController())
    {
        // Initialize the ability system component that is stored by Player State
        if (APEPlayerState* const State = GetPlayerState<APEPlayerState>())
        {
            InitializeAbilitySystemComponent(State->GetAbilitySystemComponent(), State);
        }
    }
    else
    {
        // Bot / AI
        // Not implemented yet

        // Initialize the ability system component that is stored by AI Controller
    }
}

// Called on client when the player state is replicated
void APECharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (APEPlayerState* const State = GetPlayerState<APEPlayerState>())
    {
        // Initialize the ability system component that is stored by Player State
        InitializeAbilitySystemComponent(State->GetAbilitySystemComponent(), State);
    }
}

// Called when controller is replicated, will use it to refresh the actor info on ABSC and reset the Death Tag state
void APECharacter::OnRep_Controller()
{
    Super::OnRep_Controller();

    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->RefreshAbilityActorInfo();
        AbilitySystemComponent->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(GlobalTag_DeadState)));
    }
}

void APECharacter::ApplyExtraSettings()
{
    const UPECoreSettings* const ProjectSettings = UPECoreSettings::Get();
    if (!IsValid(ProjectSettings))
    {
        return;
    }

    // Check if this character have a valid Skeletal Mesh and paint it
    if (IsValid(GetMesh()))
    {
        const auto DynamicColor_Lambda = [this](const uint8& Index, const FLinearColor& Color) -> void
            {
                if (UMaterialInstanceDynamic* const DynMat = GetMesh()->CreateDynamicMaterialInstance(Index))
                {
                    DynMat->SetVectorParameterValue(TEXT("Tint"), Color);
                }
            };

        const FLinearColor DestColor = IsBotControlled() ? ProjectSettings->BotColor : ProjectSettings->PlayerColor;

        DynamicColor_Lambda(0, DestColor);
        DynamicColor_Lambda(1, DestColor);
    }
}

UPEInventoryComponent* APECharacter::GetInventoryComponent() const
{
    return InventoryComponent.Get();
}

UAbilitySystemComponent* APECharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent.Get();
}

void APECharacter::InitializeAbilitySystemComponent(UAbilitySystemComponent* InABSC, AActor* InOwnerActor)
{
    AbilitySystemComponent = CastChecked<UPEAbilitySystemComponent>(InABSC);
    AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);
}

void APECharacter::PreInitializeComponents()
{
    UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

    Super::PreInitializeComponents();
}

void APECharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->AbilityActivatedCallbacks.AddUFunction(this, TEXT("AbilityActivated"));
        AbilitySystemComponent->AbilityCommittedCallbacks.AddUFunction(this, TEXT("AbilityCommited"));
        AbilitySystemComponent->AbilityFailedCallbacks.AddUFunction(this, TEXT("AbilityFailed"));
        AbilitySystemComponent->OnAbilityEnded.AddUFunction(this, TEXT("AbilityEnded"));
    }

    ApplyExtraSettings();

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda(
        [this]
        {
            if (IsValid(this))
            {
                Server_InitializeCharacter();
                OnCharacterInit.Broadcast();
            }
        }
    );

    constexpr float InitializationDelay = 1.f;

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, InitializationDelay, false);
}

void APECharacter::Server_InitializeCharacter_Implementation()
{
    Multicast_InitializeCharacter();
}

void APECharacter::Multicast_InitializeCharacter_Implementation()
{
    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
}

void APECharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = true;

    DOREPLIFETIME_WITH_PARAMS_FAST(APECharacter, InventoryComponent, SharedParams);
}

void APECharacter::PerformDeath()
{
    OnCharacterDeath.Broadcast();

    AbilitySystemComponent->ResetAbilitySystemComponent();

    Client_DeathSetup();
    Multicast_DeathSetup();

    Server_SpawnInventoryPackage();

    bAlwaysRelevant = false;

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda(
        [this]
        {
            if (IsValid(this))
            {
                Server_DestroyCharacter();
            }
        }
    );

    constexpr float DeathDestroyDelay = 15.f;

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DeathDestroyDelay, false);
}

void APECharacter::Client_DeathSetup_Implementation()
{
    // Unnequip all items on death
    InventoryComponent->UnnequipAll(AbilitySystemComponent.Get());
}

void APECharacter::Server_DestroyCharacter_Implementation()
{
    // Destroy the character only on server (Will replicate to clients)
    Destroy();
}

void APECharacter::Multicast_DeathSetup_Implementation()
{
    // Will perform each step above on both server and client
    UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

    if (IsValid(GetCharacterMovement()))
    {
        GetCharacterMovement()->DisableMovement();
    }

    if (IsValid(GetCapsuleComponent()))
    {
        GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
    }

    if (IsValid(GetMesh()))
    {
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
        GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_None, true, true);
    }
}

void APECharacter::Server_SpawnInventoryPackage_Implementation()
{
    // Spawn an inventory package with all character's items
    AElementusInventoryPackage* const SpawnedPackage = GetWorld()->SpawnActorDeferred<APEInventoryPackage>(APEInventoryPackage::StaticClass(), GetTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    UElementusInventoryFunctions::TradeElementusItem(InventoryComponent->GetItemsArray(), InventoryComponent, SpawnedPackage->PackageInventory);

    SpawnedPackage->SetDestroyOnEmpty(true);
    SpawnedPackage->FinishSpawning(GetTransform());
}

void APECharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    // Check if this player have a valid ABSC and cancel the Double Jump ability (if active)
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    const FGameplayTagContainer DoubleJumpTagContainer{ FGameplayTag::RequestGameplayTag("GameplayAbility.Default.DoubleJump") };

    AbilitySystemComponent->CancelAbilities(&DoubleJumpTagContainer);
}

void APECharacter::AbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& TagContainer)
{
#if WITH_EDITOR
    if (bDebugAbilities)
    {
        ABILITY_VLOG(Ability, Warning, TEXT("Ability %s failed to activate. Owner: %s"), *Ability->GetName(), *GetName());

        if (!TagContainer.IsEmpty())
        {
            ABILITY_VLOG(Ability, Warning, TEXT("Reasons:"));
            for (const FGameplayTag& TagIterator : TagContainer)
            {
                if (TagIterator.IsValid())
                {
                    ABILITY_VLOG(Ability, Warning, TEXT("Tag: %s"), *TagIterator.ToString());
                }
            }
        }

        ABILITY_VLOG(Ability, Warning, TEXT("================ START OF ABILITY SYSTEM COMPONENT DEBUG INFO ================"));

        AbilitySystemComponent->PrintDebug();

        ABILITY_VLOG(Ability, Warning, TEXT("================ END OF ABILITY SYSTEM COMPONENT DEBUG INFO ================"));
    }
#endif
}