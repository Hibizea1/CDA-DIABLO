// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UEntity.h"
#include "PlayerStats.h"
#include "Components/ActorComponent.h"
#include "CharacterComponent.generated.h"

struct FPlayerData;

// Declare a Blueprint-friendly dynamic multicast delegate that passes an FDeathStruct
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUp);

UCLASS(ClassGroup=(Character), meta=(BlueprintSpawnableComponent))
class DIABLO_API UCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	float BaseDamage = 20.f;
	float DamageMultiplier = 1.2f;

public:
	// Sets default values for this component's properties
	UCharacterComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death Drop")
	FDeathStruct DeathData;

	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	FLevelStatStruct LevelStats;
	
	UPROPERTY(BlueprintReadWrite, Category = "Player Stats")
	FStatsStruct Stats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	UPlayerStats* PlayerStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float ItemDamageMultiplier = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float RarityMultiplier;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelUp OnLevelUp;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CalculateHealth();
	void CalculateStamina();
	void CalculateStrength();
	void CalculateMana();

public:
	UFUNCTION(BlueprintCallable)
	void SetStat(FPlayerData _stat);

	UFUNCTION(BlueprintCallable)
	void AddExp(int Value);
	
	UFUNCTION(BlueprintCallable)
	void SetRequiresExp();

	UFUNCTION(BlueprintCallable)
	void UpdateStat(EStatsType statType, float value);

	UFUNCTION(BlueprintCallable)
	void CalculateStats();

	UFUNCTION(BlueprintCallable)
	void SetHealth(float damage);

	UFUNCTION(BlueprintCallable)
	FStatsStruct GetStats() const;

	UFUNCTION(BlueprintCallable)
	void LevelUpHealth();
	UFUNCTION(BlueprintCallable)
	void LevelUpMana();
	UFUNCTION(BlueprintCallable)
	void LevelUpStrength();
	UFUNCTION(BlueprintCallable)
	void LevelUpStamina();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
