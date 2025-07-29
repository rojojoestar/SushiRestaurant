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
	if (Plate && Plate->GetIngredients().Num() > 0)
	{
		// Look for OrderManager in the scene
		TArray<AActor*> Managers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderManager::StaticClass(), Managers);

		if (Managers.Num() > 0)
		{
			AOrderManager* OrderManager = Cast<AOrderManager>(Managers[0]);

			if (OrderManager->TryCompleteOrder(TableID, Plate->GetIngredientsTypes()))
			{
				// Success - destroy the plate
				Plate->Destroy();
			}
		}
	}
}
