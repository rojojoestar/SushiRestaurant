#include "OrderManager.h"
#include "Net/UnrealNetwork.h"
#include "Misc/RecipeAsset.h"

// ---------- Constructor ----------
AOrderManager::AOrderManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// ---------- Lifecycle ----------
void AOrderManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(OrderGenerationTimer, this, &AOrderManager::GenerateNewOrder, OrderGenerationInterval, true);
		GenerateNewOrder(); // Optional initial order
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

// ---------- Orders ----------
void AOrderManager::GenerateNewOrder()
{
	if (AvailableRecipes.IsEmpty()) return;

	const int32 Index = FMath::RandRange(0, AvailableRecipes.Num() - 1);
	const TSubclassOf<URecipeAsset> SelectedRecipe = AvailableRecipes[Index];

	FSushiOrder NewOrder;
	NewOrder.TableID      = FMath::RandRange(1, 4);
	NewOrder.RemainingTime = OrderTimeLimit;
	NewOrder.Recipe       = SelectedRecipe;

	ActiveOrders.Add(NewOrder);
	UE_LOG(LogTemp, Log, TEXT("New order for table %d: %s"), NewOrder.TableID, *GetNameSafe(SelectedRecipe));
}

void AOrderManager::UpdateOrderTimers(float DeltaTime)
{
	for (FSushiOrder& Order : ActiveOrders)
	{
		Order.RemainingTime -= DeltaTime;
	}
}

void AOrderManager::CheckExpiredOrders()
{
	for (int32 i = ActiveOrders.Num() - 1; i >= 0; --i)
	{
		if (ActiveOrders[i].RemainingTime <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Order expired for table %d"), ActiveOrders[i].TableID);
			ActiveOrders.RemoveAt(i);
			OnOrderExpired.Broadcast();
		}
	}
}

bool AOrderManager::TryCompleteOrder(int32 TableID, URecipeAsset* DeliveredDish)
{
	if (!DeliveredDish) return false;

	for (int32 i = 0; i < ActiveOrders.Num(); ++i)
	{
		if (ActiveOrders[i].TableID == TableID)
		{
			URecipeAsset* Expected = ActiveOrders[i].Recipe.GetDefaultObject();
			if (Expected && DeliveredDish == Expected)
			{
				ActiveOrders.RemoveAt(i);
				OnOrderCompleted.Broadcast();
				return true;
			}
			return false;
		}
	}
	return false;
}

// ---------- Replication ----------
void AOrderManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOrderManager, ActiveOrders);
}
