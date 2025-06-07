// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimatedEnemyTrait.h"


#include "MassEntityTemplateRegistry.h"

void UAnimatedEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    BuildContext.AddFragment(FConstStructView::Make(VertexAnimInfoFragment));
}