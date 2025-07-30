#include "PlateActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"
#include "Actors/IngredientActor.h"   // TODO(next-commit): remove after unifying
#include "Misc/RecipeAsset.h"

// ---------- Constructor ----------
APlateActor::APlateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	SetRootComponent(PlateMesh);
}

// ---------- IInteractable ----------
void APlateActor::Interact_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	// If the player is holding a processed ingredient, try to add it
	if (AIngredientActor* Ingredient = Cast<AIngredientActor>(Player->GetCarriedActor()))
	{
		if (TryAddIngredient(Ingredient))
		{
			Player->DetachCarriedActor(); // The ingredient stays attached to the plate
		}
		return;
	}

	// Empty-handed: pick up the plate
	if (!Player->GetCarriedActor())
	{
		Player->AttachActor(this);
	}
}

// ---------- Internals ----------
bool APlateActor::TryAddIngredient(AIngredientActor* Ingredient)
{
	if (!Ingredient || Ingredients.Num() >= MaxIngredients) return false;

	// Only non-raw ingredients allowed
	if (Ingredient->GetIngredientStates() == EIngredientStates::Raw) return false;

	Ingredient->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	Ingredients.Add(Ingredient);

	UpdateIngredientPlacement();
	CurrentIngredients.Add(static_cast<EIngredientType>(Ingredient->GetIngredientTypes()));

	return true;
}

TArray<EIngredientType> APlateActor::GetIngredientsTypes() const
{
	return CurrentIngredients;
}

URecipeAsset* APlateActor::GetFinalIngredient() const
{
	return FinalDish ? FinalDish.GetDefaultObject() : nullptr;
}

void APlateActor::UpdateIngredientPlacement()
{
	const float Radius = 15.f;
	const FVector Center(0.f, 0.f, 10.f);

	for (int32 i = 0; i < Ingredients.Num(); ++i)
	{
		const float Angle = (360.f / Ingredients.Num()) * i;
		const float Rad = FMath::DegreesToRadians(Angle);
		const FVector Offset(FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius, 10.f);

		if (AActor* const Item = Ingredients[i])
		{
			Item->SetActorRelativeLocation(Center + Offset);
			Item->SetActorRelativeRotation(FRotator::ZeroRotator);
		}
	}
}
