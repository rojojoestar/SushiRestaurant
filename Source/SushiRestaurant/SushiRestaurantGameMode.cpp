#include "SushiRestaurantGameMode.h"
#include "TimerManager.h"
#include "SushiRestaurantPlayerController.h"

// ---------- Constructor ----------
ASushiRestaurantGameMode::ASushiRestaurantGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	TimeRemaining = MatchDuration;
}

// ---------- Lifecycle ----------
void ASushiRestaurantGameMode::BeginPlay()
{
	Super::BeginPlay();

	TimeRemaining = MatchDuration;

	GetWorldTimerManager().SetTimer(TimerHandle_Countdown, this, &ASushiRestaurantGameMode::HandleCountdown, 1.0f, true);

	// Bind all existing controllers to end-match event
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASushiRestaurantPlayerController* SRPC = Cast<ASushiRestaurantPlayerController>(It->Get()))
		{
			OnMatchEnded.AddDynamic(SRPC, &ASushiRestaurantPlayerController::HandleMatchEnded);
		}
	}
}

void ASushiRestaurantGameMode::HandleCountdown()
{
	TimeRemaining -= 1.f;

	if (TimeRemaining <= 0.f)
	{
		TimeRemaining = 0.f;
		GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);
		OnMatchEnded.Broadcast();
	}
}

void ASushiRestaurantGameMode::EndMatchEarly()
{
	TimeRemaining = 0.f;
	GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);
	OnMatchEnded.Broadcast();
}

void ASushiRestaurantGameMode::HandleMatchEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("Match ended."));
}
