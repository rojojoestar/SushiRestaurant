#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class IInteractable;

/**
 * Component that finds an interactable actor in front of its owner.
 * Uses a sphere sweep to detect APickupActor and picks the closest one.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// -- Public API --
	UInteractionComponent();

	/** Attempts to interact with the best candidate in front of the owner. */
	void TryInteract();

protected:
	// -- Internals --
	virtual void BeginPlay() override;

	/** Returns the closest interactable actor in front of the owner, or nullptr. */
	AActor* GetInteractableInFront() const;
};
