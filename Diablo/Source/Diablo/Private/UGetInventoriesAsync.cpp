#include "UGetInventoriesAsync.h"
#include "UHTTPRequest.h"

UGetInventoriesAsync* UGetInventoriesAsync::GetInventories()
{
	UGetInventoriesAsync* Node = NewObject<UGetInventoriesAsync>();
	return Node;
}

void UGetInventoriesAsync::Activate()
{
	UUHTTPRequest::GetInventories(
		[this](const FPlayerData& Result)
		{
			if (Result.Items.Num() > 0)
			{
				OnSuccess.Broadcast(Result);
			}
			else
			{
				OnError.Broadcast();
			}
		}
	);
}