#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.h"
#include "Structs/RecipeData.h"
#include "CookwareStation.generated.h"

class UWidgetComponent;

/**
 * CookwareStation: Represents a cooking station (e.g. cutting board, stove)
 * Can process ingredients when dropped in its DropZone
 */
UCLASS()
class ACookwareStation : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ACookwareStation();

	/** IInteractable: Player can interact with station manually */
	virtual void Interact(APawn* Interactor);

	/** Called by DropZone when a pickup is dropped into the station */
	void ReceiveDroppedPickup(APickupActor* Pickup);

	/** Called by Interact when station already has a pickup */
	void TryProcessExistingPickup(APawn* Interactor);

	/** Scene component where ingredients are placed when dropped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cooking")
	USceneComponent* PlacePoint;

protected:
	/** Main mesh for the station */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** Widget showing progress (optional) */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* ProgressWidget;

	/** Type of station (used to validate ingredients) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	ECookwareType CookwareType;

	/** Current ingredient being processed */
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Cookware")
	APickupActor* CurrentPickup;

	/** Processing state flag */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cookware")
	bool bIsProcessing;

	/** Time it takes to process an ingredient */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	float ProcessingTime;

	/** Offset to adjust PlacePoint position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooking")
	FVector PlacePointOffset = FVector(0.f, 0.f, 20.f);

	/** Timer handle for processing */
	FTimerHandle ProcessingTimerHandle;

protected:
	/** Checks if ingredient is valid for this station */
	bool CanProcessPickup(APickupActor* Pickup) const;

	/** Starts the processing timer */
	void StartProcessing();

	/** Called when timer completes, updates ingredient state */
	void CompleteProcessing();

	/** Replication setup */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
