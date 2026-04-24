// Fill out your copyright notice in the Description page of Project Settings.


#include "UJsonHelper.h"

#include "ItemStruct.h"
#include "JsonUtilities.h"
#include "MapDataSave.h"
#include "UHTTPRequest.h"
#include "UItemSaveGame.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "StructManage.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformMisc.h"


int32 UJsonHelper::StoredUserId = -1;

static bool HasRepeatedSpecialChar(const FString& Password, const FString& SpecialChars, int32 MaxAllowed = 1)
{
	TMap<TCHAR, int32> Counts;
	for (int32 i = 0; i < Password.Len(); ++i)
	{
		TCHAR C = Password[i];
		int32 DummyIndex = INDEX_NONE;
		if (SpecialChars.FindChar(C, DummyIndex))
		{
			int32& Count = Counts.FindOrAdd(C);
			++Count;
			if (Count > MaxAllowed)
			{
				return true;
			}
		}
	}
	return false;
}


int32 UJsonHelper::GetStoredUserId()
{
	return StoredUserId;
}

FString UJsonHelper::ParseMessage(const FString& JsonString)
{
	FString Result = TEXT("");
	FString Access_Token = "";
	FString Refresh_Token = "";

	TSharedPtr<FJsonValue> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
	{
		TSharedPtr<FJsonObject> Obj = Root->AsObject();
		if (Obj.IsValid())
		{
			Obj->TryGetStringField(TEXT("status"), Result);
			Obj->TryGetStringField(TEXT("access_token"), UUHTTPRequest::Access_Token);
			Obj->TryGetStringField(TEXT("refresh_token"), UUHTTPRequest::Refresh_Token);
		}
	}

	return Result;
}


UItemSaveGame* UJsonHelper::LoadItemsSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("ItemsSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("No ItemsSaveSlot found!"));
		return nullptr;
	}

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(TEXT("ItemsSaveSlot"), 0);
	UItemSaveGame* ItemSave = Cast<UItemSaveGame>(Loaded);

	if (!ItemSave)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast loaded save to UItemSaveGame"));
		return nullptr;
	}

	return ItemSave;
}

UMapDataSave* UJsonHelper::LoadMapDataSave()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("MapDataSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("No MapDataSaveSlot found!"));
		return nullptr;
	}

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(TEXT("MapDataSaveSlot"), 0);
	UMapDataSave* MapDataSave = Cast<UMapDataSave>(Loaded);

	if (!MapDataSave)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast loaded save to UItemSaveGame"));
		return nullptr;
	}

	return MapDataSave;
}

UMonsterAndBiomsSaveGame* UJsonHelper::LoadMonsterAndBiomesSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("MonsterAndBiomesSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("No MonsterAndBiomesSaveSlot found!"));
		return nullptr;
	}

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(TEXT("MonsterAndBiomesSaveSlot"), 0);
	UMonsterAndBiomsSaveGame* MonsterAndBiomesSave = Cast<UMonsterAndBiomsSaveGame>(Loaded);

	if (!MonsterAndBiomesSave)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast loaded save to MonsterAndBiomesSaveSlot"));
		return nullptr;
	}

	return MonsterAndBiomesSave;
}

void UJsonHelper::DebugPrintAllItems()
{
	UItemSaveGame* Save = LoadItemsSaveGame();
	if (!Save) return;

	for (auto& Pair : Save->ItemsByType)
	{
		const FString& WeaponType = Pair.Key;
		const FItemArrayWrapper& Wrapper = Pair.Value;

		UE_LOG(LogTemp, Log, TEXT("=== WeaponType: %s, Items Count: %d ==="), *WeaponType, Wrapper.Items.Num());

		for (const FItemStruct& Item : Wrapper.Items)
		{
			UE_LOG(LogTemp, Log, TEXT("Item ID:%d Name:%s Type:%hhd Rarity:%d Price:%d Mesh:%s"),
			       Item.Item_ID, *Item.Name, Item.EquipmentType, Item.Rarity, Item.Price, *Item.MeshPath);
		}
	}
}

TArray<FItemDataToSave> UJsonHelper::ParseInventoriesFromJson(const FString& JsonString)
{
	TArray<FItemDataToSave> Result;

	UE_LOG(LogTemp, Log, TEXT("Incoming JSON (raw): %s"), *JsonString);

	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("INVALID JSON FORMAT"));
		return Result;
	}

	const TArray<TSharedPtr<FJsonValue>>* JsonArray;

	if (!Root->TryGetArrayField(TEXT("Items"), JsonArray) || !Root->TryGetArrayField(TEXT("Equipment"), JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("'Items' field missing or not an array"));
		return Result;
	}

	if (!FJsonObjectConverter::JsonArrayToUStruct(*JsonArray, &Result, 0, 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert array to FItemSave"));
	}

	return Result;
}


FPlayerData UJsonHelper::ParsePlayerData(const FString& JsonString)
{
	FPlayerData Result = {};

	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON"));
		return Result;
	}

	// ============================
	//   CHARACTER (ARRAY)
	// ============================
	const TArray<TSharedPtr<FJsonValue>>* CharacterArray = nullptr;
	if (!RootObj->TryGetArrayField(TEXT("character"), CharacterArray) || CharacterArray->Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("'character' missing or empty"));
		return Result;
	}

	TSharedPtr<FJsonObject> CharacterObject = (*CharacterArray)[0]->AsObject();

	// Helper
	auto GetIntSafe = [&](const FString& FieldName, int32 Default = 0)
	{
		double Num = 0;
		if (CharacterObject->TryGetNumberField(FieldName, Num))
			return (int32)Num;
		FString Str;
		if (CharacterObject->TryGetStringField(FieldName, Str))
			return FCString::Atoi(*Str);
		return Default;
	};

	// --- Basic character stats ---
	Result.Level = GetIntSafe(TEXT("level"), 1);
	Result.Experience = GetIntSafe(TEXT("xp"), 0);
	Result.Gold = GetIntSafe(TEXT("gold"), 0);
	Result.stamina = GetIntSafe(TEXT("stamina"), 0);
	Result.health = GetIntSafe(TEXT("health"), 0);
	Result.mana = GetIntSafe(TEXT("mana"), 0);
	Result.intelligence = GetIntSafe(TEXT("intelligence"), 0);
	Result.strenght = GetIntSafe(TEXT("strenght"), 0);


	// ============================
	//   ITEMS = inventories
	// ============================
	const TArray<TSharedPtr<FJsonValue>>* InvArray = nullptr;
	if (RootObj->TryGetArrayField(TEXT("inventories"), InvArray))
	{
		FJsonObjectConverter::JsonArrayToUStruct(*InvArray, &Result.Items, 0, 0);
	}

	// ============================
	//   EQUIPMENT = equipment
	// ============================
	const TArray<TSharedPtr<FJsonValue>>* EquipArray = nullptr;
	if (RootObj->TryGetArrayField(TEXT("equipment"), EquipArray))
	{
		FJsonObjectConverter::JsonArrayToUStruct(*EquipArray, &Result.Equipment, 0, 0);
	}

	return Result;
}


FString UJsonHelper::CreateMessage(const FString& Username, const FString& Password, const FString& Email)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("username"), Username);

	const FString SpecialChars = TEXT("!@#$%^&*()-_+=[]{};:'\"\\|,<.>/?`~");

	if (!Password.IsEmpty() && !HasRepeatedSpecialChar(Password, SpecialChars, 1) && Password.Len() >= 10)
	{
		JsonObject->SetStringField(TEXT("password"), Password);
	}
	else
	{
		return TEXT("failed");
	}

	JsonObject->SetStringField(TEXT("email"), Email);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

FString UJsonHelper::CreateLoginMessage(const FString& Username, const FString& Password)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("username"), Username);
	JsonObject->SetStringField(TEXT("password"), Password);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}

static void DevSyncGameDataCommand();
static FAutoConsoleCommand CVar_DevUpdateBiomesAndMonsterDB(
	TEXT("DEV_DevSyncGameDataCommand"),
	TEXT("DEV ONLY - Sync biomes, monsters & items DataTable to database"),
	FConsoleCommandDelegate::CreateStatic(&DevSyncGameDataCommand)
);


FString UJsonHelper::BuildItemsJson(const TArray<FItemStruct*>& Rows)
{
	TArray<TSharedPtr<FJsonValue>> Items;

	for (const FItemStruct* Row : Rows)
	{
		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetNumberField(TEXT("id"), Row->Item_ID);
		Obj->SetStringField(TEXT("name"), Row->Name);
		Obj->SetStringField(TEXT("iconPath"), Row->IconPath);
		Obj->SetStringField(TEXT("meshPath"), Row->MeshPath);
		Obj->SetStringField(TEXT("type"), *UEnum::GetValueAsString(Row->EquipmentType));
		Obj->SetNumberField(TEXT("rarity"), Row->Rarity == ERarity::Common
			                                    ? 0
			                                    : Row->Rarity == ERarity::Rare
			                                    ? 1
			                                    : Row->Rarity == ERarity::Epic
			                                    ? 2
			                                    : 3);
		Obj->SetNumberField(TEXT("price"), Row->ItemLevel);
		Obj->SetNumberField(TEXT("damageMultiplier"), Row->DamageMultiplier);
		Obj->SetStringField(TEXT("weaponType"), *UEnum::GetValueAsString(Row->WeaponType));


		Items.Add(MakeShared<FJsonValueObject>(Obj));
	}

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetArrayField(TEXT("items"), Items);

	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

	return Output;
}

UItemSaveGame* UJsonHelper::LoadItemsSaveGameBP()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ItemsSaveSlot"), 0))
	{
		USaveGame* Save = UGameplayStatics::LoadGameFromSlot(TEXT("ItemsSaveSlot"), 0);
		return Cast<UItemSaveGame>(Save);
	}

	return nullptr;
}



static void SerializeAndSend(TSharedPtr<FJsonObject> Root)
{
	// Serialize + send
	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Log, TEXT("DevSyncGameData payload: %s"), *Output);

	FString SyncToken = FPlatformMisc::GetEnvironmentVariable(TEXT("WIKI_SYNC_TOKEN"));
	// Envoi via helper existant (garde la logique existante). UUHTTPRequest se charge de la requete HTTP.
	UUHTTPRequest::SyncWikiGameData(Output, SyncToken);
}

static bool GetDataTable(UDataTable*& ItemsTable, UDataTable*& BiomesTable, UDataTable*& MonstersTable)
{
	ItemsTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/Blueprint/Data/ItemsData/DT_Items.DT_Items")
	);

	BiomesTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/Blueprint/Data/EnemiesData/BiomesDataTable.BiomesDataTable")
	);

	MonstersTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/Blueprint/Data/EnemiesData/MonsterDataTable.MonsterDataTable")
	);

	if (!ItemsTable && !BiomesTable && !MonstersTable)
	{
		UE_LOG(LogTemp, Error, TEXT("No DataTable found (items/biomes/monsters)."));
		return true;
	}
	return false;
}

static void DevSyncGameDataCommand()
{
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("DEV: Unified sync Items + Biomes + Monsters"));

	UDataTable* ItemsTable;
	UDataTable* BiomesTable;
	UDataTable* MonstersTable;
	if (GetDataTable(ItemsTable, BiomesTable, MonstersTable)) return;

	TArray<FItemStruct*> ItemRows;
	TArray<FBiomeStruct*> BiomeRows;
	TArray<FMonsterStruct*> MonsterRows;

	if (ItemsTable)
	{
		ItemsTable->GetAllRows(TEXT("DEV_SYNC"), ItemRows);
	}
	if (BiomesTable)
	{
		BiomesTable->GetAllRows(TEXT("DEV_SYNC"), BiomeRows);
	}
	if (MonstersTable)
	{
		MonstersTable->GetAllRows(TEXT("DEV_SYNC"), MonsterRows);
	}

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

	auto MakeSlugLocal = [](const FString& In) -> FString
	{
		FString Out = In.ToLower();
		Out = Out.Replace(TEXT(" "), TEXT("-"));
		Out = Out.Replace(TEXT("_"), TEXT("-"));
		FString Clean;
		Clean.Reserve(Out.Len());
		for (TCHAR C : Out)
		{
			const bool bAlphaNum = (C >= 'a' && C <= 'z') || (C >= '0' && C <= '9');
			if (bAlphaNum || C == '-')
			{
				Clean.AppendChar(C);
			}
		}
		while (Clean.Contains(TEXT("--")))
		{
			Clean = Clean.Replace(TEXT("--"), TEXT("-"));
		}
		Clean.RemoveFromStart(TEXT("-"));
		Clean.RemoveFromEnd(TEXT("-"));
		return Clean.IsEmpty() ? TEXT("unknown") : Clean;
	};

#pragma region Items And Item Categories
	TArray<TSharedPtr<FJsonValue>> ItemsArr;
	TSet<FString> ItemCategorySet;
	TArray<TSharedPtr<FJsonValue>> ItemCategoryLinksArr;

	for (FItemStruct* I : ItemRows)
	{
		if (!I) continue;

		const FString ItemName = I->Name;
		const FString ItemSlug = MakeSlugLocal(ItemName);

		TSharedPtr<FJsonObject> ItemObj = MakeShared<FJsonObject>();
		ItemObj->SetNumberField(TEXT("item_id"), I->Item_ID);
		ItemObj->SetNumberField(TEXT("id"), I->Item_ID); // compat ancien
		ItemObj->SetStringField(TEXT("slug"), ItemSlug);
		ItemObj->SetStringField(TEXT("name"), ItemName);
		ItemObj->SetStringField(TEXT("description"), TEXT(""));
		ItemObj->SetNumberField(TEXT("price"), 1);
		ItemObj->SetNumberField(TEXT("damageMultiplier"), I->DamageMultiplier);
		ItemObj->SetStringField(TEXT("meshPath"), I->MeshPath);
		ItemObj->SetStringField(TEXT("iconPath"), I->IconPath);
		ItemObj->SetStringField(
			TEXT("type"), StaticEnum<EEquipmentType>()->GetNameStringByValue((int64)I->EquipmentType));
		ItemObj->SetStringField(
			TEXT("weaponType"), StaticEnum<EWeaponType>()->GetNameStringByValue((int64)I->WeaponType));
		ItemObj->SetStringField(TEXT("rarity"), StaticEnum<ERarity>()->GetNameStringByValue((int64)I->Rarity));

		ItemsArr.Add(MakeShared<FJsonValueObject>(ItemObj));

		// déduire catégories d'item depuis le type
		TArray<TSharedPtr<FJsonValue>> CatSlugs;
		const FString TypeStr = StaticEnum<EEquipmentType>()->GetNameStringByValue((int64)I->EquipmentType).ToLower();

		if (TypeStr.Contains(TEXT("weapon")))
		{
			ItemCategorySet.Add(TEXT("weapon"));
			CatSlugs.Add(MakeShared<FJsonValueString>(TEXT("weapon")));
		}
		else if (
			TypeStr.Contains(TEXT("head")) ||
			TypeStr.Contains(TEXT("chest")) ||
			TypeStr.Contains(TEXT("pants")) ||
			TypeStr.Contains(TEXT("boots"))
		)
		{
			ItemCategorySet.Add(TEXT("armor"));
			CatSlugs.Add(MakeShared<FJsonValueString>(TEXT("armor")));
		}
		else
		{
			ItemCategorySet.Add(TEXT("consumable"));
			CatSlugs.Add(MakeShared<FJsonValueString>(TEXT("consumable")));
		}

		TSharedPtr<FJsonObject> LinkObj = MakeShared<FJsonObject>();
		LinkObj->SetNumberField(TEXT("item_id"), I->Item_ID);
		LinkObj->SetArrayField(TEXT("category_slugs"), CatSlugs);
		ItemCategoryLinksArr.Add(MakeShared<FJsonValueObject>(LinkObj));
	}

	Root->SetArrayField(TEXT("items"), ItemsArr);

	// item_categories
	{
		TArray<TSharedPtr<FJsonValue>> ItemCategoriesArr;
		for (const FString& S : ItemCategorySet)
		{
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetStringField(TEXT("slug"), S);
			Obj->SetStringField(TEXT("name"), S);
			Obj->SetStringField(TEXT("description"), TEXT(""));
			ItemCategoriesArr.Add(MakeShared<FJsonValueObject>(Obj));
		}
		Root->SetArrayField(TEXT("item_categories"), ItemCategoriesArr);
	}

	Root->SetArrayField(TEXT("item_category_links"), ItemCategoryLinksArr);
#pragma endregion

#pragma region Biomes and Biomes Categories
	// -------------------------
	// 2) BIOMES + biome_categories
	// -------------------------
	TArray<TSharedPtr<FJsonValue>> BiomesArr;
	TSet<FString> BiomeCategorySet;

	for (FBiomeStruct* B : BiomeRows)
	{
		if (!B) continue;

		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		const FString BiomeSlug = B->Slug.Slug.IsEmpty() ? MakeSlugLocal(B->Name) : B->Slug.Slug;
		Obj->SetStringField(TEXT("slug"), BiomeSlug);
		Obj->SetStringField(TEXT("name"), B->Name);
		Obj->SetStringField(TEXT("description"), B->Description);
		Obj->SetNumberField(TEXT("level_min"), B->LevelMin);
		Obj->SetNumberField(TEXT("level_max"), B->LevelMax);

		TArray<TSharedPtr<FJsonValue>> CatArr;
		for (const FSlugStruct& Cat : B->CategorySlugs)
		{
			if (!Cat.Slug.IsEmpty())
			{
				CatArr.Add(MakeShared<FJsonValueString>(Cat.Slug));
				BiomeCategorySet.Add(Cat.Slug);
			}
		}
		Obj->SetArrayField(TEXT("categories"), CatArr);

		BiomesArr.Add(MakeShared<FJsonValueObject>(Obj));
	}
	Root->SetArrayField(TEXT("biomes"), BiomesArr);

	// biome_categories
	{
		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FString& S : BiomeCategorySet)
		{
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetStringField(TEXT("slug"), S);
			Obj->SetStringField(TEXT("name"), S);
			Obj->SetStringField(TEXT("description"), TEXT(""));
			Arr.Add(MakeShared<FJsonValueObject>(Obj));
		}
		Root->SetArrayField(TEXT("biome_categories"), Arr);
	}

#pragma endregion

#pragma region Monsters and Monsters Categories
	// -------------------------
	// 3) MONSTERS + LOOTS + monster_categories
	// -------------------------
	TArray<TSharedPtr<FJsonValue>> MonstersArr;
	TArray<TSharedPtr<FJsonValue>> LootsArr;
	TSet<FString> MonsterCategorySet;

	for (FMonsterStruct* M : MonsterRows)
	{
		if (!M) continue;

		TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
		const FString MonsterSlug = M->Slug.Slug.IsEmpty() ? MakeSlugLocal(M->Name) : M->Slug.Slug;
		Obj->SetStringField(TEXT("slug"), MonsterSlug);
		Obj->SetStringField(TEXT("name"), M->Name);
		Obj->SetStringField(TEXT("description"), M->Description);
		Obj->SetNumberField(TEXT("level_min"), M->LevelMin);
		Obj->SetNumberField(TEXT("level_max"), M->LevelMax);
		Obj->SetStringField(TEXT("difficulty"), M->Difficulty);
		Obj->SetNumberField(TEXT("drop_xp"), M->EnnemiData.DeathData.DropExp);
		Obj->SetNumberField(TEXT("drop_gold"), M->EnnemiData.DeathData.DropGold);
		Obj->SetNumberField(TEXT("stamina"), M->EnnemiData.stamina);
		Obj->SetNumberField(TEXT("health"), M->EnnemiData.health);
		Obj->SetNumberField(TEXT("mana"), M->EnnemiData.mana);
		Obj->SetNumberField(TEXT("strength"), M->EnnemiData.strenght);

		TArray<TSharedPtr<FJsonValue>> CatArr;
		for (const FSlugStruct& Cat : M->CategorySlugs)
		{
			if (!Cat.Slug.IsEmpty())
			{
				CatArr.Add(MakeShared<FJsonValueString>(Cat.Slug));
				MonsterCategorySet.Add(Cat.Slug);
			}
		}
		Obj->SetArrayField(TEXT("categories"), CatArr);

		MonstersArr.Add(MakeShared<FJsonValueObject>(Obj));

		// drops -> loots
		for (const FMonsterDropStruct& D : M->Drops)
		{
			TSharedPtr<FJsonObject> LootObj = MakeShared<FJsonObject>();
			LootObj->SetStringField(TEXT("monster_slug"), MonsterSlug);
			LootObj->SetNumberField(TEXT("item_id"), D.Item_ID); // 0 autorise
			LootObj->SetStringField(TEXT("biome_slug"), D.BiomeSlug);
			LootObj->SetNumberField(TEXT("drop_rate"), D.DropRate);
			LootObj->SetNumberField(TEXT("min_qty"), D.MinQty);
			LootObj->SetNumberField(TEXT("max_qty"), D.MaxQty);
			LootObj->SetStringField(TEXT("notes"), D.Notes);
			LootsArr.Add(MakeShared<FJsonValueObject>(LootObj));
		}
	}

	Root->SetArrayField(TEXT("monsters"), MonstersArr);
	Root->SetArrayField(TEXT("loots"), LootsArr);

	// monster_categories
	{
		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FString& S : MonsterCategorySet)
		{
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			Obj->SetStringField(TEXT("slug"), S);
			Obj->SetStringField(TEXT("name"), S);
			Obj->SetStringField(TEXT("description"), TEXT(""));
			Arr.Add(MakeShared<FJsonValueObject>(Obj));
		}
		Root->SetArrayField(TEXT("monster_categories"), Arr);
	}

	// spawns vides -> backend genere auto via categories
	Root->SetBoolField(TEXT("spawn_from_category_slugs"), true);
	Root->SetArrayField(TEXT("spawns"), TArray<TSharedPtr<FJsonValue>>());

	// deletes par defaut vides
	{
		TSharedPtr<FJsonObject> DeletesObj = MakeShared<FJsonObject>();
		DeletesObj->SetArrayField(TEXT("biomes"), TArray<TSharedPtr<FJsonValue>>());
		DeletesObj->SetArrayField(TEXT("monsters"), TArray<TSharedPtr<FJsonValue>>());
		Root->SetObjectField(TEXT("deletes"), DeletesObj);
	}
#pragma endregion


	SerializeAndSend(Root);

#else
	UE_LOG(LogTemp, Error, TEXT("Dev sync disabled in Shipping"));
#endif
}


TMap<FString, FItemArrayWrapper> UJsonHelper::ParseItemsJSON(const FString& JsonContent)
{
	TMap<FString, FItemArrayWrapper> ItemsByType;

	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ParseItemsJSON: Failed to parse JSON"));
		return ItemsByType;
	}

	// Root object must contain "items"
	TSharedPtr<FJsonObject> ItemsField = RootObj->GetObjectField(TEXT("items"));
	if (!ItemsField.IsValid()) return ItemsByType;

	for (auto& Pair : ItemsField->Values)
	{
		FString WeaponType = Pair.Key;
		const TArray<TSharedPtr<FJsonValue>>* Arr;
		if (!Pair.Value->TryGetArray(Arr)) continue;

		FItemArrayWrapper Wrapper;

		for (const TSharedPtr<FJsonValue>& Val : *Arr)
		{
			if (!Val.IsValid() || Val->Type != EJson::Object) continue;

			TSharedPtr<FJsonObject> Obj = Val->AsObject();
			if (!Obj.IsValid()) continue;

			FItemStruct Item;
			Item.Name = Obj->GetStringField(TEXT("name"));
			FString TypeString = Obj->GetStringField(TEXT("type")); // ex: "EItemType::Consumable"
			FString TypeName;
			if (TypeString.Split(TEXT("::"), nullptr, &TypeName))
			{
				// TypeName = "Consumable"
			}
			else
			{
				TypeName = TypeString;
			}

			UEnum* TypeEnumPtr = StaticEnum<EEquipmentType>();
			if (TypeEnumPtr)
			{
				EEquipmentType TypeEnum = static_cast<EEquipmentType>(TypeEnumPtr->GetValueByName(FName(*TypeName)));
				Item.EquipmentType = TypeEnum;
			}
			FString RarityString = Obj->HasField(TEXT("rarity"))
				                       ? Obj->GetStringField(TEXT("rarity"))
				                       : TEXT("Common");

			if (UEnum* RarityEnum = StaticEnum<ERarity>())
			{
				const int64 EnumValue = RarityEnum->GetValueByName(FName(*RarityString));
				if (EnumValue != INDEX_NONE)
				{
					Item.Rarity = static_cast<ERarity>(EnumValue);
				}
				else
				{
					Item.Rarity = ERarity::Common;
				}
			}
 
			Item.Item_ID = FCString::Atoi(*Obj->GetStringField(TEXT("Item_ID")));
			Item.Price = FCString::Atoi(*Obj->GetStringField(TEXT("Price")));
			Item.MeshPath = Obj->HasField(TEXT("mesh_path")) ? Obj->GetStringField(TEXT("mesh_path")) : FString();
			Item.IconPath = Obj->HasField(TEXT("imagePath")) ? Obj->GetStringField(TEXT("imagePath")) : FString();
			if (Obj->HasField(TEXT("weaponType")))
			{
				FString WeaponString = Obj->GetStringField(TEXT("weaponType")); // ex: "EWeaponType::Sword"

				// On récupère juste le nom après "::"
				FString WeaponName;
				if (WeaponString.Split(TEXT("::"), nullptr, &WeaponName))
				{
					// WeaponName = "Sword"
				}
				else
				{
					WeaponName = WeaponString;
				}

				// Convert string -> enum
				if (UEnum* EnumPtr = StaticEnum<EWeaponType>())
				{
					Item.WeaponType = static_cast<EWeaponType>(EnumPtr->GetValueByName(FName(*WeaponName)));
				}
			}

			Wrapper.Items.Add(Item);
		}

		if (Wrapper.Items.Num() > 0)
		{
			ItemsByType.Add(WeaponType, Wrapper);
		}
	}

	return ItemsByType;
}

// cpp
TMap<FString, FMonsterAndBiomesWrapper> UJsonHelper::ParseBiomesAndMonstersJSON(const FString& JsonContent)
{
	TMap<FString, FMonsterAndBiomesWrapper> Result;

	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);

	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ParseBiomesAndMonstersJSON: Failed to parse JSON"));
		return Result;
	}

	auto ParseIntField = [](TSharedPtr<FJsonObject> Obj, const FString& FieldName, int32 Default = 0) -> int32
	{
		double Num = 0;
		if (Obj->TryGetNumberField(FieldName, Num))
			return (int32)Num;
		FString Str;
		if (Obj->TryGetStringField(FieldName, Str))
			return FCString::Atoi(*Str);
		return Default;
	};

	// Biomes
	{
		const TArray<TSharedPtr<FJsonValue>>* BiomesArr = nullptr;
		if (RootObj->TryGetArrayField(TEXT("biomes"), BiomesArr) && BiomesArr)
		{
			FMonsterAndBiomesWrapper Wrapper;
			for (const TSharedPtr<FJsonValue>& Val : *BiomesArr)
			{
				if (!Val.IsValid() || Val->Type != EJson::Object) continue;
				TSharedPtr<FJsonObject> Obj = Val->AsObject();
				if (!Obj.IsValid()) continue;

				FBiomeStruct B = {};
				// slug
				if (Obj->HasField(TEXT("slug")))
				{
					B.Slug.Slug = Obj->GetStringField(TEXT("slug"));
				}
				// name / description
				if (Obj->HasField(TEXT("name"))) B.Name = Obj->GetStringField(TEXT("name"));
				if (Obj->HasField(TEXT("description"))) B.Description = Obj->GetStringField(TEXT("description"));
				// levels
				B.LevelMin = ParseIntField(Obj, TEXT("level_min"), 0);
				B.LevelMax = ParseIntField(Obj, TEXT("level_max"), 0);

				// categories array (optional)
				const TArray<TSharedPtr<FJsonValue>>* CatArr = nullptr;
				if (Obj->TryGetArrayField(TEXT("categories"), CatArr) && CatArr)
				{
					B.CategorySlugs.Empty();
					for (const TSharedPtr<FJsonValue>& CVal : *CatArr)
					{
						if (!CVal.IsValid() || CVal->Type != EJson::String) continue;
						FSlugStruct S;
						S.Slug = CVal->AsString();
						B.CategorySlugs.Add(S);
					}
				}

				Wrapper.Biomes.Add(B);
			}

			if (Wrapper.Biomes.Num() > 0)
			{
				Result.Add(TEXT("biomes"), Wrapper);
			}
		}
	}

	// Monsters
	{
		const TArray<TSharedPtr<FJsonValue>>* MonstersArr = nullptr;
		if (RootObj->TryGetArrayField(TEXT("monsters"), MonstersArr) && MonstersArr)
		{
			FMonsterAndBiomesWrapper Wrapper;
			for (const TSharedPtr<FJsonValue>& Val : *MonstersArr)
			{
				if (!Val.IsValid() || Val->Type != EJson::Object) continue;
				TSharedPtr<FJsonObject> Obj = Val->AsObject();
				if (!Obj.IsValid()) continue;

				FMonsterStruct M = {};
				// slug
				if (Obj->HasField(TEXT("slug")))
				{
					M.Slug.Slug = Obj->GetStringField(TEXT("slug"));
				}
				// basic fields
				if (Obj->HasField(TEXT("name"))) M.Name = Obj->GetStringField(TEXT("name"));
				if (Obj->HasField(TEXT("description"))) M.Description = Obj->GetStringField(TEXT("description"));
				M.LevelMin = ParseIntField(Obj, TEXT("level_min"), 0);
				M.LevelMax = ParseIntField(Obj, TEXT("level_max"), 0);
				if (Obj->HasField(TEXT("difficulty"))) M.Difficulty = Obj->GetStringField(TEXT("difficulty"));
				// drops
				const TArray<TSharedPtr<FJsonValue>>* DropArray = nullptr;
				if (Obj->TryGetArrayField(TEXT("drops"), DropArray) && DropArray)
				{
					M.Drops.Empty();

					for (const TSharedPtr<FJsonValue>& DVal : *DropArray)
					{
						if (!DVal.IsValid() || DVal->Type != EJson::Object)
						{
							continue;
						}

						TSharedPtr<FJsonObject> DropObj = DVal->AsObject();
						if (!DropObj.IsValid())
						{
							continue;
						}

						FMonsterDropStruct Drop;

						auto ParseIntFieldLocal = [](const TSharedPtr<FJsonObject>& InObj, const FString& FieldName,
						                             int32 DefaultValue = 0) -> int32
						{
							double Num = 0.0;
							if (InObj->TryGetNumberField(FieldName, Num))
							{
								return (int32)Num;
							}

							FString Str;
							if (InObj->TryGetStringField(FieldName, Str))
							{
								return FCString::Atoi(*Str);
							}

							return DefaultValue;
						};

						auto ParseFloatFieldLocal = [](const TSharedPtr<FJsonObject>& InObj, const FString& FieldName,
						                               float DefaultValue = 0.f) -> float
						{
							double Num = 0.0;
							if (InObj->TryGetNumberField(FieldName, Num))
							{
								return (float)Num;
							}

							FString Str;
							if (InObj->TryGetStringField(FieldName, Str))
							{
								return FCString::Atof(*Str);
							}

							return DefaultValue;
						};

						Drop.Item_ID = ParseIntFieldLocal(DropObj, TEXT("item_id"), 0);
						Drop.BiomeSlug = DropObj->HasField(TEXT("biome_slug"))
							                 ? DropObj->GetStringField(TEXT("biome_slug"))
							                 : FString();
						Drop.DropRate = ParseFloatFieldLocal(DropObj, TEXT("drop_rate"), 0.f);
						Drop.MinQty = ParseIntFieldLocal(DropObj, TEXT("min_qty"), 1);
						Drop.MaxQty = ParseIntFieldLocal(DropObj, TEXT("max_qty"), 1);
						Drop.Notes = DropObj->HasField(TEXT("notes"))
							             ? DropObj->GetStringField(TEXT("notes"))
							             : FString();

						M.Drops.Add(Drop);
					}
				}

				// optional: categories
				const TArray<TSharedPtr<FJsonValue>>* CatArr = nullptr;
				if (Obj->TryGetArrayField(TEXT("categories"), CatArr) && CatArr)
				{
					M.CategorySlugs.Empty();
					for (const TSharedPtr<FJsonValue>& CVal : *CatArr)
					{
						if (!CVal.IsValid() || CVal->Type != EJson::String) continue;
						FSlugStruct S;
						S.Slug = CVal->AsString();
						M.CategorySlugs.Add(S);
					}
				}

				if (Obj->HasField(TEXT("drop_xp")))
					M.EnnemiData.DeathData.DropExp = ParseIntField(
						Obj, TEXT("drop_xp"), 0);
				if (Obj->HasField(TEXT("drop_gold")))
					M.EnnemiData.DeathData.DropGold = ParseIntField(
						Obj, TEXT("drop_gold"), 0);
				if (Obj->HasField(TEXT("stamina"))) M.EnnemiData.stamina = ParseIntField(Obj, TEXT("stamina"), 0);
				if (Obj->HasField(TEXT("health"))) M.EnnemiData.health = ParseIntField(Obj, TEXT("health"), 0);
				if (Obj->HasField(TEXT("strength"))) M.EnnemiData.strenght = ParseIntField(Obj, TEXT("strength"), 0);
				if (Obj->HasField(TEXT("mana"))) M.EnnemiData.mana = ParseIntField(Obj, TEXT("mana"), 0);

				Wrapper.Monster.Add(M);
			}

			if (Wrapper.Monster.Num() > 0)
			{
				Result.Add(TEXT("monsters"), Wrapper);
			}
		}
	}

	return Result;
}

void UJsonHelper::SaveItemsToSaveGame(const TMap<FString, FItemArrayWrapper>& ItemsByType)
{
	UItemSaveGame* SaveGame = Cast<UItemSaveGame>(UGameplayStatics::CreateSaveGameObject(UItemSaveGame::StaticClass()));
	if (!SaveGame) return;

	SaveGame->ItemsByType = ItemsByType;

	bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("ItemsSaveSlot"), 0);
	UE_LOG(LogTemp, Log, TEXT("Items saved to .sav: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
}

void UJsonHelper::SaveBiomesAndMonsterToSaveGame(const TMap<FString, FMonsterAndBiomesWrapper>& MonsterAndBiomesType)
{
	UMonsterAndBiomsSaveGame* SaveGame = Cast<UMonsterAndBiomsSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMonsterAndBiomsSaveGame::StaticClass()));
	if (!SaveGame) return;

	SaveGame->MonsterAndBiomesByType = MonsterAndBiomesType;

	bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("MonsterAndBiomesSaveSlot"), 0);
	SaveMapData(MonsterAndBiomesType);
	UE_LOG(LogTemp, Log, TEXT("Monster And Biomes saved to .sav: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
}


void UJsonHelper::SaveMapData(const TMap<FString, FMonsterAndBiomesWrapper>& Map)
{
	UMapDataSave* SaveGame = Cast<UMapDataSave>(UGameplayStatics::CreateSaveGameObject(UMapDataSave::StaticClass()));
	if (!SaveGame) return;

	auto GetWrapperByKey = [&Map](const TCHAR* ExpectedKey) -> const FMonsterAndBiomesWrapper*
	{
		for (const TPair<FString, FMonsterAndBiomesWrapper>& Pair : Map)
		{
			if (Pair.Key.Equals(ExpectedKey, ESearchCase::IgnoreCase))
			{
				return &Pair.Value;
			}
		}

		return nullptr;
	};

	const FMonsterAndBiomesWrapper* BiomesWrapper = GetWrapperByKey(TEXT("biomes"));
	if (!BiomesWrapper || BiomesWrapper->Biomes.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveMapData: no biome data to serialize"));
		return;
	}

	const FMonsterAndBiomesWrapper* MonstersWrapper = GetWrapperByKey(TEXT("monsters"));
	const TArray<FMonsterStruct>* Monsters = MonstersWrapper ? &MonstersWrapper->Monster : nullptr;

	auto HasCommonCategory = [](const TArray<FSlugStruct>& A, const TArray<FSlugStruct>& B) -> bool
	{
		for (const FSlugStruct& SA : A)
		{
			if (SA.Slug.IsEmpty())
			{
				continue;
			}

			for (const FSlugStruct& SB : B)
			{
				if (!SB.Slug.IsEmpty() && SA.Slug.Equals(SB.Slug, ESearchCase::IgnoreCase))
				{
					return true;
				}
			}
		}

		return false;
	};

	SaveGame->MapDatas.Empty();

	for (const FBiomeStruct& Biome : BiomesWrapper->Biomes)
	{
		FMapData Data = {};
		Data.Biome = Biome;

		const int32 LevelSpan = FMath::Max(1, Biome.LevelMax - Biome.LevelMin + 1);
		Data.RoomsAmmount = FMath::Clamp(6 + (LevelSpan / 4), 6, 22);
		Data.HMax = FMath::Clamp(3 + (LevelSpan / 12), 3, 9);
		Data.FloorMax = FMath::Clamp(6 + (LevelSpan / 5), 6, 24);
		Data.Height = FMath::Clamp(28 + LevelSpan, 24, 96);
		Data.Width = FMath::Clamp(28 + LevelSpan, 24, 96);
		Data.bHasWall = true;

		int32 MatchingMonsters = 0;
		Data.Enemies.Empty();

		if (Monsters)
		{
			for (const FMonsterStruct& Monster : *Monsters)
			{
				const bool bLevelOverlap = Monster.LevelMin <= Biome.LevelMax && Monster.LevelMax >= Biome.LevelMin;
				const bool bCategoryMatch = HasCommonCategory(Biome.CategorySlugs, Monster.CategorySlugs);

				if (bLevelOverlap || bCategoryMatch)
				{
					Data.Enemies.Add(Monster);
				}
			}
		}

		if (Data.Enemies.Num() == 0 && Monsters && Monsters->Num() > 0)
		{
			const int32 FallbackIndex = FMath::RandRange(0, Monsters->Num() - 1);
			Data.Enemies.Add((*Monsters)[FallbackIndex]);
		}

		Data.MaxMobPerRoom = FMath::Clamp(FMath::Max(1, Data.Enemies.Num() / 3), 1, 6);

		FString Slug = Biome.Slug.Slug;
		if (Slug.IsEmpty())
		{
			Slug = !Biome.Name.IsEmpty()
				       ? Biome.Name.ToLower().Replace(TEXT(" "), TEXT("-"))
				       : FString::Printf(TEXT("biome-%d"), Biome.Biome_ID);
		}

		if (Slug.IsEmpty())
		{
			Slug = TEXT("biome-unknown");
		}

		FString UniqueSlug = Slug;
		int32 Suffix = 1;
		while (SaveGame->MapDatas.Contains(UniqueSlug))
		{
			UniqueSlug = FString::Printf(TEXT("%s-%d"), *Slug, Suffix++);
		}

		FMapDataWrapper Wrapper;
		Wrapper.MapData = Data;
		SaveGame->MapDatas.Add(UniqueSlug, Wrapper);
	}

	const bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("MapDataSaveSlot"), 0);
	UE_LOG(LogTemp, Log, TEXT("MapData saved to .sav (%d entries): %s"), SaveGame->MapDatas.Num(),
	       bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
}
