#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SushiRestaurantGameMode.generated.h"

// Delegate to notify when the match ends
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);

UCLASS()
class ASushiRestaurantGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASushiRestaurantGameMode();

protected:
	virtual void BeginPlay() override;

	// Total match time (in seconds)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Timer")
	float MatchDuration = 180.0f;

	// Remaining time
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Timer")
	float TimeRemaining;

	// Countdown tick
	FTimerHandle TimerHandle_Countdown;

	// Updates the remaining time
	void HandleCountdown();

public:
	// Event called when time runs out
	UPROPERTY(BlueprintAssignable, Category = "Game Timer")
	FOnMatchEnded OnMatchEnded;

	UFUNCTION(BlueprintPure, Category = "Game Timer")
	float GetRemainingTime() const { return TimeRemaining; }

	UFUNCTION(BlueprintCallable, Category = "Game Timer")
	void EndMatchEarly();

	UFUNCTION()
	void HandleMatchEnded(); // ✔ Accesible desde GameMode

};
