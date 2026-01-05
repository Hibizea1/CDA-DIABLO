// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectiles.generated.h"

UCLASS()
class DIABLO_API AProjectiles : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectiles();
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	AActor* Target;
	
	UPROPERTY(Blueprintable, BlueprintReadWrite)
	float Damage;

	UPROPERTY(Blueprintable, BlueprintReadWrite)
	float ProjectilesSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
