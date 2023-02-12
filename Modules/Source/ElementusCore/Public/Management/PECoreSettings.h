// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include "PECoreSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Plugins, DefaultConfig, Meta = (DisplayName = "Project Elementus: Core"))
class ELEMENTUSCORE_API UPECoreSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UPECoreSettings(const FObjectInitializer& ObjectInitializer);
	static const UPECoreSettings* Get();
	
	/* A Blueprint Widget class to use as HUD */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "UI", Meta = (DisplayName = "HUD UMG Class"))
	TSoftClassPtr<UUserWidget> HUDClass;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Core")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Core")
	TSoftClassPtr<UAnimInstance> CharacterAnimBP;

	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Core")
	TSoftClassPtr<APawn> CharacterPawn;

	/* Color of player character's mesh */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Appearance")
	FLinearColor PlayerColor;

	/* Color of bot character's mesh */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Appearance")
	FLinearColor BotColor;

	/* Default Gravity Scale that will be added to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float DefaultGravityScale;

	/* Default Walk Speed that will be applied to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float DefaultWalkSpeed;

	/* Default Crouch Speed that will be applied to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DefaultCrouchSpeedPercentage;

	/* Default Jump Z Velocity that will be applied to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float DefaultJumpZVelocity;

	/* Default Air Control that will be applied to character's movement component */
	UPROPERTY(GlobalConfig, EditAnywhere, Category = "Character | Physics")
	float DefaultAirControl;
};
