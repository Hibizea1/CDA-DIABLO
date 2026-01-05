// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class AARoom;

UCLASS()
class DIABLO_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	bool bHasRoom = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int roomId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int currentH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Grid")
	int GridX = -1;

	UPROPERTY(VisibleAnywhere, Category = "Grid")
	int GridY = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ATile*> AroundTiles;

	const TArray<ATile*>& GetAroundTiles() const { return AroundTiles; }
	void ClearAroundTiles() { AroundTiles.Empty(); }

	UPROPERTY(Blueprintable, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* YourMeshPointer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsBorder = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector2D> neighboursPos = { {0,1}, {1,0}, {-1,0}, {0,-1} };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Check(::AARoom& Room, int CurrentH);
	void SetAroundTiles();

};
