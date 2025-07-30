#include "PlateActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"

/** Constructor */
APlateActor::APlateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup plate mesh as root
	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	RootComponent = PlateMesh;

	MaxIngredients = 3;
}

/** Player interacts with the plate */
void APlateActor::Interact_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	// If player is holding an ingredient -> try to add it
	if (APickupActor* Ingredient = Cast<APickupActor>(Player->GetCarriedActor()))
	{
		if (TryAddIngredient(Ingredient))
		{
			// Detach from player after adding
			Player->DetachCarriedActor();
		}
	}
	// If player is empty-handed -> pick up the plate
	else if (!Player->GetCarriedActor())
	{
		Player->AttachActor(this);
	}
}

void APlateActor::StopInteract_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	// Detach from character
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Character->SetCarriedActor(nullptr);

	// Reactivate physics and collision so the plate drops naturally
	if (UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// Optional: small forward offset to avoid clipping
	FVector DropLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f;
	SetActorLocation(DropLocation);
}


/** Adds ingredient to the plate if processed */
bool APlateActor::TryAddIngredient(APickupActor* Ingredient)
{
	if (!Ingredient || Ingredients.Num() >= MaxIngredients)
		return false;

	// Do not allow raw ingredients on plate
	if (Ingredient->GetIngredientState() == EIngredientState::Raw)
		return false;

	// Attach to plate mesh
	Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	Ingredients.Add(Ingredient);

	// Track type and reposition visuals
	CurrentIngredients.Add(Ingredient->GetIngredientType());
	UpdateIngredientPlacement();

	return true;
}

/** Returns ingredient types currently on the plate */
TArray<EIngredientType> APlateActor::GetIngredientsTypes() const
{
	return CurrentIngredients;
}

/** Returns final dish asset if plate is complete */
URecipeAsset* APlateActor::GetFinalIngredient() const
{
	return FinalDish ? FinalDish.GetDefaultObject() : nullptr;
}

/** Rearranges ingredients visually on the plate */
void APlateActor::UpdateIngredientPlacement()
{
	const float Radius = 15.f;
	const FVector Center = FVector(0, 0, 10.f);

	for (int32 i = 0; i < Ingredients.Num(); ++i)
	{
		float Angle = (360.f / Ingredients.Num()) * i;
		FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
								 FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
								 10.f);

		if (Ingredients[i])
		{
			Ingredients[i]->SetActorRelativeLocation(Center + Offset);
			Ingredients[i]->SetActorRelativeRotation(FRotator::ZeroRotator);
		}
	}
}
