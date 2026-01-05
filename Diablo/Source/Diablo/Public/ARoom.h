// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "ARoom.generated.h"


UCLASS()
class DIABLO_API AARoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AARoom();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation Twicks")
	TArray<ATile*> Tiles;
	int RoomID;
	int MaxFloor;
	int CurrentFloor = 0;
	int HMax;
	int MaxMobPerRoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation Twicks")
	ATile* LastTile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	ATile* GetRandomTile();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void add(ATile& Tile);
	void Init(ATile& StartTile, int HMax, int RoomID, int MaxFloor);
	void SpawnWall(ATile* Tile, float MeshUnitLength, FVector Mid, FVector Dir, float Dist);
	
	
	UFUNCTION(Blueprintable)
	void ConstructWall();
	void SetLastTile(ATile* Tile);
	void SpawnEnemies();
};
