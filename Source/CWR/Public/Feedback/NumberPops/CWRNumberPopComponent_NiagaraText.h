// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Feedback/NumberPops/CWRNumberPopComponent.h"

#include "CWRNumberPopComponent_NiagaraText.generated.h"

class UCWRDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class CWR_API UCWRNumberPopComponent_NiagaraText : public UCWRNumberPopComponent
{
	GENERATED_BODY()
	
public:

	UCWRNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UCWRNumberPopComponent interface
	virtual void AddNumberPop(const FCWRNumberPopRequest& NewRequest) override;
	//~End of UCWRNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UCWRDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
	
	
};
