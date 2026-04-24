// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "GameFramework/SaveGame.h"
#include "UItemSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FItemArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FItemStruct> Items;
};

UCLASS()
class DIABLO_API UItemSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FItemArrayWrapper> ItemsByType;

	UFUNCTION(BlueprintCallable, Category="Items")
	FItemStruct GetItemByID(int32 ItemID) const
	{
		for (const auto& Pair : ItemsByType)
		{
			for (const FItemStruct& Item : Pair.Value.Items)
			{
				if (Item.Item_ID == ItemID)
					return Item;
			}
		}

		// Return empty/default if not found
		return FItemStruct();
	}

	UFUNCTION(BlueprintCallable, Category="Items")
	FItemStruct GetItemRandom() const
	{
		// Rassembler tous les items dans un tableau
		TArray<FItemStruct> AllItems;
		AllItems.Reserve(128);
		for (const auto& Pair : ItemsByType)
		{
			for (const FItemStruct& Item : Pair.Value.Items)
			{
				AllItems.Add(Item);
			}
		}

		if (AllItems.Num() == 0)
		{
			return FItemStruct();
		}

		int32 Index = FMath::RandRange(0, AllItems.Num() - 1);
		return AllItems[Index];
	}

	// Optional: helper to get all items of a weapon type
	UFUNCTION(BlueprintCallable, Category="Items")
	TArray<FItemStruct> GetItemsByType(const FString& WeaponType) const
	{
		const FItemArrayWrapper* Wrapper = ItemsByType.Find(WeaponType);
		if (Wrapper)
		{
			return Wrapper->Items;
		}
		return TArray<FItemStruct>();
	}
};