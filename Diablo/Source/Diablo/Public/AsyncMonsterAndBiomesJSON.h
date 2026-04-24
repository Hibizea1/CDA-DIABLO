// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncMonsterAndBiomesJSON.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGetMonsterAndBiomesSuccess, const bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetMonsterAndBiomesError);

UCLASS()
class DIABLO_API UAsyncMonsterAndBiomesJSON : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	UPROPERTY(BlueprintAssignable)
	FGetMonsterAndBiomesSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FGetMonsterAndBiomesError OnError;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UAsyncMonsterAndBiomesJSON* GetMonsterAndBiomesJson();

	virtual void Activate() override;
};
