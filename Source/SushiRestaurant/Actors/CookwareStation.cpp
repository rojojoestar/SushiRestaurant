#include "CookwareStation.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurantCharacter.h"

// Sets default values
ACookwareStation::ACookwareStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

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

// Interact entry point
void ACookwareStation::Interact(APawn* Interactor)
{
	if (!HasAuthority()) return;
	if (bIsProcessing) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	APickupActor* Pickup = Cast<APickupActor>(Player->GetCarriedActor());

	if (Pickup)
	{
		if (!CanProcessPickup(Pickup)) return;

		Pickup->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		CurrentPickup = Pickup;
		bIsProcessing = true;
		StartProcessing();
	}
	else
	{
		TryProcessExistingPickup(Interactor);
	}
}

// Check if pickup is valid
bool ACookwareStation::CanProcessPickup(APickupActor* Pickup) const
{
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		return Pickup->GetIngredientType() == EIngredientType::Fish &&
		       Pickup->GetIngredientState() == EIngredientState::Raw;
	}

	// Extend with other cookware logic
	return false;
}

// Start cooking timer
void ACookwareStation::StartProcessing()
{
	GetWorldTimerManager().SetTimer(ProcessingTimerHandle, this, &ACookwareStation::CompleteProcessing, ProcessingTime, false);
	ProgressWidget->SetVisibility(true);
}

// Finish cooking
void ACookwareStation::CompleteProcessing()
{
	ProgressWidget->SetVisibility(false);
	if (!CurrentPickup) return;

	if (CookwareType == ECookwareType::CuttingBoard)
	{
		CurrentPickup->SetIngredientState(EIngredientState::Sliced);
	}

	CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentPickup = nullptr;
	bIsProcessing = false;
}

// Handle drop zone logic
void ACookwareStation::ReceiveDroppedPickup(APickupActor* Pickup)
{
	if (!HasAuthority() || bIsProcessing || CurrentPickup) return;

	CurrentPickup = Pickup;
}

// Manual processing
void ACookwareStation::TryProcessExistingPickup(APawn* Interactor)
{
	if (!HasAuthority()) return;
	if (bIsProcessing || !CurrentPickup) return;

	StartProcessing();
}

// Replication setup
void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACookwareStation, CurrentPickup);
}
