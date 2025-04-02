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
};

USTRUCT()
struct FBulletTag : public FMassTag
{
	GENERATED_BODY()
	
};

UCLASS()
class UBulletTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	FBulletFragment BulletFragment;
};

USTRUCT()
struct FBulletPierceFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float RemainingPierce = 1.f;

	UPROPERTY()
	TArray<FMassEntityHandle> AlreadyPiercedEntities; //Used to prevent multiHit one same target
};

USTRUCT(BlueprintType)
struct  FBulletPierceUpgradeFragment : public FMassSharedFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pierce Upgrade")
	int32 UpgradePierceLevel = 0;
};

UCLASS()
class UBulletPierceTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY()//Change ReadWrite?
	FBulletPierceFragment BulletPierceFragment;
	UPROPERTY(EditAnywhere)
	FBulletPierceUpgradeFragment BulletPierceUpgradeFragment;
};