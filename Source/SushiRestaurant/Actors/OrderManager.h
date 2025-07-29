#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderManager.generated.h"

// Defines an order made by a table
USTRUCT(BlueprintType)
struct FOrder
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TableID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<EIngredientType> RequiredIngredients;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RemainingTime;
};

UCLASS()
class AOrderManager : public AActor
{
	GENERATED_BODY()

public:
	AOrderManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Time between order spawns
	UPROPERTY(EditDefaultsOnly, Category = "Orders")
	float OrderGenerationInterval = 30.0f;

	// Time limit to complete an order
	UPROPERTY(EditDefaultsOnly, Category = "Orders")
	float OrderTimeLimit = 60.0f;

	// List of currently active orders
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Orders")
	TArray<FOrder> ActiveOrders;

	// Handle for periodic order generation
	FTimerHandle OrderGenerationTimer;

	// Spawns a new order
	UFUNCTION()
	void GenerateNewOrder();

	// Decrease remaining time for all orders
	void UpdateOrderTimers(float DeltaTime);

	// Remove expired orders
	void CheckExpiredOrders();

public:
	// Try to match submitted ingredients to an existing order
	UFUNCTION(BlueprintCallable)
	bool TryCompleteOrder(int32 TableID, const TArray<EIngredientType>& Ingredients);

	// Returns list of active orders
	UFUNCTION(BlueprintPure)
	const TArray<FOrder>& GetActiveOrders() const { return ActiveOrders; }
};
