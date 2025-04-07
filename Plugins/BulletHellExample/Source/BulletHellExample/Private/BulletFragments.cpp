// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletFragments.h"

#include "MassEntityTemplateRegistry.h"
#include <BulletHellGameInstance.h>


void UBulletTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment(FConstStructView::Make(BulletFragment));
	BuildContext.AddTag<FBulletTag>();
	if (UBulletHellGameInstance* GI = Cast<UBulletHellGameInstance>(World.GetGameInstance()))
	{
		if (UUpgrades* Upgrades = GI->Upgrades)
		{
			if (Upgrades->ChainLevel > 0)
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletChainFragment));
			}
			if (Upgrades->PierceLevel > 0)
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletPierceFragment));
			}
		}
	}
}


