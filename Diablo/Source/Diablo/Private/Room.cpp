// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"


Room::~Room()
{
}

Room::Room(ATile& StartTile, int HMax, int RoomID, int MaxFloor) : RoomID(RoomID), MaxFloor(MaxFloor), HMax(HMax)
{
	// CurrentFloor = 0;
	// StartTile.Check(*this, 0);
}

void Room::add(ATile& Tile)
{
	if (&Tile != nullptr)
	RoomName.Add(&Tile);
}
