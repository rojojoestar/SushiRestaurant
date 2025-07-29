#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.generated.h"

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
	EIngredientType IngredientType;

	// Ingredient processing state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientState IngredientState;

public:
	// Interaction logic
	virtual void Interact(APawn* Interactor) override;

	// Getters
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	FORCEINLINE EIngredientType GetIngredientType() const { return IngredientType; }
	FORCEINLINE EIngredientState GetIngredientState() const { return IngredientState; }

	// Setter for processing state
	void SetIngredientState(EIngredientState NewState);
};
