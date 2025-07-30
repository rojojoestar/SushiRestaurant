#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "IngredientActor.h"
#include "PickupActor.h"
#include "PlateActor.generated.h"

UCLASS()
class APlateActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Constructor
	APlateActor();

protected:
	// Static mesh component representing the plate
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PlateMesh;

	// List of ingredients currently on the plate
	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TArray<AIngredientActor*> Ingredients;

	// Maximum number of ingredients the plate can hold
	UPROPERTY(EditDefaultsOnly, Category = "Plate")
	int32 MaxIngredients;

public:
	// IInteractable implementation: called when the player interacts
	virtual void Interact_Implementation(APawn* Interactor) override;

	// Try to add an ingredient to the plate
	bool TryAddIngredient(AIngredientActor* Ingredient);

	// Returns the current ingredient actors on the plate
	const TArray<AIngredientActor*>& GetIngredients() const { return Ingredients; }
	
	// Returns a simplified list of ingredient types for comparison
	TArray<EIngredientType> GetIngredientsTypes() const;

	// Stores ingredient types separately for quick validation
	UPROPERTY(VisibleAnywhere)
	TArray<EIngredientType> CurrentIngredients;

	UFUNCTION(BlueprintCallable)
	URecipeAsset* GetFinalIngredient() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Plate")
	TSubclassOf<URecipeAsset> FinalDish;

protected:
	// Arranges ingredients visually around the plate
	void UpdateIngredientPlacement();
};
