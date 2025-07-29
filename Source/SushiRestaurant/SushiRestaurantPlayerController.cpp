// Copyright Epic Games, Inc. All Rights Reserved.


#include "SushiRestaurantPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"

void ASushiRestaurantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Disable Input
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	// Create and show the Game Over UI
	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}

void ASushiRestaurantPlayerController::HandleMatchEnded()
{
	// For now, just log something
	UE_LOG(LogTemp, Warning, TEXT("Game has ended. Controller received the signal."));

	// TODO: Add Game Over screen display or camera transition here
}
