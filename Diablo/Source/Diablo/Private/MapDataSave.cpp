// Fill out your copyright notice in the Description page of Project Settings.


#include "MapDataSave.h"

FMapData UMapDataSave::GetRandomMapData() const
{
	if (MapDatas.IsEmpty())
	{
		return FMapData();
	}

	TArray<FString> Slugs;
	MapDatas.GenerateKeyArray(Slugs);

	const int32 RandomIndex = FMath::RandRange(0, Slugs.Num() - 1);
	if (const FMapDataWrapper* Wrapper = MapDatas.Find(Slugs[RandomIndex]))
	{
		return Wrapper->MapData;
	}

	return FMapData();
}

FMapData UMapDataSave::GetMapDataBySlug(const FString& Slug) const
{
	if (const FMapDataWrapper* Wrapper = MapDatas.Find(Slug))
	{
		return Wrapper->MapData;
	}

	return FMapData();
}

TArray<FMapData> UMapDataSave::GetAllMapData() const
{
	TArray<FMapData> Data;
	Data.Reserve(MapDatas.Num());

	for (const TPair<FString, FMapDataWrapper>& MapData : MapDatas)
	{
		Data.Add(MapData.Value.MapData);
	}

	return Data;
}

