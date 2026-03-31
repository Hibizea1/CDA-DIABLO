// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncItemJson.h"

#include "UHTTPRequest.h"

UAsyncItemJson* UAsyncItemJson::GetItemJson()
{
	UAsyncItemJson* Node = NewObject<UAsyncItemJson>();
	return Node;
}

void UAsyncItemJson::Activate()
{
	UUHTTPRequest::GetItemJSON(
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
