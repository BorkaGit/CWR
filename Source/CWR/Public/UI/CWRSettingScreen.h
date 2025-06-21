// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "Widgets/GameSettingScreen.h"

#include "CWRSettingScreen.generated.h"

class UGameSettingRegistry;
class UCWRTabListWidgetBase;
class UObject;

UCLASS(Abstract, meta = (Category = "Settings", DisableNativeTick))
class CWR_API UCWRSettingScreen : public UGameSettingScreen
{
	GENERATED_BODY()
	
	public:
    
    protected:
    	virtual void NativeOnInitialized() override;
    	virtual UGameSettingRegistry* CreateRegistry() override;
    
    	void HandleBackAction();
    	void HandleApplyAction();
    	void HandleCancelChangesAction();
    
    	virtual void OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty) override;
    	
    protected:
    	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true))
    	TObjectPtr<UCWRTabListWidgetBase> TopSettingsTabs;
    	
    	UPROPERTY(EditDefaultsOnly)
    	FDataTableRowHandle BackInputActionData;
    
    	UPROPERTY(EditDefaultsOnly)
    	FDataTableRowHandle ApplyInputActionData;
    
    	UPROPERTY(EditDefaultsOnly)
    	FDataTableRowHandle CancelChangesInputActionData;
    
    	FUIActionBindingHandle BackHandle;
    	FUIActionBindingHandle ApplyHandle;
    	FUIActionBindingHandle CancelChangesHandle;
	
	
};
