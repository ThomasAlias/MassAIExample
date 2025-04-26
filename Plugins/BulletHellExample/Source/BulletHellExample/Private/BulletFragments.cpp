// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletFragments.h"

#include "MassEntityTemplateRegistry.h"
#include <BulletHellGameInstance.h>


void UBulletTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment(FConstStructView::Make(BulletFragment));
	BuildContext.AddTag<FBulletTag>();

	if (UBulletHellGameInstance* GI = Cast<UBulletHellGameInstance>(World.GetGameInstance())) //TODO OPTI : Always add fragment even if no crit, because add all fragment existing in spell data table.
	{
		if (UUpgrades* Upgrades = GI->UpgradesInstance)
		{
			const TMap<EWeaponStatType, float>& Stats = Upgrades->FinalStats;

			if (Stats.Contains(EWeaponStatType::Chain))
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletChainFragment));
			}
			if (Stats.Contains(EWeaponStatType::Pierce))
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletPierceFragment));
			}

				BuildContext.AddFragment(FConstStructView::Make(BulletDamageFragment));

			if (Stats.Contains(EWeaponStatType::CritChance))
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletCritChanceFragment));
			}
			if (Stats.Contains(EWeaponStatType::CritDamage))
			{
				BuildContext.AddFragment(FConstStructView::Make(BulletCritDamageFragment));
			}
		}
	}
}



