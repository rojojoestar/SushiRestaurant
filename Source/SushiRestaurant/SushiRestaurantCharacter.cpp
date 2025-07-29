// Copyright Epic Games, Inc. All Rights Reserved.

#include "SushiRestaurantCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "ActorComponent/InteractionComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASushiRestaurantCharacter::ASushiRestaurantCharacter()
{
	//
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASushiRestaurantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		// Movement
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Move);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASushiRestaurantCharacter::Interact);

		// Run (si quieres usarlo más adelante)
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ASushiRestaurantCharacter::Run);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASushiRestaurantCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASushiRestaurantCharacter::Interact(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();

	if(bIsPressed)
	{
		DoInteract();
		
	}
}

void ASushiRestaurantCharacter::Run(const FInputActionValue& Value)
{
}


void ASushiRestaurantCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ASushiRestaurantCharacter::DoInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("DoInteract() called"));
	if (CarriedActor)
	{
		// Drop the carried actor if already holding one
		DetachCarriedActor();
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

void ASushiRestaurantCharacter::AttachActor(AActor* ActorToAttach)
{
	if (!ActorToAttach)
		return;

	// Attach to a socket on the mesh, or just to the root
	ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_rSocket")); // Replace with your socket name
	CarriedActor = ActorToAttach;
}

void ASushiRestaurantCharacter::DetachCarriedActor()
{
	if (CarriedActor)
	{
		// Detach actor from character
		CarriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// Reactivate physics if it has a mesh
		if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(CarriedActor->GetComponentByClass(UStaticMeshComponent::StaticClass())))
		{
			MeshComponent->SetSimulatePhysics(true);
		}

		CarriedActor = nullptr;
	}
}


AActor* ASushiRestaurantCharacter::GetCarriedActor() const
{
	return CarriedActor;
}

void ASushiRestaurantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASushiRestaurantCharacter, CarriedActor);
}

