// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies.h"
#include "CharacterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items.h"
#include "MapGenerator.h"
#include "UJsonHelper.h"
#include "UItemSaveGame.h"
#include "Engine/World.h"

AEnemies::AEnemies()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemies::BeginPlay()
{
	Super::BeginPlay();
}

int32 AEnemies::GetPlayerLevel() const
{
	if (!GetWorld())
	{
		return 1;
	}

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerChar)
	{
		return 1;
	}

	if (UCharacterComponent* CharComp = PlayerChar->FindComponentByClass<UCharacterComponent>())
	{
		return FMath::Max(1, CharComp->GetStats().CurrentLevel);
	}

	return 1;
}

int32 AEnemies::GetZoneLevel() const
{
	// Niveau zone = moyenne du range biome courant
	const FMapData& MapData = AMapGenerator::Get().CurrentMapData;
	const int32 MinZone = FMath::Max(1, MapData.Biome.LevelMin);
	const int32 MaxZone = FMath::Max(MinZone, MapData.Biome.LevelMax);
	return (MinZone + MaxZone) / 2;
}

int32 AEnemies::GetMonsterLevel() const
{
	const int32 MinMob = FMath::Max(1, Enemies.LevelMin);
	const int32 MaxMob = FMath::Max(MinMob, Enemies.LevelMax);
	return (MinMob + MaxMob) / 2;
}

int32 AEnemies::GetRarityLevelBonus(const ERarity Rarity) const
{
	switch (Rarity)
	{
	case ERarity::Common: return 0;
	case ERarity::Rare: return 1;
	case ERarity::Epic: return 3;
	case ERarity::Legendary: return 5;
	default: return 0;
	}
}

int32 AEnemies::CalculateLevel(const ERarity& Rarity) const
{
	const int32 PlayerLevel = GetPlayerLevel();
	const int32 ZoneLevel = GetZoneLevel();
	const int32 MonsterLevel = GetMonsterLevel();

	const float Weighted =
		(PlayerLevel * 0.5f) +
		(ZoneLevel * 0.3f) +
		(MonsterLevel * 0.2f);

	const int32 BaseLevel = FMath::RoundToInt(Weighted);
	const int32 RandomDelta = FMath::RandRange(-2, 2);
	const int32 RarityBonus = GetRarityLevelBonus(Rarity);

	UE_LOG(LogTemp, Log, TEXT("Item Level : %d "), FMath::Clamp(BaseLevel + RandomDelta + RarityBonus, 1, 100));
	return FMath::Clamp(BaseLevel + RandomDelta + RarityBonus, 1, 100);
}

bool AEnemies::RollRarityAndCanSpawn(ERarity& OutRarity)
{
	const int32 RarityRoll = FMath::RandRange(1, 100);
	if (RarityRoll <= 60) OutRarity = ERarity::Common;
	else if (RarityRoll <= 85) OutRarity = ERarity::Rare;
	else if (RarityRoll <= 97) OutRarity = ERarity::Epic;
	else OutRarity = ERarity::Legendary;

	float SpawnChance = 0.0f;
	switch (OutRarity)
	{
	case ERarity::Common: SpawnChance = 80.0f;
		break;
	case ERarity::Rare: SpawnChance = 45.0f;
		break;
	case ERarity::Epic: SpawnChance = 20.0f;
		break;
	case ERarity::Legendary: SpawnChance = 7.0f;
		break;
	default: SpawnChance = 0.0f;
		break;
	}

	const float Roll = FMath::FRandRange(0.0f, 100.0f);
	UE_LOG(LogTemp, Log, TEXT("Item Rarity : %hhd "), OutRarity);
	return Roll <= SpawnChance;
}

void AEnemies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemies::SpawnItem()
{
#pragma region Check And Get
	if (!GetWorld())
	{
		return;
	}

	if (!ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItem: ItemClass non défini"));
		return;
	}

	// Charge la save d'items
	UItemSaveGame* ItemSave = UJsonHelper::LoadItemsSaveGame();
	if (!ItemSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItem: impossible de charger la save d'items"));
		return;
	}
#pragma endregion

	ERarity RolledRarity = ERarity::Common;
	if (!RollRarityAndCanSpawn(RolledRarity))
	{
		UE_LOG(LogTemp, Log, TEXT("SpawnItem: drop rate failed"));
		return;
	}
	FItemData RandomItem = ItemSave->GetItemByID(Enemies.Drops[0].Item_ID);
	FItemStruct ConvertedItem = UJsonHelper::CreateNewItem(RandomItem, CalculateLevel(RolledRarity), RolledRarity);

	if (RandomItem.ItemID == 0 && RandomItem.Name.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItem: aucun item valide trouvé"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AItems* SpawnedItem = GetWorld()->SpawnActor<AItems>(
		ItemClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams
	);

	if (!SpawnedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItem: échec du spawn de l'item"));
		return;
	}

	SpawnedItem->InitItem(ConvertedItem);
}

void AEnemies::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
