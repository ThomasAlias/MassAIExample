#pragma once

#include "CoreMinimal.h"
#include "MassEntitySubsystem.h"
#include "MassCommonFragments.h"
#include "UObject/NoExportTypes.h"
#include "BulletFragments.h"
#include <MassEntityConfigAsset.h>
#include "Upgrades.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BULLETHELLEXAMPLE_API UUpgrades : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
    int32 DamageLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
    int32 CritChanceLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
    int32 CritDamageLevel = 2;
    //---------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
    int32 PierceLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
    int32 ChainLevel = 0;

    UFUNCTION(BlueprintCallable)
    void ResetUpgrade()
    {
        DamageLevel = 1;
        CritChanceLevel = 0;
        CritDamageLevel = 0;
        PierceLevel = 1;
        ChainLevel = 2;
    }


    UFUNCTION(BlueprintCallable)
    void SetFromBlueprint(int32 InDamage, int32 InCritChance, int32 InCritDamage, int32 InChain, int32 InPierce)
    {
        DamageLevel = InDamage;
        CritChanceLevel = InCritChance;
        CritDamageLevel = InCritDamage;
        PierceLevel = InPierce;
        ChainLevel = InChain;
    }
};
