// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonPlayerController.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Teams/CWRTeamAgentInterface.h"
#include "CWRPlayerController.generated.h"


class UCWRSettingsShared;
class ACWRPlayerState;
class ACWRHUD;
class UCWRWeaponStateComponent;
class UCWRInventoryManagerComponent;
class UCWRInputConfig;
class UCWRInputComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCWRAbilitySystemComponent;

UCLASS()
class CWR_API ACWRPlayerController : public ACommonPlayerController, public ICWRTeamAgentInterface
{
	GENERATED_BODY()
	
public:

	ACWRPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "CWR|PlayerController")
	ACWRPlayerState* GetCWRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "CWR|PlayerController")
	UCWRAbilitySystemComponent* GetCWRAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CWR|PlayerController")
	ACWRHUD* GetCWRHUD() const;
	
	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
	
	//~ICWRTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnCWRTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ICWRTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "CWR|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "CWR|Character")
	bool GetIsAutoRunning() const;
	
protected:

	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

	void OnSettingsChanged(UCWRSettingsShared* Settings);
	
	void OnStartAutoRun();
	void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
	
	virtual void Tick(float DeltaSeconds) override;

	//virtual void SetupInputComponent() override;
	
	void Move(const FInputActionValue& Value);
	
	void LookMouse(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCWRWeaponStateComponent> WeaponStateComponent;
	
private:

	UPROPERTY()
	UCWRInputComponent* CWRInputComponent = nullptr;
	
	/*UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CWRContext = nullptr;*/

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	/*UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UCWRInputConfig> InputConfig = nullptr;*/
	
	UCWRAbilitySystemComponent* GetAbilitySystemComponent();
	
	UPROPERTY()
	TObjectPtr<UCWRAbilitySystemComponent> CWRAbilitySystemComponent;


	UPROPERTY()
	FOnCWRTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	void BroadcastOnPlayerStateChanged();
};
