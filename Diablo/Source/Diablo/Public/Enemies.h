// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Enemies.generated.h"


class UEnemiesData;

UCLASS()
class DIABLO_API AEnemies : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemies();

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<UEnemiesData> Enemies;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
