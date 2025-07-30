#include "DeliveryZone.h"
#include "Components/BoxComponent.h"
#include "PlateActor.h"
#include "OrderManager.h"
#include "Misc/RecipeAsset.h"

ADeliveryZone::ADeliveryZone()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	SetRootComponent(TriggerZone);
	TriggerZone->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
}

void ADeliveryZone::BeginPlay()
{
	Super::BeginPlay();
	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ADeliveryZone::OnOverlapBegin);
}

void ADeliveryZone::OnOverlapBegin(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
								   UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/,
								   bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	APlateActor* Plate = Cast<APlateActor>(OtherActor);
	if (!Plate || !OrderManager) return;

	URecipeAsset* DeliveredDish = Plate->GetFinalIngredient();
	if (!DeliveredDish) return;

	if (OrderManager->TryCompleteOrder(TableID, DeliveredDish))
	{
		UE_LOG(LogTemp, Log, TEXT("Order delivered for table %d."), TableID);
		Plate->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong dish delivered to table %d."), TableID);
	}
}
