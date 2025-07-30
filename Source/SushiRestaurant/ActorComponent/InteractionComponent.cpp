#include "InteractionComponent.h"
#include "Interface/InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Actors/PickupActor.h"
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
	if (AActor* Interactable = GetInteractableInFront())
	{
		// Verificar explícitamente la interfaz
		if (Interactable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(Interactable, Cast<APawn>(GetOwner()));
		}
		// Opcional: Mensaje de depuración si no implementa
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor %s no implementa IInteractable"), *Interactable->GetName());
		}
	}
}


AActor* UInteractionComponent::GetInteractableInFront() const
{
	FVector Start = GetOwner()->GetActorLocation() + FVector(0,0,50.f); // 50cm arriba del suelo
	FVector End = Start + GetOwner()->GetActorForwardVector() * 200.f;  // 200cm hacia adelante
	float Radius = 150.f; // más pequeño si quieres precisión

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

	TArray<FHitResult> Hits;
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (bHit)
	{
		AActor* Closest = nullptr;
		float MinDist = TNumericLimits<float>::Max();

		for (const FHitResult& HitResult : Hits)
		{
			if (APickupActor* Pickup = Cast<APickupActor>(HitResult.GetActor()))
			{
				float Dist = FVector::Dist(Start, Pickup->GetActorLocation());
				if (Dist < MinDist)
				{
					MinDist = Dist;
					Closest = Pickup;
				}
			}
		}

		if (Closest)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup más cercano: %s"), *Closest->GetName());
			return Closest;
		}
	}
	return nullptr;
}


