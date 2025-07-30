#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.h"
#include "Misc/RecipeAsset.h"
#include "PlateActor.generated.h"

/**
 * PlateActor: Represents a plate that can hold processed ingredients.
 * Player can interact with it to add ingredients or pick it up.
 */
UCLASS()
class APlateActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	/** Constructor */
	APlateActor();

	/** IInteractable: Called when the player interacts with this plate */
	virtual void Interact_Implementation(APawn* Interactor) override;

	/** IInteractable: Called when the player Stop interact with this plate */
	virtual void StopInteract_Implementation(APawn* Interactor) override;

	/** Returns a list of ingredient types currently on the plate */
	TArray<EIngredientType> GetIngredientsTypes() const;

	/** Returns the full recipe asset if this plate is complete */
	UFUNCTION(BlueprintCallable)
	URecipeAsset* GetFinalIngredient() const;

protected:
	/** Mesh representing the plate */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PlateMesh;

	/** Holds the current ingredient actors attached to the plate */
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TArray<APickupActor*> Ingredients;

	/** Maximum number of ingredients allowed on the plate */
	UPROPERTY(EditDefaultsOnly, Category = "Plate")
	int32 MaxIngredients;

	/** Simplified list of ingredient types for order validation */
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TArray<EIngredientType> CurrentIngredients;

	/** Final dish asset (linked when plate is completed) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Plate")
	TSubclassOf<URecipeAsset> FinalDish;

protected:
	/** Adds an ingredient actor to the plate (only if valid) */
	bool TryAddIngredient(APickupActor* Ingredient);

	/** Rearranges ingredient positions visually on the plate */
	void UpdateIngredientPlacement();
};
