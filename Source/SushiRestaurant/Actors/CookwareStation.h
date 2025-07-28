#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "IngredientActor.h"
#include "CookwareStation.generated.h"

UENUM(BlueprintType)
enum class ECookwareType : uint8
{
	CuttingBoard UMETA(DisplayName = "Cutting Board"),
	Pot          UMETA(DisplayName = "Pot"),
	Assembler    UMETA(DisplayName = "Assembler")
};
class UWidgetComponent;


UCLASS()
class ACookwareStation : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values
	ACookwareStation();

protected:
	// Mesh for the station
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Type of cookware
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	ECookwareType CookwareType;

	// Current ingredient being processed
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Cookware")
	AIngredientActor* CurrentIngredient;

	// Timer handle for processing
	FTimerHandle ProcessingTimerHandle;

	// Duration of processing (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	float ProcessingTime;

	// Flag to prevent overlapping uses
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cookware")
	bool bIsProcessing;

public:
	// Interact implementation
	virtual void Interact(APawn* Interactor) override;

	// External drop handler
	void ReceiveDroppedIngredient(AIngredientActor* Ingredient);

	// Manual trigger if player is not holding anything
	void TryProcessExistingIngredient(APawn* Interactor);

protected:
	// Begin processing ingredient
	void StartProcessing();

	// Finish processing logic
	void CompleteProcessing();

	// Validate ingredient before processing
	bool CanProcessIngredient(AIngredientActor* Ingredient) const;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* ProgressWidget;

};
