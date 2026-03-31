// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"

#include "ARoom.h"
#include "NavigationSystem.h"
#include "Tile.h"
#include "Engine/BrushShape.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	_instance = this;
	GenerateMap();
}

void AMapGenerator::ClearMap()
{
	if (GetWorld())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Found);
		for (AActor* Actor : Found)
		{
			if (Actor && Actor->GetOwner() == this && !Actor->IsPendingKillPending())
			{
				Actor->Destroy();
			}
		}
	}
	// Détruire les rooms existantes
	for (AARoom* room : Rooms)
	{
		if (room && !room->IsPendingKillPending())
		{
			room->Destroy();
		}
	}
	Rooms.Empty();

	// Détruire les tiles existantes
	for (auto& row : Grid)
	{
		for (ATile* tile : row)
		{
			if (tile && !tile->IsPendingKillPending())
			{
				tile->Destroy();
			}
		}
	}
	Grid.Empty();
}

void AMapGenerator::GenerateMap()
{
	const int MaxRegenerations = 10;
	const int MinRooms = FMath::Max(1, RoomsAmount / 2);

	if (Width < 3 || Height < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid trop petite pour créer des rooms (Width/Height < 3)"));
		return;
	}

	ClearMap();

	for (int attempt = 0; attempt < MaxRegenerations; ++attempt)
	{
		InitGrid();

		if (Grid.Num() == 0 || Width == 0 || Height == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Empty Grid"));
			ClearMap();
			return;
		}

		ATile* startTile = GetTile(Width / 2, Height / 2);

		Rooms.Empty();
		int CurrentRoomNumber = 1;
		while (CurrentRoomNumber <= RoomsAmount)
		{
			FVector SpawnLocation = startTile->GetActorLocation();
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			AARoom* room = GetWorld()->SpawnActor<AARoom>(SpawnLocation, SpawnRotation, SpawnParams);
			if (!room)
			{
				UE_LOG(LogTemp, Warning, TEXT("Échec du SpawnActor de AARoom"));
				break;
			}
			room->Init(*startTile, HMax, CurrentRoomNumber, FloorMax);

			room->SetLastTile(room->LastTile);
			Rooms.Add(room);

			if (room->LastTile)
			{
				startTile = room->LastTile;
				if (startTile->bIsBorder)
				{
					UE_LOG(LogTemp, Warning, TEXT("LastTile est sur un bord, arrêt de la génération de rooms"));
					break;
				}

				CurrentRoomNumber++;
			}
			else
				break;
		}

		if (Rooms.Num() >= MinRooms)
		{
			SpawnPlayer();
			for (auto room : Rooms)
			{
				if (room)
				{
					room->ConstructWall();
					room->SpawnEnemies();
				}
			}
			UE_LOG(LogTemp, Log, TEXT("Génération réussie après %d tentative(s)"), attempt + 1);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Trop peu de rooms (%d), tentative %d/%d, régénération..."), Rooms.Num(),
		       attempt + 1, MaxRegenerations);
		ClearMap();
	}

	UE_LOG(LogTemp, Warning,
	       TEXT("Échec de la génération après %d tentatives, garder la dernière tentative (ou revoir les paramètres)"),
	       MaxRegenerations);
}

void AMapGenerator::InitGrid()
{
	Grid.SetNum(Height);
	for (int32 Y = 0; Y < Height; ++Y)
	{
		Grid[Y].Init(nullptr, Width);
	}

	if (!GetWorld())
	{
		return;
	}

	if (!TileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("TileClass non défini dans MapGenerator"));
		return;
	}

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(X * TileSize, Y * TileSize, 0.f);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			ATile* SpawnedTile = GetWorld()->SpawnActor<ATile>(TileClass, SpawnLocation, SpawnRotation, SpawnParams);
			Grid[Y][X] = SpawnedTile;
			if (SpawnedTile)
			{
				SpawnedTile->roomId = 0;
				SpawnedTile->GridX = X;
				SpawnedTile->GridY = Y;
				SpawnedTile->bIsBorder = (X == 0 || Y == 0 || X == Width - 1 || Y == Height - 1);
				if (SpawnedTile->bIsBorder)
				{
					SpawnedTile->roomId = -1;
				}
			}
		}
	}

	for (auto& row : Grid)
	{
		for (auto* _tile : row)
		{
			if (_tile) _tile->SetAroundTiles();
		}
	}
}

void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AMapGenerator& AMapGenerator::Get()
{
	return *_instance;
}

void AMapGenerator::GetRandomEnemies(const FVector& Location, const FRotator& Rotation)
{
	if (Enemies.Num() == 0 || !GetWorld()) return ;
	int32 Index = FMath::RandRange(0, Enemies.Num() - 1);
	TSubclassOf<AUEntity> EnemyClass = Enemies[Index];
	if (!EnemyClass) return ;
	FActorSpawnParameters SpawnParams;
	UE_LOG(LogTemp, Display, TEXT("SpawnEnemies"));
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<AUEntity>(EnemyClass, Location, Rotation, SpawnParams);
}

ATile* AMapGenerator::GetTile(int32 X, int32 Y)
{
	if (X < 0 || Y < 0 || X >= Width || Y >= Height)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetTile hors bounds: %d,%d"), X, Y);
		return nullptr;
	}

	if (!Grid.IsValidIndex(Y) || !Grid[Y].IsValidIndex(X))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetTile index invalide dans Grid: %d,%d"), X, Y);
		return nullptr;
	}

	return Grid[Y][X];
}

AMapGenerator* AMapGenerator::_instance = nullptr;
