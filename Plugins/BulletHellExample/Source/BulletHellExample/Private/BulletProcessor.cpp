// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProcessor.h"

#include "BulletFragments.h"
#include "BulletHellSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassSignalSubsystem.h"

#include "BHEnemyFragments.h"
#include <BulletHellGameInstance.h>

void UBulletInitializerProcessor::ConfigureQueries()
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite); 
	EntityQuery.AddRequirement<FBulletPierceFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddSharedRequirement<FBulletPierceUpgradeFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
	EntityQuery.RegisterWithProcessor(*this);
	
}

void UBulletInitializerProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, BulletHell::Signals::BulletSpawned);
}

void UBulletInitializerProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
                                                 FMassSignalNameLookup& EntitySignals)
{


	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
	{
		auto SignalSubsystem = Context.GetMutableSubsystem<UMassSignalSubsystem>();
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();
		auto VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
		auto TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();

		const FBulletPierceUpgradeFragment& SharedPierceUpgrade = Context.GetSharedFragment<FBulletPierceUpgradeFragment>();
		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();

		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& BulletFragment = BulletFragments[EntityIdx];
			auto& VelocityFragment = VelocityFragments[EntityIdx];
			auto& TransformFragment = TransformFragments[EntityIdx];

			VelocityFragment.Value = BulletFragment.Direction.GetSafeNormal() * BulletFragment.Speed;
			TransformFragment.GetMutableTransform().SetLocation(BulletFragment.SpawnLocation);

			if (UWorld* World = Context.GetWorld())
			{
				if (UBulletHellGameInstance* GI = Cast<UBulletHellGameInstance>(World->GetGameInstance()))
				{
					if (UUpgrades* Upgrades = GI->Upgrades)
					{
						BulletPierceFragments[EntityIdx].RemainingPierce = Upgrades->PierceLevel; //SharedPierceUpgrade.UpgradePierceLevel
					}
				}
			}
			SignalSubsystem->DelaySignalEntityDeferred(Context, BulletHell::Signals::BulletDestroy, Context.GetEntity(EntityIdx), BulletFragment.Lifetime);
		}
	});
}

void UBulletDestroyerProcessor::ConfigureQueries()
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.RegisterWithProcessor(*this);
}

void UBulletDestroyerProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, BulletHell::Signals::BulletDestroy);
}

void UBulletDestroyerProcessor::SignalEntities(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
	FMassSignalNameLookup& EntitySignals)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			Context.Defer().DestroyEntity(Context.GetEntity(EntityIdx));
		}
	});
}

void UBulletCollisionProcessor::ConfigureQueries()
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FBulletPierceFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddSubsystemRequirement<UBulletHellSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.RegisterWithProcessor(*this);
}

void UBulletCollisionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager](FMassExecutionContext& Context)
	{
		auto BulletHellSubsystem = Context.GetSubsystem<UBulletHellSubsystem>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& TransformFragment = TransformFragments[EntityIdx];
			FBulletPierceFragment* BulletPierceFragment = EntityManager.GetFragmentDataPtr<FBulletPierceFragment>(Context.GetEntity(EntityIdx));
			auto Location = TransformFragment.GetTransform().GetLocation();
			
			TArray<FMassEntityHandle> Entities;
			BulletHellSubsystem->GetHashGrid().Query(FBox::BuildAABB(Location, FVector(50.f)), Entities);

			Entities = Entities.FilterByPredicate([&Location, &EntityManager](const FMassEntityHandle& Entity)
			{
				auto EntityLocation = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity)->GetTransform().GetLocation();
				return FVector::Dist(Location, EntityLocation) <= 50.f;
			});

			if (BulletPierceFragment)
			{
				Entities = Entities.FilterByPredicate([BulletPierceFragment](const FMassEntityHandle& EnemyEntity)
					{
						return !BulletPierceFragment->AlreadyPiercedEntities.Contains(EnemyEntity);// Only keep enemies that haven't already been pierced.
					});
			}

			TArray<FMassEntityHandle> EntitiesToDestroy;
			if (Entities.Num() > 0 && BulletPierceFragment) 
			{
				BulletPierceFragment->RemainingPierce = BulletPierceFragment->RemainingPierce-Entities.Num();
				if (BulletPierceFragment->RemainingPierce +1 <= 0) 
				{
					EntitiesToDestroy.Add(Context.GetEntity(EntityIdx));//destroy the bullet
				}
			}
			if (Entities.Num() > 0 && !BulletPierceFragment)
			{
				EntitiesToDestroy.Add(Context.GetEntity(EntityIdx));//destroy the bullet
			}

			for (FMassEntityHandle& EnemyEntity : Entities)
			{
				FBHEnemyFragment* EnemyFragment = EntityManager.GetFragmentDataPtr<FBHEnemyFragment>(EnemyEntity);

				if (EnemyFragment)
				{
					EnemyFragment->Health -= 1;
					if (EnemyFragment->Health <= 0)
					{
						EntitiesToDestroy.Add(EnemyEntity);
					}
				}
			}
				Context.Defer().DestroyEntities(EntitiesToDestroy);
		}
	});
}
