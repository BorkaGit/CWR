// Fill out your copyright notice in the Description page of Project Settings.


#include "Teams/CWRTeamSpawnPoint.h"

#include "Character/CWRCharacter_Base.h"
#include "Components/SphereComponent.h"
#include "Player/CWRPlayerStart.h"
#include "Teams/CWRTeamDisplayAsset.h"
#include "Teams/CWRTeamSubsystem.h"

TAutoConsoleVariable<bool> CVarDrawDebugTeamSpawnPointsPlayerCount(TEXT("CWR.DrawDebug.TeamSpawnPointsPlayerCount"), false, TEXT("Draw Team Spawn Point Player Count in format { Team1's players, Team2's players... }"), ECVF_Cheat);
TAutoConsoleVariable<bool> CVarDrawDebugTeamSpawnPointsRadius(TEXT("CWR.DrawDebug.TeamSpawnPointsRadius"), false, TEXT("Draw Team Spawn Points Radius"), ECVF_Cheat);

ACWRTeamSpawnPoint::ACWRTeamSpawnPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(1500.f);
	SphereComponent->SetVisibility(false);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ACWRTeamSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
}

void ACWRTeamSpawnPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CVarDrawDebugTeamSpawnPointsRadius.GetValueOnAnyThread())
	{
		DrawDebugSphere(GetWorld(), SphereComponent->GetComponentLocation(), SphereComponent->GetScaledSphereRadius(), 32, TeamColor, false, DeltaSeconds);
	}
	if (CVarDrawDebugTeamSpawnPointsPlayerCount.GetValueOnAnyThread())
	{
		for ( int i = 0; i < PlayerCountNearPoint.Num(); ++i )
		{
			DrawDebugString(GetWorld(), SphereComponent->GetComponentLocation() + FVector(0.f, 0.f, 100.f + i * 100.f), FString::Printf(TEXT("[ %d : %d ]"),PlayerCountNearPoint[i].Key, PlayerCountNearPoint[i].Value), nullptr, FColor::Purple, DeltaSeconds,true );
		}
	}
}


ACWRPlayerStart* ACWRTeamSpawnPoint::FindUnclaimedPlayerStart()
{
	for ( TSoftObjectPtr<ACWRPlayerStart> PlayerStartSoft : AssociatedPlayerStarts )
	{
		 if ( ACWRPlayerStart* PlayerStart = PlayerStartSoft.Get()  )
		 {
			 if ( !PlayerStart->IsClaimed() )
			 {
			 	return PlayerStart;
			 }
		 }
	}

	const auto PlayerStart = GetWorld()->SpawnActor<ACWRPlayerStart>(GetActorLocation(), GetActorRotation());
	const TSoftObjectPtr<ACWRPlayerStart> SoftPlayerStart(PlayerStart);
	AssociatedPlayerStarts.Add(SoftPlayerStart);
	return PlayerStart;
}

void ACWRTeamSpawnPoint::CapturePoint(const int32 TeamId)
{
	if ( !bWasEverCaptured )
	{
		bWasEverCaptured = true;
	}
	
	CurrentTeamId = TeamId;

	if (UCWRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UCWRTeamSubsystem>())
	{
		UCWRTeamDisplayAsset* TeamDisplayAsset = TeamSubsystem->GetTeamDisplayAsset(CurrentTeamId, INDEX_NONE);
		if (const FLinearColor* LinearColor = TeamDisplayAsset->ColorParameters.Find(FName("TeamColor")))
		{
			TeamColor = LinearColor->ToFColorSRGB();
		}
	}
}

void ACWRTeamSpawnPoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ProcessActorOverlaps(OtherActor, true);
}

void ACWRTeamSpawnPoint::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ProcessActorOverlaps(OtherActor, false);
}

void ACWRTeamSpawnPoint::ProcessActorOverlaps(AActor* OtherActor, bool bIsInside)
{
	if (!OtherActor)
	{
		return;
	}

	ACWRCharacter_Base* CWRCharacter = Cast<ACWRCharacter_Base>(OtherActor);
	if (!CWRCharacter)
	{
		return;
	}

	const UCWRTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UCWRTeamSubsystem>();
	if (!ensure(TeamSubsystem)) // Ensure subsystem exists
	{
		return;
	}
	
	auto CharacterTeamSet = [this, TeamSubsystem, CWRCharacter, bIsInside]()
	{
		int32 CharacterTeamId = TeamSubsystem->FindTeamFromObject(CWRCharacter);
		
		TPair<int32, int32>* FoundTeam = Algo::FindByPredicate(PlayerCountNearPoint, 
[CharacterTeamId](const TPair<int32, int32>& Pair) 
{
	return Pair.Key == CharacterTeamId;
});

		if (FoundTeam)
		{
			bIsInside ? ++FoundTeam->Value : --FoundTeam->Value;
		}
		else
		{
			PlayerCountNearPoint.Emplace(CharacterTeamId, 1);

			if (!bIsInside)
				UE_LOG(LogTemp, Warning, TEXT("Tried to remove Player from TeamSpawnPoint's PlayerList but there was no his team"));
		}
	
		RecalculateCapture(CharacterTeamId);
	};
		
	if (CWRCharacter->IsWaitingForTeamSet())
		CWRCharacter->OnTeamSetDelegate.AddWeakLambda(this, CharacterTeamSet);
	else
		CharacterTeamSet();
}

void ACWRTeamSpawnPoint::RecalculateCapture(const int32 NewPlayerTeamId)
{
	if ( PlayerCountNearPoint.IsEmpty() )
	{
		return;
	}

	int32 MaxPlayerNum = PlayerCountNearPoint[0].Value;
	int32 MaxTeamId = PlayerCountNearPoint[0].Key;
	
	for ( const TPair<int32, int32>& Pair :  PlayerCountNearPoint )
	{
		if (Pair.Value > MaxPlayerNum)
		{
			MaxPlayerNum = Pair.Value;   
			MaxTeamId = Pair.Key;      
		}
	}

	if ( MaxPlayerNum == 0 )
	{
		GetWorldTimerManager().SetTimer(UnCaptureTimerHandle, this, &ThisClass::UnCapturePoint,TimeToUnCapture,false);
		return;
	}

	if ( NewPlayerTeamId == CurrentTeamId)
	{
		GetWorldTimerManager().ClearTimer(UnCaptureTimerHandle);
	}

	if ( MaxTeamId != CurrentTeamId )
	{
		CapturePoint(MaxTeamId);
	}
}

void ACWRTeamSpawnPoint::UnCapturePoint()
{
	PlayerCountNearPoint.Empty();
	CurrentTeamId = INDEX_NONE;
}

