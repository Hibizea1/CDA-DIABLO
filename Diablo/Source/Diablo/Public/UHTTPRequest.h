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
	static void SaveRequest(const FPlayerData& Player);
	
	static void GetItemJSON(TFunction<void(bool, FString)> OnCompleted);

	UFUNCTION(BlueprintCallable)
	static void SaveInventories(const TArray<FItemSave>& Items, const TArray<FItemSave>& Equipment);

	static void SendToServer(const FString& Json);

	static TArray<FString> GetCSVFilePath(FString Path);
	
	static FString CleanWeaponType(FString& Cell);

	static FString Access_Token;

	static FString Refresh_Token;
};
