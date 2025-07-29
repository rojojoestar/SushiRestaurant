#include "DropZone.h"
#include "Components/BoxComponent.h"
#include "CookwareStation.h"
#include "PickupActor.h"

ADropZone::ADropZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
	SetRootComponent(ZoneTrigger);

	ZoneTrigger->SetBoxExtent(FVector(75.f, 75.f, 50.f));
	ZoneTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ZoneTrigger->SetGenerateOverlapEvents(true);
	ZoneTrigger->SetHiddenInGame(false);
	ZoneTrigger->bDrawOnlyIfSelected = false;
	ZoneTrigger->bHiddenInGame = false;

	ZoneTrigger->ShapeColor = FColor::Green;
}

void ADropZone::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap event
	ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADropZone::OnZoneBeginOverlap);
}

void ADropZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!LinkedStation)
		return;

	// Check if the overlapping actor is a pickup
	APickupActor* Pickup = Cast<APickupActor>(OtherActor);
	if (!Pickup)
		return;

	// Attach the pickup to the station
	Pickup->AttachToComponent(LinkedStation->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

	// Disable physics while it's in the station
	if (UStaticMeshComponent* Mesh = Pickup->FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetSimulatePhysics(false);
	}

	// Inform the station
	LinkedStation->ReceiveDroppedPickup(Pickup);
}
