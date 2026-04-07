// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructManage.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DeathSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, FDeathStruct, deathStruct);

/**
 * 
 */
UCLASS()
class DIABLO_API UDeathSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public :
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;
};
