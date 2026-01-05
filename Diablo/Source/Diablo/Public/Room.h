// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"

/**
 * 
 */
class DIABLO_API Room
{
public:
	~Room();

	Room(ATile& StartTile, int HMax, int RoomID, int MaxFloor);
	
	TArray<ATile*> RoomName;
	int RoomID;
	int MaxFloor;
	int CurrentFloor = 0;
	int HMax;

	ATile* lastTile;


	
	void add(ATile& Tile);
	
};
