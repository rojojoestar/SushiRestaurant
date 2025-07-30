#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PlateActor.generated.h"

class UStaticMeshComponent;
class URecipeAsset;
class AIngredientActor;        // TODO(next-commit): remove after unifying into PickupActor
enum class EIngredientType : uint8;

/**
 * Plate that can hold processed ingredients and be delivered.
 */
UCLASS()
class APlateActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// -- Public API --
	APlateActor();

	// IInteractable
	virtual void Interact_Implementation(APawn* Interactor) override;

	/** Returns the current ingredient actors on the plate. */
	const TArray<AIngredientActor*>& GetIngredients() const { return Ingredients; }

	/** Returns a simplified list of ingredient types. */
	TArray<EIngredientType> GetIngredientsTypes() const;

	/** Returns the final dish asset if this plate represents one. */
	UFUNCTION(BlueprintCallable)
	URecipeAsset* GetFinalIngredient() const;

	/** Optional: the "final dish" represented by this plate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Plate")
	TSubclassOf<URecipeAsset> FinalDish;

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* PlateMesh;

	// -- Data --
	UPROPERTY(VisibleAnywhere, Category="Plate")
	TArray<AIngredientActor*> Ingredients; // TODO(next-commit): switch to APickupActor*

	UPROPERTY(EditDefaultsOnly, Category="Plate")
	int32 MaxIngredients = 3;

	// -- Internals --
	/** Attempts to add a processed ingredient to the plate. */
	bool TryAddIngredient(AIngredientActor* Ingredient);

	/** Layouts ingredients around the plate center. */
	void UpdateIngredientPlacement();

	/** Cached ingredient types for quick validation. */
	UPROPERTY(VisibleAnywhere, Category="Plate")
	TArray<EIngredientType> CurrentIngredients;
};
