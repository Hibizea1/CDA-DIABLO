// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "UItemSaveGame.h"
#include "GameFramework/Actor.h"
#include "Items.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSpawn);

UCLASS()
class DIABLO_API AItems : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItems();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Item")
	FItemStruct ItemData;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitItem(const FItemStruct& InItem);
	

	UPROPERTY(BlueprintAssignable, Category = "Item")
	FOnItemSpawn OnItemSpawn;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Blueprintable, Category = "Items")
	void SpawnItem();
};
