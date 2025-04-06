// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"

#include "CWRWeaponUserInterface.generated.h"

class UCWRWeaponInstance;
class UObject;
struct FGeometry;

UCLASS()
class CWR_API UCWRWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UCWRWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UCWRWeaponInstance* OldWeapon, UCWRWeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<UCWRWeaponInstance> CurrentInstance;
};
