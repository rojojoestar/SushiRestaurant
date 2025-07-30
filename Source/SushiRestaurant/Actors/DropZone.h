#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropZone.generated.h"

class UBoxComponent;
class ACookwareStation;
class APickupActor;

UCLASS()
class ADropZone : public AActor
{
	GENERATED_BODY()

public:
	ADropZone();
	bool IsPlayerInside() const { return PawnsInside.Num() > 0; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Zone")
	UBoxComponent* ZoneTrigger;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Drop Zone")
	ACookwareStation* LinkedStation;

	// Track which pawns are currently inside the DropZone
	UPROPERTY()
	TSet<TWeakObjectPtr<APawn>> PawnsInside;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
