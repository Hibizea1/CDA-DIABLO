// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponent.h"

#include "FPlayerData.h"

// Sets default values for this component's properties
UCharacterComponent::UCharacterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


void UCharacterComponent::UpdateStat(EStatsType statType, float value)
{
	switch (statType)
	{
	case EStatsType::Health:
		LevelStats.HealthLevel += value;
		break;
	case EStatsType::Stamina:
		LevelStats.StaminaLevel += value;
		break;
	case EStatsType::Mana:
		LevelStats.ManaLevel += value;
		break;
	case EStatsType::Strength:
		LevelStats.StrengthLevel += value;
		break;
	default:
		break;
	}
}

void UCharacterComponent::CalculateStats()
{
	CalculateHealth();
	CalculateStamina();
	CalculateStrength();
	CalculateMana();
}

void UCharacterComponent::SetHealth(float damage)
{
	Stats.CurrentHealth -= damage;
	if (Stats.CurrentHealth <= 0)
	{
		// Clamp to zero
		Stats.CurrentHealth = 0.f;
		OnDeath.Broadcast(DeathData);
	}
}

FStatsStruct UCharacterComponent::GetStats() const
{
	return Stats;
}

void UCharacterComponent::LevelUpHealth()
{
	LevelStats.HealthLevel++;
	CalculateHealth();
}

void UCharacterComponent::LevelUpMana()
{
	LevelStats.ManaLevel++;
	CalculateMana();
}

void UCharacterComponent::LevelUpStrength()
{
	LevelStats.StrengthLevel++;
	CalculateStrength();
}

void UCharacterComponent::LevelUpStamina()
{
	LevelStats.StaminaLevel++;
	CalculateStamina();
}

// Called every frame
void UCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UCharacterComponent::CalculateHealth()
{
	float _baseHealthMultiplier = PlayerStats->Health * 12;
	float _healthStaminaMultiplier = PlayerStats->Stamina * 8 * LevelStats.HealthLevel;
	float _healthLevelMultiplier = LevelStats.HealthLevel * LevelStats.HealthLevel * 10;

	Stats.MaxHealth = _baseHealthMultiplier + _healthStaminaMultiplier + _healthLevelMultiplier;
	Stats.CurrentHealth = Stats.MaxHealth;
}

void UCharacterComponent::CalculateStamina()
{
}

void UCharacterComponent::CalculateStrength()
{
	float _strengthPercent = LevelStats.StrengthLevel / 100.f;
	float _strengthBasePercent = _strengthPercent + 1.f;

	float _strengthBaseDamage = BaseDamage * _strengthBasePercent * DamageMultiplier;

	float _weaponDamage = ItemDamageMultiplier * RarityMultiplier;

	Stats.Strength = _strengthBaseDamage + _weaponDamage;
}

void UCharacterComponent::CalculateMana()
{
}

void UCharacterComponent::SetStat(FPlayerData _stat)
{
	LevelStats.HealthLevel = _stat.health;
	LevelStats.StaminaLevel = _stat.stamina;
	LevelStats.StrengthLevel = _stat.strenght;
	LevelStats.ManaLevel = _stat.mana;
	CalculateStats();
}
