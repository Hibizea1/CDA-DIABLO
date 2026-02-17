// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "ItemStruct.h"
#include "FItemsRoot.generated.h"

USTRUCT(BlueprintType)
struct FItemsRoot : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 Item_ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 Price;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	EWeaponType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	ERarity rarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString mesh_path;
};
