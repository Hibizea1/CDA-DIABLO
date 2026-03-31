#pragma once

#include "CoreMinimal.h"
#include "StructManage.h"
#include "GameFramework/Character.h"
#include "UEntity.generated.h"



USTRUCT(BlueprintType)
struct FLevelStatStruct
{
	GENERATED_BODY()

public :
	
	UPROPERTY(BlueprintReadOnly)
	int HealthLevel;

	UPROPERTY(BlueprintReadOnly)
	int StaminaLevel;

	UPROPERTY(BlueprintReadOnly)
	int StrengthLevel;
	
	UPROPERTY(BlueprintReadOnly)
	int ManaLevel;
	
};


USTRUCT(BlueprintType)
struct FStatsStruct
{
	GENERATED_BODY()

public :
	
	UPROPERTY(BlueprintReadWrite)
	float CurrentHealth;

	UPROPERTY(BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite)
	float CurrentExperience;

	UPROPERTY(BlueprintReadWrite)
	float RequiredExperience;

	UPROPERTY(BlueprintReadWrite)
	float Stamina;

	UPROPERTY(BlueprintReadWrite)
	float Strength;
	
	UPROPERTY(BlueprintReadWrite)
	float Mana;
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDead, FDeathStruct*);

UCLASS()
class DIABLO_API AUEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUEntity();

protected:
	virtual void BeginPlay() override;
	
public:

	FOnDead OnDead;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};


