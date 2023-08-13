// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEAbilityProjectile.h"
#include "Core/PEAbilitySystemComponent.h"
#include "Core/PEAbilityData.h"
#include <Components/SphereComponent.h>
#include <GameFramework/Character.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <AbilitySystemGlobals.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEAbilityProjectile)

APEAbilityProjectile::APEAbilityProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bReplicates = true;

    bOnlyRelevantToOwner = false;
    bAlwaysRelevant = true;
    AActor::SetReplicateMovement(false);
    NetUpdateFrequency = 75.f;
    NetPriority = 1.f;
    NetDormancy = ENetDormancy::DORM_Initial;

    InitialLifeSpan = 3.f;

    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
    CollisionComponent->InitSphereRadius(12.5f);
    CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));
    ProjectileMovement->SetUpdatedComponent(CollisionComponent);

    ProjectileMovement->InitialSpeed = 2500.f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.25f;
    ProjectileMovement->ProjectileGravityScale = 0.025f;
}

void APEAbilityProjectile::BeginPlay()
{
    Super::BeginPlay();
    CollisionComponent->OnComponentHit.AddDynamic(this, &APEAbilityProjectile::OnProjectileHit);
}

void APEAbilityProjectile::FireInDirection(const FVector& Direction)
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    SetNetDormancy(ENetDormancy::DORM_Awake);
    ProjectileMovement->Velocity = ProjectileMovement->InitialSpeed * Direction;
}

void APEAbilityProjectile::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!IsValid(OtherActor) || !IsValid(OtherComp))
    {
        return;
    }

    const FVector ImpulseVelocity = ProjectileMovement->Velocity * (ImpulseMultiplier / 10.f);

    if (OtherActor->GetClass()->IsChildOf<ACharacter>())
    {
        if (ACharacter* const Character = Cast<ACharacter>(OtherActor))
        {
            Character->LaunchCharacter(ImpulseVelocity, true, true);
        }

        if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
        {
            ApplyProjectileEffect(TargetABSC);
        }
    }
    else if (OtherComp->IsSimulatingPhysics() && OtherComp->Mobility == EComponentMobility::Movable)
    {
        OtherComp->AddImpulseAtLocation(ImpulseVelocity, Hit.ImpactPoint, Hit.BoneName);
    }

    Destroy();
}

void APEAbilityProjectile::ApplyProjectileEffect(UAbilitySystemComponent* TargetABSC)
{
    if (GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    if (UPEAbilitySystemComponent* const TargetGASC = Cast<UPEAbilitySystemComponent>(TargetABSC))
    {
        for (const FGameplayEffectGroupedData& Effect : ProjectileEffects)
        {
            TargetGASC->ApplyEffectGroupedDataToSelf(Effect);
        }
    }
}