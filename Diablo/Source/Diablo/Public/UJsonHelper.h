
#pragma once

#include "CoreMinimal.h"
#include "UJsonHelper.generated.h"
struct FPlayerScore;
/**
 * 
 */
UCLASS(BlueprintType)
class UJsonHelper: public UBlueprintFunctionLibrary
{
GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable)
	static TArray<FPlayerScore> ParseScores(const FString& JsonString);
	
	UFUNCTION(BlueprintCallable)
	static FString ParseMessage (const FString& JsonString);

	UFUNCTION(BlueprintCallable)
	static FString CreateMessage (const FString& Username, const FString& Password, const FString& Email);
};
