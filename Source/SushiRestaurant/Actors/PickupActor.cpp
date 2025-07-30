#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// ---------- Constructor ----------
APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	MeshComponent->SetSimulatePhysics(true);
}

// ---------- IInteractable ----------
void APickupActor::Interact_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character || Character->GetCarriedActor()) return;

	// Disable physics/collision and attach to character hand
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Character->AttachActor(this);

	// Debug
	const FString TypeStr  = UEnum::GetValueAsString(IngredientType);
	const FString StateStr = UEnum::GetValueAsString(IngredientState);
	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("Picked up: %s (%s)"), *TypeStr, *StateStr),
		true, true, FLinearColor::Green, 1.5f
	);
}

void APickupActor::StopInteract_Implementation(APawn* Interactor)
{
	if (!Interactor) return;

	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	// Detach and drop slightly in front of the player
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Character->SetCarriedActor(nullptr);

	const FVector DropLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f;
	SetActorLocation(DropLocation);

	// Re-enable physics and overlaps so DropZone can pick it up
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->WakeAllRigidBodies();

	UKismetSystemLibrary::PrintString(this, TEXT("Dropped item"), true, true, FLinearColor::Red, 1.0f);
}
