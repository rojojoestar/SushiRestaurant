#include "InteractionComponent.h"
#include "Interface/InteractableInterface.h"
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
	const FVector Start = GetOwner()->GetActorLocation() + FVector(0, 0, 50.f);
	const FVector End   = Start + GetOwner()->GetActorForwardVector() * 300.f;
	const float Radius  = 150.f;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(Radius), Params);

	AActor* ClosestInteractable = nullptr;
	float MinDistSq = TNumericLimits<float>::Max();

	if (bHit)
	{
		for (const FHitResult& H : Hits)
		{
			AActor* HitActor = H.GetActor();
			if (!HitActor) continue;

			// Accept anything that implements the Interactable interface (Pickup, Plate, Spawner, Stations, etc.)
			if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				const float DistSq = FVector::DistSquared(Start, HitActor->GetActorLocation());
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					ClosestInteractable = HitActor;
				}
			}
		}
	}

	return ClosestInteractable;
}

