#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "IngredientActor.generated.h"

// Enumeration for ingredient states
UENUM(BlueprintType)
enum class EIngredientStates : uint8
{
	Raw     UMETA(DisplayName = "Raw"),
	Sliced  UMETA(DisplayName = "Sliced"),
	Cooked  UMETA(DisplayName = "Cooked"),
	Ready   UMETA(DisplayName = "Ready")
};

// Enumeration for ingredient types
UENUM(BlueprintType)
enum class EIngredientTypes : uint8
{
	None    UMETA(DisplayName = "None"),
	Fish    UMETA(DisplayName = "Fish"),
	Rice    UMETA(DisplayName = "Rice"),
	Seaweed UMETA(DisplayName = "Seaweed")
};

UCLASS()
class AIngredientActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIngredientActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mesh for the ingredient
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Ingredient type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientTypes IngredientTypes;

	// Current state of the ingredient
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientStates IngredientStates;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Interact interface implementation
	virtual void Interact(APawn* Interactor) override;

	// Getters
	EIngredientTypes GetIngredientTypes() const { return IngredientTypes; }
	EIngredientStates GetIngredientStates() const { return IngredientStates; }

	// Setters
	void SetIngredientState(EIngredientStates NewState) { IngredientStates = NewState; }
};
