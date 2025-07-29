#include "OrderManager.h"
#include "PickupActor.h"
#include "Net/UnrealNetwork.h"

AOrderManager::AOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AOrderManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Start timer to spawn orders periodically
		GetWorldTimerManager().SetTimer(OrderGenerationTimer, this, &AOrderManager::GenerateNewOrder, OrderGenerationInterval, true);
		GenerateNewOrder(); // Optional: start with one order
	}
}

void AOrderManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdateOrderTimers(DeltaTime);
		CheckExpiredOrders();
	}
}

// Spawns a new random order
void AOrderManager::GenerateNewOrder()
{
	FOrder NewOrder;
	NewOrder.TableID = FMath::RandRange(1, 4); // Random table
	NewOrder.RemainingTime = OrderTimeLimit;

	// Sample hardcoded recipe (Fish + Seaweed)
	NewOrder.RequiredIngredients = { EIngredientType::Fish, EIngredientType::Seaweed };

	ActiveOrders.Add(NewOrder);
}

// Decrease the remaining time for all orders
void AOrderManager::UpdateOrderTimers(float DeltaTime)
{
	for (FOrder& Order : ActiveOrders)
	{
		Order.RemainingTime -= DeltaTime;
	}
}

// Remove any expired orders
void AOrderManager::CheckExpiredOrders()
{
	for (int32 i = ActiveOrders.Num() - 1; i >= 0; i--)
	{
		if (ActiveOrders[i].RemainingTime <= 0.0f)
		{
			// TODO: Handle failure feedback
			ActiveOrders.RemoveAt(i);
		}
	}
}

// Try to match a plate to an existing order
bool AOrderManager::TryCompleteOrder(int32 TableID, const TArray<EIngredientType>& Ingredients)
{
	for (int32 i = 0; i < ActiveOrders.Num(); i++)
	{
		if (ActiveOrders[i].TableID == TableID)
		{
			if (ActiveOrders[i].RequiredIngredients == Ingredients)
			{
				ActiveOrders.RemoveAt(i);
				return true;
			}
			return false;
		}
	}
	return false;
}

// Set up variable replication
void AOrderManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOrderManager, ActiveOrders);
}
