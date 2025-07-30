#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropZone.generated.h"

class UBoxComponent;
class ACookwareStation;
class APickupActor;

/**
 * Trigger volume that accepts a PickupActor drop when a player is inside.
 * It forwards the item to the linked CookwareStation.
 */
UCLASS()
class ADropZone : public AActor
{
	GENERATED_BODY()

public:
	// -- Public API --
	ADropZone();

	/** True if at least one pawn is inside the zone. */
	bool IsPlayerInside() const { return PawnsInside.Num() > 0; }

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop Zone")
	UBoxComponent* ZoneTrigger;

	// -- Data --
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Drop Zone")
	ACookwareStation* LinkedStation = nullptr;

	// -- Internals --
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY()
	TSet<TWeakObjectPtr<APawn>> PawnsInside;
};
