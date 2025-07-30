#include "InteractionComponent.h"
#include "Interface/InteractableInterface.h"
#include "Actors/PickupActor.h"
#include "GameFramework/Actor.h"

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
	if (AActor* Interactable = GetInteractableInFront())
	{
		if (Interactable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(Interactable, Cast<APawn>(GetOwner()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor %s does not implement IInteractable."), *Interactable->GetName());
		}
	}
}

AActor* UInteractionComponent::GetInteractableInFront() const
{
	// Start slightly above ground to avoid floor hits
	const FVector Start = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector End   = Start + GetOwner()->GetActorForwardVector() * 200.f;
	const float Radius  = 150.f;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	const bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (!bHit) return nullptr;

	AActor* Closest = nullptr;
	float MinDist = TNumericLimits<float>::Max();

	for (const FHitResult& HitResult : Hits)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor) continue;

		// Aceptar cualquier actor que implemente la interfaz IInteractable
		if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			float Dist = FVector::Dist(Start, HitActor->GetActorLocation());
			if (Dist < MinDist)
			{
				MinDist = Dist;
				Closest = HitActor;
			}
		}
	}

	if (Closest)
	{
		UE_LOG(LogTemp, Warning, TEXT("Closest pickup: %s"), *Closest->GetName());
		return Closest;
	}

	return Closest;
}
