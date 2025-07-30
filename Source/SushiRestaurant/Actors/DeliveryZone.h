#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryZone.generated.h"

class UBoxComponent;
class AOrderManager;
class APlateActor;
class URecipeAsset;

/**
 * Trigger area that validates a delivered plate against the active orders.
 */
UCLASS()
class ADeliveryZone : public AActor
{
	GENERATED_BODY()

public:
	// -- Public API --
	ADeliveryZone();

	/** Table identifier this zone serves. */
	UPROPERTY(EditAnywhere, Category="Delivery")
	int32 TableID = 0;

	/** Order manager to validate deliveries. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Delivery")
	AOrderManager* OrderManager = nullptr;

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerZone;

	// -- Internals --
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
};
