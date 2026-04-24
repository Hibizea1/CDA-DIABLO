// Fill out your copyright notice in the Description page of Project Settings.

#include "UHTTPRequest.h"

#include "FPlayerData.h"
#include "UJsonHelper.h"
#include "http.h"
#include "MonsterAndBiomsSaveGame.h"
#include "Misc/AES.h"
#include "Misc/Base64.h"



struct FItemData;
FString UUHTTPRequest::Access_Token = "";
FString UUHTTPRequest::Refresh_Token = "";


void UUHTTPRequest::SaveRequest(const FPlayerData& Player)
{
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
		HttpModule.CreateRequest();

	Request->SetURL(FString("https://localhost:8443/php/update_character.php"));
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Build JSON payload from the Player array
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();

	TSharedRef<FJsonObject> PlayerObj = MakeShared<FJsonObject>();
	PlayerObj->SetNumberField("level", Player.Level);
	PlayerObj->SetNumberField("xp", Player.Experience);
	PlayerObj->SetNumberField("gold", Player.Gold);
	PlayerObj->SetNumberField("strength", Player.strenght);
	PlayerObj->SetNumberField("health", Player.health);
	PlayerObj->SetNumberField("Experience", Player.Experience);
	PlayerObj->SetNumberField("intelligence", Player.intelligence);
	PlayerObj->SetNumberField("mana", Player.mana);
	PlayerObj->SetNumberField("stamina", Player.stamina);

	RootObject->SetObjectField("character", PlayerObj);

	// Serialize JSON to string
	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	if (!FJsonSerializer::Serialize(RootObject, Writer))
	{
		UE_LOG(LogHttp, Error, TEXT("SaveRequest: Failed to serialize JSON payload"));
		return;
	}

	Request->SetContentAsString(JsonPayload);

	// Bind the completion delegate BEFORE starting the request
	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr request, FHttpResponsePtr response, bool success)
		{
			if (!success || !response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
				const FString Msg = response.IsValid()
					                    ? response->GetContentAsString()
					                    : TEXT("No response from server");
				return;
			}

			const int32 Code = response->GetResponseCode();
			const FString ResponseBody = response->GetContentAsString();

			if (Code != 200)
			{
				UE_LOG(LogHttp, Error, TEXT("HTTP Error code: %d Message: %s"), Code, *ResponseBody);
				return;
			}

			UE_LOG(LogHttp, Log, TEXT("SaveRequest Server Response: %s"), *ResponseBody);
		}
	);

	Request->ProcessRequest();
}

void UUHTTPRequest::GetItemJSON(TFunction<void(bool, FString)> OnCompleted)
{
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("https://localhost:8443/php/get_item.php");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetHeader("Content-Type", "application/json");

	Request->OnProcessRequestComplete().BindLambda(
		[OnCompleted](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (!bSuccess || !Res.IsValid())
			{
				UE_LOG(LogHttp, Error, TEXT("GetItemJSON: Failed to download JSON"));
				OnCompleted(false, Res->GetContentAsString());
				return;
			}
			UE_LOG(LogHttp, Log, TEXT("GetItemJSON: %s"), *Res->GetContentAsString());

			FString JsonContent = Res->GetContentAsString();
			TMap<FString, FItemArrayWrapper> ItemsByType = UJsonHelper::ParseItemsJSON(JsonContent);

			UJsonHelper::SaveItemsToSaveGame(ItemsByType);
			OnCompleted(true, "success");
		});

	Request->ProcessRequest();
}

void UUHTTPRequest::GetMonsterAndBiomesJSON(TFunction<void(bool, FString)> OnCompleted)
{
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("https://localhost:8443/php/get_monsters_biomes.php");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetHeader("Content-Type", "application/json");

	Request->OnProcessRequestComplete().BindLambda(
		[OnCompleted](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (!bSuccess || !Res.IsValid())
			{
				UE_LOG(LogHttp, Error, TEXT("GetMonsterAndBiomesJSON: Failed to download JSON"));
				OnCompleted(false, Res->GetContentAsString());
				return;
			}
			UE_LOG(LogHttp, Log, TEXT("GetMonsterAndBiomesJSON: %s"), *Res->GetContentAsString());

			FString JsonContent = Res->GetContentAsString();
			TMap<FString, FMonsterAndBiomesWrapper> ItemsByType = UJsonHelper::ParseBiomesAndMonstersJSON(JsonContent);

			UJsonHelper::SaveBiomesAndMonsterToSaveGame(ItemsByType);
			OnCompleted(true, "success");
		});

	Request->ProcessRequest();
}

void UUHTTPRequest::SaveInventories(const TArray<FItemSave>& Items, const TArray<FItemSave>& Equipment)
{
	// === Construction manuelle du JSON ===
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> JsonItems;

	for (const FItemSave& Item : Items)
	{
		TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetNumberField("item_id", Item.item_id);
		Obj->SetNumberField("rarity", Item.rarity);
		Obj->SetNumberField("lvl", Item.lvl);

		JsonItems.Add(MakeShared<FJsonValueObject>(Obj));
	}

	TArray<TSharedPtr<FJsonValue>> JsonEquipment;
	for (const FItemSave& _Equipment : Equipment)
	{
		TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetNumberField("item_id", _Equipment.item_id);
		Obj->SetNumberField("rarity", _Equipment.rarity);
		Obj->SetNumberField("lvl", _Equipment.lvl);

		JsonEquipment.Add(MakeShared<FJsonValueObject>(Obj));
	}

	RootObject->SetArrayField("Items", JsonItems);
	RootObject->SetArrayField("Equipment", JsonEquipment);

	// Convertir en string
	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(RootObject, Writer);

	UE_LOG(LogHttp, Log, TEXT("SaveInventories JSON: %s"), *JsonPayload);

	// === Requête HTTP ===
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http.CreateRequest();

	Request->SetURL("https://localhost:8443/php/save_inventories.php");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetContentAsString(JsonPayload);

	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (!bSuccess || !Res.IsValid())
			{
				UE_LOG(LogHttp, Error, TEXT("SaveInventories: request failed."));
				return;
			}

			UE_LOG(LogHttp, Log, TEXT("SaveInventories response: %s"), *Res->GetContentAsString());
		}
	);

	Request->ProcessRequest();
}


TArray<FString> UUHTTPRequest::GetCSVFilePath(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	if (FPaths::FileExists(CsvFile))
	{
		UE_LOG(LogHttp, Log, TEXT("File %s exists"), *CsvFile);
		FFileHelper::LoadANSITextFileToStrings(*CsvFile, NULL, Lines);
	}

	return Lines;
}

void UUHTTPRequest::SendToServer(const FString& Json)
{
	FHttpModule& Http = FHttpModule::Get();

	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("https://localhost:8443/php/update_items.php");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(Json);

	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (bSuccess && Res.IsValid())
			{
				UE_LOG(LogHttp, Log, TEXT("Items synced: %s"),
				       *Res->GetContentAsString());
			}
			else
			{
				UE_LOG(LogHttp, Error, TEXT("Items sync failed"));
			}
		}
	);

	Request->ProcessRequest();
}

void UUHTTPRequest::SyncWikiGameData(const FString& Json, const FString& SyncToken)
{
    FHttpModule& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest> Request = Http.CreateRequest();

    // Endpoint and method
    Request->SetURL("https://localhost:8443/php/sync_wiki_game_data.php");
    Request->SetVerb("POST");

    // Required headers
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("X-Client-Type"), TEXT("game"));

    // Optional sync token header (required server-side if configured)
    if (!SyncToken.IsEmpty())
    {
        Request->SetHeader(TEXT("X-Sync-Token"), SyncToken);
    }

    Request->SetContentAsString(Json);

    Request->OnProcessRequestComplete().BindLambda(
        [](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
        {
            if (!bSuccess || !Res.IsValid())
            {
                UE_LOG(LogHttp, Error, TEXT("SyncWikiGameData: HTTP request failed or no response"));
                return;
            }

            const int32 Code = Res->GetResponseCode();
            const FString Body = Res->GetContentAsString();

            if (Code >= 200 && Code < 300)
            {
                UE_LOG(LogHttp, Log, TEXT("SyncWikiGameData success (code %d): %s"), Code, *Body);
            }
            else
            {
                UE_LOG(LogHttp, Error, TEXT("SyncWikiGameData failed (code %d): %s"), Code, *Body);
            }
        }
    );

    Request->ProcessRequest();
}

FString UUHTTPRequest::CleanWeaponType(FString& RawType)
{
	// Trim spaces
	RawType = RawType.TrimStartAndEnd();

	// Remove enum prefix
	if (RawType.Contains("::"))
	{
		RawType = RawType.RightChop(RawType.Find("::") + 2);
	}

	// Replace forbidden characters
	RawType.ReplaceInline(TEXT(" "), TEXT("_"));
	RawType.ReplaceInline(TEXT("/"), TEXT("_"));
	RawType.ReplaceInline(TEXT("\\"), TEXT("_"));
	RawType.ReplaceInline(TEXT(":"), TEXT("_"));
	RawType.ReplaceInline(TEXT("*"), TEXT("_"));
	RawType.ReplaceInline(TEXT("?"), TEXT("_"));
	RawType.ReplaceInline(TEXT("\""), TEXT("_"));
	RawType.ReplaceInline(TEXT("<"), TEXT("_"));
	RawType.ReplaceInline(TEXT(">"), TEXT("_"));
	RawType.ReplaceInline(TEXT("|"), TEXT("_"));

	return RawType;
}
