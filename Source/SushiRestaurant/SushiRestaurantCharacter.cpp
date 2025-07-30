#include "SushiRestaurantCharacter.h"
#include "ActorComponent/InteractionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Interface/InteractableInterface.h"
#include "Net/UnrealNetwork.h"

// ---------- Constructor ----------
ASushiRestaurantCharacter::ASushiRestaurantCharacter()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	// Capsule & movement setup
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	auto* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 500.f, 0.f);
	Move->JumpZVelocity = 500.f;
	Move->AirControl = 0.35f;
	Move->MaxWalkSpeed = 500.f;
	Move->MinAnalogWalkSpeed = 20.f;
	Move->BrakingDecelerationWalking = 2000.f;
	Move->BrakingDecelerationFalling = 1500.f;
}

// ---------- Input setup ----------
void ASushiRestaurantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction,     ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Move);
		EIC->BindAction(InteractAction, ETriggerEvent::Started,   this, &ASushiRestaurantCharacter::Interact);
		EIC->BindAction(RunAction,      ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Run);
	}
}

// ---------- Input handlers ----------
void ASushiRestaurantCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	DoMove(Axis.X, Axis.Y);
}

void ASushiRestaurantCharacter::Interact(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		DoInteract();
	}
}

void ASushiRestaurantCharacter::Run(const FInputActionValue& /*Value*/)
{
	// Reserved for sprint logic if needed
}

// ---------- Input-to-logic ----------
void ASushiRestaurantCharacter::DoMove(float Right, float Forward)
{
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightVector   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, Forward);
		AddMovementInput(RightVector, Right);
	}
}

void ASushiRestaurantCharacter::DoInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("DoInteract() called"));
	if (CarriedActor)
	{
		DropCarriedActor();
		return;
	}

	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void ASushiRestaurantCharacter::DoRun()
{
}

void ASushiRestaurantCharacter::SetIsProcessingAtStation(bool bNewValue)
{
	bIsProcessingAtStation = bNewValue;
}

// ---------- Carry helpers ----------
void ASushiRestaurantCharacter::AttachActor(AActor* ActorToAttach)
{
	if (!ActorToAttach) return;
	ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_rSocket"));
	CarriedActor = ActorToAttach;
}

void ASushiRestaurantCharacter::DetachCarriedActor()
{
	if (!CarriedActor) return;

	CarriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (UStaticMeshComponent* CarriedMesh = Cast<UStaticMeshComponent>(CarriedActor->GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		CarriedMesh->SetSimulatePhysics(true);
	}

	CarriedActor = nullptr;
}

void ASushiRestaurantCharacter::SetCarriedActor(AActor* NewActor)
{
	CarriedActor = NewActor;
}

void ASushiRestaurantCharacter::DropCarriedActor()
{
	if (CarriedActor)
	{
		IInteractable::Execute_StopInteract(CarriedActor, this);
		CarriedActor = nullptr;
	}
}

// ---------- Replication ----------
void ASushiRestaurantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASushiRestaurantCharacter, CarriedActor);
	DOREPLIFETIME(ASushiRestaurantCharacter, bIsProcessingAtStation);
}
