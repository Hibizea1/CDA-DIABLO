// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "MapGenerator.generated.h"

class ANavMeshBoundsVolume;

UCLASS()
class DIABLO_API AMapGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapGenerator();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int RoomsAmount;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int HMax;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int FloorMax;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int32 Height;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	bool bHasWall;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int MaxMobPerRoom;

	[[nodiscard]] int32 Height1() const
	{
		return Height;
	}

	[[nodiscard]] int32 Width1() const
	{
		return Width;
	}

	
	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	int32 Width;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Generation Twicks")
	float TileSize = 100.f;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Grid")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Grid")
	TArray<TSubclassOf<AEnemies>> Enemies;

	TArray<TArray<ATile*>> Grid;
	UPROPERTY(Blueprintable, BlueprintReadOnly, Category = "Creation")
	TArray<AARoom*> Rooms;
protected:
	
	virtual void BeginPlay() override;
	void ClearMap();

	void GenerateMap();
	void InitGrid();
	static AMapGenerator* _instance;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Generation Twicks")
	void SpawnPlayer();

	static AMapGenerator& Get();

	void GetRandomEnemies(const FVector& Location, const FRotator& Rotation);

	ATile* GetTile(int32 X, int32 Y);
};
