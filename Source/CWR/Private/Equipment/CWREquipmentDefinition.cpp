// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/CWREquipmentDefinition.h"
#include "Equipment/CWREquipmentInstance.h"


UCWREquipmentDefinition::UCWREquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UCWREquipmentInstance::StaticClass();
}

