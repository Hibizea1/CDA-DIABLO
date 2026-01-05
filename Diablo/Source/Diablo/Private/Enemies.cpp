// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies.h"

AEnemies::AEnemies()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemies::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemies::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

