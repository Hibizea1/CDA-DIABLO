#pragma once

#include "CoreMinimal.h"
#include "UItemSaveGame.h"
#include "FPlayerData.h"
#include "MapDataSave.h"
#include "MonsterAndBiomsSaveGame.h"
#include "UJsonHelper.generated.h"
struct FItemStruct;
struct FPlayerScore;
/**
 * 
 */
UCLASS(BlueprintType)
class UJsonHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static int32 StoredUserId;

	// Getter propre pour Blueprint
	UFUNCTION(BlueprintCallable, Category="JSON")
	static int32 GetStoredUserId();

	UFUNCTION(BlueprintCallable)
	static TArray<FPlayerScore> ParseScores(const FString& JsonString);

	UFUNCTION(BlueprintCallable)
	static FString ParseMessage(const FString& JsonString);

	UFUNCTION(BlueprintCallable)
	static FPlayerData ParsePlayerData(const FString& JsonString);

	UFUNCTION(BlueprintCallable)
	static FString CreateMessage(const FString& Username, const FString& Password, const FString& Email);

	UFUNCTION(BlueprintCallable)
	static FString CreateLoginMessage(const FString& Username, const FString& Password);

	static void SaveItemsToSaveGame(const TMap<FString, FItemArrayWrapper>& ItemsByType);

	static void SaveBiomesAndMonsterToSaveGame(const TMap<FString, FMonsterAndBiomesWrapper>& MonsterAndBiomesType);
	static void SaveMapData(const TMap<FString, FMonsterAndBiomesWrapper>& MonsterAndBiomesType);

	static FItemStruct CreateNewItem(const FItemData& Item, int32 Level, ERarity Rarity);

	static TMap<FString, FItemArrayWrapper> ParseItemsJSON(const FString& JsonContent);

	static TMap<FString, FMonsterAndBiomesWrapper> ParseBiomesAndMonstersJSON(const FString& JsonContent);


	static void DebugPrintAllItems();

	UFUNCTION(BlueprintCallable)
	static UItemSaveGame* LoadItemsSaveGame();

	UFUNCTION(BlueprintCallable)
	static UMapDataSave* LoadMapDataSave();

	UFUNCTION(BlueprintCallable)
	static UMonsterAndBiomsSaveGame* LoadMonsterAndBiomesSaveGame();

	UFUNCTION(BlueprintCallable)
	static TArray<FItemSave> ParseInventoriesFromJson(const FString& JsonString);

	static FString BuildItemsJson(const TArray<FItemStruct*>& Rows);

	UFUNCTION(BlueprintCallable)
	static UItemSaveGame* LoadItemsSaveGameBP();
};
