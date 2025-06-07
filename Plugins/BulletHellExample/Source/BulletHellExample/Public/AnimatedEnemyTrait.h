// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletHellSubsystem.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "Mass/Animation/VertexAnimProcessor.h"


#include "AnimatedEnemyTrait.generated.h"



UCLASS(meta = (DisplayName = "Animated Enemy"))
class BULLETHELLEXAMPLE_API UAnimatedEnemyTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:

	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(Category = "Bullet Hell", EditAnywhere)
	FVertexAnimInfoFragment VertexAnimInfoFragment;
};
