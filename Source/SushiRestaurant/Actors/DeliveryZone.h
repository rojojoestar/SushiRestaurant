#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryZone.generated.h"

class AOrderManager;
// Represents a delivery area for one table
UCLASS()
class ADeliveryZone : public AActor
{
	GENERATED_BODY()

public:
	ADeliveryZone();

	// Identifier for the table this zone serves
	UPROPERTY(EditAnywhere, Category = "Delivery")
	int32 TableID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	AOrderManager* OrderManager;

protected:
	// Collision box that detects plates
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerZone;

	// Overlap callback
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
};
