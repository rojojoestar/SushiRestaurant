// OrderManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/RecipeAsset.h"
#include "OrderManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOrderEvent);

USTRUCT(BlueprintType)
struct FSushiOrder
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Order")
	int32 TableID = -1;

	UPROPERTY(BlueprintReadWrite, Category = "Order")
	float RemainingTime = 60.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Order")
	TSubclassOf<URecipeAsset> Recipe;
};

UCLASS()
class SUSHIRESTAURANT_API AOrderManager : public AActor
{
	GENERATED_BODY()

public:
	AOrderManager();

	UPROPERTY(EditAnywhere, Category = "Order")
	TArray<TSubclassOf<URecipeAsset>> AvailableRecipes;

	UPROPERTY(EditAnywhere, Category = "Order")
	float OrderGenerationInterval = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Order")
	float OrderTimeLimit = 60.0f;

	UPROPERTY(BlueprintAssignable)
	FOrderEvent OnOrderCompleted;

	UPROPERTY(BlueprintAssignable)
	FOrderEvent OnOrderExpired;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool TryCompleteOrder(int32 TableID, URecipeAsset* DeliveredDish);

protected:
	void GenerateNewOrder();
	void UpdateOrderTimers(float DeltaTime);
	void CheckExpiredOrders();

private:
	UPROPERTY(Replicated)
	TArray<FSushiOrder> ActiveOrders;

	FTimerHandle OrderGenerationTimer;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};