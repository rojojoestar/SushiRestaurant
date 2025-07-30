#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SushiRestaurantCharacter.generated.h"

class UInputAction;
struct FInputActionValue;
class UInteractionComponent;

/**
 * Third-person character with interaction and carry/drop logic.
 */
UCLASS(abstract)
class ASushiRestaurantCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// -- Public API --
	ASushiRestaurantCharacter();

	/** Returns the actor currently carried (if any). */
	AActor* GetCarriedActor() const { return CarriedActor; }

	/** Attaches an actor to the hand socket. */
	void AttachActor(AActor* ActorToAttach);

	/** Physically detaches the actor and restores physics. */
	void DetachCarriedActor();

	/** Sets the carried actor (exposed for BP if needed). */
	UFUNCTION(BlueprintCallable)
	void SetCarriedActor(AActor* NewActor);

	/** Input-to-logic entry points (called by input bindings). */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	// Kept as Server RPC because your code already used it this way
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Input")
	virtual void DoInteract();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoRun();

protected:
	// -- Components --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction", meta=(AllowPrivateAccess="true"))
	UInteractionComponent* InteractionComponent;

	// -- Input --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* InteractAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* RunAction = nullptr;

	// -- Carry state --
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Carry")
	AActor* CarriedActor = nullptr;

	/** Drops the carried actor properly (calls StopInteract on it). */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void DropCarriedActor();

	// -- Internals --
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Input handlers
	void Move(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
};
