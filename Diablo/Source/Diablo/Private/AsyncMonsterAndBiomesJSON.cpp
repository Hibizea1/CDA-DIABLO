// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncMonsterAndBiomesJSON.h"
#include "UHTTPRequest.h"

UAsyncMonsterAndBiomesJSON* UAsyncMonsterAndBiomesJSON::GetMonsterAndBiomesJson()
{
	UAsyncMonsterAndBiomesJSON* Node = NewObject<UAsyncMonsterAndBiomesJSON>();
	return Node;
}

void UAsyncMonsterAndBiomesJSON::Activate()
{
	UUHTTPRequest::GetMonsterAndBiomesJSON(
		[this](bool bSuccess, const FString& Message)
		{
			if (bSuccess)
			{
				OnSuccess.Broadcast(true, Message);
			}
			else
			{
				OnError.Broadcast();
			}
		}
	);
}