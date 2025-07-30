#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "SushiRestaurantCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// ---------- Constructor ----------
APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	ApplyStateVisuals();
}

// ---------- IInteractable ----------
void APickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickupActor, IngredientState);
}

void APickupActor::OnRep_IngredientState()
{
	ApplyStateVisuals();
}

void APickupActor::ApplyStateVisuals()
{
	if (UStaticMesh** FoundMesh = StateMeshes.Find(IngredientState))
	{
		if (UStaticMesh* NewMesh = *FoundMesh)
		{
			MeshComponent->SetStaticMesh(NewMesh);
		}
	}
}

void APickupActor::SetIngredientState(EIngredientState NewState)
{
	if (IngredientState == NewState) return;

	// Server-authoritative: update and replicate
	IngredientState = NewState;
	ApplyStateVisuals();
}

void APickupActor::Interact_Implementation(APawn* Interactor)
{
	if (!Interactor) return;
	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	if (Character->GetCarriedActor()) return;

	// Disable physics + collision for carrying
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Character->AttachActor(this);

	// Debug (optional)
	const FString TypeStr = UEnum::GetValueAsString(IngredientType);
	const FString StateStr = UEnum::GetValueAsString(IngredientState);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Picked up: %s (%s)"), *TypeStr, *StateStr), true, true, FLinearColor::Green, 1.5f);
}

void APickupActor::StopInteract_Implementation(APawn* Interactor)
{
	if (!Interactor) return;
	ASushiRestaurantCharacter* Character = Cast<ASushiRestaurantCharacter>(Interactor);
	if (!Character) return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Character->SetCarriedActor(nullptr);

	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	const FVector DropLoc = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f;
	SetActorLocation(DropLoc);
}