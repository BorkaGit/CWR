// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRInteractionComponent.h"

#include "Character/InteractionInterface.h"
#include "Physics/CWRCollisionChannels.h"
#include "UI/CWRWorldUserWidget.h"

static TAutoConsoleVariable CVarDebugDrawInteraction(TEXT("cwr.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Internal Component."), ECVF_Cheat);

UCWRInteractionComponent::UCWRInteractionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCWRInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

//	if ( !bCurrentlyInteracting )
	{
		FindBestInteractable();
	}
}

void UCWRInteractionComponent::PrimaryInteract()
{
	if (!FocusedActor)
	{
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	//bCurrentlyInteracting = true;
	IInteractionInterface::Execute_Interact(FocusedActor, MyPawn);

	if (DefaultWidgetInstance)
		DefaultWidgetInstance->RemoveFromParent();
}

void UCWRInteractionComponent::EndInteract()
{
	if ( bCurrentlyInteracting )
	{
		bCurrentlyInteracting = false;
		IInteractionInterface::Execute_StopInteraction(FocusedActor);
	}
}

void UCWRInteractionComponent::FindBestInteractable()
{
	const bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	
	ObjectQueryParams.AddObjectTypesToQuery(CWR_TraceChannel_Interaction);
	
	FVector EyeLocation;
	FRotator EyeRotation;
	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector End  = EyeLocation + (EyeRotation.Vector() * TraceDistance);
	
	TArray<FHitResult> Hits;
	
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams,Shape );

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	FocusedActor = nullptr;
	
	for (FHitResult Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint,TraceRadius,32,LineColor,false,2.0f );
		}
		
		if ( AActor* HitActor = Hit.GetActor() )
		{
			if (HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				FocusedActor = HitActor;
				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (!DefaultWidgetInstance && DefaultWidgetClass)
		{
			DefaultWidgetInstance = CreateWidget<UCWRWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->SetAttachedActor(FocusedActor);

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(),EyeLocation,End,LineColor, false, 2.0f, 0, 2.0f);
	}
}
