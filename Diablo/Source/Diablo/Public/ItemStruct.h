// Diablo/Source/Diablo/Public/ItemStruct.h
#pragma once
#include "CoreMinimal.h"
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

USTRUCT(BlueprintType)
struct FItemStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 Item_ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString Slug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	TArray<FString> CategorySlugs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 ItemLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 DamageMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	int32 Price = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EEquipmentType EquipmentType = EEquipmentType::Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EWeaponType WeaponType = EWeaponType::Bow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	ERarity Rarity = ERarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString IconPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Struct")
	FString MeshPath;

	bool operator==(const FItemStruct& Other) const
	{
		return Item_ID == Other.Item_ID;
	}
};
