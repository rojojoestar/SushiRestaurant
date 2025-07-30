// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.h"
#include "Components/BoxComponent.h"
#include "IngredientSpawner.generated.h"

/**
 * IngredientSpawner (base):
 * - Box trigger detects pawns inside the spawn area.
 * - Implements IInteractable: when a pawn inside presses Interact, it spawns a PickupActor with a predefined type.
 * - Supports a MaxActivePickups cap and a RespawnCooldown.
 */
UCLASS()
class AIngredientSpawner : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AIngredientSpawner();

	/** IInteractable: spawn on interact if pawn is inside and cap allows it */
	virtual void Interact_Implementation(APawn* Interactor) override;

protected:
	/** Trigger area to enable interaction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawner|Components")
	UBoxComponent* Trigger;

	/** Where the pickup is spawned (offset above ground to avoid clipping) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawner|Components")
	USceneComponent* SpawnPoint;

	/** Class of pickup to spawn (must be APickupActor or child BP) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawner|Config")
	TSubclassOf<APickupActor> PickupClass;

	/** Ingredient type to assign to the spawned pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawner|Config")
	EIngredientType SpawnIngredientType = EIngredientType::None;

	/** Maximum simultaneous pickups spawned by this spawner (-1 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawner|Config")
	int32 MaxActivePickups = 5;

	/** Cooldown between spawns (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawner|Config")
	float RespawnCooldown = 0.5f;

	/** Tracks pawns currently inside the trigger */
	UPROPERTY()
	TSet<TWeakObjectPtr<APawn>> PawnsInside;

	/** Keeps weak references to spawned pickups to enforce MaxActivePickups */
	UPROPERTY()
	TArray<TWeakObjectPtr<APickupActor>> SpawnedPickups;

	/** When false, spawns are throttled until cooldown elapses */
	bool bCanSpawn = true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Removes null / destroyed entries from SpawnedPickups */
	void CompactSpawnedList();

	/** Returns true if Interactor is currently inside the trigger */
	bool IsInteractorInside(APawn* Interactor) const;

	/** Spawns the pickup actor with configured type and sets initial physics */
	void SpawnPickup(APawn* InstigatorPawn);

	/** Cooldown reset */
	void EnableSpawning();
};

