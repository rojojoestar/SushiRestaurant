#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "IngredientActor.generated.h"

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
	EIngredientType IngredientType;

	// Current state of the ingredient
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientState IngredientState;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Interact interface implementation
	virtual void Interact(APawn* Interactor) override;

	// Getters
	EIngredientType GetIngredientType() const { return IngredientType; }
	EIngredientState GetIngredientState() const { return IngredientState; }

	// Setters
	void SetIngredientState(EIngredientState NewState) { IngredientState = NewState; }
};
