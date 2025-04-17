#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponStatRow.h" 
#include "RuneStatRow.generated.h"

USTRUCT(BlueprintType)
struct FRuneStatRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune Stat")
	EWeaponStatType StatType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune Stat")
	float StatLevelPerRuneLevel;

	// A display name for UI and localization.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune Stat")
	FText DisplayName;
};//tet
