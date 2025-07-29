#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.h"
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
	ACookwareStation();

protected:
	// Mesh for the station
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Type of cookware
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	ECookwareType CookwareType;

	// Current pickup being processed
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Cookware")
	APickupActor* CurrentPickup;

	// Timer handle for processing
	FTimerHandle ProcessingTimerHandle;

	// Processing duration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
	float ProcessingTime;

	// Is currently processing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cookware")
	bool bIsProcessing;

	// Progress widget
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UWidgetComponent* ProgressWidget;

public:
	virtual void Interact(APawn* Interactor) override;

	void ReceiveDroppedPickup(APickupActor* Pickup);
	void TryProcessExistingPickup(APawn* Interactor);

protected:
	void StartProcessing();
	void CompleteProcessing();
	bool CanProcessPickup(APickupActor* Pickup) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
