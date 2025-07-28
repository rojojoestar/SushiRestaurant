#include "CookwareStation.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "SushiRestaurantCharacter.h"

// Sets default values
ACookwareStation::ACookwareStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Mesh setup
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Default values
	CookwareType = ECookwareType::CuttingBoard;
	ProcessingTime = 2.0f;
	bIsProcessing = false;
	CurrentIngredient = nullptr;
	
	ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
	ProgressWidget->SetupAttachment(RootComponent);
	ProgressWidget->SetWidgetSpace(EWidgetSpace::Screen);
	ProgressWidget->SetDrawAtDesiredSize(true);
	ProgressWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // Adjust height
	ProgressWidget->SetVisibility(false); // Hidden by default
}

// Interaction entry point
void ACookwareStation::Interact(APawn* Interactor)
{
	// Only the server should process gameplay logic
	if (!HasAuthority())
		return;

	// Check if already processing
	if (bIsProcessing)
		return;

	// Cast to character
	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player)
		return;

	// Check if player has an ingredient
	AIngredientActor* Ingredient = Cast<AIngredientActor>(Player->GetCarriedActor());


	if (Ingredient)
	{
		// Pickup logic as before
		if (!CanProcessIngredient(Ingredient)) return;

		Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		CurrentIngredient = Ingredient;
		bIsProcessing = true;
		StartProcessing();
	}
	else
	{
		// If not holding anything, try process existing
		TryProcessExistingIngredient(Interactor);
	}


	// Check if valid for this station
	if (!CanProcessIngredient(Ingredient))
		return;

	// Attach the ingredient to the station (optional)
	Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	// Set and start processing
	CurrentIngredient = Ingredient;
	bIsProcessing = true;

	StartProcessing();
}

// Validate if the ingredient can be processed here
bool ACookwareStation::CanProcessIngredient(AIngredientActor* Ingredient) const
{
	// Example rule: Cutting board only processes raw fish
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		return Ingredient->GetIngredientType() == EIngredientType::Fish &&
			   Ingredient->GetIngredientState() == EIngredientState::Raw;
	}

	// Extend with other rules for Pot, Assembler, etc.
	return false;
}

// Start timer for processing
void ACookwareStation::StartProcessing()
{
	GetWorldTimerManager().SetTimer(ProcessingTimerHandle, this, &ACookwareStation::CompleteProcessing, ProcessingTime, false);
	ProgressWidget->SetVisibility(true);
}

// Finish processing
void ACookwareStation::CompleteProcessing()
{
	ProgressWidget->SetVisibility(false);
	if (!CurrentIngredient)
		return;

	// Example: Change state from Raw -> Sliced
	if (CookwareType == ECookwareType::CuttingBoard)
	{
		CurrentIngredient->SetIngredientState(EIngredientState::Sliced);
	}

	// Detach and release ingredient
	CurrentIngredient->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentIngredient = nullptr;
	bIsProcessing = false;
}

// Replication settings
void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACookwareStation, CurrentIngredient);
}

void ACookwareStation::ReceiveDroppedIngredient(AIngredientActor* Ingredient)
{
	if (!HasAuthority() || bIsProcessing || CurrentIngredient)
		return;

	CurrentIngredient = Ingredient;
}

void ACookwareStation::TryProcessExistingIngredient(APawn* Interactor)
{
	if (!HasAuthority())
		return;

	if (bIsProcessing || !CurrentIngredient)
		return;

	StartProcessing();
}
