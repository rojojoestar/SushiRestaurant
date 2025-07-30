#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "PickupActor.h"
#include "Structs/RecipeData.h"
#include "CookwareStation.generated.h"

class UWidgetComponent;
class APickupActor;
class URecipeAsset;

/**
 * CookwareStation: Represents a cooking station (e.g. cutting board, stove)
 * Can process ingredients when dropped in its DropZone
 */
UCLASS()
class ACookwareStation : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    ACookwareStation();

    /** IInteractable */
    virtual void Interact_Implementation(APawn* Interactor) override;

    /** Called by DropZone when a pickup is dropped inside */
    void ReceiveDroppedPickup(APickupActor* Pickup, APawn* InstigatorPawn);

    /** Place Point for pickup anchoring */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooking")
    USceneComponent* PlacePoint;

protected:
    /** Visual mesh */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    /** Optional UI widget (progress) */
    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* ProgressWidget;

    /** Station type */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
    ECookwareType CookwareType = ECookwareType::CuttingBoard;

    /** Currently processed pickup (single-item stations) */
    UPROPERTY(Replicated, VisibleAnywhere, Category = "Cookware")
    APickupActor* CurrentPickup = nullptr;

    /** Is currently processing */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cookware")
    bool bIsProcessing = false;

    /** Time to process */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cookware")
    float ProcessingTime = 2.0f;

    /** Offset for PlacePoint */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooking")
    FVector PlacePointOffset = FVector(0.f, 0.f, 20.f);

    /** Timer for processing */
    FTimerHandle ProcessingTimerHandle;

    /** Pawn who initiated processing (for animation) */
    UPROPERTY()
    APawn* LastInteractorPawn = nullptr;

    // -------- Mixing Table support --------

    /** Only used when CookwareType == MixingTable: cached pickups inside the table */
    UPROPERTY(VisibleAnywhere, Category="Mixing")
    TArray<APickupActor*> BufferPickups;

    /** Max items for mixing (Fish + Rice + Seaweed = 3) */
    UPROPERTY(EditDefaultsOnly, Category="Mixing")
    int32 MaxBufferSize = 3;

    /** Class to spawn for the final sushi pickup (visual + interactable) */
    UPROPERTY(EditDefaultsOnly, Category="Mixing")
    TSubclassOf<APickupActor> SushiPickupClass;

    /** Recipe asset that identifies the final dish "Sushi" */
    UPROPERTY(EditDefaultsOnly, Category="Mixing")
    TSubclassOf<URecipeAsset> SushiRecipeAsset;

protected:
    /** Validate if this pickup can be processed by this station */
    bool CanProcessPickup(APickupActor* Pickup) const;

    /** Start processing timer */
    void StartProcessing();

    /** Called when timer completes */
    void CompleteProcessing();

    /** Helpers for mixing logic */
    bool CanAcceptInBuffer(APickupActor* Pickup) const;
    bool HasValidSushiMix() const;
    void ArrangeBufferOnTable() const;
    void ProduceSushiResult(); // creates a Sushi pickup with FinalDish set

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
