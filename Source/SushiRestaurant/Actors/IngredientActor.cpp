#include "IngredientActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AIngredientActor::AIngredientActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create and set mesh as root component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Set initial ingredient values
	IngredientTypes = EIngredientTypes::None;
	IngredientStates = EIngredientStates::Raw;
}

// Called when the game starts or when spawned
void AIngredientActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame (disabled for now)
void AIngredientActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Implementation of interaction logic
void AIngredientActor::Interact(APawn* Interactor)
{
	// Print interaction debug log
	FString TypeString = UEnum::GetValueAsString(IngredientTypes);
	FString StateString = UEnum::GetValueAsString(IngredientStates);
	UE_LOG(LogTemp, Log, TEXT("Ingredient Interacted: Type = %s | State = %s"), *TypeString, *StateString);

	// Optional: visualize with debug
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Interacted with: %s (%s)"), *TypeString, *StateString), true, true, FLinearColor::Green, 1.5f);
}
