#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.generated.h"

class URecipeAsset;

// Enumeration for ingredient states
UENUM(BlueprintType)
enum class EIngredientState : uint8
{
	Raw     UMETA(DisplayName = "Raw"),
	Sliced  UMETA(DisplayName = "Sliced"),
	Cooked  UMETA(DisplayName = "Cooked"),
	Ready   UMETA(DisplayName = "Ready")
};

// Enumeration for ingredient types
UENUM(BlueprintType)
enum class EIngredientType : uint8
{
	None    UMETA(DisplayName = "None"),
	Fish    UMETA(DisplayName = "Fish"),
	Rice    UMETA(DisplayName = "Rice"),
	Seaweed UMETA(DisplayName = "Seaweed")
};

UCLASS()
class APickupActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values
	APickupActor();

protected:
	// Static mesh to represent the pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Ingredient type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientType IngredientType = EIngredientType::None;

	// Ingredient processing state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientState IngredientState = EIngredientState::Raw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plate")
	TSubclassOf<URecipeAsset> FinalDish;

public:
	// Interaction logic
	void Interact_Implementation(APawn* Interactor) override;

	void StopInteract_Implementation(APawn* Interactor) override;
	

	// Getters
	EIngredientType GetIngredientType() const { return IngredientType; }
	EIngredientState GetIngredientState() const { return IngredientState; }
	
	// Setter for processing state
	void SetIngredientState(EIngredientState NewState) { IngredientState = NewState; }
};
