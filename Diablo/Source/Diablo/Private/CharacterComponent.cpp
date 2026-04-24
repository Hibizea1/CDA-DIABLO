// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponent.h"

#include "DeathSubSystem.h"
#include "FPlayerData.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

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

void UCharacterComponent::GetPlayerInventory()
{
	if (!GetWorld())
	{
		return;
	}

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerChar)
	{
		return;
	}

	if (UInventoryComponent* CharComp = PlayerChar->FindComponentByClass<UInventoryComponent>())
	{
		Inventory = CharComp;
	}
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
	GetPlayerInventory();

	CalculateHealth();
	CalculateStamina();
	CalculateStrength();
	CalculateMana();
}

void UCharacterComponent::SetHealth(float damage)
{
	Stats.CurrentHealth -= damage;
	OnTakeDamage.Broadcast();
	if (Stats.CurrentHealth <= 0)
	{
		Stats.CurrentHealth = 0.f;
		UGameInstance* GI = GetWorld()->GetGameInstance();
		if (GI)
		{
			UDeathSubSystem* Subsystem = GI->GetSubsystem<UDeathSubSystem>();
			if (Subsystem)
			{
				Subsystem->OnDeath.Broadcast(DeathData);
			}
		}
		if (AActor* Owner = GetOwner())
		{
			OnDeathCharacter.Broadcast();

			Owner->Destroy();
		}
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
	if (!PlayerStats)
	{
		UE_LOG(LogTemp, Warning, TEXT("CalculateStrength: PlayerStats is null"));
		return;
	}

	if (!Inventory)
	{
		GetPlayerInventory();
	}

	const float StrengthPercent = LevelStats.StrengthLevel / 100.f;
	const float StrengthBasePercent = StrengthPercent + 1.f;
	const float StrengthBaseDamage = PlayerStats->Damage * StrengthBasePercent * DamageMultiplier;

	float ItemMultiplier = 1.f;

	if (Inventory && Inventory->HasWeapon())
	{
		const FItemStruct EquippedWeapon = Inventory->GetEquipedWeapon();
		if (EquippedWeapon.DamageMultiplier > 0.f)
		{
			ItemMultiplier = EquippedWeapon.DamageMultiplier;
		}
	}

	const float SafeRarityMultiplier = (RarityMultiplier > 0.f) ? RarityMultiplier : 1.f;
	const float WeaponDamage = ItemMultiplier * SafeRarityMultiplier;

	Stats.Strength = StrengthBaseDamage + WeaponDamage;
}

void UCharacterComponent::CalculateMana()
{
}

void UCharacterComponent::SetStatByPlayerData(FPlayerData _stat)
{
	LevelStats.HealthLevel = _stat.health;
	LevelStats.StaminaLevel = _stat.stamina;
	LevelStats.StrengthLevel = _stat.strenght;
	LevelStats.ManaLevel = _stat.mana;
	Stats.CurrentLevel = _stat.Level;
	Stats.CurrentExperience = _stat.Experience;
	CalculateStats();
}

void UCharacterComponent::SetStatByEnnemiData(FEnnemiData _stat)
{
	DeathData = _stat.DeathData;

	Stats.MaxHealth = _stat.health;
	Stats.CurrentHealth = _stat.health;
	Stats.Stamina = _stat.stamina;
	Stats.Strength = _stat.strenght;
	Stats.Mana = _stat.mana;
}

void UCharacterComponent::AddExp(int Value)
{
	Stats.CurrentExperience += Value;
	if (Stats.CurrentExperience >= Stats.RequiredExperience)
	{
		while (Stats.CurrentExperience >= Stats.RequiredExperience)
		{
			Stats.CurrentLevel++;
			Stats.CurrentExperience -= Stats.RequiredExperience;
			OnLevelUp.Broadcast();
			SetRequiresExp();
		}
	}
}

void UCharacterComponent::SetRequiresExp()
{
	float levelPow = (Stats.CurrentLevel * Stats.CurrentLevel * Stats.CurrentLevel) * 0.5f;
	float levelMultiplier = Stats.CurrentLevel * 40;

	UE_LOG(LogTemp, Log, TEXT("RequiredExperience = %f"), levelPow + levelMultiplier);
	Stats.RequiredExperience = levelPow + levelMultiplier;
}
