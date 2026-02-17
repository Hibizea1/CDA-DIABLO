// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FPlayerData.h"
#include "UGetInventoriesAsync.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventorySuccess, const FPlayerData&, PlayerData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryError);

UCLASS()
class DIABLO_API UGetInventoriesAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FInventorySuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FInventoryError OnError;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UGetInventoriesAsync* GetInventories();

	virtual void Activate() override;
};

