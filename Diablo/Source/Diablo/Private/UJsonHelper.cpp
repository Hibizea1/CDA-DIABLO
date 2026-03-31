// Fill out your copyright notice in the Description page of Project Settings.


#include "UJsonHelper.h"

#include "FPlayerScore.h"
#include "ItemStruct.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "UHTTPRequest.h"
#include "UItemSaveGame.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"


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

TArray<FPlayerScore> UJsonHelper::ParseScores(const FString& JsonString)
{
	TArray<FPlayerScore> Result;

	TSharedPtr<FJsonValue> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, Root) && Root->Type == EJson::Array)
	{
		for (const auto& Value : Root->AsArray())
		{
			TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj) continue;

			FPlayerScore Entry;
			Obj->TryGetStringField(TEXT("username"), Entry.Username);
			int32 ScoreTemp = 0;
			if (Obj->TryGetNumberField(TEXT("score"), ScoreTemp))
			{
				Entry.Score = ScoreTemp;
			}
			else
			{
				Entry.Score = 0;
			}

			Result.Add(Entry);
		}
	}

	return Result;
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

void UJsonHelper::DebugPrintAllItems()
{
	UItemSaveGame* Save = LoadItemsSaveGame();
	if (!Save) return;

	for (auto& Pair : Save->ItemsByType)
	{
		const FString& WeaponType = Pair.Key;
		const FItemArrayWrapper& Wrapper = Pair.Value;

		UE_LOG(LogTemp, Log, TEXT("=== WeaponType: %s, Items Count: %d ==="), *WeaponType, Wrapper.Items.Num());

		for (const FItemData& Item : Wrapper.Items)
		{
			UE_LOG(LogTemp, Log, TEXT("ID:%d Name:%s Type:%hhd Rarity:%d ItemID:%d Price:%d Mesh:%s"),
				Item.ID, *Item.Name, Item.Type, Item.Rarity, Item.ItemID, Item.Price, *Item.MeshPath);
		}
	}
}

TArray<FItemSave> UJsonHelper::ParseInventoriesFromJson(const FString& JsonString)
{
	TArray<FItemSave> Result;

	UE_LOG(LogTemp, Warning, TEXT("Incoming JSON (raw): %s"), *JsonString);

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
	Result.Level        = GetIntSafe(TEXT("level"), 1);
	Result.Experience   = GetIntSafe(TEXT("xp"), 0);
	Result.Gold         = GetIntSafe(TEXT("gold"), 0);
	Result.stamina      = GetIntSafe(TEXT("stamina"), 0);
	Result.health       = GetIntSafe(TEXT("health"), 0);
	Result.mana         = GetIntSafe(TEXT("mana"), 0);
	Result.intelligence = GetIntSafe(TEXT("intelligence"), 0);
	Result.strenght     = GetIntSafe(TEXT("strenght"), 0);


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


FString UJsonHelper::CreateMessage(
	const FString& Username,
	const FString& Password,
	const FString& Email)
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

// Forward declare the console handler and register it.
static void DevUpdateItemsDBCommand();
static FAutoConsoleCommand CVar_DevUpdateItemDB(
	TEXT("Dev_UpdateItemDB"),
	TEXT("DEV ONLY - Sync items DataTable to database"),
	FConsoleCommandDelegate::CreateStatic(&DevUpdateItemsDBCommand)
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
		Obj->SetNumberField(TEXT("rarity"), Row->Rarity == ERarity::Common ? 0 :
			Row->Rarity == ERarity::Rare ? 1 :
			Row->Rarity == ERarity::Epic ? 2 : 3);
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


static void DevUpdateItemsDBCommand()
{
#if !UE_BUILD_SHIPPING

	UE_LOG(LogTemp, Warning, TEXT("DEV: Updating Items Database"));

	UDataTable* ItemsTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/Blueprint/Data/ItemsData/Weapon/Bow/DT_Items.DT_Items")
	);

	if (!ItemsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Items DataTable not found"));
		return;
	}

	TArray<FItemStruct*> Rows;
	ItemsTable->GetAllRows(TEXT("DEV_SYNC"), Rows);

	FString JsonPayload = UJsonHelper::BuildItemsJson(Rows);
	UUHTTPRequest::SendToServer(JsonPayload);

#else
	UE_LOG(LogTemp, Error, TEXT("Dev_UpdateItemsDB disabled in Shipping"));
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

			FItemData Item;
			Item.ID = FCString::Atoi(*Obj->GetStringField(TEXT("id")));
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
				Item.Type = TypeEnum;
			}
			Item.Rarity = FCString::Atoi(*Obj->GetStringField(TEXT("rarity")));
			Item.ItemID = FCString::Atoi(*Obj->GetStringField(TEXT("Item_ID")));
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

void UJsonHelper::SaveItemsToSaveGame(const TMap<FString, FItemArrayWrapper>& ItemsByType)
{
	UItemSaveGame* SaveGame = Cast<UItemSaveGame>(UGameplayStatics::CreateSaveGameObject(UItemSaveGame::StaticClass()));
	if (!SaveGame) return;

	SaveGame->ItemsByType = ItemsByType;

	bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("ItemsSaveSlot"), 0);
	UE_LOG(LogTemp, Log, TEXT("Items saved to .sav: %s"), bSaved ? TEXT("SUCCESS") : TEXT("FAILED"));
}