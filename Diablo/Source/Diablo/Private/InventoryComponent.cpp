// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "FPlayerData.h"
#include "UItemSaveGame.h"
#include "UJsonHelper.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Called every frame
static FItemStruct ConvertSavedToRuntime(const FItemSave& Saved)
{
	FItemStruct Out;

	// Charger la save d'items (UItemSaveGame) via le helper
	UItemSaveGame* ItemSaveGame = UJsonHelper::LoadItemsSaveGame();
	if (!ItemSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("ConvertSavedToRuntime: Failed to load ItemSaveGame"));
		return Out; // valeurs par défaut
	}

	// Récupérer les données de l'item par son ID
	FItemData ItemData = ItemSaveGame->GetItemByID(Saved.item_id);

	// Si ItemData est vide (ID non trouvé), log et retourner une structure par défaut
	if (ItemData.ItemID == 0 && ItemData.Name.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ConvertSavedToRuntime: Item with ID %d not found in ItemSaveGame"), Saved.item_id);
		return Out;
	}

	// Mapper les champs de FItemData -> FItemStruct
	Out.Item_ID = ItemData.ItemID != 0 ? ItemData.ItemID : Saved.item_id;
	Out.Name = ItemData.Name;
	// Utiliser le lvl sauvegardé si présent sinon garder ItemLevel par défaut
	Out.ItemLevel = (Saved.lvl > 0) ? Saved.lvl : 1;
	// DamageMultiplier n'existe pas dans ItemData; on peut dériver d'ItemLevel ou laisser par défaut
	Out.DamageMultiplier = 1;
	Out.EquipmentType = ItemData.Type;
	Out.WeaponType = ItemData.WeaponType;
	// ItemData.Rarity est un int32 ; convertir en ERarity en clampant
	int32 R = ItemData.Rarity;
	if (Saved.rarity > 0)
	{
		R = Saved.rarity; // préférer la rareté sauvegardée si fournie
	}
	switch (R)
	{
	case 0:
		Out.Rarity = ERarity::Common; break;
	case 1:
		Out.Rarity = ERarity::Rare; break;
	case 2:
		Out.Rarity = ERarity::Epic; break;
	case 3:
		Out.Rarity = ERarity::Legendary; break;
	default:
		Out.Rarity = ERarity::Common; break;
	}

	Out.IconPath = ItemData.IconPath;
	Out.MeshPath = ItemData.MeshPath;

	// Note: Quantity et d'autres propriétés de sauvegarde ne sont pas dans FItemStruct actuel.
	// Si vous souhaitez inclure Quantity, ajoutez un champ correspondant à FItemStruct.

	return Out;
}

void UInventoryComponent::SetInventory(FPlayerData playerData)
{
	Items.Empty();
	Equipments.Empty();

	for (const FItemSave& Saved : playerData.Items)
	{
		Items.Add(ConvertSavedToRuntime(Saved));
	}

	for (const FItemSave& Saved : playerData.Equipment)
	{
		Equipments.Add(ConvertSavedToRuntime(Saved));
	}
}

void UInventoryComponent::AddItem(EInventoryType inventoryType, const FItemStruct& NewItem)
{
	switch (inventoryType)
	{
	case Inventory:
		Items.Add(NewItem);
		break;
	case Equipment:
		Equipments.Add(NewItem);
		break;
	}
}

FItemStruct UInventoryComponent::GetItem(EInventoryType inventoryType, int32 index)
{
	switch (inventoryType)
	{
	case Inventory:
		if (Items.IsValidIndex(index))
		{
			return Items[index];
		}
		break;
	case Equipment:
		if (Equipments.IsValidIndex(index)) 
		{
			return Equipments[index];
		}
		break;
	default:
		return FItemStruct();
	}

	return FItemStruct();
}

void UInventoryComponent::RemoveItem(EInventoryType inventoryType, FItemStruct ItemToRemove)
{
	switch (inventoryType)
	{
	case Inventory:
		if (Items.Contains(ItemToRemove))
		{
			Items.Remove(ItemToRemove);
		}
		break;
	case Equipment:
		if (Equipments.Contains(ItemToRemove))
		{
			Equipments.Remove(ItemToRemove);
		}
		break;
	}
}
