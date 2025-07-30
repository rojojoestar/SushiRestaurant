#include "SushiRestaurantPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

// ---------- Input ----------
void ASushiRestaurantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Disable input initially (per your current behavior)
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	// Create and show Game Over UI now (you may want to move this to HandleMatchEnded)
	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* Ctx : DefaultMappingContexts)
		{
			Subsys->AddMappingContext(Ctx, 0);
		}
	}
}

// ---------- Lifecycle ----------
void ASushiRestaurantPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASushiRestaurantPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Delay camera selection for one second to ensure level actors exist
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (ACameraActor* LevelCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass())))
		{
			SetViewTarget(LevelCamera);
			UE_LOG(LogTemp, Warning, TEXT("Camera set to: %s"), *LevelCamera->GetActorLocation().ToString());
		}
	}, 1.0f, false);
}

void ASushiRestaurantPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// ---------- Events ----------
void ASushiRestaurantPlayerController::HandleMatchEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("Game has ended. Controller received the signal."));
	// TODO(next-commit): Show Game Over screen or transition camera here
}
