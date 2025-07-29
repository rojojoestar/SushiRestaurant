#include "PlateActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"

// Constructor
APlateActor::APlateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create and set up mesh component
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	RootComponent = PlateMesh;

	// Default max capacity
	MaxIngredients = 3;
}

// Called when player interacts with the plate
void APlateActor::Interact(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	// Try to add the ingredient if the player is carrying one
	if (AIngredientActor* Ingredient = Cast<AIngredientActor>(Player->GetCarriedActor()))
	{
		if (TryAddIngredient(Ingredient))
		{
			Player->DetachCarriedActor(); // Drop the ingredient
		}
	}
	else if (!Player->GetCarriedActor())
	{
		// If empty-handed, pick up the plate
		Player->AttachActor(this);
	}
}

// Attempts to add a processed ingredient to the plate
bool APlateActor::TryAddIngredient(AIngredientActor* Ingredient)
{
	if (!Ingredient || Ingredients.Num() >= MaxIngredients)
		return false;

	// Only allow processed (not raw) ingredients
	if (Ingredient->GetIngredientStates() == EIngredientStates::Raw)
		return false;

	// Attach to plate mesh
	Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	Ingredients.Add(Ingredient);

	// Rearrange for visual clarity
	UpdateIngredientPlacement();

	// Track ingredient type
	CurrentIngredients.Add(Ingredient->GetIngredientTypes());

	return true;
}

// Returns the list of ingredient types currently on the plate
TArray<EIngredientType> APlateActor::GetIngredientsTypes() const
{
	return CurrentIngredients;
}

// Repositions ingredients in a circle above the plate
void APlateActor::UpdateIngredientPlacement()
{
	const float Radius = 15.f;
	const FVector Center = FVector(0, 0, 10.f);

	for (int32 i = 0; i < Ingredients.Num(); ++i)
	{
		float Angle = (360.f / Ingredients.Num()) * i;
		float Rad = FMath::DegreesToRadians(Angle);
		FVector Offset = FVector(FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius, 10.f);

		if (Ingredients[i])
		{
			Ingredients[i]->SetActorRelativeLocation(Center + Offset);
			Ingredients[i]->SetActorRelativeRotation(FRotator::ZeroRotator);
		}
	}
}
