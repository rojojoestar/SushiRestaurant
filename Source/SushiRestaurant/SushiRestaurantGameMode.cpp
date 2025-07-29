#include "SushiRestaurantGameMode.h"

#include "SushiRestaurantPlayerController.h"
#include "TimerManager.h"

ASushiRestaurantGameMode::ASushiRestaurantGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	TimeRemaining = MatchDuration;
}

void ASushiRestaurantGameMode::BeginPlay()
{
	Super::BeginPlay();

	TimeRemaining = MatchDuration;

	GetWorldTimerManager().SetTimer(TimerHandle_Countdown, this, &ASushiRestaurantGameMode::HandleCountdown, 1.0f, true);

	// Link controller to event
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			if (ASushiRestaurantPlayerController* SRPC = Cast<ASushiRestaurantPlayerController>(PC))
			{
				OnMatchEnded.AddDynamic(SRPC, &ASushiRestaurantPlayerController::HandleMatchEnded);
			}
		}
	}
}

void ASushiRestaurantGameMode::HandleCountdown()
{
	TimeRemaining -= 1.0f;

	if (TimeRemaining <= 0.0f)
	{
		TimeRemaining = 0.0f;
		GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);

		// Broadcast end match event
		OnMatchEnded.Broadcast();

		// Optionally, do something extra here like pause game, freeze input, etc.
	}
}

void ASushiRestaurantGameMode::EndMatchEarly()
{
	TimeRemaining = 0.0f;
	GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);
	OnMatchEnded.Broadcast();
}

void ASushiRestaurantGameMode::HandleMatchEnded()
{
	// TODO: Implement any cleanup or transition logic here
	UE_LOG(LogTemp, Warning, TEXT("Match has ended."));
}
