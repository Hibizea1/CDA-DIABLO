#pragma once

#include "CoreMinimal.h"
#include "Enemies.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class DIABLO_API ASpawner : public AActor
{
	GENERATED_BODY()

public:
	ASpawner();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Grid")
	TArray<TSubclassOf<AEnemies>> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0.1"))
	float SpawnInterval = 3.0f;

	// Cap local: nb max de mobs vivants gérés par CE spawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Cap", meta = (ClampMin = "0"))
	int32 MaxAliveEnemies = 8;

	// Rayon autour du spawner pour chercher une position de spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Placement", meta = (ClampMin = "0"))
	float SpawnRadius = 200.f;

	FTimerHandle SpawnTimer;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void SpawnEnemies();

private:
	TArray<TWeakObjectPtr<AEnemies>> AliveEnemies;

	void CleanupAliveEnemies();
	bool TryGetGroundedLocationInMap(FVector& OutLocation) const;
};
