// Fill out your copyright notice in the Description page of Project Settings.

#include "UHTTPRequest.h"

#include "FItemsRoot.h"
#include "FPlayerData.h"
#include "UJsonHelper.h"
#include "http.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"


FString UUHTTPRequest::Access_Token = "";
FString UUHTTPRequest::Refresh_Token = "";


void UUHTTPRequest::SaveRequest(const int Level, const int Experience, const int Gold)
{
	auto user_id = UJsonHelper::GetStoredUserId();
	FHttpModule& HttpModule = FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
		HttpModule.CreateRequest();

	Request->SetURL(FString("http://127.0.0.1:8080/update_character.php"));
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetContentAsString(FString::Printf(
		TEXT("{\"user_id\":%d,\"character\":{\"level\":%d,\"xp\":%d,\"gold\":\"%d\"}}"),
		user_id,
		Level,
		Experience,
		Gold));
	Request->ProcessRequest();
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr request, FHttpResponsePtr response, bool success)
	{
		if (!success || !response.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
			return;
		}

		const int32 Code = response->GetResponseCode();

		if (Code != 200)
		{
			UE_LOG(LogTemp, Error, TEXT("HTTP Error code: %d"), Code);
			return;
		}

		const FString ResponseBody = response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Server Response: %s"), *ResponseBody);
	});
}

void UUHTTPRequest::GetItemCSV()
{
	FHttpModule& Http = FHttpModule::Get();

	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("http://127.0.0.1:8080/items.csv");
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "text/csv");

	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (!bSuccess || !Res.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed To GetItemCSV"));
				return;
			}

			// Get CSV content
			const FString CsvContent = Res->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("CSV RECEIVED:\n%s"), *CsvContent);

			// Split CSV lines
			TArray<FString> Lines;
			CsvContent.ParseIntoArrayLines(Lines);

			if (Lines.Num() <= 1)
			{
				UE_LOG(LogTemp, Error, TEXT("CSV file empty or invalid"));
				return;
			}

			// Header parsing
			TArray<FString> Headers;
			Lines[0].ParseIntoArray(Headers, TEXT(","));

			int32 TypeIndex = Headers.Find(TEXT("type"));
			if (TypeIndex == INDEX_NONE)
			{
				UE_LOG(LogTemp, Error, TEXT("CSV missing 'type' column"));
				return;
			}

			// Items grouped by weapon type
			TMap<FString, TArray<TSharedPtr<FJsonObject>>> SplitJson;

			for (int32 i = 1; i < Lines.Num(); i++)
			{
				TArray<FString> Cells;
				Lines[i].ParseIntoArray(Cells, TEXT(";"));

				if (Cells.Num() != Headers.Num())
					continue;

				FString WeaponType = Cells[TypeIndex];

				// Create JSON object for each row
				TSharedPtr<FJsonObject> ItemJson = MakeShared<FJsonObject>();

				for (int32 c = 0; c < Headers.Num(); c++)
					ItemJson->SetStringField(Headers[c], Cells[c]);

				// Add to map
				SplitJson.FindOrAdd(WeaponType).Add(ItemJson);
			}

			// Save each type in a separate JSON file
			const FString SaveDir = FPaths::ProjectSavedDir() / TEXT("ItemsJSON/");
			IFileManager::Get().MakeDirectory(*SaveDir, true);

			for (auto& Pair : SplitJson)
			{
				FString TypeName = Pair.Key;
				TArray<TSharedPtr<FJsonValue>> JsonArray;

				for (auto& Obj : Pair.Value)
					JsonArray.Add(MakeShared<FJsonValueObject>(Obj));

				TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
				Root->SetArrayField("items", JsonArray);

				// Convert to JSON string
				FString OutputJson;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJson);
				FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

				// File path
				FString FilePath = SaveDir / (TypeName + TEXT(".json"));

				if (FFileHelper::SaveStringToFile(OutputJson, *FilePath))
				{
					UE_LOG(LogTemp, Log, TEXT("Saved JSON for type %s -> %s"), *TypeName, *FilePath);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to save %s.json"), *TypeName);
				}
			}

			UE_LOG(LogTemp, Log, TEXT("All weapon type JSON files generated successfully."));
		}
	);

	Request->ProcessRequest();
}


void UUHTTPRequest::GetInventories(TFunction<void(FPlayerData)> OnCompleted)
{
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("http://127.0.0.1:8080/load_inventories.php");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);

	Request->OnProcessRequestComplete().BindLambda(
		[OnCompleted](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			FPlayerData Data; // variable locale
			if (bSuccess && Res.IsValid())
			{
				Data.Items = UJsonHelper::ParseInventoriesFromJson(Res->GetContentAsString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("HTTP request failed"));
			}

			// Renvoie les données via callback
			OnCompleted(Data);
		}
	);

	Request->ProcessRequest();
}

void UUHTTPRequest::SaveInventories(
	const TArray<FItemSave>& Items,
	TFunction<void(bool, const FString&)> OnCompleted)
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

	RootObject->SetArrayField("result", JsonItems);

	// Convertir en string
	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(RootObject, Writer);

	UE_LOG(LogTemp, Log, TEXT("SaveInventories JSON: %s"), *JsonPayload);

	// === Requête HTTP ===
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http.CreateRequest();

	Request->SetURL("http://127.0.0.1:8080/save_inventories.php");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), TEXT("Bearer ") + Access_Token);
	Request->SetContentAsString(JsonPayload);

	Request->OnProcessRequestComplete().BindLambda(
		[OnCompleted](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (!bSuccess || !Res.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("SaveInventories: request failed."));
				OnCompleted(false, Res->GetContentAsString());
				return;
			}

			UE_LOG(LogTemp, Log, TEXT("SaveInventories response: %s"), *Res->GetContentAsString());
			OnCompleted(true, Res->GetContentAsString());
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
		UE_LOG(LogTemp, Log, TEXT("File %s exists"), *CsvFile);
		FFileHelper::LoadANSITextFileToStrings(*CsvFile, NULL, Lines);
	}

	return Lines;
}

void UUHTTPRequest::SendToServer(const FString& Json)
{
	FHttpModule& Http = FHttpModule::Get();

	TSharedRef<IHttpRequest> Request = Http.CreateRequest();
	Request->SetURL("http://127.0.0.1:8080/update_items.php");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(Json);

	Request->OnProcessRequestComplete().BindLambda(
		[](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
		{
			if (bSuccess && Res.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Items synced: %s"),
				       *Res->GetContentAsString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Items sync failed"));
			}
		}
	);

	Request->ProcessRequest();
}
