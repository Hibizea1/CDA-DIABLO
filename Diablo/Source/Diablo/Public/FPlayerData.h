// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FItemDataToSave.h"
#include "FPlayerData.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct DIABLO_API FPlayerData 
{
	GENERATED_BODY()	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 strenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 intelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemDataToSave> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemDataToSave> Equipment;
};
