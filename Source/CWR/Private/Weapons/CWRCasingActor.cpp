// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CWRCasingActor.h"

#include "Kismet/GameplayStatics.h"

ACWRCasingActor::ACWRCasingActor()
{
	PrimaryActorTick.bCanEverTick = false;

	FP = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FP"));
	FP->SetupAttachment(GetRootComponent());
	FP->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	FP->SetNotifyRigidBodyCollision(true);
	FP->SetCollisionObjectType(ECC_GameTraceChannel5);
	FP->SetCollisionResponseToAllChannels(ECR_Ignore);
	FP->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	FP->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	FP->SetOnlyOwnerSee(true);

	TP = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TP"));
	TP->SetupAttachment(GetRootComponent());
	FP->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	FP->SetNotifyRigidBodyCollision(true);
	FP->SetCollisionObjectType(ECC_GameTraceChannel5);
	FP->SetCollisionResponseToAllChannels(ECR_Ignore);
	FP->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	FP->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	TP->SetOnlyOwnerSee(true);
	
}

void ACWRCasingActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FP->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ACWRCasingActor::BeginPlay()
{
	Super::BeginPlay();

	const FVector Impulse = GetActorTransform().TransformVectorNoScale(FVector(FMath::FRandRange(250.f,500.f), FMath::FRandRange(40.f,60.f), 0.f));

	const float TorqueXY = FMath::FRandRange(5.f, 10.f);
	const FVector Torque = FVector(TorqueXY, TorqueXY, 150000.f);
	
	TP->SetStaticMesh(CasingMesh);
	TP->SetSimulatePhysics(true);
	TP->AddImpulse(Impulse,NAME_None,true);
	TP->AddTorqueInRadians(Torque,NAME_None,true);
	
	FP->SetStaticMesh(CasingMesh);
	FP->SetSimulatePhysics(true);
	FP->AddImpulse(Impulse,NAME_None,true);
	FP->AddTorqueInRadians(Torque,NAME_None,true);
	
}

void ACWRCasingActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bWasHit) return;
	
	bWasHit = true;
	
	UGameplayStatics::PlaySoundAtLocation(this, CasingSound,Hit.ImpactPoint,FRotationMatrix::MakeFromX(Hit.ImpactNormal).Rotator(),5.f,1.f,0.f,CasingSoundAttenuation);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::StopSimulatingPhysics, 3.0f, false);
}

void ACWRCasingActor::StopSimulatingPhysics()
{
	FP->SetSimulatePhysics(false);
	TP->SetSimulatePhysics(false);

	if (HasAuthority())
	{
		SetReplicateMovement(false);

		FTimerHandle TimerHandle;
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{	
			Destroy();
		}), 4.0f, false);
	}
}


