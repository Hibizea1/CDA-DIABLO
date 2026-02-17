#include "USaveInventoriesAsync.h"
#include "UHTTPRequest.h"

USaveInventoriesAsync* USaveInventoriesAsync::SaveInventories(const TArray<FItemSave>& Item)
{
	USaveInventoriesAsync* Node = NewObject<USaveInventoriesAsync>();
	Node->InputItem = Item;
	return Node;
}

void USaveInventoriesAsync::Activate()
{
	// Appelle ta fonction HTTP
	UUHTTPRequest::SaveInventories(InputItem,
		[this](bool bSuccess, const FString& ServerMessage)
		{
			if (bSuccess)
			{
				OnSuccess.Broadcast(InputItem, ServerMessage);
			}
			else
			{
				OnError.Broadcast(ServerMessage);
			}
		}
	);
}