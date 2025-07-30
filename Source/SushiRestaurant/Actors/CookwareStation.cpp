#include "CookwareStation.h"
#include "Actors/PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurantCharacter.h"

// ---------- Constructor ----------
ACookwareStation::ACookwareStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	PlacePoint = CreateDefaultSubobject<USceneComponent>(TEXT("PlacePoint"));
	PlacePoint->SetupAttachment(MeshComponent);
	PlacePoint->SetRelativeLocation(PlacePointOffset);

	ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
	ProgressWidget->SetupAttachment(MeshComponent);
	ProgressWidget->SetWidgetSpace(EWidgetSpace::Screen);
	ProgressWidget->SetDrawAtDesiredSize(true);
	ProgressWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	ProgressWidget->SetVisibility(false);
}

// ---------- IInteractable ----------
void ACookwareStation::Interact_Implementation(APawn* Interactor)
{
	if (!HasAuthority() || bIsProcessing) return;

	if (ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor))
	{
		if (APickupActor* Pickup = Cast<APickupActor>(Player->GetCarriedActor()))
		{
			if (!CanProcessPickup(Pickup)) return;

			// Snap to station and begin
			Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			CurrentPickup = Pickup;
			bIsProcessing = true;
			StartProcessing();
			return;
		}
	}

	// If player is not carrying anything, process an existing pickup if any.
	TryProcessExistingPickup(Interactor);
}

// ---------- Public station helpers ----------
void ACookwareStation::ReceiveDroppedPickup(APickupActor* Pickup)
{
	if (!HasAuthority() || bIsProcessing || CurrentPickup) return;
	if (!CanProcessPickup(Pickup)) return;

	Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CurrentPickup = Pickup;
	bIsProcessing = true;
	StartProcessing();
}

void ACookwareStation::TryProcessExistingPickup(APawn* /*Interactor*/)
{
	if (!HasAuthority() || bIsProcessing || !CurrentPickup) return;
	StartProcessing();
}

// ---------- Internals ----------
bool ACookwareStation::CanProcessPickup(APickupActor* Pickup) const
{
	if (!Pickup) return false;

	// Example rule for Cutting Board
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		return Pickup->GetIngredientType() == EIngredientType::Fish &&
		       Pickup->GetIngredientState() == EIngredientState::Raw;
	}

	// TODO(next-commit): Extend for Stove / MixingTable, etc.
	return false;
}

void ACookwareStation::StartProcessing()
{
	GetWorldTimerManager().SetTimer(
		ProcessingTimerHandle,
		this,
		&ACookwareStation::CompleteProcessing,
		ProcessingTime,
		false
	);

	ProgressWidget->SetVisibility(true);
}

void ACookwareStation::CompleteProcessing()
{
	ProgressWidget->SetVisibility(false);
	if (!CurrentPickup) return;

	// Example: Cutting board produces "Sliced"
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		CurrentPickup->SetIngredientState(EIngredientState::Sliced);
	}

	// Detach and leave it at the station (player can pick it again)
	CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentPickup = nullptr;
	bIsProcessing = false;
}

void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACookwareStation, CurrentPickup);
}
