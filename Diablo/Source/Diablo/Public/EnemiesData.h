// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemiesData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EEnemiesType : uint8
{
	Orc UMETA(DisplayName = "Orc"),
	Golem UMETA(DisplayName = "Golem"),
	Skeleton UMETA(DisplayName = "Skeleton"),
	Boss UMETA(DisplayName = "Boss")
};

UCLASS()
class DIABLO_API UEnemiesData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Type")
	EEnemiesType EnemiesType = EEnemiesType::Orc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	int Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	int Strength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	int DropExperience;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	int DropGold;
	
};

