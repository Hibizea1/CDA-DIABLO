// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructManage.h"
#include "GameFramework/SaveGame.h"
#include "MonsterAndBiomsSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FMonsterAndBiomesWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FMonsterStruct> Monster;
	TArray<FBiomeStruct> Biomes;
};

UCLASS()
class DIABLO_API UMonsterAndBiomsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FMonsterAndBiomesWrapper> MonsterAndBiomesByType;

	UFUNCTION(BlueprintCallable, Category="Monster")
	FMonsterStruct GetMonsterById(int32 MonsterID) const
	{
		for (const auto& Pair : MonsterAndBiomesByType)
		{
			for (const FMonsterStruct& monster : Pair.Value.Monster)
			{
				if (monster.Monster_ID == MonsterID)
					return monster;
			}
		}

		// Return empty/default if not found
		return FMonsterStruct();
	}

	UFUNCTION(BlueprintCallable, Category="Biomes")
	FBiomeStruct GetBiomesById(int32 Biome_ID) const
	{
		for (const auto& Pair : MonsterAndBiomesByType)
		{
			for (const FBiomeStruct& monster : Pair.Value.Biomes)
			{
				if (monster.Biome_ID == Biome_ID)
					return monster;
			}
		}

		// Return empty/default if not found
		return FBiomeStruct();
	}

	UFUNCTION(BlueprintCallable, Category="Monster")
	FMonsterStruct GetMonsterRandom() const
	{
		// Rassembler tous les items dans un tableau
		TArray<FMonsterStruct> AllMonsters;
		AllMonsters.Reserve(128);
		for (const auto& Pair : MonsterAndBiomesByType)
		{
			for (const FMonsterStruct& Monster : Pair.Value.Monster)
			{
				AllMonsters.Add(Monster);
			}
		}

		if (AllMonsters.Num() == 0)
		{
			return FMonsterStruct();
		}

		int32 Index = FMath::RandRange(0, AllMonsters.Num() - 1);
		return AllMonsters[Index];
	}
	
	UFUNCTION(BlueprintCallable, Category="Monster")
	FBiomeStruct GetBiomesRandom() const
	{
		// Rassembler tous les items dans un tableau
		TArray<FBiomeStruct> AllMonsters;
		AllMonsters.Reserve(128);
		for (const auto& Pair : MonsterAndBiomesByType)
		{
			for (const FBiomeStruct& Monster : Pair.Value.Biomes)
			{
				AllMonsters.Add(Monster);
			}
		}

		if (AllMonsters.Num() == 0)
		{
			return FBiomeStruct();
		}

		int32 Index = FMath::RandRange(0, AllMonsters.Num() - 1);
		return AllMonsters[Index];
	}

	// Optional: helper to get all items of a weapon type
	UFUNCTION(BlueprintCallable, Category="Items")
	TArray<FMonsterStruct> GetMonsterByBiomes(const FString& MonsterBiomes) const
	{
		const FMonsterAndBiomesWrapper* Wrapper = MonsterAndBiomesByType.Find(MonsterBiomes);
		if (Wrapper)
		{
			return Wrapper->Monster;
		}
		return TArray<FMonsterStruct>();
	}

	UFUNCTION(BlueprintCallable, Category="Items")
	TArray<FMonsterStruct> GetMonsterByType(const FString& MonsterBiomes) const
	{
		const FMonsterAndBiomesWrapper* Wrapper = MonsterAndBiomesByType.Find(MonsterBiomes);
		if (Wrapper)
		{
			return Wrapper->Monster;
		}
		return TArray<FMonsterStruct>();
	}

	UFUNCTION(BlueprintCallable, Category="Items")
	TArray<FBiomeStruct> GetAllBiomes() const
	{
			return MonsterAndBiomesByType.Find("biomes")->Biomes;
	}
};
