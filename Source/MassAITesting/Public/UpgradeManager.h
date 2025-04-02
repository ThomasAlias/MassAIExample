// Fill out your copyright notice in the Description page of Project Settings.


//TODO Delete this class
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UpgradeManager.generated.h"

UCLASS()
class MASSAITESTING_API AUpgradeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUpgradeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
