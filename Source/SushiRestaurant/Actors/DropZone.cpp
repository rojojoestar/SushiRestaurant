#include "DropZone.h"
#include "Components/BoxComponent.h"
#include "CookwareStation.h"
#include "Actors/PickupActor.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"

// ---------- Constructor ----------
ADropZone::ADropZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
	SetRootComponent(ZoneTrigger);

	ZoneTrigger->SetBoxExtent(FVector(80.f, 80.f, 70.f));
	ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ZoneTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ZoneTrigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	ZoneTrigger->SetGenerateOverlapEvents(true);
}

// ---------- Lifecycle ----------
void ADropZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADropZone::OnZoneBeginOverlap);
	ZoneTrigger->OnComponentEndOverlap.AddDynamic(this, &ADropZone::OnZoneEndOverlap);
}

// ---------- Overlaps ----------
void ADropZone::OnZoneBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 /*OtherBodyIndex*/,
                                   bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	UE_LOG(LogTemp, Verbose, TEXT("DropZone overlap with %s"), *GetNameSafe(OtherActor));
	if (!HasAuthority() || !LinkedStation) return;

	// Track pawns entering the zone
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		PawnsInside.Add(Pawn);
		return;
	}

	// Only accept pickups if a player is inside
	if (APickupActor* Pickup = Cast<APickupActor>(OtherActor))
	{
		if (PawnsInside.Num() == 0) return;

		if (UStaticMeshComponent* Mesh = Pickup->FindComponentByClass<UStaticMeshComponent>())
		{
			Mesh->SetSimulatePhysics(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Snap to station point with a tiny delay to ensure transforms are valid
		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, Pickup]()
		{
			if (IsValid(LinkedStation) && IsValid(Pickup) && LinkedStation->PlacePoint)
			{
				Pickup->AttachToComponent(LinkedStation->PlacePoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
				LinkedStation->ReceiveDroppedPickup(Pickup);
			}
		}, 0.05f, false);
	}
}

void ADropZone::OnZoneEndOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
                                 UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		PawnsInside.Remove(Pawn);
	}
}
