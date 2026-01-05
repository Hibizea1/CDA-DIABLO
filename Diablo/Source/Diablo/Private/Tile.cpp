// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

#include "ARoom.h"
#include "MapGenerator.h"
#include "ARoom.h"
// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bHasRoom = false;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	if (!MeshComponent)
	{
		MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	}
	ClearAroundTiles();
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::Check(AARoom& Room, int CurrentH)
{
	if (bIsBorder) return;

	if (&Room == nullptr) return;
	if (!bHasRoom)
	{
		Room.CurrentFloor++;
		Room.add(*this);
		roomId = Room.RoomID;
	}
	bHasRoom = true;
	currentH = CurrentH;
	if (CurrentH >= Room.HMax)
	{
		// Room.LastTile = this;
		return;
	}

	CurrentH++;
	int X = GridX;
	int Y = GridY;

	if (X < 0 || Y < 0)
		return;


	TArray<FVector2D> options = neighboursPos;
	for (int i = 0; i < options.Num(); ++i)
	{
		int j = FMath::RandRange(i, options.Num() - 1);
		options.Swap(i, j);
	}

	for (int idx = 0; idx < options.Num(); ++idx)
	{
		const FVector2D& off = options[idx];
		int newX = X + off.X;
		int newY = Y + off.Y;

		if (newX < 0 || newY < 0 || newX >= AMapGenerator::Get().Width1() || newY >= AMapGenerator::Get().Height1())
			continue;

		auto neighbour = AMapGenerator::Get().GetTile(newX, newY);


		if (!neighbour || neighbour->bHasRoom)
			continue;

		if (Room.CurrentFloor < Room.MaxFloor / 4)
		{
			neighbour->Check(Room, CurrentH);
		}
		else
		{
			if (FMath::FRand() < 0.80f)
			{
				neighbour->Check(Room, CurrentH);
			}
		}
	}
}

void ATile::SetAroundTiles()
{
	AroundTiles.Empty();
	for (const auto& offset : neighboursPos)
	{
		int nx = GridX + offset.X;
		int ny = GridY + offset.Y;
		ATile* neighbour = AMapGenerator::Get().GetTile(nx, ny);
		if (neighbour) // n'ajoute que les tiles valides
		{
			AroundTiles.Add(neighbour);
		}
	}
}
