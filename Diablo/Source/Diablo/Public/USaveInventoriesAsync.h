#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FPlayerData.h"
#include "USaveInventoriesAsync.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemSuccess, const TArray<FItemSave>&, ItemSave, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemError, const FString&, ErrorMessage);

UCLASS()
class DIABLO_API USaveInventoriesAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FItemSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FItemError OnError;

	// ---- APPEL BLUEPRINT ----
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"), Category="Inventory")
	static USaveInventoriesAsync* SaveInventories(const TArray<FItemSave>&  Item);

	virtual void Activate() override;

private:

	// Données reçues en entrée
	TArray<FItemSave> InputItem;
};
