// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "GameFramework/SaveGame.h"
#include "UItemSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 ID;

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	EEquipmentType Type;

	UPROPERTY(BlueprintReadWrite)
	int32 Rarity;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(BlueprintReadWrite)
	int32 Price;

	UPROPERTY(BlueprintReadWrite)
	FString MeshPath;
	
	UPROPERTY(BlueprintReadWrite)
	FString IconPath;

	UPROPERTY(BlueprintReadWrite)
	EWeaponType WeaponType;
};

USTRUCT(BlueprintType)
struct FItemArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FItemData> Items;
};

UCLASS()
class DIABLO_API UItemSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FItemArrayWrapper> ItemsByType;

	UFUNCTION(BlueprintCallable, Category="Items")
	FItemData GetItemByID(int32 ItemID) const
	{
		for (const auto& Pair : ItemsByType)
		{
			for (const FItemData& Item : Pair.Value.Items)
			{
				if (Item.ItemID == ItemID)
					return Item;
			}
		}

		// Return empty/default if not found
		return FItemData();
	}

	UFUNCTION(BlueprintCallable, Category="Items")
	FItemData GetItemRandom() const
	{
		// Rassembler tous les items dans un tableau
		TArray<FItemData> AllItems;
		AllItems.Reserve(128);
		for (const auto& Pair : ItemsByType)
		{
			for (const FItemData& Item : Pair.Value.Items)
			{
				AllItems.Add(Item);
			}
		}

		if (AllItems.Num() == 0)
		{
			return FItemData();
		}

		int32 Index = FMath::RandRange(0, AllItems.Num() - 1);
		return AllItems[Index];
	}

	// Optional: helper to get all items of a weapon type
	UFUNCTION(BlueprintCallable, Category="Items")
	TArray<FItemData> GetItemsByType(const FString& WeaponType) const
	{
		const FItemArrayWrapper* Wrapper = ItemsByType.Find(WeaponType);
		if (Wrapper)
		{
			return Wrapper->Items;
		}
		return TArray<FItemData>();
	}
};