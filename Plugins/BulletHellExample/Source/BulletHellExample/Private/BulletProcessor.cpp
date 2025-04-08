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
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadOnly);

	EntityQuery.AddRequirement<FBulletDamageFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletCritChanceFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FBulletCritDamageFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);



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

		auto BulletDamageFragments = Context.GetMutableFragmentView<FBulletDamageFragment>();
		auto BulletCritChanceFragments = Context.GetMutableFragmentView <FBulletCritChanceFragment>();
		auto BulletCritDamageFragments = Context.GetMutableFragmentView <FBulletCritDamageFragment>();


		auto BulletChainFragments = Context.GetMutableFragmentView<FBulletChainFragment>();
		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();
		const bool bHasChain = (BulletChainFragments.GetData() != nullptr);//Ca a pas l'air d'être la "bonne" façon de faire je pense.
		const bool bHasPierce = (BulletPierceFragments.GetData() != nullptr);
		const bool bHasCritChance = (BulletCritChanceFragments.GetData() != nullptr);
		const bool bHasCritDamage = (BulletCritDamageFragments.GetData() != nullptr);
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
						BulletDamageFragments[EntityIdx].Damage = Upgrades->DamageLevel;
						if (bHasCritChance)
						{
							BulletCritChanceFragments[EntityIdx].CritChance = Upgrades->CritChanceLevel*0.25;
						}
						if (bHasCritDamage)
						{
							BulletCritDamageFragments[EntityIdx].CritDamage = 2+Upgrades->CritDamageLevel*0.5;
						}
						//-------------------
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
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FBulletFragment>(EMassFragmentAccess::ReadWrite);

	EntityQuery.AddRequirement<FBulletDamageFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FBulletCritChanceFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FBulletCritDamageFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);


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
		auto VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
		auto BulletFragments = Context.GetFragmentView<FBulletFragment>();

		auto BulletDamageFragments = Context.GetFragmentView<FBulletDamageFragment>();
		auto BulletCritChanceFragments = Context.GetFragmentView <FBulletCritChanceFragment>();
		auto BulletCritDamageFragments = Context.GetFragmentView <FBulletCritDamageFragment>();


		auto BulletPierceFragments = Context.GetMutableFragmentView<FBulletPierceFragment>();
		auto BulletChainFragments = Context.GetMutableFragmentView<FBulletChainFragment>();

		const int32 NumEntities = Context.GetNumEntities();

		for (int EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			auto& TransformFragment = TransformFragments[EntityIdx];
			FBulletFragment* BulletFragment = EntityManager.GetFragmentDataPtr<FBulletFragment>(Context.GetEntity(EntityIdx));

			FBulletDamageFragment* BulletDamageFragment = EntityManager.GetFragmentDataPtr<FBulletDamageFragment>(Context.GetEntity(EntityIdx));
			FBulletCritChanceFragment* BulletCritChanceFragment = EntityManager.GetFragmentDataPtr<FBulletCritChanceFragment>(Context.GetEntity(EntityIdx));
			FBulletCritDamageFragment* BulletCritDamageFragment = EntityManager.GetFragmentDataPtr<FBulletCritDamageFragment>(Context.GetEntity(EntityIdx));

			FBulletPierceFragment* BulletPierceFragment = EntityManager.GetFragmentDataPtr<FBulletPierceFragment>(Context.GetEntity(EntityIdx));
			FBulletChainFragment* BulletChainFragment = EntityManager.GetFragmentDataPtr<FBulletChainFragment>(Context.GetEntity(EntityIdx));

			auto Location = TransformFragment.GetTransform().GetLocation();
			
			TArray<FMassEntityHandle> Entities;
			BulletHellSubsystem->GetHashGrid().Query(FBox::BuildAABB(Location, FVector(50.f)), Entities);

			TArray<FMassEntityHandle> HitTargets;
			TArray<FMassEntityHandle> VicinityTargets;
			for (const FMassEntityHandle& Entity : Entities)
			{
				auto* EnemyTransform = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
				const float Dist = FVector::Dist(Location, EnemyTransform->GetTransform().GetLocation());

				if (BulletFragment->AlreadyHitEntities.Contains(Entity))
					continue;

				if (Dist <= 50.f)//Replace by bullet Size
				{
					HitTargets.Add(Entity);
				}
				else if (Dist <= 500.f) // Replace Chain detection range
				{
					VicinityTargets.Add(Entity); 
				}
			}
			Entities = HitTargets;

			HitTargets = HitTargets.FilterByPredicate([BulletFragment](const FMassEntityHandle& EnemyEntity)
			{
				return !BulletFragment->AlreadyHitEntities.Contains(EnemyEntity);// Only keep enemies that haven't already been hit.
			});

			TArray<FMassEntityHandle> EntitiesToDestroy;
			bool OutOfChain = !BulletChainFragment;
			float AvailableTargetsNumber = HitTargets.Num();
			int consumedTargets =0;
			if (BulletChainFragment && AvailableTargetsNumber > 0)
			{
				if (AvailableTargetsNumber > BulletChainFragment->RemainingChain) //TODO Recheck réveillé Attention si float car virgule
				{
					OutOfChain = true;
					consumedTargets = BulletChainFragment->RemainingChain;
					BulletChainFragment->RemainingChain = 0;
				}
				else 
				{
					consumedTargets = AvailableTargetsNumber;
					BulletChainFragment->RemainingChain -= consumedTargets;
				}
				AvailableTargetsNumber -= consumedTargets;
			}
			bool OutOfPierce = !BulletPierceFragment;
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
				EntitiesToDestroy.Add(Context.GetEntity(EntityIdx));//destroy the bullet
			}
			bool enemyHaveBeenHit = false;

			float damageDealt = BulletDamageFragment->Damage;
			if (BulletCritChanceFragment && FMath::FRand() < BulletCritChanceFragment->CritChance) {
				if (BulletCritDamageFragment)
				{
					damageDealt *= BulletCritDamageFragment->CritDamage;
				}
				else
				{
					damageDealt *= 2;
				}
			}

			for (FMassEntityHandle& EnemyEntity : HitTargets)
			{
				FBHEnemyFragment* EnemyFragment = EntityManager.GetFragmentDataPtr<FBHEnemyFragment>(EnemyEntity);
				if (EnemyFragment)
				{
					enemyHaveBeenHit = true;
					EnemyFragment->Health -= damageDealt;
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

			if (BulletChainFragment && BulletChainFragment->RemainingChain > 0 && VicinityTargets.Num() > 0 && enemyHaveBeenHit)
			{
				const int32 Index = FMath::RandRange(0, VicinityTargets.Num() - 1);
				FMassEntityHandle NewTarget = VicinityTargets[0];

				const auto* TargetTransform = EntityManager.GetFragmentDataPtr<FTransformFragment>(NewTarget);
				FVector NewDirection = (TargetTransform->GetTransform().GetLocation() - Location).GetSafeNormal();
				NewDirection.Z = 0.f; 
				BulletFragment->Direction = NewDirection;

				FMassVelocityFragment& Velocity = VelocityFragments[EntityIdx];
				Velocity.Value = NewDirection * BulletFragment->Speed;
			}
				Context.Defer().DestroyEntities(EntitiesToDestroy);
		}
	});
}

