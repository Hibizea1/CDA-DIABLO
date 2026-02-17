// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "FItemSave.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemSave : public FTableRowBase
{
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Save")
	int32 item_id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Save")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Save")
	int32 lvl = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Save")
	int rarity;
};
