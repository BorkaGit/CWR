// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CWRHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "EnhancedInputSubsystems.h"
#include "Player/CWRPlayerController.h"
#include "Player/CWRPlayerState.h"
#include "Character/CWRPawnExtensionComponent.h"
#include "Character/CWRPawnData.h"
#include "AbilitySystem/CWRAbilitySystemComponent.h"
#include "Input/CWRInputConfig.h"
#include "Input/CWRInputComponent.h"
#include "CWRGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "Player/CWRLocalPlayer.h"


#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace CWRHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UCWRHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UCWRHeroComponent::NAME_ActorFeatureName("Hero");

UCWRHeroComponent::UCWRHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void UCWRHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogTemp, Error, TEXT("[UCWRHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("CWRHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("CWRHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UCWRHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == FCWRGameplayTags::Get().InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == FCWRGameplayTags::Get().InitState_Spawned && DesiredState == FCWRGameplayTags::Get().InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ACWRPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ACWRPlayerController* CWRPC = GetController<ACWRPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !CWRPC || !CWRPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == FCWRGameplayTags::Get().InitState_DataAvailable && DesiredState == FCWRGameplayTags::Get().InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ACWRPlayerState* CWRPS = GetPlayerState<ACWRPlayerState>();

		return CWRPS && Manager->HasFeatureReachedInitState(Pawn, UCWRPawnExtensionComponent::NAME_ActorFeatureName, FCWRGameplayTags::Get().InitState_DataInitialized);
	}
	else if (CurrentState == FCWRGameplayTags::Get().InitState_DataInitialized && DesiredState == FCWRGameplayTags::Get().InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UCWRHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == FCWRGameplayTags::Get().InitState_DataAvailable && DesiredState == FCWRGameplayTags::Get().InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ACWRPlayerState* CWRPS = GetPlayerState<ACWRPlayerState>();
		if (!ensure(Pawn && CWRPS))
		{
			return;
		}

		const UCWRPawnData* PawnData = nullptr;

		if (UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UCWRPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(CWRPS->GetCWRAbilitySystemComponent(), CWRPS);
		}

		if (ACWRPlayerController* CWRPC = GetController<ACWRPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

	}
}

void UCWRHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UCWRPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FCWRGameplayTags::Get().InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UCWRHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { FCWRGameplayTags::Get().InitState_Spawned, FCWRGameplayTags::Get().InitState_DataAvailable, FCWRGameplayTags::Get().InitState_DataInitialized, FCWRGameplayTags::Get().InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UCWRHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UCWRPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(FCWRGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UCWRHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UCWRHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UCWRLocalPlayer* LP = Cast<UCWRLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UCWRPawnData* PawnData = PawnExtComp->GetPawnData<UCWRPawnData>())
		{
			if (const UCWRInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The CWR Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UCWRInputComponent or modify this component accordingly.
				UCWRInputComponent* CWRIC = Cast<UCWRInputComponent>(PlayerInputComponent);
				if (ensureMsgf(CWRIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCWRInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					CWRIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					CWRIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
					
					CWRIC->BindNativeAction(InputConfig, FCWRGameplayTags::Get().InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					CWRIC->BindNativeAction(InputConfig, FCWRGameplayTags::Get().InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
//					CWRIC->BindNativeAction(InputConfig, CWRGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
//					CWRIC->BindNativeAction(InputConfig, CWRGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
//					CWRIC->BindNativeAction(InputConfig, CWRGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
					
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UCWRHeroComponent::AddAdditionalInputConfig(const UCWRInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UCWRInputComponent* CWRIC = Pawn->FindComponentByClass<UCWRInputComponent>();
		if (ensureMsgf(CWRIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCWRInputComponent or a subclass of it.")))
		{
			CWRIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UCWRHeroComponent::RemoveAdditionalInputConfig(const UCWRInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UCWRHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UCWRHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UCWRAbilitySystemComponent* CWRASC = PawnExtComp->GetCWRAbilitySystemComponent())
			{
				CWRASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UCWRHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UCWRPawnExtensionComponent* PawnExtComp = UCWRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UCWRAbilitySystemComponent* CWRASC = PawnExtComp->GetCWRAbilitySystemComponent())
		{
			CWRASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UCWRHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	/*APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (ACWRPlayerController* CWRController = Cast<ACWRPlayerController>(Controller))
	{
		CWRController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}*/

	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), MovementVector.Y);
		ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), MovementVector.X);
	}
}

void UCWRHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	/*APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}*/

	FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if ( APawn* ControlledPawn = GetPawn<APawn>() )
	{
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void UCWRHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * CWRHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * CWRHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UCWRHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	/*
	if (ACWRCharacter* Character = GetPawn<ACWRCharacter>())
	{
		Character->ToggleCrouch();
	}*/
}

void UCWRHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (ACWRPlayerController* Controller = Cast<ACWRPlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}	
	}
}

