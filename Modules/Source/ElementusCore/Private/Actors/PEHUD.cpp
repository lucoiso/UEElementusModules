// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Actors/PEHUD.h"
#include "Actors/PECharacter.h"
#include "Management/PECoreSettings.h"
#include "LogElementusCore.h"
#include <Blueprint/UserWidget.h>
#include <Attributes/PEBasicStatusAS.h>
#include <Attributes/PECustomStatusAS.h>
#include <Attributes/PELevelingAS.h>
#include <ViewModels/Attributes/PEVM_AttributeBasic.h>
#include <ViewModels/Attributes/PEVM_AttributeCustom.h>
#include <ViewModels/Attributes/PEVM_AttributeLeveling.h>
#include <AbilitySystemGlobals.h>
#include <GameplayEffectTypes.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEHUD)

APEHUD::APEHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BasicAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeBasic>(TEXT("BasicAttributes_ViewModel"));
	CustomAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeCustom>(TEXT("CustomAttributes_ViewModel"));
	LevelingAttributes_VM = CreateDefaultSubobject<UPEVM_AttributeLeveling>(TEXT("LevelingAttributes_ViewModel"));
}

void APEHUD::BeginPlay()
{
	Super::BeginPlay();

	// Initialize a Blueprint Widget as HUD if Player Controller is valid
	if (ensureAlwaysMsgf(IsValid(GetOwningPlayerController()), TEXT("%s have a invalid Controller"), *GetName()))
	{
		const UPECoreSettings* const ProjectSettings= UPECoreSettings::Get();
		const TSubclassOf<UUserWidget> UMGHUDClass = ProjectSettings->HUDClass.IsNull() ? nullptr : ProjectSettings->HUDClass.LoadSynchronous();

		if (!IsValid(UMGHUDClass))
		{
			UE_LOG(LogElementusCore_Internal, Error, TEXT("%s - Missing setting: HUD Class"), *FString(__func__));
			return;
		}

		if (HUDHandle = CreateWidget(GetOwningPlayerController(), UMGHUDClass, TEXT("Main HUD")); HUDHandle.IsValid())
		{
			HUDHandle->AddToPlayerScreen();

			if (APECharacter* const Character = Cast<APECharacter>(GetOwningPawn()))
			{
				Character->OnCharacterInit.AddDynamic(this, &APEHUD::InitializeAttributeViewModels);
			}
		}
		else
		{
			UE_LOG(LogElementusCore_Internal, Error, TEXT("%s - Failed to initialize UMG HUD"), *FString(__func__));
		}
	}
}

#define REGISTER_ATTRIBUTE_DELEGATE(AttributeClass, AttributeName, ViewModelClass, ViewModelObject) \
if (TargetABSC->GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).IsBoundToObject(this)) \
{ \
	TargetABSC->GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).RemoveAll(this); \
} \
TargetABSC->GetGameplayAttributeValueChangeDelegate(##AttributeClass##::Get##AttributeName##Attribute()).AddUObject(this, &APEHUD::OnViewModelAttributeChange); \
/* This block is used to initialize the first value of the attribute because the viewmodel is only updating when the attribute changes after the binding occurs */ \
{ \
	NotifyAttributeChange(##AttributeClass##::Get##AttributeName##Attribute(), TargetABSC->GetNumericAttribute(##AttributeClass##::Get##AttributeName##Attribute())); \
}

void APEHUD::InitializeAttributeViewModels()
{
	UE_LOG(LogElementusCore_Internal, Display, TEXT("%s - Initializing attribute view models"), *FString(__func__));

	if (UAbilitySystemComponent* const TargetABSC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerController()))
	{
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Health, UPEVM_AttributeBasic, BasicAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxHealth, UPEVM_AttributeBasic, BasicAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Stamina, UPEVM_AttributeBasic, BasicAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxStamina, UPEVM_AttributeBasic, BasicAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, Mana, UPEVM_AttributeBasic, BasicAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPEBasicStatusAS, MaxMana, UPEVM_AttributeBasic, BasicAttributes_VM);

		REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, AttackRate, UPEVM_AttributeCustom, CustomAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, DefenseRate, UPEVM_AttributeCustom, CustomAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, SpeedRate, UPEVM_AttributeCustom, CustomAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, JumpRate, UPEVM_AttributeCustom, CustomAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPECustomStatusAS, Gold, UPEVM_AttributeCustom, CustomAttributes_VM);

		REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, CurrentLevel, UPEVM_AttributeLeveling, LevelingAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, CurrentExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
		REGISTER_ATTRIBUTE_DELEGATE(UPELevelingAS, RequiredExperience, UPEVM_AttributeLeveling, LevelingAttributes_VM);
	}
}

#undef REGISTER_ATTRIBUTE_DELEGATE

void APEHUD::OnViewModelAttributeChange(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.OldValue == AttributeChangeData.NewValue)
	{
		return;
	}

	NotifyAttributeChange(AttributeChangeData.Attribute, AttributeChangeData.NewValue);
}

void APEHUD::NotifyAttributeChange(const FGameplayAttribute& Attribute, const float& NewValue)
{
	if (Attribute.GetAttributeSetClass()->IsChildOf<UPEBasicStatusAS>())
	{
		BasicAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
	else if (Attribute.GetAttributeSetClass()->IsChildOf<UPECustomStatusAS>())
	{
		CustomAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
	else if (Attribute.GetAttributeSetClass()->IsChildOf<UPELevelingAS>())
	{
		LevelingAttributes_VM->NotifyAttributeChange(Attribute, NewValue);
	}
}
