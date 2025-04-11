#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponStatRow.generated.h"

UENUM(BlueprintType)
enum class EWeaponStatType : uint8
{
	Damage     UMETA(DisplayName = "Damage"),
	CritChance UMETA(DisplayName = "Crit Chance"),
	CritDamage UMETA(DisplayName = "Crit Damage"),
	Cooldown   UMETA(DisplayName = "Cooldown"),
	ManaCost   UMETA(DisplayName = "Mana Cost"),
	Chain     UMETA(DisplayName = "Chain"),
	Split     UMETA(DisplayName = "Split"),
	Pierce     UMETA(DisplayName = "Pierce"),
	Socket     UMETA(DisplayName = "Socket")
};

USTRUCT(BlueprintType)
struct FWeaponStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Define what kind of stat this row represents.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
	EWeaponStatType StatType;

	// The base value for the stat.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
	float BaseValue;

	// The scaling factor (how much bonus is added per rune level, for example).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
	float ScalingFactor;

	// A display name for the stat to support localization and UI, using FText.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
	FText DisplayName;
};
