// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Foundation/CWRButtonBase.h"  
#include "Weapons/CWRAttachmentDetails.h"
#include "CWRAttachmentButton.generated.h"

UCLASS()
class CWR_API UCWRAttachmentButton : public UCWRButtonBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn="true"))
	FCWRAttachmentDetails Attachment;
};
