// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include "Attributes/PEAttributeBase.h"
#include "PECustomStatusAS.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSABILITYSYSTEM_API UPECustomStatusAS final : public UPEAttributeBase
{
	GENERATED_BODY()

public:
	explicit UPECustomStatusAS(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	class UCharacterMovementComponent* GetOwningCharacterMovementComponent() const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float DefaultCharJumpRate;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float DefaultCharWalkSpeedRate;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Project Elementus | Properties")
	float DefaultCharCrouchSpeedRate;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_AttackRate)
	FGameplayAttributeData AttackRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, AttackRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_DefenseRate)
	FGameplayAttributeData DefenseRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, DefenseRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_SpeedRate)
	FGameplayAttributeData SpeedRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, SpeedRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_JumpRate)
	FGameplayAttributeData JumpRate;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, JumpRate)

	UPROPERTY(BlueprintReadOnly, Category = "Project Elementus | Properties", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UPECustomStatusAS, Gold)

protected:
	UFUNCTION()
	void OnRep_AttackRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_DefenseRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_SpeedRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_JumpRate(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue) const;
};
