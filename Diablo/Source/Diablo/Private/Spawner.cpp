#include "Spawner.h"

#include "CharacterComponent.h"
#include "MapGenerator.h"
#include "Engine/World.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnInterval <= 0.f)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawner::SpawnEnemies, SpawnInterval, true);
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawner::CleanupAliveEnemies()
{
	AliveEnemies.RemoveAll([](const TWeakObjectPtr<AEnemies>& EnemyPtr)
	{
		return !EnemyPtr.IsValid();
	});
}

bool ASpawner::TryGetGroundedLocationInMap(FVector& OutLocation) const
{
	if (!GetWorld())
	{
		return false;
	}

	const AMapGenerator& Gen = AMapGenerator::Get();

	// 1) point candidat autour du spawner
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	const float Radius = FMath::FRandRange(0.f, SpawnRadius);
	FVector Candidate = GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);

	// 2) clamp XY dans la map (rectangle de tiles)
	const FVector MapOrigin = Gen.GetActorLocation();
	const float MinX = MapOrigin.X;
	const float MinY = MapOrigin.Y;
	const float MaxX = MapOrigin.X + (Gen.CurrentMapData.Width - 1) * Gen.TileSize;
	const float MaxY = MapOrigin.Y + (Gen.CurrentMapData.Height - 1) * Gen.TileSize;

	Candidate.X = FMath::Clamp(Candidate.X, MinX, MaxX);
	Candidate.Y = FMath::Clamp(Candidate.Y, MinY, MaxY);

	// 3) snap au sol via line trace vertical
	FHitResult Hit;
	const FVector TraceStart = Candidate + FVector(0.f, 0.f, 1500.f);
	const FVector TraceEnd = Candidate - FVector(0.f, 0.f, 3000.f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnerGroundTrace), false, this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params);

	if (!bHit)
	{
		return false;
	}

	OutLocation = Hit.ImpactPoint + FVector(0.f, 0.f, 2.f);
	return true;
}

void ASpawner::SpawnEnemies()
{
	if (!GetWorld() || Enemies.Num() == 0)
	{
		return;
	}

	AMapGenerator& Gen = AMapGenerator::Get();
	if (Gen.CurrentMapData.Enemies.Num() == 0)
	{
		return;
	}

	CleanupAliveEnemies();
	if (MaxAliveEnemies > 0 && AliveEnemies.Num() >= MaxAliveEnemies)
	{
		return; // cap atteint
	}

	FVector SpawnLocation;
	if (!TryGetGroundedLocationInMap(SpawnLocation))
	{
		return;
	}

	const int32 ClassIndex = FMath::RandRange(0, Enemies.Num() - 1);
	TSubclassOf<AEnemies> EnemyClass = Enemies[ClassIndex];
	if (!EnemyClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemies* SpawnedEnemy = GetWorld()->SpawnActor<AEnemies>(EnemyClass, SpawnLocation, GetActorRotation(),
	                                                          SpawnParams);
	if (!SpawnedEnemy)
	{
		return;
	}

	const int32 DataIndex = FMath::RandRange(0, Gen.CurrentMapData.Enemies.Num() - 1);
	const FMonsterStruct& MonsterData = Gen.CurrentMapData.Enemies[DataIndex];
	SpawnedEnemy->Enemies = MonsterData;

	if (UCharacterComponent* CharComp = SpawnedEnemy->FindComponentByClass<UCharacterComponent>())
	{
		CharComp->SetStatByEnnemiData(MonsterData.EnnemiData);
	}

	AliveEnemies.Add(SpawnedEnemy);
}
