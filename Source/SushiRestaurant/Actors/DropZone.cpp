#include "DropZone.h"
#include "Components/BoxComponent.h"
#include "IngredientActor.h"
#include "CookwareStation.h"

ADropZone::ADropZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
	SetRootComponent(ZoneTrigger);

	ZoneTrigger->SetBoxExtent(FVector(75.f, 75.f, 50.f));
	ZoneTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ZoneTrigger->SetGenerateOverlapEvents(true);
	ZoneTrigger->SetHiddenInGame(false); // Keep visible for debug
	ZoneTrigger->bDrawOnlyIfSelected = false;
	ZoneTrigger->bHiddenInGame = false;

	// Optional: colorize the volume
	ZoneTrigger->ShapeColor = FColor::Green;
}

void ADropZone::BeginPlay()
{
	Super::BeginPlay();
	ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADropZone::OnZoneBeginOverlap);
}
