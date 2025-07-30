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

	/** IInteractable */
	virtual void Interact_Implementation(APawn* Interactor) override;
	virtual void StopInteract_Implementation(APawn* Interactor) override;

	/** Returns the ingredient type */
	EIngredientType GetIngredientType() const { return IngredientType; }

	/** Optional setter if spawners need to set type in C++ */
	void SetIngredientType(EIngredientType NewType) { IngredientType = NewType; }

	/** Returns current processing state */
	EIngredientState GetIngredientState() const { return IngredientState; }

	/** Updates state and applies the visual change (server) */
	void SetIngredientState(EIngredientState NewState);

	/** Final dish accessor */
	TSubclassOf<URecipeAsset> GetFinalDish() const { return FinalDish; }
	void SetFinalDish(TSubclassOf<URecipeAsset> InDish) { FinalDish = InDish; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** Type of ingredient (Fish, Rice, Seaweed, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientType IngredientType = EIngredientType::None;

	/** Processing state (replicated) */
	UPROPERTY(ReplicatedUsing=OnRep_IngredientState, VisibleAnywhere, BlueprintReadOnly, Category = "Ingredient")
	EIngredientState IngredientState = EIngredientState::Raw;

	/** Optional "final dish" data asset class (e.g., Sushi) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plate")
	TSubclassOf<URecipeAsset> FinalDish;

	/** Meshes per state; if set, the mesh will update automatically when state changes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	TMap<EIngredientState, UStaticMesh*> StateMeshes;
	
	virtual void BeginPlay() override;

	/** Applies the static mesh that corresponds to current IngredientState (if any) */
	void ApplyStateVisuals();

	/** Replication notification for IngredientState */
	UFUNCTION()
	void OnRep_IngredientState();

	/** Replication setup */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
