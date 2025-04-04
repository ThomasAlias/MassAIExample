// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Upgrades.h>
#include "BulletHellGameInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BULLETHELLEXAMPLE_API UBulletHellGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UUpgrades> Upgrades;

	virtual void Init() override;
};
