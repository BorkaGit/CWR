// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "System/GameplayTagStack.h"
#include "Teams/CWRTeamAgentInterface.h"

#include "CWRPlayerState.generated.h"

struct FCWRVerbMessage;
class AController;
class ACWRPlayerController;
class APlayerState;
class FName;
class UAbilitySystemComponent;
class UCWRAbilitySystemComponent;
class UCWRAttributeSet;
class UCWRExperienceDefinition;
class UCWRPawnData;
class UObject;
struct FFrame;
struct FGameplayTag;

/** Defines the types of client connected */
UENUM()
enum class ECWRPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * ACWRPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class CWR_API ACWRPlayerState : public AModularPlayerState , public IAbilitySystemInterface, public ICWRTeamAgentInterface
{
	GENERATED_BODY()

public:
	
	ACWRPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "CWR|PlayerState")
	ACWRPlayerController* GetCWRPlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category = "CWR|PlayerState")
	UCWRAbilitySystemComponent* GetCWRAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	bool IsWaitingForTeamSet() const { return bIsWaitingForTeamSet; }

template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UCWRPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	//~ICWRTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnCWRTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ICWRTeamAgentInterface interface

	static const FName NAME_CWRAbilityReady;

	void SetPlayerConnectionType(ECWRPlayerConnectionType NewType);
	ECWRPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	/** Returns the Squad ID of the squad the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	void SetSquadID(int32 NewSquadID);

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "CWR|PlayerState")
	void ClientBroadcastMessage(const FCWRVerbMessage Message);

	// Gets the replicated view rotation of this player, used for spectating
	FRotator GetReplicatedViewRotation() const;

	// Sets the replicated view rotation, only valid on the server
	void SetReplicatedViewRotation(const FRotator& NewRotation);
	
	FORCEINLINE UCWRAttributeSet* GetAttributeSet() const{ return AttributeSet; }
	
private:
	void OnExperienceLoaded(const UCWRExperienceDefinition* CurrentExperience);

	bool bIsWaitingForTeamSet = true;
	
protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UCWRPawnData> PawnData;

private:
	
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "CWR|PlayerState")
	TObjectPtr<UCWRAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCWRAttributeSet> AttributeSet;

	UPROPERTY(Replicated)
	ECWRPlayerConnectionType MyPlayerConnectionType;

	UPROPERTY()
	FOnCWRTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
	int32 MySquadID;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnRep_MySquadID();
};
