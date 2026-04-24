// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

struct FPlayerData;

UENUM(Blueprintable)
enum EInventoryType
{
	Inventory,
	Equipment
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DIABLO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();
	int32 GetPlayerCharacter() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStruct> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemStruct> Equipments;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Items")
	void SetInventory(FPlayerData playerData);

	UFUNCTION(BlueprintCallable)
	int AddItem(EInventoryType inventoryType,const FItemStruct& NewItem);

	UFUNCTION(BlueprintCallable)
	FItemStruct GetItem(EInventoryType inventoryType, int32 index);
	bool HasWeapon();

	UFUNCTION(BlueprintCallable)
	FItemStruct GetEquipedWeapon();

	UFUNCTION(BlueprintCallable)
	void RemoveItem(EInventoryType inventoryType, int32 Index);
};