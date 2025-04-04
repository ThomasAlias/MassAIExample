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
    int32 PierceLevel = 0;
};
