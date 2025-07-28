#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.generated.h"

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

public:
	// Interaction logic
	virtual void Interact(APawn* Interactor) override;

	// Getter for mesh
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
