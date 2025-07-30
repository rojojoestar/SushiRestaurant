// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SushiRestaurantPlayerController.generated.h"

class UInputMappingContext;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ASushiRestaurantPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
	

	// Reference to Game Over Widget class
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	// Active widget
	UPROPERTY()
	UUserWidget* GameOverWidget;

	virtual void BeginPlay() override;

	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaSeconds) override;

public:
	UFUNCTION()
	void HandleMatchEnded();

	// Setup Height Camera (Editable from editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float CameraFixedHeight = 500.0f; 

};
