// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRWeaponActor.h"

#include "AkComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Character/CWRCharacter_Base.h"
#include "Character/CWRDismembermentComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "CWR/CWRCharacter_Player.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "UI/CWRWorldUserWidget.h"
#include "Weapons/CWRAttachmentActor.h"
#include "Weapons/CWRCasingActor.h"
#include "Weapons/CWRWeaponCustomizationItem.h"


ACWRWeaponActor::ACWRWeaponActor()
{
 
	PrimaryActorTick.bCanEverTick = false;

	bNetUseOwnerRelevancy = true;
	SetNetUpdateFrequency(30.f);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TPMesh"));
	TPMesh->SetOwnerNoSee(true);
	TPMesh->bUseAttachParentBound = true;
	TPMesh->SetupAttachment(RootComponent);

	TPShellEject = CreateDefaultSubobject<UArrowComponent>(TEXT("TPShellEject"));
	TPShellEject->ArrowSize = 0.25f;
	TPShellEject->SetupAttachment(TPMesh);
	
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetCastShadow(false);
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->bUseAsOccluder = false;
	FPMesh->SetupAttachment(RootComponent);

	FPShellEject = CreateDefaultSubobject<UArrowComponent>(TEXT("FPShellEject"));
	FPShellEject->ArrowSize = 0.25f;
	FPShellEject->SetupAttachment(FPMesh);

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);


}

void ACWRWeaponActor::TriggerFireAudio() const
{
	if ( IsValid(AkComponent) )
	{
		AkComponent->PostAkEvent(FireSound);
	}
}

ACWRCharacter_Base* ACWRWeaponActor::GetOwningCharacter() const
{
	return Cast<ACWRCharacter_Base>(Owner);
}

void ACWRWeaponActor::TriggerMuzzleFireEffect() 
{
	/*
	if ( IsValid(NiagaraComponent) )
	{
		NiagaraComponent->Activate();
	}*/
}

void ACWRWeaponActor::DisableMuzzleFireEffect()
{
	
}

void ACWRWeaponActor::SpawnAttachment(UCWRWeaponCustomizationItem* CustomizationItem)
{
	UStaticMeshComponent* AttachmentMeshComponent = NewObject<UStaticMeshComponent>(this);
	FName AttachmentSocketName;
	switch (CustomizationItem->ItemCategory)
	{
	case ECWRWeaponItemCategory::Scope:
		
		AttachmentSocketName = ScopeSocketName;
		break;
		
	case ECWRWeaponItemCategory::Barrel:
		
		AttachmentSocketName = BarrelSocketName;
		break;
		
	case ECWRWeaponItemCategory::Magazine:

		AttachmentSocketName = MagazineSocketName;
		break;

		default:
			AttachmentSocketName = "";
			break;
	}
	
/*	AttachmentMeshComponent->SetupAttachment(GetWeaponMesh(), AttachmentSocketName);
	AttachmentMeshComponent->SetStaticMesh(CustomizationItem->Mesh);
	AttachmentMeshComponent->CanCharacterStepUpOn = ECB_No;
	AttachmentMeshComponent->SetCanEverAffectNavigation(false);
	AttachmentMeshComponent->SetGenerateOverlapEvents(false);
	AttachmentMeshComponent->SetOnlyOwnerSee(true);
	AttachmentMeshComponent->SetRelativeTransform(CustomizationItem->RelativeTransform);
	AttachmentMeshComponent->RegisterComponent();
	AttachmentMeshComponent->SetSimulatePhysics(false);
	AttachmentMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	
}

void ACWRWeaponActor::FireWeapon()
{
	FireLinetrace();
	PlayEffectsOnClient();
	IncreaseHeat();
	SVR_PlayTPMuzzleFlash();
	PlayShotShake();
}

void ACWRWeaponActor::FireLinetrace()
{
	if ( APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0) )
	{
		FVector Impulse = CameraManager->GetTransformComponent()->GetForwardVector();

		FTransform MuzzleTransform = FPMesh->GetSocketTransform(MuzzleSocketName);

		FVector Start = MuzzleTransform.GetLocation();
		FVector ForwardVector = MuzzleTransform.Rotator().Vector();

		SVR_LineTrace(Start, ForwardVector, GetOwningCharacter());

		if ( !HasAuthority() )
		{
			// Prevents Client waiting for server to catchup
			Linetrace(Start, ForwardVector, true, GetOwningCharacter());
		}
	}
	
}

void ACWRWeaponActor::Linetrace(FVector TraceStart, FVector ForwardVector, bool bClient, APawn* PawnInstigator)
{
	OutHit.Empty();

	InstigatorRef = PawnInstigator;


	FHitResult TracedOutHit;

	FCollisionQueryParams CollisionParameters;
	CollisionParameters.bTraceComplex = false;
	CollisionParameters.AddIgnoredActor(InstigatorRef);
	CollisionParameters.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(TracedOutHit, TraceStart, TraceStart + WeaponRange * ForwardVector, ECC_Visibility,CollisionParameters );

	OutHit.Add(TracedOutHit);
	CurrentHit = TracedOutHit;

	if (bClient)
	{
		if ( AActor* HitActor = CurrentHit.GetActor()  )
		{
			SVR_LineTraceHit(CurrentHit, InstigatorRef, 1.f);

			if (const auto DismembermentComponent = HitActor->GetComponentByClass<UCWRDismembermentComponent>() )
			{

				const FTransform MuzzleTransform = FPMesh->GetSocketTransform(MuzzleSocketName);
			
				DismembermentComponent->HitLimb(5,CurrentHit.BoneName,this, HitActor,MuzzleTransform.Rotator().Vector() * BulletForce, CurrentHit.Location);
			}
		}
	}

	FHitResult TracedOutHitInteraction;
	
	GetWorld()->LineTraceSingleByChannel(TracedOutHit, TraceStart, TraceStart + WeaponRange * ForwardVector, ECC_GameTraceChannel1,CollisionParameters);

	if ( TracedOutHitInteraction.bBlockingHit )
	{
		const FRotator LookRotation = FRotationMatrix::MakeFromX(TracedOutHitInteraction.ImpactNormal).Rotator();
		MC_SubsonicSupression(TracedOutHitInteraction.Location, LookRotation, TracedOutHitInteraction.GetActor());
	}

	if ( bClient )
	{
		for (const FHitResult Out : OutHit )
		{
			CallEffects(Out);
		}
	}
	else
	{
		SVR_CallHitImpactEffect(OutHit);
	}
	
}

void ACWRWeaponActor::SVR_LineTrace_Implementation(FVector TraceStart, FVector ForwardVector, APawn* PawnInstigator)
{
	Linetrace(TraceStart, ForwardVector, false, PawnInstigator);
	
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseLoudness,GetOwner(), NoiseRange);
}

void ACWRWeaponActor::SVR_LineTraceHit_Implementation(FHitResult Hit, APawn* PawnInstigator, float DamageMutator)
{
	FetchLinetraceHit(Hit, PawnInstigator, DamageMutator);
}

void ACWRWeaponActor::MC_SubsonicSupression_Implementation(FVector Location, FRotator Rotation, AActor* OwningActor)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSubsonicSound, Location, Rotation, FMath::FRandRange(0.8f, 1.2f), 1.f,0.f, SubsonicSoundAttenuation, nullptr, OwningActor);
}


void ACWRWeaponActor::SVR_CallHitImpactEffect_Implementation(const TArray<FHitResult>& HitResults)
{
	MC_HitImpactEffect(HitResults);
}

void ACWRWeaponActor::MC_ApplyImpulse_Implementation(UPrimitiveComponent* Target, FVector Start, FVector Location, FName BoneName)
{
	Target->AddImpulseAtLocation(Start * BulletForce, Location, BoneName);
}

void ACWRWeaponActor::MC_HitImpactEffect_Implementation(const TArray<FHitResult>& HitResults)
{
	if (GetWorld()->IsNetMode(NM_DedicatedServer)) return;

	if (HasAuthority() && UGameplayStatics::GetPlayerPawn(this, 0) != GetOwner())
	{
		for (const FHitResult HitResult : HitResults )
		{
			CallEffects(HitResult);
		}
	}
}

void ACWRWeaponActor::PlayEffectsOnClient()
{
	if ( !FPMesh ) return;

	PlayWeaponFireAnimation();
	PlayWeaponFireSound();
	EmptyAnimation();
	SVR_SpawnCasing();

	if ( bHasSuppressor )
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashParticle,FPMesh,MuzzleSocketName,FVector::ZeroVector,FPMesh->GetSocketRotation(MuzzleSocketName));
	}
	else
	{
		const FTransform MuzzleSocketTransform = FPMesh->GetSocketTransform(MuzzleSocketName);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShotSmokeParticle,MuzzleSocketTransform.GetLocation(), MuzzleSocketTransform.Rotator());
	}
}

void ACWRWeaponActor::PlayWeaponFireAnimation() const
{
	TPMesh->PlayAnimation(ShotAnimation, false);
	FPMesh->PlayAnimation(ShotAnimation, false);
}

void ACWRWeaponActor::PlayWeaponFireSound() const
{
	const FTransform MuzzleSocketTransform = FPMesh->GetSocketTransform(MuzzleSocketName);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(),WeaponFireSFX,MuzzleSocketTransform.GetLocation(),MuzzleSocketTransform.Rotator());
}

void ACWRWeaponActor::EmptyAnimation() const
{
	FPMesh->PlayAnimation(EmptySlideAnimation, false);
	UGameplayStatics::PlaySound2D(GetWorld(),SlideLockSFX);
}

void ACWRWeaponActor::FetchLinetraceHit(FHitResult Hit, APawn* InstigatorPawn, float DamageMutator)
{
	UGameplayStatics::ApplyPointDamage(Hit.GetActor(),Hit.BoneName == "head" ? Damage * 5.f : Damage * DamageMutator, Hit.TraceEnd - Hit.TraceStart, Hit, InstigatorPawn->GetController(), InstigatorPawn,DamageType );

	if (Hit.Component->IsSimulatingPhysics() )
	{
		MC_ApplyImpulse_Implementation(Hit.GetComponent(),TPMesh->GetSocketRotation(MuzzleSocketName).Vector(),Hit.Location,Hit.BoneName);
	}
}

void ACWRWeaponActor::IncreaseHeat()
{
	++EffectsBuildup;
	HeatBuildup();
}

void ACWRWeaponActor::HeatBuildup()
{
	GetWorldTimerManager().ClearTimer(HeatupTimer);
	
	Heat = FMath::Clamp(Heat + 0.1f,0.f,30.f);

	if (bHasSuppressor)
	{
		//SuppressorRef->HeatUp
	}
}

void ACWRWeaponActor::PlayShotShake() const
{
	APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
	PlayerCameraManager->StartCameraShake(FireShakeClass, bIsAiming ? 0.6f : 1.f);
}

void ACWRWeaponActor::SVR_SpawnCasing_Implementation()
{
	MC_SpawnCasing();
}

void ACWRWeaponActor::MC_SpawnCasing_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer) return;

/*	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.bDeferConstruction = true;
	
	ACWRCasingActor* CasingActor = GetWorld()->SpawnActor<ACWRCasingActor>(ACWRCasingActor::StaticClass(),FPShellEject->GetComponentToWorld(),SpawnParameters );
	CasingActor->SetCasingMesh(CasingMesh);
	CasingActor->SetCasingSound(CasingSound);
	CasingActor->SetCasingSoundAttenuation(CasingSoundAttenuation);
	CasingActor->FinishSpawning(FPShellEject->GetComponentToWorld(), false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);;

	CasingActor->GetFP()->SetWorldTransform(FPShellEject->GetComponentToWorld());
	CasingActor->GetTP()->SetWorldTransform(TPShellEject->GetComponentToWorld());*/
}

void ACWRWeaponActor::SVR_PlayTPMuzzleFlash_Implementation()
{
	MC_TPMuzzleFlash();
}

void ACWRWeaponActor::MC_TPMuzzleFlash_Implementation()
{
	if (GetNetMode() == NM_DedicatedServer) return;

	if ( GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0) ) return;

	if ( bHasSuppressor )
	{
		PlayWeaponFireSound();
	}
	else
	{
		const FTransform MuzzleTransform = TPMesh->GetSocketTransform(MuzzleSocketName,RTS_Component);
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashParticle,TPMesh,MuzzleSocketName, FVector::ZeroVector,MuzzleTransform.Rotator());
	}
}

void ACWRWeaponActor::SVR_SetHasSuppressor_Implementation(bool bInHasSuppressor)
{
	bHasSuppressor = bInHasSuppressor;
}

void ACWRWeaponActor::SVR_SetHasSight_Implementation(bool bInHasSight)
{
	bHasSight = bInHasSight;
}

void ACWRWeaponActor::SpawnSmoke()
{
	if ( EffectsBuildup <= 10.f ) return;

	if ( !SpawnedSmokeTrail )
	{
		SpawnedSmokeTrail = UNiagaraFunctionLibrary::SpawnSystemAttached(SmokeTrail,FPMesh,MuzzleSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::Type::KeepRelativeOffset,false);
		FTimerHandle SmokeTrailTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(SmokeTrailTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(SpawnedSmokeTrail))
			{
				SpawnedSmokeTrail->DestroyComponent();
			}
		}), 8.0f, false);
	}

	if ( !SpawnedSmokeCloud )
	{
		SpawnedSmokeCloud = UGameplayStatics::SpawnEmitterAttached(BarrelSmokeParticle, FPMesh, "Muzzle_Smoke");
		FTimerHandle SmokeCloudTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(SmokeCloudTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(SpawnedSmokeCloud))
			{
				SpawnedSmokeCloud->DestroyComponent();
			}
		}), 8.0f, false);
	}
}

void ACWRWeaponActor::SpawnHeatDistortion()
{
	if ( EffectsBuildup <= 10.f ) return;

	if ( !SpawnedHeat )
	{
		SpawnedHeat = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleDistortion,FPMesh,MuzzleSocketName,FVector::ZeroVector,FPMesh->GetSocketRotation(MuzzleSocketName),EAttachLocation::Type::KeepRelativeOffset,false);
		FTimerHandle HeatDistortionTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(HeatDistortionTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(SpawnedHeat))
			{
				SpawnedHeat->DestroyComponent();
			}
		}), 3.0f, false);
	}
}

void ACWRWeaponActor::TimerUndoEffectBuildup()
{
	GetWorld()->GetTimerManager().SetTimer(BuildupTimer, FTimerDelegate::CreateLambda([this]()
	{
		EffectsBuildup = FMath::FInterpTo(EffectsBuildup, 0.f,GetWorld()->GetTimeSeconds(), 1.f);
		if ( FMath::IsNearlyEqual(EffectsBuildup, 0.f,0.001f) )
		{
			GetWorldTimerManager().ClearTimer(BuildupTimer);
		}
		
	}), 0.05f, true);
}

void ACWRWeaponActor::TimerCooldownHeat()
{
	GetWorld()->GetTimerManager().SetTimer(HeatupTimer, FTimerDelegate::CreateLambda([this]()
	{
		Heat = FMath::Clamp(Heat - 0.005f, 0.f, Heat);

		if (bHasSuppressor)
		{
			//SuppressorRef->Cooldown;
		}
		
	}), 0.05f, true);
}

void ACWRWeaponActor::CalculateSightTransform()
{
	if (const auto PlayerCharacter = Cast<ACWRCharacter_Player>(GetOwningCharacter()) )
	{
		for ( int32 i = 0; i < Sights.Num(); ++i )
		{
			FTransform NewTransform = FTransform::Identity;
			FTransform SightMainSocketTransform = Sights[i].MainComponent->GetSocketTransform(Sights[i].MainSocket);
			FTransform RightHandTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform("hand_r");
			FTransform RelativeSightToHandTransform = SightMainSocketTransform.GetRelativeTransform(RightHandTransform);
			NewTransform.SetLocation(RelativeSightToHandTransform.GetLocation());
			FTransform RelativeHandToSightTransform = RightHandTransform.GetRelativeTransform(SightMainSocketTransform);
			NewTransform.SetScale3D(FVector(RelativeHandToSightTransform.GetLocation().X * -1.f + DistanceFromSight, 1.f, 1.f));
				
			if ( Sights[i].PartType == ECWRSightType::Optic )
			{
				NewTransform.SetRotation(RelativeSightToHandTransform.GetRotation());
			}
			else if ( Sights[i].PartType == ECWRSightType::Ironsight )
			{
				FVector SightFrontSocketLocation = Sights[i].FrontComponent->GetSocketLocation(Sights[i].FrontSocket);

				FRotator SightRotation = FRotationMatrix::MakeFromX(SightMainSocketTransform.GetLocation() - SightFrontSocketLocation).Rotator();

				FRotator InverseRotation = RightHandTransform.InverseTransformRotation(SightRotation.Quaternion()).Rotator();
					
				FRotator NewRotation = FRotator(RelativeSightToHandTransform.Rotator().Roll,InverseRotation.Pitch, InverseRotation.Yaw);
				NewTransform.SetRotation(NewRotation.Quaternion());
			}
				
			Sights[i].HandTransform = NewTransform;
		}

		PlayerCharacter->SetSightTransform(Sights.IsEmpty() ? FTransform::Identity : Sights[CurrentSightIndex].HandTransform);
		if ( HasAuthority() )
		{
			PlayerCharacter->SightTransformChanged();
		}
	}
}

void ACWRWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	if ( HasAuthority() )
	{
		AddWeaponStartingParts();
		FindSights();
		CycleCurrentSights();
	}
}

void ACWRWeaponActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if (const auto CWRCharacter = Cast<ACWRCharacter_Base>(InstigatorPawn) )
	{
		CWRCharacter->PickUpItem(WeaponDefinition);
	}
}

FText ACWRWeaponActor::GetInteractText_Implementation(APawn* InstigatorPawn) const
{
	return FText::GetEmpty();
}

void ACWRWeaponActor::AddWeaponStartingParts()
{
	for ( const auto [AttachmentClass, BaseIndex, BaseSocket, bActioned] : Attachments )
	{
		if ( BaseIndex == INDEX_NONE )
		{
			SpawnFPAttachments(AttachmentClass, BaseSocket, bActioned);
			SpawnTPAttachments(AttachmentClass, BaseSocket);
		}
	}
}

void ACWRWeaponActor::SpawnFPAttachments(const TSubclassOf<ACWRAttachmentActor>& AttachmentClass, const FName AttachmentSocket, const bool bAttachmentIsActioned)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
	SpawnParameters.bDeferConstruction = true;
	SpawnParameters.Owner = GetOwningCharacter();
	ACWRAttachmentActor* Attachment = GetWorld()->SpawnActor<ACWRAttachmentActor>(AttachmentClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	Attachment->SetVisibility(true);
	Attachment->SetFP(true);
	Attachment->FinishSpawning(FTransform::Identity, false, nullptr, ESpawnActorScaleMethod::SelectDefaultAtRuntime);

	if ( IsValid(Attachment) )
	{
		if ( Attachment->IsAttachmentToggleable() )
		{
			Attachment->SetEnabled(bAttachmentIsActioned);
		}
		Attachment->AttachToComponent(FPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true),AttachmentSocket);

		FPCurrentAttachments.AddUnique(Attachment);
	}
}

void ACWRWeaponActor::SpawnTPAttachments(const TSubclassOf<ACWRAttachmentActor>& AttachmentClass, const FName AttachmentSocket)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
	SpawnParameters.bDeferConstruction = true;
	SpawnParameters.Owner = GetOwningCharacter();
	ACWRAttachmentActor* Attachment = GetWorld()->SpawnActor<ACWRAttachmentActor>(AttachmentClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	Attachment->SetVisibility(true);
	Attachment->SetTP(true);
	Attachment->FinishSpawning(FTransform::Identity, false, nullptr, ESpawnActorScaleMethod::SelectDefaultAtRuntime);

	if ( IsValid(Attachment) )
	{
		Attachment->AttachToComponent(TPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true),AttachmentSocket);

		TPCurrentAttachments.AddUnique(Attachment);
	}
}

void ACWRWeaponActor::FindSights()
{
	Sights.Empty();
	CurrentSightIndex = 0;

	TArray<UPrimitiveComponent*> ComponentArray;
	
	if ( UMeshComponent* MeshComponent = GetComponentByClass<UMeshComponent>() )
	{
		ComponentArray.AddUnique(MeshComponent);

		TArray<USceneComponent*> Components;
		MeshComponent->GetChildrenComponents(true, Components);

		for ( USceneComponent* Component : Components )
		{
			if ( auto PrimitiveComponent = Cast<UPrimitiveComponent>(Component) )
			{
				ComponentArray.AddUnique(PrimitiveComponent);
			}
		}
	}
	
	TArray<AActor*> FoundActors;
	UKismetSystemLibrary::GetActorListFromComponentList(ComponentArray, ACWRAttachmentActor::StaticClass(), FoundActors);

	TArray<ACWRAttachmentActor*> AttachmentList;
	for ( AActor* FoundActor : FoundActors )
	{
		if ( auto AttachmentActor = Cast<ACWRAttachmentActor>(FoundActor) )
		{
			AttachmentList.AddUnique(AttachmentActor);
		}
	}
	
	for ( const ACWRAttachmentActor* Attachment : AttachmentList )
	{
		for ( const FCWRActualAimInfo Sight : Attachment->GetSight() )
		{
			FCWRSightData SightData;
			SightData.PartType = ECWRSightType::Optic;
			SightData.MainSocket = Sight.SocketName;
			SightData.MainComponent = Sight.MeshComponent;
			Sights.AddUnique(SightData);
		}
 
		for ( const FCWRActualAimInfo FrontSight : Attachment->GetFrontSight() )
		{
			if ( !FrontSight.bCurrentlyUsing )
			{
				continue;
			}
			
			bool bSightFound = false;
			
			for ( const ACWRAttachmentActor* AttachmentActor : AttachmentList )
			{
				for ( const FCWRActualAimInfo RearSight : AttachmentActor->GetRearSight() )
				{
					if ( RearSight.bCurrentlyUsing )
					{
						FCWRSightData SightData;
						SightData.PartType = ECWRSightType::Ironsight;
						SightData.MainSocket = FrontSight.SocketName;
						SightData.FrontSocket = RearSight.SocketName;
						SightData.MainComponent = FrontSight.MeshComponent;
						SightData.FrontComponent = RearSight.MeshComponent;
						Sights.AddUnique(SightData);

						bSightFound = true;
						break;
					}
				}
				
				if (bSightFound)
				{
					break;
				}
			}
		}
	}
	
}

void ACWRWeaponActor::HasStartingParts()
{
	SetRecoilMagnification();
	SetSound();

	
}

void ACWRWeaponActor::SetRecoilMagnification()
{
	if ( !HasAuthority() ) return;

	if ( !bApplyRecoilMagnification || !bHasSight ) return;

	if ( !bSetRecoilMagnificationFired )
	{
		bSetRecoilMagnificationFired = true;

		MinRotX *= RecoilMagnificationFactor;
		MaxRotX *= RecoilMagnificationFactor;
	}
}

void ACWRWeaponActor::SetSound()
{
	if ( IsValid(SuppressorRef) )
	{
		WeaponFireSFX = WeaponFireSuppressedSFX;
	}
}

void ACWRWeaponActor::CallEffects(FHitResult Hit)
{
	const float DecalSize = FMath::FRandRange(3.0f, 6.0f);
	const FRotator Rotation = FRotationMatrix::MakeFromX(Hit.ImpactNormal).Rotator();
	if ( UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultImpactDecal,FVector(DecalSize),Hit.ImpactPoint, Rotation, 15.f) )
	{
		SpawnedDecal->SetFadeScreenSize(0.01f);
	}

	const float EmitterSize = FMath::FRandRange(0.5f, 1.25f);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DefaultImpactParticle,Hit.ImpactPoint,Rotation,FVector(EmitterSize),true, EPSCPoolMethod::AutoRelease);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),ImpactDefaultSFX,Hit.ImpactPoint,FRotator::ZeroRotator,1.f,1.f,0.f,ImpactSoundAttenuation);
	
}

void ACWRWeaponActor::CycleCurrentSights_Implementation()
{
	if ( Sights.Num() <= 1 ) return;

	Sights.Num() - 1 > CurrentSightIndex ? ++CurrentSightIndex : CurrentSightIndex = 0;

	const FCWRSightData Sight = Sights[CurrentSightIndex];
	const FTransform HandTransform = Sight.HandTransform;

	if (const auto PlayerCharacter =  Cast<ACWRCharacter_Player>(GetOwningCharacter()) )
	{
		PlayerCharacter->SetSightTransform(HandTransform);
		if ( HasAuthority() )
		{
			PlayerCharacter->SightTransformChanged();
		}
	}
	
}




