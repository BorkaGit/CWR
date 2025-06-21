// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CWRAttachmentDetails.generated.h"

class UCWRInventoryItemDefinition;
class ACWRAttachmentActor;

UENUM(BlueprintType)
enum class ECWRAttachmentType : uint8
{
	Sight,
	Rail,
	PistolGrip,
	Stock,
	MuzzleAttachment,
	HandGuard,
	BaseWeapon,
	AlternateSight
};

USTRUCT(BlueprintType)
struct FCWRAttachmentDetails : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACWRAttachmentActor> AttachmentClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachmentName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> AttachmentThumbnail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachToSocketName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECWRAttachmentType AttachmentType = ECWRAttachmentType::Sight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ShortenedAttachmentName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UCWRInventoryItemDefinition>> CompatibleWeapons;
};
