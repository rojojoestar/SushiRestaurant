#include "CookwareStation.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SushiRestaurantCharacter.h"

ACookwareStation::ACookwareStation()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    PlacePoint = CreateDefaultSubobject<USceneComponent>(TEXT("PlacePoint"));
    PlacePoint->SetupAttachment(MeshComponent);
    PlacePoint->SetRelativeLocation(PlacePointOffset);

    ProgressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidget"));
    ProgressWidget->SetupAttachment(RootComponent);
    ProgressWidget->SetWidgetSpace(EWidgetSpace::Screen);
    ProgressWidget->SetDrawAtDesiredSize(true);
    ProgressWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    ProgressWidget->SetVisibility(false);
}

void ACookwareStation::Interact_Implementation(APawn* Interactor)
{
    if (!HasAuthority() || bIsProcessing) return;

    if (ASushiRestaurantCharacter* Player = Cast<ASushiRestaurantCharacter>(Interactor))
    {
        if (APickupActor* Pickup = Cast<APickupActor>(Player->GetCarriedActor()))
        {
            // MixingTable accepts multiple items into a buffer
            if (CookwareType == ECookwareType::MixingTable)
            {
                if (!CanAcceptInBuffer(Pickup)) return;

                // Attach and buffer
                Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::KeepWorldTransform);
                BufferPickups.Add(Pickup);
                Player->DetachCarriedActor();

                ArrangeBufferOnTable();

                // Start processing only when we have a valid mix
                if (HasValidSushiMix())
                {
                    LastInteractorPawn = Player;
                    StartProcessing();
                }
                return;
            }

            // Single-item stations
            if (!CanProcessPickup(Pickup)) return;

            Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
            CurrentPickup = Pickup;
            LastInteractorPawn = Player;
            StartProcessing();
            return;
        }
    }
}

void ACookwareStation::ReceiveDroppedPickup(APickupActor* Pickup, APawn* InstigatorPawn)
{
    if (!HasAuthority() || bIsProcessing || !Pickup) return;

    if (CookwareType == ECookwareType::MixingTable)
    {
        if (!CanAcceptInBuffer(Pickup)) return;

        Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::KeepWorldTransform);
        BufferPickups.Add(Pickup);
        ArrangeBufferOnTable();

        if (HasValidSushiMix())
        {
            LastInteractorPawn = InstigatorPawn;
            StartProcessing();
        }
        return;
    }

    if (!CanProcessPickup(Pickup) || CurrentPickup) return;

    Pickup->AttachToComponent(PlacePoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
    CurrentPickup = Pickup;
    LastInteractorPawn = InstigatorPawn;
    StartProcessing();
}

bool ACookwareStation::CanProcessPickup(APickupActor* Pickup) const
{
    if (!Pickup) return false;

    switch (CookwareType)
    {
    case ECookwareType::CuttingBoard:
        // Fish Raw -> Sliced
        return Pickup->GetIngredientType() == EIngredientType::Fish &&
               Pickup->GetIngredientState() == EIngredientState::Raw;

    case ECookwareType::Stove:
        // Rice Raw -> Cooked
        return Pickup->GetIngredientType() == EIngredientType::Rice &&
               Pickup->GetIngredientState() == EIngredientState::Raw;

    case ECookwareType::MixingTable:
        // Handled by CanAcceptInBuffer() instead
        return false;

    default:
        return false;
    }
}

bool ACookwareStation::CanAcceptInBuffer(APickupActor* Pickup) const
{
    if (!Pickup) return false;
    if (BufferPickups.Num() >= MaxBufferSize) return false;

    // Only allow the three types we need for Sushi
    const EIngredientType Type = Pickup->GetIngredientType();
    const EIngredientState State = Pickup->GetIngredientState();

    // We expect: Fish(Sliced), Rice(Cooked), Seaweed(any)
    const bool bIsFishOk     = (Type == EIngredientType::Fish)    && (State == EIngredientState::Sliced);
    const bool bIsRiceOk     = (Type == EIngredientType::Rice)    && (State == EIngredientState::Cooked);
    const bool bIsSeaweedOk  = (Type == EIngredientType::Seaweed) && (State != EIngredientState::Raw || true); // Seaweed can be raw

    // Avoid duplicates of the same role
    auto HasInBuffer = [this](EIngredientType WantedType, EIngredientState WantedState) -> bool
    {
        for (const APickupActor* It : BufferPickups)
        {
            if (!It) continue;
            if (It->GetIngredientType() == WantedType)
            {
                // For fish/rice we also match state
                if (WantedType == EIngredientType::Fish && It->GetIngredientState() != EIngredientState::Sliced)
                    continue;
                if (WantedType == EIngredientType::Rice && It->GetIngredientState() != EIngredientState::Cooked)
                    continue;
                return true;
            }
        }
        return false;
    };

    if (bIsFishOk && !HasInBuffer(EIngredientType::Fish, EIngredientState::Sliced)) return true;
    if (bIsRiceOk && !HasInBuffer(EIngredientType::Rice, EIngredientState::Cooked)) return true;
    if (bIsSeaweedOk && !HasInBuffer(EIngredientType::Seaweed, EIngredientState::Ready)) return true;

    return false;
}

bool ACookwareStation::HasValidSushiMix() const
{
    bool bHasFishSliced = false;
    bool bHasRiceCooked = false;
    bool bHasSeaweed    = false;

    for (const APickupActor* It : BufferPickups)
    {
        if (!It) continue;
        switch (It->GetIngredientType())
        {
        case EIngredientType::Fish:    bHasFishSliced = (It->GetIngredientState() == EIngredientState::Sliced); break;
        case EIngredientType::Rice:    bHasRiceCooked = (It->GetIngredientState() == EIngredientState::Cooked); break;
        case EIngredientType::Seaweed: bHasSeaweed    = true; break;
        default: break;
        }
    }
    return bHasFishSliced && bHasRiceCooked && bHasSeaweed;
}

void ACookwareStation::ArrangeBufferOnTable() const
{
    // Simple circle layout around PlacePoint for visual clarity
    if (!PlacePoint) return;

    const FVector Center = PlacePoint->GetComponentLocation() + FVector(0,0,5.f);
    const float   Radius = 12.f;

    const int32 Count = BufferPickups.Num();
    for (int32 i = 0; i < Count; ++i)
    {
        APickupActor* P = BufferPickups[i];
        if (!P) continue;

        const float AngleDeg = (360.f / FMath::Max(1, Count)) * i;
        const float AngleRad = FMath::DegreesToRadians(AngleDeg);
        const FVector Offset(FMath::Cos(AngleRad) * Radius, FMath::Sin(AngleRad) * Radius, 0.f);

        P->SetActorLocation(Center + Offset);
        P->SetActorRotation(FRotator::ZeroRotator);
    }
}

void ACookwareStation::StartProcessing()
{
    bIsProcessing = true;
    ProgressWidget->SetVisibility(true);

    if (ASushiRestaurantCharacter* Char = Cast<ASushiRestaurantCharacter>(LastInteractorPawn))
    {
        Char->SetIsProcessingAtStation(true);
    }

    GetWorldTimerManager().SetTimer(ProcessingTimerHandle, this, &ACookwareStation::CompleteProcessing, ProcessingTime, false);
}

void ACookwareStation::CompleteProcessing()
{
    ProgressWidget->SetVisibility(false);

    // Single-item stations: update state and release
    if (CookwareType == ECookwareType::CuttingBoard && CurrentPickup)
    {
        CurrentPickup->SetIngredientState(EIngredientState::Sliced);

        if (UStaticMeshComponent* Mesh = CurrentPickup->FindComponentByClass<UStaticMeshComponent>())
        {
            Mesh->SetSimulatePhysics(true);
            Mesh->SetEnableGravity(true);
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }

        CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentPickup = nullptr;
    }
    else if (CookwareType == ECookwareType::Stove && CurrentPickup)
    {
        CurrentPickup->SetIngredientState(EIngredientState::Cooked);

        if (UStaticMeshComponent* Mesh = CurrentPickup->FindComponentByClass<UStaticMeshComponent>())
        {
            Mesh->SetSimulatePhysics(true);
            Mesh->SetEnableGravity(true);
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }

        CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentPickup = nullptr;
    }
    else if (CookwareType == ECookwareType::MixingTable)
    {
        // Consume buffer and produce Sushi pickup
        if (HasValidSushiMix())
        {
            ProduceSushiResult();
        }

        // Release any leftover items if something odd happened
        for (APickupActor* P : BufferPickups)
        {
            if (!P) continue;
            if (UStaticMeshComponent* Mesh = P->FindComponentByClass<UStaticMeshComponent>())
            {
                Mesh->SetSimulatePhysics(true);
                Mesh->SetEnableGravity(true);
                Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }
            P->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        }
        BufferPickups.Reset();
    }

    // Turn off "processing" flag in the instigator for animations
    if (ASushiRestaurantCharacter* Char = Cast<ASushiRestaurantCharacter>(LastInteractorPawn))
    {
        Char->SetIsProcessingAtStation(false);
    }

    bIsProcessing = false;
    LastInteractorPawn = nullptr;
}

void ACookwareStation::ProduceSushiResult()
{
    if (!SushiPickupClass) { UE_LOG(LogTemp, Warning, TEXT("SushiPickupClass is not set")); return; }

    // Destroy or hide buffer items (choose one; here we destroy to simplify)
    for (APickupActor* P : BufferPickups)
    {
        if (P) P->Destroy();
    }
    BufferPickups.Reset();

    // Spawn a new pickup representing the Sushi final dish
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector SpawnLoc = PlacePoint ? PlacePoint->GetComponentLocation() : GetActorLocation();
    const FRotator SpawnRot = FRotator::ZeroRotator;

    APickupActor* Sushi = GetWorld()->SpawnActor<APickupActor>(SushiPickupClass, SpawnLoc, SpawnRot, Params);
    if (Sushi)
    {
        // Mark this pickup as a final dish (Sushi)
        Sushi->SetFinalDish(SushiRecipeAsset);

        // Optional: Set type/state to Ready for clarity
        // (not strictly needed if FinalDish drives gameplay)
        // Sushi->SetIngredientState(EIngredientState::Ready);

        // Release it with physics so the player can pick it up
        if (UStaticMeshComponent* Mesh = Sushi->FindComponentByClass<UStaticMeshComponent>())
        {
            Mesh->SetSimulatePhysics(true);
            Mesh->SetEnableGravity(true);
            Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}

void ACookwareStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACookwareStation, CurrentPickup);
}