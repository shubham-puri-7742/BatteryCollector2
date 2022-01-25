// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector2Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Pickup.h"
#include "BatteryPickup.h"

//////////////////////////////////////////////////////////////////////////
// ABatteryCollector2Character

ABatteryCollector2Character::ABatteryCollector2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create the first person camera
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(GetMesh(), FName("head")); // Attach it to the player mesh's head
	FPCamera->bUsePawnControlRotation = true; // For accurate first person rotation with the camera
	// Position at the head, facing forward
	FPCamera->SetRelativeRotation(FRotator(0, 90, -90));
	FPCamera->SetRelativeLocation(FVector(-10, 20, 0));
	this->bUseControllerRotationYaw = true; // For turning the mesh with the camera in first person.
	FPCam = true; // Start the game in first person

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Initialise CollectSphere
	CollectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectSphere"));
	CollectSphere->AttachTo(RootComponent);
	CollectSphere->SetSphereRadius(200.0f);

	//Initialise the player at full power
	initialPower = 2000.f;
	currentPower = initialPower;

	// Initialise speeds. Can be modified to be different for different levels/difficulties
	speedFactor = 0.75f;
	baseSpeed = 10.0f;


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ABatteryCollector2Character::deathCam() // Because the first-person 'death' looks awkward :3
{
	if (FPCam) { // If FP cam
		FollowCamera->SetActive(true); // Switch to the follow cam
		FPCamera->SetActive(false); // Deactivate FPCam
		this->bUseControllerRotationYaw = false; // Don't use controller yaw
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollector2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Binding to collect pickups
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryCollector2Character::CollectPickups);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryCollector2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryCollector2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABatteryCollector2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABatteryCollector2Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABatteryCollector2Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABatteryCollector2Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABatteryCollector2Character::OnResetVR);

	// Switch Camera
	PlayerInputComponent->BindAction("CameraToggle", IE_Pressed, this, &ABatteryCollector2Character::CameraToggle);
}


void ABatteryCollector2Character::CollectPickups()
{
	// Get all overlapping actors in an array
	TArray<AActor*> Collected;
	CollectSphere->GetOverlappingActors(Collected);

	// Reinitialise every time something is collected
	float collectedPower = 0.0f;

	// For each overlapped actor
	for (int32 iCollected = 0; iCollected < Collected.Num(); ++iCollected) {
		// Cast to a pickup (loosely, check if overlapping a pickup)
		APickup* const TestPickup = Cast<APickup>(Collected[iCollected]);
		// If the cast succeeds, the pickup is not pending destruction, and is active
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive()) {
			// Collect it (Call the Collect function on the object), add the power, & deactivate it
			TestPickup->Collect();
			ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(TestPickup);
			if (TestBattery) { // Add the battery's power to the collected power
				collectedPower += TestBattery->GetPower(); // Add power
			}

			TestPickup->SetActive(false); // Deactivate the pickup (no other actor can pick it up)
		}
	}

	if (collectedPower != 0) {
		UpdatePower(collectedPower);
	}
}

float ABatteryCollector2Character::GetInitialPower()
{
	return initialPower;
}

float ABatteryCollector2Character::GetCurrentPower()
{
	return currentPower;
}

void ABatteryCollector2Character::UpdatePower(float PowerChange)
{
	// Update the power
	currentPower += PowerChange;
	// currentPower = FMath::Clamp(currentPower, 0.0f, initialPower); // Uncommenting needs tweaking the win conditions

	// Define speed as a function of the power
	GetCharacterMovement()->MaxWalkSpeed = baseSpeed + speedFactor * currentPower;

	PowerChangeEffect(); // Do whatever's defined in this function's BP implementation. HERE: Material colour change
}

void ABatteryCollector2Character::OnResetVR()
{
	// If BatteryCollector2 is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in BatteryCollector2.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABatteryCollector2Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ABatteryCollector2Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ABatteryCollector2Character::CameraToggle() // MODULE 2: CAMERAS - MODIFIED FROM FOLLOW/TOP-DOWN CAMS to FIRSTPERSON-THIRDPERSON CAMS
{
	if (FPCam) { // If FP cam (default)
		FollowCamera->SetActive(true); // Switch to the follow cam
		FPCamera->SetActive(false); // Deactivate FPCam
		this->bUseControllerRotationYaw = false; // Don't use controller yaw
	}
	else { // If follow cam
		FPCamera->SetActive(true); // Switch to FPCam
		FollowCamera->SetActive(false); // Disable FollowCam
		this->bUseControllerRotationYaw = true; // Use controller yaw
	}
	FPCam = !FPCam; // Flipflop basically
}

void ABatteryCollector2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollector2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollector2Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollector2Character::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
