// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CWRRangedWeaponInstance.h"
#include "Character/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "CWRWeaponActor.generated.h" 

class ACWRCharacter_Base;
struct FCWRActualAimInfo;
class UArrowComponent;
class UCWRInventoryItemDefinition;
class UCWRWorldUserWidget;
class UCWRWeaponCustomizationItem;
class UNiagaraSystem;
class UAkAudioEvent;
class UAkComponent;
class UNiagaraComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class ECWRSightType : uint8
{
	Optic,
	Ironsight
};

USTRUCT(BlueprintType)
struct FCWRSightData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECWRSightType PartType = ECWRSightType::Ironsight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MainSocket = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FrontSocket = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent* MainComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent* FrontComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform HandTransform = FTransform::Identity;
};

UCLASS(Blueprintable)
class CWR_API ACWRWeaponActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ACWRWeaponActor();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAkComponent* GetAkComponent() const { return AkComponent; }
	
//	UFUNCTION(BlueprintCallable)
//	FORCEINLINE UNiagaraComponent* GetNiagaraComponent() const { return NiagaraComponent; }

	FORCEINLINE void SetAttachments(const TArray<FCWRAttachmentInfo>& InAttachments) { Attachments = InAttachments; }
	
	UFUNCTION(BlueprintCallable)
	void TriggerFireAudio() const;

	UFUNCTION(BlueprintPure)
	ACWRCharacter_Base* GetOwningCharacter() const;

	UFUNCTION(BlueprintCallable)
	void TriggerMuzzleFireEffect();

	UFUNCTION()
	void DisableMuzzleFireEffect() ;

	UFUNCTION(BlueprintCallable)
	void SpawnAttachment(UCWRWeaponCustomizationItem* CustomizationItem);
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> RootSphereComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MuzzleSocketName = "MuzzleSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  BarrelSocketName = "BarrelSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  ScopeSocketName = "ScopeSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MagazineSocketName = "MagazineSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAkAudioEvent> FireSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> MuzzleFireNiagara;

	/** Interaction Interface */
	void Interact_Implementation(APawn* InstigatorPawn);
	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) const override;
	/** end Interaction Interface */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCWRInventoryItemDefinition> WeaponDefinition; 

	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<FCWRAttachmentInfo> Attachments;

	UPROPERTY(Transient, BlueprintReadWrite)
	TArray<FCWRSightData> Sights;
	
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TPMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FPMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> TPShellEject = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> FPShellEject = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAkComponent> AkComponent = nullptr;
};
