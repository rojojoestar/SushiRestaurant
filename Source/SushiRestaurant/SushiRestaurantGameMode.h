#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SushiRestaurantGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);

/**
 * Simple match timer that broadcasts when the time is over.
 */
UCLASS()
class ASushiRestaurantGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASushiRestaurantGameMode();

	// Clase del GameMode que se puede seleccionar desde el Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<AGameModeBase> SelectedGameModeClass;

	// Instancia del GameMode seleccionado (opcional, si lo quieres instanciar manualmente)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameMode")
	AGameModeBase* SelectedGameModeInstance;

	/** Remaining time in seconds. */
	UFUNCTION(BlueprintPure, Category="Game Timer")
	float GetRemainingTime() const { return TimeRemaining; }

	/** Ends the match early (broadcasts event). */
	UFUNCTION(BlueprintCallable, Category="Game Timer")
	void EndMatchEarly();

	/** Event when the match ends. */
	UPROPERTY(BlueprintAssignable, Category="Game Timer")
	FOnMatchEnded OnMatchEnded;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Timer", meta=(AllowPrivateAccess="true"))
	float MatchDuration = 180.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game Timer", meta=(AllowPrivateAccess="true"))
	float TimeRemaining = 0.f;

	FTimerHandle TimerHandle_Countdown;

	void HandleCountdown();

	UFUNCTION()
	void HandleMatchEnded();
};
