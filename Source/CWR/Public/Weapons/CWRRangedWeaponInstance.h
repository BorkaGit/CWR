// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkAudioEvent.h"
#include "Curves/CurveFloat.h"
#include "CWRWeaponInstance.h"
#include "GameFramework/Actor.h"
#include "CWRRangedWeaponInstance.generated.h"

class UCWRWeaponCustomizationItem;
class UGameplayAbility;
class UCWRGameplayAbility;
class UAbilitySystemComponent;
struct FGameplayAbilitySpecHandle;
class ACWRCharacter_Player;
class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UAkComponent;

UCLASS(BlueprintType)
class CWR_API UCWRRangedWeaponInstance : public UCWRWeaponInstance
{
	GENERATED_BODY()
	
public:	
	
	UCWRRangedWeaponInstance();

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	void UpdateDebugVisualization();
#endif

	void Tick(float DeltaSeconds);

	//~UCWREquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void SpawnEquipmentActors(const TArray<FCWREquipmentActorToSpawn>& ActorsToSpawn);
	//~End of UCWREquipmentInstance interface
	
	void AddSpread();
	
	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandGripTransform() const;

	UFUNCTION(BlueprintCallable)
	FVector GetAimOffsetLocation() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRecoilMultiplier() const { return RecoilMultiplier; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRecoilAmount() const { return RecoilAmount; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAttackRange() const { return AttackRange; }

	FORCEINLINE float GetCalculatedSpreadAngle() const { return CurrentSpreadAngle; }

	FORCEINLINE float GetCalculatedSpreadAngleMultiplier() const { return bHasFirstShotAccuracy ? 0.f : CurrentSpreadAngleMultiplier; }
	
	FORCEINLINE float GetSpreadExponent() const { return SpreadExponent; }

	FORCEINLINE float GetBulletTraceSweepRadius() const { return BulletTraceSweepRadius; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDamage() const { return Damage; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetFireRate() const { return FireRate; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetLastFireTime() const { return LastFireTime; }
	


	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAkAudioEvent* GetFireSound() const { return FireSound; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsWithSilencer() const { return bWithSilencer; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetBulletsPerCartridge() const { return BulletsPerCartridge; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetBulletsAmount() const { return CurrentBulletsAmount; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetBulletsAmount(const int32 InBulletsAmount) { CurrentBulletsAmount = InBulletsAmount; }

	UFUNCTION(BlueprintCallable)
	void DecrementBulletsAmount();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMagazineSize() const { return MagazineSize; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetReloadTime() const { return ReloadTime; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsFiring ( const bool bInFiring ) { bIsFiring = bInFiring; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsFiring () const { return bIsFiring; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsReloading ( const bool bInReloading ) { bIsReloading = bInReloading; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsReloading () const { return bIsReloading; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsEmpty () const { return CurrentBulletsAmount == 0; }

	UFUNCTION(BlueprintPure)
	bool CanReload() const;
	
	UFUNCTION(BlueprintCallable)
	void SetRecoilTransform();

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void RefillAmmo();
	
protected: 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Scope_1P = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Scope_3P = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName LeftHandGripSocketName = "LeftHandGripSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float RecoilMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float RecoilAmount = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MinVerticalRecoilOffset = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxVerticalRecoilOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MinHorizontalRecoilOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHorizontalRecoilOffset = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float AttackRange = 50000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Damage = 33.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float FireRate = 0.1f;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 BulletsPerCartridge = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 CurrentBulletsAmount = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 BackpackBulletsAmount = 90;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 MagazineSize = 30;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 MagazinesAmount = 4;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float ReloadTime = 4.f;

	// Spread exponent, affects how tightly shots will cluster around the center line
	// when the weapon has spread (non-perfect accuracy). Higher values will cause shots
	// to be closer to the center (default is 1.0 which means uniformly within the spread range)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0.1), Category="Spread|Fire Params")
	float SpreadExponent = 1.0f;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Config", meta=(ForceUnits=cm))
	float BulletTraceSweepRadius = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<UAkAudioEvent> FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> BulletHoleClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	bool bWithSilencer = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	TArray< TObjectPtr<UCWRWeaponCustomizationItem> > Attachments;

	UPROPERTY(Transient, BlueprintReadOnly)
	FTransform RecoilTransform = FTransform();

	bool bIsFiring = false;
	bool bIsReloading = false;
	
private:
	
	float LastFireTime = 0.f;

	// The current spread angle (in degrees, diametrical)
	float CurrentSpreadAngle = 0.0f;

	// Do we currently have first shot accuracy?
	bool bHasFirstShotAccuracy = false;

	// The current *combined* spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;
};
