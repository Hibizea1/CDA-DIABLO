// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UEntity.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Enemies.generated.h"



enum class ERarity : uint8;
struct FItemData;
class AItems;
class UEnemiesData;

UCLASS()
class DIABLO_API AEnemies : public AUEntity
{
	GENERATED_BODY()

public:
	AEnemies();

	UPROPERTY(Blueprintable, BlueprintReadWrite, VisibleAnywhere, Category = "Stats")
	FMonsterStruct Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<AItems> ItemClass;



protected:
	virtual void BeginPlay() override;

private:
	int32 CalculateLevel(const ERarity& Rarity) const;
	bool RollRarityAndCanSpawn(ERarity& OutRarity);
	int32 GetPlayerLevel() const;
	int32 GetZoneLevel() const;
	int32 GetMonsterLevel() const;
	int32 GetRarityLevelBonus(const ERarity Rarity) const;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SpawnItem();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
