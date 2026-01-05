// Fill out your copyright notice in the Description page of Project Settings.


#include "ARoom.h"

#include "GroomVisualizationData.h"
#include "MapGenerator.h"
#include "Tile.h"
#include "DynamicMesh/DynamicMesh3.h"

// Sets default values
AARoom::AARoom()
{
	RoomID = 0;
	MaxFloor = 0;
	HMax = 0;
}

// Called when the game starts or when spawned
void AARoom::BeginPlay()
{
	Super::BeginPlay();
}

ATile* AARoom::GetRandomTile()
{
	return Tiles[FMath::RandRange(0, Tiles.Num() - 1)];
}

// Called every frame
void AARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARoom::add(ATile& Tile)
{
	if (&Tile != nullptr)
		Tiles.Add(&Tile);
}

void AARoom::Init(ATile& StartTile, int _HMax, int _RoomID, int _MaxFloor)
{
	HMax = _HMax;
	RoomID = _RoomID;
	MaxFloor = _MaxFloor;
	CurrentFloor = 0;
	StartTile.Check(*this, 0);
}

void AARoom::ConstructWall()
{
	for (int i = 0; i < Tiles.Num(); ++i)
	{
		ATile* Tile = Tiles[i];
		if (!Tile) continue;

		auto Neighbors = Tile->GetAroundTiles();
		for (auto Neighbor : Neighbors)
		{
			if (!Neighbor) continue;

			if ((Neighbor->roomId != 0 && Neighbor->roomId != 1 && !AMapGenerator::Get().bHasWall) ||
				(Neighbor->roomId == RoomID && AMapGenerator::Get().bHasWall))
				continue;

			uintptr_t AddrA = reinterpret_cast<uintptr_t>(Tile);
			uintptr_t AddrB = reinterpret_cast<uintptr_t>(Neighbor);
			FString Key = AddrA < AddrB
				              ? FString::Printf(TEXT("%llu_%llu"), (unsigned long long)AddrA, (unsigned long long)AddrB)
				              : FString::Printf(TEXT("%llu_%llu"), (unsigned long long)AddrB,
				                                (unsigned long long)AddrA);

			UStaticMesh* MeshAsset = Neighbor->YourMeshPointer ? Neighbor->YourMeshPointer : Tile->YourMeshPointer;
			if (!MeshAsset) continue;

			FVector A = Tile->GetActorLocation();
			FVector B = Neighbor->GetActorLocation();
			FVector Mid = (A + B) * 0.5f;
			FVector Dir = (B - A).GetSafeNormal();
			float Dist = FVector::Dist(A, B);

			if (!Tile->GetRootComponent())
			{
				USceneComponent* Root = NewObject<USceneComponent>(Tile);
				Root->RegisterComponent();
				Root->SetMobility(EComponentMobility::Movable);
				Tile->SetRootComponent(Root);
			}
			float MeshUnitLength = 200.f;
			Neighbor->AroundTiles.Remove(Tile);

			if (Neighbor->roomId > 0 && Neighbor->roomId != RoomID)
			{
				if (Tile == LastTile)
				{
					continue;
				}

				SpawnWall(Tile, MeshUnitLength, Mid, Dir, Dist);
			}
			else
			{
				SpawnWall(Tile, MeshUnitLength, Mid, Dir, Dist);
			}
		}
	}
}

void AARoom::SetLastTile(ATile* Tile)
{
	auto IsValidLast = [this](ATile* T) -> bool
	{
		if (!T) return false;
		if (T->bIsBorder) return false; // ne doit pas être un bord de map

		auto Neighbors = T->GetAroundTiles();
		bool hasOutside = false;

		for (auto N : Neighbors)
		{
			// doit avoir au moins un voisin libre qui n'est pas bord de map
			if (N && !N->bIsBorder && N->roomId == 0)
			{
				hasOutside = true;
			}

			// si adjacent à une autre salle différente => rejeter
			if (N && N->roomId > 0 && N->roomId != RoomID)
			{
				return false;
			}
		}

		return hasOutside;
	};

	// Si le tile fourni est valide, l'utiliser
	if (IsValidLast(Tile))
	{
		LastTile = Tile;
		return;
	}

	// Chercher parmi les tiles de la room
	for (auto T : Tiles)
	{
		if (IsValidLast(T))
		{
			LastTile = T;
			return;
		}
	}

	// Secours : choisir le tile (non bord de map) ayant le moins de voisins du même RoomID
	ATile* Best = nullptr;
	int BestCount = std::numeric_limits<int>::max();
	for (auto T : Tiles)
	{
		if (!T || T->bIsBorder) continue; // ignorer les tiles qui sont bord de map
		int sameCount = 0;
		for (auto N : T->GetAroundTiles())
		{
			if (N && N->roomId == RoomID) ++sameCount;
		}
		if (sameCount < BestCount)
		{
			BestCount = sameCount;
			Best = T;
		}
	}

	LastTile = Best;
}


void AARoom::SpawnEnemies()
{
	auto& gen = AMapGenerator::Get();
	if (Tiles.Num() == 0) return;

	const float MinOffset = 200.f; 
	const float MaxOffset = 400.f; 
	const float SpawnZ = 100.f;

	for (int i = 0; i < gen.MaxMobPerRoom; i++)
	{
		ATile* tile = GetRandomTile();
		if (!tile) continue;

		float Angle = FMath::FRandRange(0.f, 2.f * PI);
		float Radius = FMath::FRandRange(MinOffset, MaxOffset);
		FVector Offset = FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);

		FVector Pos = tile->GetActorLocation() + Offset + FVector(0.f, 0.f, SpawnZ);

		gen.GetRandomEnemies(Pos, tile->GetActorRotation());
	}
}

void AARoom::SpawnWall(ATile* Tile, float MeshUnitLength, FVector Mid, FVector Dir, float Dist)
{
	UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(Tile);
	if (!MeshComp) return;

	MeshComp->SetStaticMesh(Tile->YourMeshPointer);
	MeshComp->SetMobility(EComponentMobility::Movable);
	MeshComp->SetupAttachment(Tile->GetRootComponent());
	MeshComp->RegisterComponent();
	MeshComp->AttachToComponent(Tile->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	FVector LocalMid = Tile->GetActorTransform().InverseTransformPosition(Mid);
	FRotator LocalRot = Tile->GetActorTransform().InverseTransformRotation(Dir.Rotation().Quaternion()).Rotator();

	MeshComp->SetRelativeLocation(LocalMid);
	MeshComp->SetRelativeRotation(LocalRot);
}
