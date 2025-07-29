#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Enable physics for interaction
	MeshComponent->SetSimulatePhysics(true);

	// Default ingredient values
	IngredientType = EIngredientType::None;
	IngredientState = EIngredientState::Raw;
}

// Interaction with character
void APickupActor::Interact(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	// If already carrying something, do nothing
	if (Character->GetCarriedActor()) return;

	// Disable physics before attachment
	MeshComponent->SetSimulatePhysics(false);

	// Attach to character
	Character->AttachActor(this);

	// Debug print for clarity
	FString TypeStr = UEnum::GetValueAsString(IngredientType);
	FString StateStr = UEnum::GetValueAsString(IngredientState);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Picked up: %s (%s)"), *TypeStr, *StateStr), true, true, FLinearColor::Green, 1.5f);
}

// Change ingredient state externally
void APickupActor::SetIngredientState(EIngredientState NewState)
{
	IngredientState = NewState;
}
