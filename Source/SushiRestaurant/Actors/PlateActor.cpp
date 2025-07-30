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

void APlateActor::BeginPlay()
{
	Super::BeginPlay();

	PlateMesh->SetCollisionProfileName(TEXT("PhysicsActor")); // o BlockAllDynamic si no es físico
	PlateMesh->SetSimulatePhysics(false); // por defecto sin físicas


}

/** Player interacts with the plate */
void APlateActor::Interact_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Player) return;

	// Si ya lleva algo en las manos, no puede recoger
	if (Player->GetCarriedActor()) return;

	// 1. Deactive Physics & Collisions before attach
	PlateMesh->SetSimulatePhysics(false);
	PlateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlateMesh->SetEnableGravity(false);

	// 2. Attach to socket
	AttachToComponent(Player->GetMesh(), 
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
		TEXT("hand_rSocket"));

	// 3. Registry Player that is carrying the actor
	Player->SetCarriedActor(this);
}

void APlateActor::StopInteract_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	// Detach from character
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Character->SetCarriedActor(nullptr);

	// Search the ground under the character
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 500.f); // 5m hacia abajo
	FHitResult Hit;

	bool bHasGround = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_WorldStatic
	);


	// Reactivate physics and collision so the plate drops naturally
	if (UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>())
	{
		if (bHasGround)
		{
			// Colócalo justo sobre el suelo y activa físicas
			SetActorLocation(Hit.ImpactPoint + FVector(0, 0, 2.f));
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		else
		{
			// Si no hay suelo, lo dejamos quieto sin físicas
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
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

	// If this pickup is a final dish (e.g., Sushi), attach it and mark plate as "complete dish"
	if (Ingredient->GetFinalDish())
	{
		// Attach visually to the plate
		Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		Ingredients.Add(Ingredient);

		// Mark this plate as holding a final dish
		FinalDish = Ingredient->GetFinalDish();

		// Optional: Clean CurrentIngredients or leave them as is (they are irrelevant now)
		CurrentIngredients.Reset();

		UpdateIngredientPlacement();
		return true;
	}

	// Only allow processed ingredients (no raw) for non-final items
	if (Ingredient->GetIngredientState() == EIngredientState::Raw)
		return false;

	Ingredient->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	Ingredients.Add(Ingredient);

	// Track simplified type list for non-final recipes (optional legacy path)
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
