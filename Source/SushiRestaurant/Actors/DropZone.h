#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropZone.generated.h"

class UBoxComponent;
class ACookwareStation;

UCLASS()
class ADropZone : public AActor
{
	GENERATED_BODY()

public:
	ADropZone();

protected:
	// Visible drop area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Zone")
	UBoxComponent* ZoneTrigger;

	// Associated station
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Drop Zone")
	ACookwareStation* LinkedStation;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);
};
