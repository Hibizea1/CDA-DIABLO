#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerStats.generated.h"



UENUM(BlueprintType)
enum class EStatsType : uint8
{
	Health      UMETA(DisplayName = "Health"),
	Stamina      UMETA(DisplayName = "Stamina"),
	Mana UMETA(DisplayName = "Mana"),
	Intelligence UMETA(DisplayName = "Intelligence"),
	Strength UMETA(DisplayName = "Strength")
};



UCLASS(BlueprintType)
class DIABLO_API UPlayerStats : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Intelligence;
};
