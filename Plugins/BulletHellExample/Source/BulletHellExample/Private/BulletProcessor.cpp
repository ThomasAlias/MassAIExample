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
#include <BulletHellGameInstance.h>//Remove GI and place it in fragment.cpp trait buidl template

void UBulletInitializerProcessor::ConfigureQueries()
{
	EntityQuery.AddTagRequirement<FBulletTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite); 

	EntityQuery.AddRequirement<FBulletPierceFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FBulletChainFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);

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
		auto BulletChainFragments = Context.GetMutableFragmentView<FBulletChainFragment>();
		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();
		const bool bHasChain = (BulletChainFragments.GetData() != nullptr);//Ca a pas l'air d'être la "bonne" façon de faire je pense.
		const bool bHasPierce = (BulletPierceFragments.GetData() != nullptr);
		const int32 NumEntities = Context.GetNumEntities();
		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& BulletFragment = BulletFragments[EntityIdx];
			auto& VelocityFragment = VelocityFragments[EntityIdx];
			auto& TransformFragment = TransformFragments[EntityIdx];

			VelocityFragment.Value = BulletFragment.Direction.GetSafeNormal() * BulletFragment.Speed;
			TransformFragment.GetMutableTransform().SetLocation(BulletFragment.SpawnLocation);

			auto& BulletChainFragment = BulletChainFragments[EntityIdx];
			auto& BulletPierceFragment = BulletPierceFragments[EntityIdx];

			if (UWorld* World = Context.GetWorld())
			{
				if (UBulletHellGameInstance* GI = Cast<UBulletHellGameInstance>(World->GetGameInstance()))
				{
					if (UUpgrades* Upgrades = GI->Upgrades)
					{
						if (bHasChain)
						{
							BulletChainFragments[EntityIdx].RemainingChain = Upgrades->ChainLevel;
						}
						if (bHasPierce)
						{
							BulletPierceFragments[EntityIdx].RemainingPierce = Upgrades->PierceLevel;

						}
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
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadWrite);//TODO ensure we don't get write if not needed

	EntityQuery.AddRequirement<FBulletPierceFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FBulletChainFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);

	EntityQuery.AddSubsystemRequirement<UBulletHellSubsystem>(EMassFragmentAccess::ReadOnly);
	EntityQuery.RegisterWithProcessor(*this);
}

void UBulletCollisionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager](FMassExecutionContext& Context)
	{
		auto BulletHellSubsystem = Context.GetSubsystem<UBulletHellSubsystem>();
		auto TransformFragments = Context.GetFragmentView<FTransformFragment>();
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();

		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();
		auto BulletChainFragments = Context.GetMutableFragmentView<FBulletChainFragment>();

		const int32 NumEntities = Context.GetNumEntities();

		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& TransformFragment = TransformFragments[EntityIdx];
			FBulletFragment* BulletFragment = EntityManager.GetFragmentDataPtr<FBulletFragment>(Context.GetEntity(EntityIdx));

			FBulletPierceFragment* BulletPierceFragment = EntityManager.GetFragmentDataPtr<FBulletPierceFragment>(Context.GetEntity(EntityIdx));
			FBulletChainFragment* BulletChainFragment = EntityManager.GetFragmentDataPtr<FBulletChainFragment>(Context.GetEntity(EntityIdx));

			auto Location = TransformFragment.GetTransform().GetLocation();
			
			TArray<FMassEntityHandle> Entities;
			BulletHellSubsystem->GetHashGrid().Query(FBox::BuildAABB(Location, FVector(50.f)), Entities);

			Entities = Entities.FilterByPredicate([&Location, &EntityManager](const FMassEntityHandle& Entity)
			{
				auto EntityLocation = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity)->GetTransform().GetLocation();
				return FVector::Dist(Location, EntityLocation) <= 50.f;
			});

			TArray<FMassEntityHandle> ValidHitTargets;
			for (const FMassEntityHandle& Entity : Entities)
			{
				auto* EnemyTransform = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
				const float Dist = FVector::Dist(Location, EnemyTransform->GetTransform().GetLocation());

				if (BulletFragment->AlreadyHitEntities.Contains(Entity))
					continue;

				if (Dist <= 50.f)
				{
					ValidHitTargets.Add(Entity); // currently overlapping → apply damage
				}
				//else if (Dist <= 500.f) // some "chaining radius"
				//{
				//	PotentialNewTargets.Add(Entity); // maybe redirect bullet
				//}
			}
			Entities = ValidHitTargets;

			Entities = Entities.FilterByPredicate([BulletFragment](const FMassEntityHandle& EnemyEntity)
			{
				return !BulletFragment->AlreadyHitEntities.Contains(EnemyEntity);// Only keep enemies that haven't already been hit.
			});

			TArray<FMassEntityHandle> EntitiesToDestroy;
			bool OutOfChain = !BulletChainFragment;//TODO Pas sûr, le but est d'init le bool sur true si le fragment existe pas
			if (OutOfChain) {
				UE_LOG(LogTemp, Warning, TEXT("OutOfChain true despite BulletChainFragment fragment present"));
			}
			float AvailableTargetsNumber = Entities.Num();
			int consumedTargets =0;
			if (BulletChainFragment && AvailableTargetsNumber > 0)
			{
				if (AvailableTargetsNumber > BulletChainFragment->RemainingChain) //TODO Recheck réveillé Attention si float car virgule
				{
					OutOfChain = true;
					consumedTargets = BulletChainFragment->RemainingChain;
					BulletChainFragment->RemainingChain = 0;
					UE_LOG(LogTemp, Warning, TEXT("CHAIN : Consumed %d targets, %f remaining Chain"), consumedTargets, BulletChainFragment->RemainingChain);
				}
				else 
				{
					consumedTargets = AvailableTargetsNumber;
					BulletChainFragment->RemainingChain -= consumedTargets;
				} //TODO ici ou ailleur : Assigner une nouvelle direction  après la chain (signal processor pour ça?)
				AvailableTargetsNumber -= consumedTargets;
			}
			bool OutOfPierce = !BulletPierceFragment;//TODO Pas sûr, le but est d'init le bool sur true si le fragment existe pas
			if (BulletPierceFragment && AvailableTargetsNumber > 0)
			{
				if (AvailableTargetsNumber > BulletPierceFragment->RemainingPierce) //TODO Recheck réveillé Attention si float car virgule
				{
					OutOfPierce = true;
					consumedTargets = BulletPierceFragment->RemainingPierce;
					BulletPierceFragment->RemainingPierce = 0;
				}
				else
				{
					consumedTargets = AvailableTargetsNumber;
					BulletPierceFragment->RemainingPierce -= consumedTargets;
				}
				AvailableTargetsNumber -= consumedTargets;
			}
			if (AvailableTargetsNumber > 0 && OutOfChain && OutOfPierce)
			{
				UE_LOG(LogTemp, Warning, TEXT("Out of chain and Pierce, remaining targets: %f"), AvailableTargetsNumber);
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
					else 
					{
						BulletFragment->AlreadyHitEntities.Add(EnemyEntity);
					}
				}
			}
				Context.Defer().DestroyEntities(EntitiesToDestroy);
		}
	});
}
