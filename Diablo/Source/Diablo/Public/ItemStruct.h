// Diablo/Source/Diablo/Public/ItemStruct.h
#pragma once
#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "ItemStruct.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class ERarity : uint8
{
	Common    UMETA(DisplayName = "Common"),
	Rare      UMETA(DisplayName = "Rare"),
	Epic      UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FItemStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 ItemLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EEquipmentType EquipmentType = EEquipmentType::Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EWeaponType WeaponType = EWeaponType::Bow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	ERarity Rarity = ERarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	UStaticMesh* Mesh = nullptr;
};
