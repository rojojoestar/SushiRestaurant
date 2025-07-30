#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "Structs/RecipeData.h"  // ECookwareType & steps
#include "CookwareStation.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class USceneComponent;
class APickupActor;

/**
 * Station that processes a PickupActor according to its type.
 * Example: CuttingBoard turns Fish (Raw) into Fish (Sliced).
 */
UCLASS()
class ACookwareStation : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// -- Public API --
	ACookwareStation();

	// IInteractable
	virtual void Interact_Implementation(APawn* Interactor) override;

	/** Receives a pickup dropped by a DropZone (server only). */
	void ReceiveDroppedPickup(APickupActor* Pickup);

	/** Optional: if a pickup is already present, start processing. */
	void TryProcessExistingPickup(APawn* Interactor);

	/** Anchor point where the pickup will be visually placed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooking")
	USceneComponent* PlacePoint;

	/** Offset for the PlacePoint relative location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooking")
	FVector PlacePointOffset = FVector(0.f, 0.f, 20.f);

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="UI")
	UWidgetComponent* ProgressWidget;

	// -- Data --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cookware")
	ECookwareType CookwareType = ECookwareType::CuttingBoard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cookware")
	float ProcessingTime = 2.0f;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Cookware")
	APickupActor* CurrentPickup = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cookware")
	bool bIsProcessing = false;

	// -- Internals --
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Validates if this station can process the given pickup. */
	bool CanProcessPickup(APickupActor* Pickup) const;

	/** Starts processing timer (server only). */
	void StartProcessing();

	/** Finishes processing and updates pickup state. */
	void CompleteProcessing();

private:
	FTimerHandle ProcessingTimerHandle;
};
