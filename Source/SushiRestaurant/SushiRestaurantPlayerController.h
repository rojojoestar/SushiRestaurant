#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SushiRestaurantPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 * PlayerController that sets input contexts and handles the Game Over UI.
 */
UCLASS(abstract)
class ASushiRestaurantPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// -- Public API --
	UFUNCTION()
	void HandleMatchEnded();

protected:
	// -- UI --
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY() UUserWidget* GameOverWidget = nullptr;

	// -- Input --
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess="true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	// -- Internals --
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Settings")
	float CameraFixedHeight = 500.f;
};
