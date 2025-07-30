// Fill out your copyright notice in the Description page of Project Settings.


#include "IngredientSpawner.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

AIngredientSpawner::AIngredientSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->InitBoxExtent(FVector(80.f, 80.f, 80.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Trigger);
	SpawnPoint->SetRelativeLocation(FVector(0.f, 0.f, 40.f)); // small height offset
}

void AIngredientSpawner::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AIngredientSpawner::OnBoxBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AIngredientSpawner::OnBoxEndOverlap);
}

void AIngredientSpawner::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	if (APawn* P = Cast<APawn>(OtherActor))
	{
		PawnsInside.Add(P);
	}
}

void AIngredientSpawner::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APawn* P = Cast<APawn>(OtherActor))
	{
		PawnsInside.Remove(P);
	}
}

bool AIngredientSpawner::IsInteractorInside(APawn* Interactor) const
{
	return Interactor && PawnsInside.Contains(Interactor);
}

void AIngredientSpawner::CompactSpawnedList()
{
	SpawnedPickups.RemoveAll([](const TWeakObjectPtr<APickupActor>& Ptr) { return !Ptr.IsValid(); });
}

void AIngredientSpawner::EnableSpawning()
{
	bCanSpawn = true;
}

void AIngredientSpawner::SpawnPickup(APawn* InstigatorPawn)
{
	if (!PickupClass) return;

	CompactSpawnedList();

	// Enforce MaxActivePickups, unless unlimited (-1)
	if (MaxActivePickups >= 0 && SpawnedPickups.Num() >= MaxActivePickups)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Instigator = InstigatorPawn;

	const FVector Loc = SpawnPoint ? SpawnPoint->GetComponentLocation() : GetActorLocation();
	const FRotator Rot = FRotator::ZeroRotator;

	APickupActor* NewPickup = GetWorld()->SpawnActor<APickupActor>(PickupClass, Loc, Rot, Params);
	if (NewPickup)
	{
		NewPickup->SetActorEnableCollision(true);
		NewPickup->SetIngredientState(EIngredientState::Raw);
		NewPickup->SetFinalDish(nullptr); // ensure not a final dish
		// Assign ingredient type
		// (Optional) If you want IngredientType to be editable from BP subclass, you can override here.
		// Else just set it directly if you have a setter (or expose the property).
		// If IngredientType is protected, add a setter in APickupActor as needed.
		// Assuming a setter exists:
		// NewPickup->SetIngredientType(SpawnIngredientType);

		// Fallback: if no setter, consider exposing IngredientType as EditAnywhere in APickupActor or make this spawner a friend.
		// For now, we assume you added SetIngredientType:
		NewPickup->SetIngredientType(SpawnIngredientType);

		// Let physics handle initial drop
		if (UStaticMeshComponent* Mesh = NewPickup->FindComponentByClass<UStaticMeshComponent>())
		{
			Mesh->SetSimulatePhysics(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetEnableGravity(true);
		}

		SpawnedPickups.Add(NewPickup);
	}
}

void AIngredientSpawner::Interact_Implementation(APawn* Interactor)
{
	// Only allow spawning if the interactor is inside the trigger and cooldown allows it
	if (!IsInteractorInside(Interactor) || !bCanSpawn) return;

	SpawnPickup(Interactor);

	// Start cooldown
	bCanSpawn = false;
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		// Use a timed timer instead of NextTick if RespawnCooldown > 0
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AIngredientSpawner::EnableSpawning, RespawnCooldown, false);
	});
}


