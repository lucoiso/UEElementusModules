// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#pragma once

#include <CoreMinimal.h>
#include <AbilitySystemComponent.h>
#include <AttributeSet.h>
#include "PEAttributeBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
			GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
			GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS(Abstract, NotBlueprintable, NotPlaceable, Category = "Project Elementus | Classes")
class ELEMENTUSABILITYSYSTEM_API UPEAttributeBase : public UAttributeSet
{
    GENERATED_BODY()

public:
    explicit UPEAttributeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void InitFromMetaDataTable(const UDataTable* DataTable);

    template<typename ComponentTy>
    ComponentTy* GetCastedAbilitySystemComponent()
    {
        return Cast<ComponentTy>(GetOwningAbilitySystemComponent());
    }
};
