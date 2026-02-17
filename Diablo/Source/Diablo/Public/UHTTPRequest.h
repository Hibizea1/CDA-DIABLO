// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FItemSave.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UHTTPRequest.generated.h"

struct FPlayerData;
/**
 * 
 */
UCLASS(BlueprintType)
class UUHTTPRequest : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SaveRequest(const int Level, const int Experience, const int Gold);
	UFUNCTION(BlueprintCallable)
	static void GetItemCSV();

	// This function uses a C++ TFunction callback and a plain struct (FPlayerData).
	// Such a signature cannot be exposed to Blueprints (UFUNCTION) because
	// TFunction is not reflectable. Keep it C++-only by removing UFUNCTION.
	static void GetInventories(TFunction<void(FPlayerData)> OnCompleted);

	static void SaveInventories(
		const TArray<FItemSave>& Items,
		TFunction<void(bool, const FString&)> OnCompleted
	);

	static void SendToServer(const FString& Json);

	static TArray<FString> GetCSVFilePath(FString Path);


	static FString Access_Token;

	static FString Refresh_Token;
};
