// Fill out your copyright notice in the Description page of Project Settings.


#include "UJsonHelper.h"
#include "FPlayerScore.h"
#include "Json.h"
#include "JsonUtilities.h"


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

	TSharedPtr<FJsonValue> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
	{
		TSharedPtr<FJsonObject> Obj = Root->AsObject();
		if (Obj.IsValid())
		{
			Obj->TryGetStringField(TEXT("status"), Result);
		}
	}

	return Result;
}


FString UJsonHelper::CreateMessage(
	const FString& Username,
	const FString& Password,
	const FString& Email)
{
	// Création de l'objet JSON
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("username"), Username);
	JsonObject->SetStringField(TEXT("password"), Password);
	JsonObject->SetStringField(TEXT("email"), Email);

	// Sérialisation en FString
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}
