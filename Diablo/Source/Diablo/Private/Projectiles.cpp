// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles.h"

// Sets default values
AProjectiles::AProjectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectiles::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectiles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target == nullptr) return;

	FVector Location = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FVector ToTarget = TargetLocation - Location;
	float Distance = ToTarget.Size();
	if (Distance <= KINDA_SMALL_NUMBER) return; 

	FVector Direction = ToTarget.GetSafeNormal();

	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = Direction.Rotation();

	float AngleDiff = FMath::Abs(FRotator::NormalizeAxis(TargetRot.Yaw - CurrentRot.Yaw));
	const float MaxSnapAngle = 60.f; 
	FRotator NewRot;
	if (AngleDiff > MaxSnapAngle)
	{
		NewRot = TargetRot;
	}
	else
	{
		const float RotationSpeed = 10.f;
		NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);
	}
	SetActorRotation(NewRot);

	float MoveStep = ProjectilesSpeed * DeltaTime;
	if (MoveStep > Distance) MoveStep = Distance; 
	SetActorLocation(Location + Direction * MoveStep);
}
