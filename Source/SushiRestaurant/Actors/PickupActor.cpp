#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"

// Sets default values
APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup static mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Enable physics by default (optional)
	MeshComponent->SetSimulatePhysics(true);
}

// When the player interacts
void APickupActor::Interact(APawn* Interactor)
{
	if (!Interactor)
		return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character)
		return;

	// If already carrying something, do nothing
	if (Character->GetCarriedActor())
		return;

	// Stop physics simulation before attaching
	MeshComponent->SetSimulatePhysics(false);

	// Attach this actor to the character
	Character->AttachActor(this);
}
