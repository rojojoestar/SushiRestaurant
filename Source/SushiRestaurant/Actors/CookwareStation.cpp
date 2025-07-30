#include "CookwareStation.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurantCharacter.h"

ACookwareStation::ACookwareStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Root mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Place point for pickups
	PlacePoint = CreateDefaultSubobject<USceneComponent>(TEXT("PlacePoint"));
	PlacePoint->SetupAttachment(MeshComponent);
	PlacePoint->SetRelativeLocation(PlacePointOffset);

	// Progress widget (UI)
	ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
	ProgressWidget->SetupAttachment(RootComponent);
	ProgressWidget->SetWidgetSpace(EWidgetSpace::Screen);
	ProgressWidget->SetDrawAtDesiredSize(true);
	ProgressWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	ProgressWidget->SetVisibility(false);

	CookwareType = ECookwareType::CuttingBoard;
	ProcessingTime = 2.0f;
	bIsProcessing = false;
	CurrentPickup = nullptr;
}

/** Player presses interact */
void ACookwareStation::Interact(APawn* Interactor)
{
	if (!HasAuthority() || bIsProcessing) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	APickupActor* Pickup = Cast<APickupActor>(Player->GetCarriedActor());
	if (Pickup)
	{
		if (!CanProcessPickup(Pickup)) return;

		// Place and process
		Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CurrentPickup = Pickup;
		bIsProcessing = true;
		StartProcessing();
	}
	else
	{
		TryProcessExistingPickup(Interactor);
	}
}

/** Validates ingredient */
bool ACookwareStation::CanProcessPickup(APickupActor* Pickup) const
{
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		return Pickup->GetIngredientType() == EIngredientType::Fish &&
			   Pickup->GetIngredientState() == EIngredientState::Raw;
	}
	return false;
}

/** Called by DropZone when pickup is dropped */
void ACookwareStation::ReceiveDroppedPickup(APickupActor* Pickup)
{
	if (!HasAuthority() || bIsProcessing || CurrentPickup) return;

	// Snap to PlacePoint
	Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	CurrentPickup = Pickup;
	StartProcessing();
}

/** If there's already a pickup in station, process it */
void ACookwareStation::TryProcessExistingPickup(APawn* Interactor)
{
	if (!HasAuthority() || bIsProcessing || !CurrentPickup) return;

	StartProcessing();
}

/** Starts timer */
void ACookwareStation::StartProcessing()
{
	bIsProcessing = true;
	ProgressWidget->SetVisibility(true);

	GetWorldTimerManager().SetTimer(ProcessingTimerHandle, this,
		&ACookwareStation::CompleteProcessing, ProcessingTime, false);
}

/** Completes processing */
void ACookwareStation::CompleteProcessing()
{
	ProgressWidget->SetVisibility(false);

	if (!CurrentPickup) return;

	// Change ingredient state (example: raw -> sliced)
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		CurrentPickup->SetIngredientState(EIngredientState::Sliced);
	}

	// Allow pickup to be grabbed again
	CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentPickup = nullptr;
	bIsProcessing = false;
}

/** Replication setup */
void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACookwareStation, CurrentPickup);
}
