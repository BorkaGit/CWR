// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CWRPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "CommonActivatableWidget.h"
#include "CommonInputSubsystem.h"
#include "CWRGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "CWR/CWRCharacter_Player.h"
#include "Development/CWRDeveloperSettings.h"
#include "GameFramework/Character.h"
#include "Input/CWRInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/CWRLocalPlayer.h"
#include "Player/CWRPlayerState.h"
#include "Settings/CWRSettingsShared.h"
#include "UI/HUD/CWRHUD.h"
#include "UI/Weapons/CWRCustomizationPreview.h"
#include "Weapons/CWRAttachmentActor.h"
#include "Weapons/CWRWeaponStateComponent.h"

namespace CWR
{
	namespace Input
	{
		static int32 ShouldAlwaysPlayForceFeedback = 0;
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("CWRPC.ShouldAlwaysPlayForceFeedback"),
			ShouldAlwaysPlayForceFeedback,
			TEXT("Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}

ACWRPlayerController::ACWRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ACWRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ACWRPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ACWRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*check(CWRContext);

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedSubsystem->AddMappingContext(CWRContext, 0);
	}*/

/*#if WITH_RPC_REGISTRY
	FHttpServerModule::Get().StartAllListeners();
	int32 RpcPort = 0;
	if (FParse::Value(FCommandLine::Get(), TEXT("rpcport="), RpcPort))
	{
		UCWRGameplayRpcRegistrationComponent* ObjectInstance = UCWRGameplayRpcRegistrationComponent::GetInstance();
		if (ObjectInstance && ObjectInstance->IsValidLowLevel())
		{
			ObjectInstance->RegisterAlwaysOnHttpCallbacks();
			ObjectInstance->RegisterInMatchHttpCallbacks();
		}
	}
#endif*/
	SetActorHiddenInGame(false);
}

void ACWRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACWRPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Disable replicating the PC target view as it doesn't work well for replays or client-side spectating.
	// The engine TargetViewRotation is only set in APlayerController::TickActor if the server knows ahead of time that 
	// a specific pawn is being spectated and it only replicates down for COND_OwnerOnly.
	// In client-saved replays, COND_OwnerOnly is never true and the target pawn is not always known at the time of recording.
	// To support client-saved replays, the replication of this was moved to ReplicatedViewRotation and updated in PlayerTick.
	DISABLE_REPLICATED_PROPERTY(APlayerController, TargetViewRotation);
}

void ACWRPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ACWRPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	if (GetIsAutoRunning())
	{
		if (APawn* CurrentPawn = GetPawn())
		{
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);	
		}
	}

	ACWRPlayerState* CWRPlayerState = GetCWRPlayerState();

	if (PlayerCameraManager && CWRPlayerState)
	{
		APawn* TargetPawn = PlayerCameraManager->GetViewTargetPawn();

		if (TargetPawn)
		{
			// Update view rotation on the server so it replicates
			if (HasAuthority() || TargetPawn->IsLocallyControlled())
			{
				CWRPlayerState->SetReplicatedViewRotation(TargetPawn->GetViewRotation());
			}

			// Update the target view rotation if the pawn isn't locally controlled
			if (!TargetPawn->IsLocallyControlled())
			{
				CWRPlayerState = TargetPawn->GetPlayerState<ACWRPlayerState>();
				if (CWRPlayerState)
				{
					// Get it from the spectated pawn's player state, which may not be the same as the PC's playerstate
					TargetViewRotation = CWRPlayerState->GetReplicatedViewRotation();
				}
			}
		}
	}
}

ACWRPlayerState* ACWRPlayerController::GetCWRPlayerState() const
{
	return CastChecked<ACWRPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UCWRAbilitySystemComponent* ACWRPlayerController::GetCWRAbilitySystemComponent() const
{
	const ACWRPlayerState* CWRPlayerState = GetCWRPlayerState();
	return (CWRPlayerState ? CWRPlayerState->GetCWRAbilitySystemComponent() : nullptr);
}

ACWRHUD* ACWRPlayerController::GetCWRHUD() const
{
	return CastChecked<ACWRHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void ACWRPlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void ACWRPlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implemet without having to hook all the other events
}

void ACWRPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState != nullptr)
	{
		if (ICWRTeamAgentInterface* PlayerStateTeamInterface = Cast<ICWRTeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		}
	}

	// Bind to the new player state, if any
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState != nullptr)
	{
		if (ICWRTeamAgentInterface* PlayerStateTeamInterface = Cast<ICWRTeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}

	// Broadcast the team change (if it really has)
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}

void ACWRPlayerController::ROC_RemoveAllPrimaryDefaultParts_Implementation()
{
	if ( PreviewStock )
	{
		PreviewStock->Destroy();
	}
	if ( PreviewPistolGrip )
	{
		PreviewPistolGrip->Destroy();
	}
	if ( PreviewMuzzle )
	{
		PreviewMuzzle->Destroy();
	}
	if ( PreviewFrontSight )
	{
		PreviewFrontSight->Destroy();
	}
	if ( PreviewRearSight )
	{
		PreviewRearSight->Destroy();
	}
	if ( PreviewHandle )
	{
		PreviewHandle->Destroy();
	}
}

void ACWRPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void ACWRPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void ACWRPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

void ACWRPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UCWRLocalPlayer* CWRLocalPlayer = Cast<UCWRLocalPlayer>(InPlayer))
	{
		UCWRSettingsShared* UserSettings = CWRLocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void ACWRPlayerController::AddCheats(bool bForce)
{
/*#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER*/
}

void ACWRPlayerController::ServerCheat_Implementation(const FString& Msg)
{/*
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogCWR, Warning, TEXT("ServerCheat: %s"), *Msg);
		ClientMessage(ConsoleCommand(Msg));
	}
#endif // #if USING_CHEAT_MANAGER*/
}

bool ACWRPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void ACWRPlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
/*#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogCWR, Warning, TEXT("ServerCheatAll: %s"), *Msg);
		for (TActorIterator<ACWRPlayerController> It(GetWorld()); It; ++It)
		{
			ACWRPlayerController* CWRPC = (*It);
			if (CWRPC)
			{
				CWRPC->ClientMessage(CWRPC->ConsoleCommand(Msg));
			}
		}
	}
#endif // #if USING_CHEAT_MANAGER*/
}

bool ACWRPlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	return true;
}

void ACWRPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ACWRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent())
	{
		CWRASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ACWRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

#if WITH_SERVER_CODE && WITH_EDITOR
	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
	{
		for (const FCWRCheatToRun& CheatRow : GetDefault<UCWRDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
			{
				ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	SetIsAutoRunning(false);
}

void ACWRPlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool ACWRPlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent())
	{
	//	bIsAutoRunning = CWRASC->GetTagCount(CWRGameplayTags::Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void ACWRPlayerController::OnStartAutoRun()
{
	if (UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent())
	{
//		CWRASC->SetLooseGameplayTagCount(CWRGameplayTags::Status_AutoRunning, 1);
		K2_OnStartAutoRun();
	}	
}

void ACWRPlayerController::OnEndAutoRun()
{
	if (UCWRAbilitySystemComponent* CWRASC = GetCWRAbilitySystemComponent())
	{
//		CWRASC->SetLooseGameplayTagCount(CWRGameplayTags::Status_AutoRunning, 0);
		K2_OnEndAutoRun();
	}
}

void ACWRPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (CWR::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}
	
	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void ACWRPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				for (UPrimitiveComponent* Comp : InComponents)
				{
					if (Comp->IsRegistered())
					{
						OutHiddenComponents.Add(Comp->GetPrimitiveSceneId());

						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
							{
								OutHiddenComponents.Add(AttachChildPC->GetPrimitiveSceneId());
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}

void ACWRPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(LogTemp, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId ACWRPlayerController::GetGenericTeamId() const
{
	if (const ICWRTeamAgentInterface* PSWithTeamInterface = Cast<ICWRTeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnCWRTeamIndexChangedDelegate* ACWRPlayerController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ACWRPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

void ACWRPlayerController::OnSettingsChanged(UCWRSettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}
/*
void ACWRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	CWRInputComponent = CastChecked<UCWRInputComponent>(InputComponent);

	CWRInputComponent->BindNativeAction(InputConfig,FCWRGameplayTags::Get().InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move, false );
	CWRInputComponent->BindNativeAction(InputConfig,FCWRGameplayTags::Get().InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::LookMouse, false );
	
	CWRInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}*/

void ACWRPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), MovementVector.Y);
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), MovementVector.X);
	}
}

void ACWRPlayerController::LookMouse(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACWRPlayerController::ROC_SpawnPrimaryCustomizationWeapon_Implementation()
{
	if ( !CustomizableWeapon )
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		CustomizableWeapon = GetWorld()->SpawnActor<ACWRCustomizationPreview>(CustomizationPreviewClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
}

void ACWRPlayerController::CreatePrimaryCustomizationWeapon()
{
	if ( !CustomizationWidget )
	{
		CustomizationWidget = NewObject<UCommonActivatableWidget>(GetWorld(), CustomizationWidgetClass);
		CustomizationWidget->AddToViewport();
	}

	SetViewTargetWithBlend(CustomizableWeapon, 0.25f);
}

void ACWRPlayerController::SwapPreviewPrimaryDefaultParts(TSubclassOf<ACWRAttachmentActor> InPreviewStock, TSubclassOf<ACWRAttachmentActor> InPreviewPistolGrip, TSubclassOf<ACWRAttachmentActor> InPreviewMuzzle, TSubclassOf<ACWRAttachmentActor> InPreviewFrontSight, TSubclassOf<ACWRAttachmentActor> InPreviewRearSight,TSubclassOf<ACWRAttachmentActor> InPreviewHandGuard)
{
	ROC_RemoveAllPrimaryDefaultParts();

	if ( InPreviewStock )
	{
		PreviewStock = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewStock, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewStock->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_Stock");
		PreviewStock->SetReplicates(false);
	}

	if ( InPreviewPistolGrip )
	{
		PreviewPistolGrip = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewPistolGrip, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewPistolGrip->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_Grip");
		PreviewPistolGrip->SetReplicates(false);
	}

	if ( InPreviewMuzzle )
	{
		PreviewMuzzle = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewMuzzle, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewMuzzle->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_Muzzle");
		PreviewMuzzle->SetReplicates(false);
	}

	if ( InPreviewFrontSight )
	{
		PreviewFrontSight = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewFrontSight, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewFrontSight->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_FrontSight_1");
		PreviewFrontSight->SetReplicates(false);
	}

	if ( InPreviewRearSight )
	{
		PreviewRearSight = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewRearSight, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewRearSight->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_RearSight_1");
		PreviewRearSight->SetReplicates(false);
	}

	if ( InPreviewHandGuard )
	{
		PreviewHandle = GetWorld()->SpawnActor<ACWRAttachmentActor>(InPreviewHandGuard, FVector::ZeroVector, FRotator::ZeroRotator);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
		TransformRules.bWeldSimulatedBodies = true;
		PreviewHandle->AttachToComponent(CustomizableWeapon->GetWeaponMesh(), TransformRules, "Base_Handle");
		PreviewHandle->SetReplicates(false);
	}
}

void ACWRPlayerController::ROC_PreparePrimaryWeapon_Implementation(USkeletalMesh* NewMesh, ESlateVisibility InVisibility, bool bUnselectWeaponList)
{
	CustomizableWeapon->GetWeaponMesh()->SetSkeletalMesh(NewMesh);
}

void ACWRPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	
}

void ACWRPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}

void ACWRPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}

UCWRAbilitySystemComponent* ACWRPlayerController::GetAbilitySystemComponent()
{
	if (!CWRAbilitySystemComponent)
	{
	//	CWRAbilitySystemComponent = Cast<UCWRAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		if (const auto CWRCharacter = Cast<ACWRCharacter_Player>(GetCharacter()))
		{
			CWRAbilitySystemComponent = Cast<UCWRAbilitySystemComponent>(CWRCharacter->GetAbilitySystemComponent());
		}
	}
	return CWRAbilitySystemComponent;
}
