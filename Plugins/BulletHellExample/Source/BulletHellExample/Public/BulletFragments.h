// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "BulletFragments.generated.h"



USTRUCT()
struct FBulletFragment : public FMassFragment
{
	GENERATED_BODY()

	FVector SpawnLocation;
	
	FVector Direction;

	UPROPERTY(EditAnywhere)
	float Speed = 500.f;

	UPROPERTY(EditAnywhere)
	float Lifetime = 5.f;

	UPROPERTY()
	TArray<FMassEntityHandle> AlreadyHitEntities;
};

USTRUCT()
struct FBulletTag : public FMassTag
{
	GENERATED_BODY()
	
};

USTRUCT()
struct FBulletChainFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float RemainingChain = 1.f;

	UPROPERTY()
	TArray<FMassEntityHandle> AlreadyChainedEntities; //Used to prevent multiHit one same target //TODO DEPRECATE
};



USTRUCT()
struct FBulletPierceFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float RemainingPierce = 1.f;

	UPROPERTY()
	TArray<FMassEntityHandle> AlreadyPiercedEntities; //Used to prevent multiHit one same target // TODO DEPRECATE
};

UCLASS()
class UBulletTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	FBulletFragment BulletFragment;
	//------------------------------------------------------------------Damage upgrades------------------------------------------------------

	//------------------------------------------------------------------Speed upgrades------------------------------------------------------

	//------------------------------------------------------------------Multi hit upgrades------------------------------------------------

	UPROPERTY()
	FBulletChainFragment BulletChainFragment;
	UPROPERTY()//Change ReadWrite?
		FBulletPierceFragment BulletPierceFragment;
};