// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletHellGameInstance.h"

void UBulletHellGameInstance::Init()
{
	Super::Init();

	// Instantiate your Upgrades object
	Upgrades = NewObject<UUpgrades>(this);
}