// Fill out your copyright notice in the Description page of Project Settings.


#include "UEntity.h"

// Sets default values
AUEntity::AUEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

