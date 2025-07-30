// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ActorComponent/InteractionComponent.h"
#include "SushiRestaurantCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ASushiRestaurantCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* RunAction;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Carry")
	AActor* CarriedActor;

	UFUNCTION(Blueprintable, Category="Interaction")
	void DropCarriedActor();

public:

	/** Constructor */
	ASushiRestaurantCharacter();
	
	// Return currently carried actor
	AActor* GetCarriedActor() const;
	
	// Attach an actor to the character
	void AttachActor(AActor* ActorToAttach);

	// Detach the currently held actor
	void DetachCarriedActor();

	UFUNCTION(Blueprintable)
	void SetCarriedActor(AActor* NewActor);

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for interact input */
	void Interact(const FInputActionValue& Value);
	/** Called for Run input */
	void Run(const FInputActionValue& Value);

	
	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;



public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Input")
	virtual void DoInteract();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoRun();
};

