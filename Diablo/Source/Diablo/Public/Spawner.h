// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ASpawner();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Grid")
	TArray<TSubclassOf<AEnemies>> Enemies;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemies();

};
