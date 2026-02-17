// Fill out your copyright notice in the Description page of Project Settings.


#include "UJsonHelper.h"
#include "FPlayerScore.h"
#include "ItemStruct.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "UHTTPRequest.h"
#include "Engine/DataTable.h"


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



// Implémentation
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

	if (!Root->TryGetArrayField(TEXT("result"), JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("'result' field missing or not an array"));
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
		return Result;
	}

	// 🔹 USER ID (entier, PAS un objet)
	if (!RootObj->TryGetNumberField(TEXT("id"), StoredUserId))
	{
		return Result;
	}

	// 🔹 CHARACTER
	const TSharedPtr<FJsonObject>* CharacterObj;
	if (!RootObj->TryGetObjectField(TEXT("character"), CharacterObj))
	{
		return Result;
	}

	int32 Level = (*CharacterObj)->GetIntegerField(TEXT("level"));
	int32 Xp = (*CharacterObj)->GetIntegerField(TEXT("xp"));
	int32 Gold = (*CharacterObj)->GetIntegerField(TEXT("gold"));
	FString Class = (*CharacterObj)->GetStringField(TEXT("class"));

	Result.Level = Level;
	Result.Experience = Xp;
	Result.Gold = Gold;

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
		Obj->SetStringField(TEXT("type"), *UEnum::GetValueAsString(Row->EquipmentType));
		Obj->SetNumberField(TEXT("rarity"), Row->Rarity == ERarity::Common ? 0 :
			Row->Rarity == ERarity::Rare ? 1 :
			Row->Rarity == ERarity::Epic ? 2 : 3);
		Obj->SetNumberField(TEXT("price"), Row->ItemLevel);
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