#include "InteractionComponent.h"
#include "Interface/InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TryInteract()
{
	if (IInteractable* Interactable = GetInteractableInFront())
	{
		Interactable->Interact(Cast<APawn>(GetOwner()));
	}
}

IInteractable* UInteractionComponent::GetInteractableInFront() const
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + GetOwner()->GetActorForwardVector() * 200.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Cast<IInteractable>(Hit.GetActor());
	}

	return nullptr;
}
