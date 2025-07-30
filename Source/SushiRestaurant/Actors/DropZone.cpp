// DropZone.cpp
#include "DropZone.h"
#include "Components/BoxComponent.h"
#include "CookwareStation.h"
#include "PickupActor.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"

ADropZone::ADropZone()
{
    PrimaryActorTick.bCanEverTick = false;

    ZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTrigger"));
    SetRootComponent(ZoneTrigger);

    ZoneTrigger->SetBoxExtent(FVector(80.f, 80.f, 70.f));
    ZoneTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ZoneTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    // Overlap pawns and physics bodies (pickups)
    ZoneTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    ZoneTrigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    ZoneTrigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

    ZoneTrigger->SetGenerateOverlapEvents(true);
}

void ADropZone::BeginPlay()
{
    Super::BeginPlay();

    ZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADropZone::OnZoneBeginOverlap);
    ZoneTrigger->OnComponentEndOverlap.AddDynamic(this, &ADropZone::OnZoneEndOverlap);
}

void ADropZone::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("DropZone: Overlap con %s"), *OtherActor->GetName());

    if (!LinkedStation)
    {
        UE_LOG(LogTemp, Error, TEXT("DropZone: LinkedStation es nullptr!"));
        return;
    }

    if (!LinkedStation->PlacePoint)
    {
        UE_LOG(LogTemp, Error, TEXT("DropZone: PlacePoint es nullptr!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("DropZone: PlacePoint encontrado en %s"), *LinkedStation->PlacePoint->GetOwner()->GetName());

    
    if (!HasAuthority() || !LinkedStation) return;

    // Track pawns that enter the zone
    if (APawn* Pawn = Cast<APawn>(OtherActor))
    {
        PawnsInside.Add(Pawn);
        return;
    }

    // Only accept a pickup if at least one pawn is inside the zone
    if (APickupActor* Pickup = Cast<APickupActor>(OtherActor))
    {
        if (PawnsInside.Num() == 0) return;

        if (UStaticMeshComponent* Mesh = Pickup->FindComponentByClass<UStaticMeshComponent>())
        {
            Mesh->SetSimulatePhysics(false);
            Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Mesh->SetWorldLocation(LinkedStation->PlacePoint->GetComponentLocation());
        }

        // AQUI usas el PlacePoint de la estación
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Pickup]() {

            Pickup->SetActorLocation(LinkedStation->PlacePoint->GetComponentLocation());
        }, 1.0f, false);

        
        

        LinkedStation->ReceiveDroppedPickup(Pickup);
    }
}

void ADropZone::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (APawn* Pawn = Cast<APawn>(OtherActor))
    {
        PawnsInside.Remove(Pawn);
    }
}
