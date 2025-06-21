// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CWRRangedWeaponInstance.h"
#include "Character/InteractionInterface.h"
#include "GameFramework/Actor.h"
#include "Inventory/CWRInventoryItemInstance.h"
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

	FORCEINLINE bool operator==(const FCWRSightData& Other) const
	{
		return PartType == Other.PartType
			&& MainSocket == Other.MainSocket
			&& FrontSocket == Other.FrontSocket
			&& MainComponent == Other.MainComponent
			&& FrontComponent == Other.FrontComponent
			&& HandTransform.Equals(Other.HandTransform, UE_SMALL_NUMBER);
	}
};

UCLASS(Blueprintable)
class CWR_API ACWRWeaponActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ACWRWeaponActor();
	
	//UFUNCTION(BlueprintCallable)
	//FORCEINLINE UAkComponent* GetAkComponent() const { return AkComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USkeletalMeshComponent* GetFPMesh() const { return FPMesh; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USkeletalMeshComponent* GetTPMesh() const { return TPMesh; }

	FORCEINLINE FName GetFPSocketAttach() const { return FPSocketAttach; }
	FORCEINLINE FName GetTPSocketAttach() const { return TPSocketAttach; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetAnimationSlot() const { return AnimationSlot; }

	FORCEINLINE UCurveFloat*		GetPitchRecoilCurve()					const { return PitchRecoilCurve;				}
	FORCEINLINE UCurveFloat*		GetYawRecoilCurve()						const { return YawRecoilCurve;				}

	FORCEINLINE float				GetMinRotX()							const { return MinRotX;							}
	FORCEINLINE float				GetMinRotY()							const { return MinRotY;							}
	FORCEINLINE float				GetMinRotZ()							const { return MinRotZ;							}
	FORCEINLINE float				GetMaxRotX()							const { return MaxRotX;							}
	FORCEINLINE float				GetMaxRotY()							const { return MaxRotY;							}
	FORCEINLINE float				GetMaxRotZ()							const { return MaxRotZ;							}
	FORCEINLINE float				GetMinLocX()							const { return MinLocX;							}
	FORCEINLINE float				GetMinLocY()							const { return MinLocY;							}
	FORCEINLINE float				GetMinLocZ()							const { return MinLocZ;							}
	FORCEINLINE float				GetMaxLocX()							const { return MaxLocX;							}
	FORCEINLINE float				GetMaxLocY()							const { return MaxLocY;							}
	FORCEINLINE float				GetMaxLocZ()							const { return MaxLocZ;							}
	
	FORCEINLINE float				GetHipFireRecoilCurveMultiplier()		const { return HipFireRecoilCurveMultiplier;	}
	FORCEINLINE float				GetRecoilRecoverSpeed()					const { return RecoilRecoverSpeed;				}
	FORCEINLINE float				GetHipFireRecoilLocationMultiplier()	const { return HipFireRecoilLocationMultiplier; }
	FORCEINLINE float				GetHipFireRecoilRotationMultiplier()	const { return HipFireRecoilRotationMultiplier; }
	FORCEINLINE float				GetLengthOfWeapon()						const { return LengthOfWeapon;					}
	FORCEINLINE float				GetAimOutSpeed()						const { return AimOutSpeed;						}
	FORCEINLINE float				GetAimInSpeed()							const { return AimInSpeed;						}
	FORCEINLINE float				GetCycleSightSpeed()					const { return CycleSightSpeed;					}
	FORCEINLINE float				GetADSFov()								const { return ADSFov;							}
	
	FORCEINLINE UAnimMontage*		GetTPMontage_ReloadWeapon()				const { return TPMontage_ReloadWeapon;		}
	FORCEINLINE UAnimMontage*		GetTPMontage_ReloadWeapon_Empty()		const { return TPMontage_ReloadWeapon_Empty;	}
	FORCEINLINE UAnimMontage*		GetTPMontage_UnequipWeapon()			const { return TPMontage_UnequipWeapon;		} 
	FORCEINLINE UAnimMontage*		GetTPMontage_EquipWeapon()				const { return TPMontage_EquipWeapon;			}
	FORCEINLINE UAnimMontage*		GetTPMontage_MeleeAttack()				const { return TPMontage_MeleeAttack;			}
	FORCEINLINE UAnimMontage*		GetTPMontage_FireWeapon()				const { return TPMontage_FireWeapon;			}
	FORCEINLINE UAnimMontage*		GetFPMontage_ReloadWeapon()				const { return FPMontage_ReloadWeapon;		}
	FORCEINLINE UAnimMontage*		GetFPMontage_ReloadWeapon_Empty()		const { return FPMontage_ReloadWeapon_Empty;	}
	FORCEINLINE UAnimMontage*		GetFPMontage_UnequipWeapon()			const { return FPMontage_UnequipWeapon;		}
	FORCEINLINE UAnimMontage*		GetFPMontage_EquipWeapon()				const { return FPMontage_EquipWeapon;			} 
	FORCEINLINE UAnimMontage*		GetFPMontage_MeleeAttack()				const { return FPMontage_MeleeAttack;			}
	
	FORCEINLINE UAnimSequence*		GetReloadWeaponAnimation()				const { return ReloadWeaponAnimation;			}
	FORCEINLINE UAnimSequence*		GetReloadWeaponAnimation_Empty()		const { return ReloadWeaponAnimation_Empty;	}

	FORCEINLINE USkeletalMesh*		GetDroppedMagazineMesh()				const { return DroppedMagazineMesh;			}
	FORCEINLINE USoundBase*			GetMagazineDropSound()					const { return MagazineDropSound;				}
	FORCEINLINE	USoundAttenuation*	GetMagazineDropSoundAttenuation()		const {	return MagazineDropSoundAttenuation;	}


	
	FORCEINLINE void SetIsAiming	(const bool bInAiming) { bIsAiming = bInAiming; }
	
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

	UFUNCTION(BlueprintCallable)
	void FireWeapon();

	void FireLinetrace();

	void Linetrace(FVector TraceStart, FVector ForwardVector, bool bClient, APawn* PawnInstigator);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_LineTrace(FVector TraceStart, FVector ForwardVector, APawn* PawnInstigator);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_LineTraceHit(FHitResult Hit, APawn* PawnInstigator, float DamageMutator);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_SubsonicSupression(FVector Location, FRotator Rotation, AActor* OwningActor);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_ApplyImpulse(UPrimitiveComponent* Target, FVector Start, FVector Location, FName BoneName);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SVR_CallHitImpactEffect(const TArray<FHitResult>& HitResults);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_HitImpactEffect(const TArray<FHitResult>& HitResults);

	void PlayEffectsOnClient();
	void PlayWeaponFireAnimation() const;
	void PlayWeaponFireSound() const;
	void EmptyAnimation() const;

	void FetchLinetraceHit(FHitResult Hit, APawn* InstigatorPawn, float DamageMutator);
	
	void IncreaseHeat();

	void HeatBuildup();

	void PlayShotShake() const;
	
	UFUNCTION(Server, Unreliable)
	void SVR_SpawnCasing();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_SpawnCasing();

	UFUNCTION(Server, Reliable)
	void SVR_PlayTPMuzzleFlash();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_TPMuzzleFlash();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SVR_SetHasSuppressor(bool bInHasSuppressor);

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SVR_SetHasSight(bool bInHasSight);

	UFUNCTION(BlueprintCallable)
	void SpawnSmoke();
	
	UFUNCTION(BlueprintCallable)
	void SpawnHeatDistortion();

	UFUNCTION(BlueprintCallable)
	void TimerUndoEffectBuildup();

	UFUNCTION(BlueprintCallable)
	void TimerCooldownHeat();

	UFUNCTION(BlueprintCallable)
	void CalculateSightTransform();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void EnableScope(bool bInEnabled);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void EnableLaser(bool bInToggle);
	
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> RootSphereComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MuzzleSocketName = "Muzzle";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  BarrelSocketName = "BarrelSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  ScopeSocketName = "ScopeSocket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName  MagazineSocketName = "MagazineSocket";

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//TObjectPtr<UAkAudioEvent> FireSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> MuzzleFireNiagara;

	/** Interaction Interface */
	void Interact_Implementation(APawn* InstigatorPawn);
	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) const override;
	/** end Interaction Interface */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCWRInventoryItemDefinition> WeaponDefinition; 

	UPROPERTY(Transient,BlueprintReadWrite, Category="Components")
	UNiagaraComponent* SpawnedSmokeTrail = nullptr;; 

	UPROPERTY(Transient,BlueprintReadWrite, Category="Components")
	UNiagaraComponent* SpawnedHeat = nullptr;

	UPROPERTY(Transient,BlueprintReadWrite, Category="Components")
	UParticleSystemComponent* SpawnedSmokeCloud = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	bool bApplyRecoilMagnification = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinRotX = -4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinRotY = -5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinRotZ = -1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxRotX = -6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxRotY = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxRotZ = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinLocX = -0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinLocY = -3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MinLocZ = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxLocX = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxLocY = -1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float MaxLocZ = 1.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float RecoilMagnificationFactor = 2.6f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float HipFireRecoilCurveMultiplier = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float RecoilRecoverSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float HipFireRecoilRotationMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	float HipFireRecoilLocationMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	TObjectPtr<UCurveFloat> YawRecoilCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recoil")
	TObjectPtr<UCurveFloat> PitchRecoilCurve = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Conditions")
	bool bHasSight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Conditions")
	bool bReloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Conditions")
	bool bHasIronSights = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Conditions")
	bool bHasSuppressor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Conditions")
	bool bIsAiming = false;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category="Attachments")
	int32 CurrentSightIndex = 0;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Attachments")
	TObjectPtr<ACWRAttachmentActor> SuppressorRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachments")
	float DistanceFromSight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachments")
	float CycleSightSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachments")
	float AimInSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachments")
	float AimOutSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> WeaponFireSFX = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundCue> NoAmmoFireSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundCue> SelectFireModeSFX = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> WeaponFireSuppressedSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> WeaponSubsonicSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> ImpactDefaultSFX = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> SlideLockSFX = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> CasingSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundBase> MagazineDropSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundAttenuation> ImpactSoundAttenuation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundAttenuation> SubsonicSoundAttenuation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundAttenuation> CasingSoundAttenuation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	TObjectPtr<USoundAttenuation> MagazineDropSoundAttenuation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type")
	FName AnimationSlot = "2";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	TObjectPtr<UAnimSequence> ShotAnimation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	TObjectPtr<UAnimSequence> EmptySlideAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	TObjectPtr<UAnimSequence> ReloadWeaponAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	TObjectPtr<UAnimSequence> ReloadWeaponAnimation_Empty = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_ReloadWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_ReloadWeapon_Empty = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_UnequipWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_EquipWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_MeleeAttack = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TPAnimations")
	TObjectPtr<UAnimMontage> TPMontage_FireWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FPAnimations")
	TObjectPtr<UAnimMontage> FPMontage_ReloadWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FPAnimations")
	TObjectPtr<UAnimMontage> FPMontage_ReloadWeapon_Empty = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FPAnimations")
	TObjectPtr<UAnimMontage> FPMontage_UnequipWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FPAnimations")
	TObjectPtr<UAnimMontage> FPMontage_EquipWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FPAnimations")
	TObjectPtr<UAnimMontage> FPMontage_MeleeAttack = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UMaterialInterface> DefaultImpactDecal = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UParticleSystem> DefaultImpactParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UParticleSystem> MuzzleFlashParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UParticleSystem> BarrelSmokeParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UNiagaraSystem> ShotSmokeParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UNiagaraSystem> SmokeTrail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UNiagaraSystem> MuzzleDistortion = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<UStaticMesh> CasingMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float EffectsBuildup = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float Heat = 0.f;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Effect")
	FTimerHandle BuildupTimer;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category="Effect")
	FTimerHandle HeatupTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TSubclassOf<UCameraShakeBase> FireShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TObjectPtr<USkeletalMesh> DroppedMagazineMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float NoiseLoudness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float NoiseRange = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponRange = 25000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float BulletForce = 5000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float Damage = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float LengthOfWeapon = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float BulletVelocity = 60000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float FireRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float BurstFireRate = 0.07f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FName FPSocketAttach = "GripPoint";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FName TPSocketAttach = "TPGripSocket";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float ADSFov = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	float ADSFovInterpSpeed = 3.f;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category="Misc")
	TArray<FHitResult> OutHit;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Misc")
	FHitResult CurrentHit;

	UPROPERTY(Transient, BlueprintReadWrite, Category="Misc")
	TObjectPtr<APawn> InstigatorRef;
	
	UPROPERTY(Transient, BlueprintReadWrite)
	TArray<FCWRAttachmentInfo> Attachments;

	UPROPERTY(Transient, BlueprintReadWrite)
	TArray<ACWRAttachmentActor*> FPCurrentAttachments;

	UPROPERTY(Transient, BlueprintReadWrite)
	TArray<ACWRAttachmentActor*> TPCurrentAttachments;

	UPROPERTY(Transient, BlueprintReadWrite)
	TArray<FCWRSightData> Sights;
	
	void AddWeaponStartingParts();
	
	void SpawnFPAttachments(const TSubclassOf<ACWRAttachmentActor>& AttachmentClass, FName AttachmentSocket, bool bAttachmentIsActioned);
	void SpawnTPAttachments(const TSubclassOf<ACWRAttachmentActor>& AttachmentClass, FName AttachmentSocket);

	void FindSights();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CycleCurrentSights();

	void HasStartingParts();

	void SetRecoilMagnification();

	void SetSound();

	UFUNCTION(BlueprintCallable)
	void CallEffects(FHitResult Hit);
	
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TPMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FPMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> TPShellEject = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> FPShellEject = nullptr;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UAkComponent> AkComponent = nullptr;

	bool bSetRecoilMagnificationFired = false;
};
