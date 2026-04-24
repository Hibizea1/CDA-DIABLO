// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructManage.generated.h"

class AUEntity;

#pragma region Struct
USTRUCT(BlueprintType)
struct FDeathStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DropGold;
};

USTRUCT(BlueprintType)
struct FSlugStruct
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slug")
	FString Slug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slug")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slug")
	FString Description;
};

USTRUCT(Blueprintable)
struct FEnnemiData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDeathStruct DeathData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 strenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 mana;
};

USTRUCT(BlueprintType)
struct FBiomeStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	int32 Biome_ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FSlugStruct Slug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	int32 LevelMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	int32 LevelMax = 100;

	// Categories envoyees au sync (ex: "outdoor", "early-game")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	TArray<FSlugStruct> CategorySlugs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FString IconPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FString MeshPath;

	bool operator==(const FBiomeStruct& Other) const
	{
		return Biome_ID == Other.Biome_ID;
	}
};

USTRUCT(BlueprintType)
struct FMonsterDropStruct
{
	GENERATED_BODY()

	// Doit correspondre a FItemStruct::Item_ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	int32 Item_ID = 0;

	// Optionnel: si le drop est specifique a un biome
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	FString BiomeSlug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	float DropRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	int32 MinQty = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	int32 MaxQty = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Drop")
	FString Notes;
};

USTRUCT(BlueprintType)
struct FMonsterStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	int32 Monster_ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FSlugStruct Slug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	int32 LevelMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	int32 LevelMax = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FString Difficulty = TEXT("normal");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	TArray<FSlugStruct> CategorySlugs;

	// Drops relies aux items via Item_ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	TArray<FMonsterDropStruct> Drops;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FString IconPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FString MeshPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Struct")
	FEnnemiData EnnemiData;
	
	bool operator==(const FMonsterStruct& Other) const
	{
		return Monster_ID == Other.Monster_ID;
	}
};

#pragma region Map Data

USTRUCT(BlueprintType)
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
	FBiomeStruct Biome;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Grid")
	TArray<FMonsterStruct> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
	int RoomsAmmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
	int HMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
	int FloorMax;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int32 Height;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int32 Width;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	bool bHasWall;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int MaxMobPerRoom;
};

#pragma endregion
#pragma endregion