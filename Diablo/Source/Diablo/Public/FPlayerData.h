// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FItemSave.h"
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
	TArray<FItemSave> Items;
};
