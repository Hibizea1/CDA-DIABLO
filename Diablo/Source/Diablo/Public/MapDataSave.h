// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructManage.h"
#include "GameFramework/SaveGame.h"
#include "MapDataSave.generated.h"


USTRUCT(Blueprintable)
struct FMapDataWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FMapData MapData;
	
};
/**
 * 
 */
UCLASS()
class DIABLO_API UMapDataSave : public USaveGame
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FMapDataWrapper> MapDatas;

	FMapData GetRandomMapData() const;

	FMapData GetMapDataBySlug(const FString& Slug) const;

	TArray<FMapData> GetAllMapData() const;
	
};
