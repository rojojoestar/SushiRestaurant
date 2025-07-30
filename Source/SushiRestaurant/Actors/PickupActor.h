#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.generated.h"

class UStaticMeshComponent;
class URecipeAsset;

/** Ingredient processing state for a pickup. */
UENUM(BlueprintType)
enum class EIngredientState : uint8
{
	Raw     UMETA(DisplayName="Raw"),
	Sliced  UMETA(DisplayName="Sliced"),
	Cooked  UMETA(DisplayName="Cooked"),
	Ready   UMETA(DisplayName="Ready")
};

/** Ingredient type for a pickup. */
UENUM(BlueprintType)
enum class EIngredientType : uint8
{
	None    UMETA(DisplayName="None"),
	Fish    UMETA(DisplayName="Fish"),
	Rice    UMETA(DisplayName="Rice"),
	Seaweed UMETA(DisplayName="Seaweed")
};

/**
 * Generic pickup the player can carry and process at stations.
 */
UCLASS()
class APickupActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// -- Public API --
	APickupActor();

	// IInteractable
	virtual void Interact_Implementation(APawn* Interactor) override;
	virtual void StopInteract_Implementation(APawn* Interactor) override;

	// Getters/Setters
	EIngredientType  GetIngredientType()  const { return IngredientType; }
	EIngredientState GetIngredientState() const { return IngredientState; }
	void SetIngredientState(EIngredientState NewState) { IngredientState = NewState; }

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	// -- Data --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient")
	EIngredientType IngredientType = EIngredientType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ingredient")
	EIngredientState IngredientState = EIngredientState::Raw;

	/** Optional: the final dish this pickup represents (used when placed on a plate). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Plate")
	TSubclassOf<URecipeAsset> FinalDish;
};
