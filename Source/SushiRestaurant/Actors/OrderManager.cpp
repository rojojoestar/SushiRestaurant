// OrderManager.cpp
#include "OrderManager.h"
#include "Misc/RecipeAsset.h"
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
        // Start generating orders
        GetWorldTimerManager().SetTimer(OrderGenerationTimer, this, &AOrderManager::GenerateNewOrder, OrderGenerationInterval, true);
        GenerateNewOrder(); // Start with one order
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

void AOrderManager::GenerateNewOrder()
{
    if (AvailableRecipes.IsEmpty()) return;

    int32 Index = FMath::RandRange(0, AvailableRecipes.Num() - 1);
    TSubclassOf<URecipeAsset> SelectedRecipe = AvailableRecipes[Index];

    FSushiOrder NewOrder;
    NewOrder.TableID = FMath::RandRange(1, 4);
    NewOrder.RemainingTime = OrderTimeLimit;
    NewOrder.Recipe = SelectedRecipe; // Store the recipe, not ingredients

    ActiveOrders.Add(NewOrder);
    UE_LOG(LogTemp, Log, TEXT("Generated order for Table %d: %s"), NewOrder.TableID, *SelectedRecipe->GetName());
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
    for (int32 i = ActiveOrders.Num() - 1; i >= 0; i--)
    {
        if (ActiveOrders[i].RemainingTime <= 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Order for Table %d expired!"), ActiveOrders[i].TableID);
            ActiveOrders.RemoveAt(i);
            OnOrderExpired.Broadcast(); // Trigger Game Over or UI update
        }
    }
}

bool AOrderManager::TryCompleteOrder(int32 TableID, URecipeAsset* DeliveredDish)
{
    if (!DeliveredDish) return false;

    for (int32 i = 0; i < ActiveOrders.Num(); i++)
    {
        if (ActiveOrders[i].TableID == TableID)
        {
            URecipeAsset* ExpectedDish = ActiveOrders[i].Recipe.GetDefaultObject();
            if (ExpectedDish && DeliveredDish == ExpectedDish)
            {
                ActiveOrders.RemoveAt(i);
                OnOrderCompleted.Broadcast(); // Score + UI update
                return true;
            }
            return false; // Wrong dish
        }
    }
    return false; // No order for this table
}

void AOrderManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AOrderManager, ActiveOrders);
}