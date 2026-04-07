// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructManage.generated.h"

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
