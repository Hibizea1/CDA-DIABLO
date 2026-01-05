// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon      UMETA(DisplayName = "Weapon"),
	Head      UMETA(DisplayName = "Head"),
	Chest UMETA(DisplayName = "Chest"),
	Leggings UMETA(DisplayName = "Leggings"),
	Bots UMETA(DisplayName = "Bots")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Bow      UMETA(DisplayName = "Bow"),
	Sword      UMETA(DisplayName = "Sword"),
	Axe UMETA(DisplayName = "Axe"),
	Shield UMETA(DisplayName = "Shield")
};


UCLASS()
class DIABLO_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Models")
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Models")
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EEquipmentType EquipmentType = EEquipmentType::Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EWeaponType WeaponType = EWeaponType::Bow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Intelligence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float Mana;
};
