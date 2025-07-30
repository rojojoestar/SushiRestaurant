#include "DeliveryZone.h"
#include "Components/BoxComponent.h"
#include "PlateActor.h"
#include "OrderManager.h"
#include "Kismet/GameplayStatics.h"

ADeliveryZone::ADeliveryZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create and configure collision zone
	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	RootComponent = TriggerZone;
	TriggerZone->SetBoxExtent(FVector(100, 100, 50));
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
}

void ADeliveryZone::BeginPlay()
{
	Super::BeginPlay();

	// Register overlap event
	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryZone::OnOverlapBegin);
}

// Handle when a plate overlaps the delivery zone
void ADeliveryZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlateActor* Plate = Cast<APlateActor>(OtherActor);
	if (!Plate || !OrderManager) return; // Ahora usa OrderManager como propiedad

	URecipeAsset* DeliveredDish = Plate->GetFinalIngredient();
	if (!DeliveredDish) return;

	if (OrderManager->TryCompleteOrder(TableID, DeliveredDish))
	{
		UE_LOG(LogTemp, Log, TEXT("Order delivered correctly for Table %d!"), TableID);
		Plate->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong dish delivered to Table %d"), TableID);
	}
}
