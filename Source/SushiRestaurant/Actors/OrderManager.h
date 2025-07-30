#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderManager.generated.h"

class URecipeAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrderEvent);

/** One active order assigned to a table. */
USTRUCT(BlueprintType)
struct FSushiOrder
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Order")
	int32 TableID = -1;

	UPROPERTY(BlueprintReadWrite, Category="Order")
	float RemainingTime = 60.f;

	UPROPERTY(BlueprintReadWrite, Category="Order")
	TSubclassOf<URecipeAsset> Recipe;
};

/**
 * Generates and tracks active orders, their timers, and completion.
 */
UCLASS()
class SUSHIRESTAURANT_API AOrderManager : public AActor
{
	GENERATED_BODY()

public:
	// -- Public API --
	AOrderManager();

	/** Pool of possible recipes to spawn orders from. */
	UPROPERTY(EditAnywhere, Category="Order")
	TArray<TSubclassOf<URecipeAsset>> AvailableRecipes;

	UPROPERTY(EditAnywhere, Category="Order")
	float OrderGenerationInterval = 30.0f;

	UPROPERTY(EditAnywhere, Category="Order")
	float OrderTimeLimit = 60.0f;

	UPROPERTY(BlueprintAssignable)
	FOrderEvent OnOrderCompleted;

	UPROPERTY(BlueprintAssignable)
	FOrderEvent OnOrderExpired;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Attempts to complete an order for a given table with the delivered dish. */
	UFUNCTION(BlueprintCallable)
	bool TryCompleteOrder(int32 TableID, URecipeAsset* DeliveredDish);

protected:
	// -- Internals --
	void GenerateNewOrder();
	void UpdateOrderTimers(float DeltaTime);
	void CheckExpiredOrders();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	TArray<FSushiOrder> ActiveOrders;

	FTimerHandle OrderGenerationTimer;
};
