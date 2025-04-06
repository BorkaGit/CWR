// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CWRGameplayEffectContext.h"

#include "AbilitySystem/CWRAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

class FArchive;

FCWRGameplayEffectContext* FCWRGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FCWRGameplayEffectContext::StaticStruct()))
	{
		return (FCWRGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FCWRGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FCWRGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(CWRGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FCWRGameplayEffectContext::SetAbilitySource(const ICWRAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const ICWRAbilitySourceInterface* FCWRGameplayEffectContext::GetAbilitySource() const
{
	return Cast<ICWRAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FCWRGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

