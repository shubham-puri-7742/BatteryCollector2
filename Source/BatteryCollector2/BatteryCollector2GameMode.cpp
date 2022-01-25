// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector2GameMode.h"
#include "BatteryCollector2Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpawnVolume.h"

ABatteryCollector2GameMode::ABatteryCollector2GameMode()
{
	PrimaryActorTick.bCanEverTick = true; // Enable tick

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// base decay rate. Can be modified to be different for each level/difficulty
	decayRate = 0.01f;
}

void ABatteryCollector2GameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get all actors
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), Actors);

	// Cast each actor found to a spawn volume and add to the array of spawn volumes on success
	for (auto actor : Actors) {
		ASpawnVolume* SpawnVol = Cast<ASpawnVolume>(actor); // Unique to avoid duplicates
		if (SpawnVol) { // if valid cast
			SpawnVolumes.AddUnique(SpawnVol);
		}
	}

	// Set to playing
	SetGameState(EBatteryPlayState::EPlaying);

	// Power to win as a function of the initial power
	character = Cast<ABatteryCollector2Character>(UGameplayStatics::GetPlayerPawn(this, 0)); // Get the player character & check if using the correct class
	if (character) { // if the character exists
		powerToWin = (character->GetInitialPower()) * 1.50; // 1.5 times the initial power to win. Could be tweaked
	}

	if (HUDWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass); // Create the HUD
		// Add the current widget to the viewport
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}
}

EBatteryPlayState ABatteryCollector2GameMode::GetGameState() const
{
	return gameState;
}

void ABatteryCollector2GameMode::SetGameState(EBatteryPlayState newState)
{
	// Set a new state and call the function to handle it
	gameState = newState;
	HandleNewState(newState);
}

void ABatteryCollector2GameMode::HandleNewState(EBatteryPlayState newState)
{
	switch (newState) { // switch on the new state
	case EBatteryPlayState::EPlaying: { // Playing state
		// Activate all spawn volumes.
		for (ASpawnVolume* SpawnVol : SpawnVolumes) {
			SpawnVol->SetSpawningActive(true);
		}
		break;
	}
	case EBatteryPlayState::EGameOver: { // Lose state
		// Deactivate all spawn volumes
		for (ASpawnVolume* SpawnVol : SpawnVolumes) {
			SpawnVol->SetSpawningActive(false);
		}

		// Disable input
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController) { // If the controller exists
			PlayerController->SetCinematicMode(true, false, false, true, true);
		}

		// 'Kill' the player (enable ragdoll physics)
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0); // Get the player character
		if (PlayerCharacter) {
			PlayerCharacter->GetMesh()->SetSimulatePhysics(true); // Ragdoll.
			PlayerCharacter->GetMovementComponent()->MovementState.bCanJump = false;
		}

		character->deathCam(); // Activate a 'death cam'

		break;
	}
	case EBatteryPlayState::EWon: { // Win state
		// Deactivate all spawn volumes
		for (ASpawnVolume* SpawnVol : SpawnVolumes) {
			SpawnVol->SetSpawningActive(false);
		}
		break;
	}
	default:
	case EBatteryPlayState::EUnknown: { // Error state
		UE_LOG(LogClass, Log, TEXT("Entered an unknown state!")); // DEBUG message
		break;
	}
	}
}

void ABatteryCollector2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (character) { // If the character is valid
		if (character->GetCurrentPower() > powerToWin) { // If the player has powered up enough
			SetGameState(EBatteryPlayState::EWon); // We've won! =D
		}
		else if (character->GetCurrentPower() > 0) { // If at a positive nonzero health
			character->UpdatePower(-DeltaTime * decayRate * (character->GetInitialPower())); // Drain the power
		}
		else { // Power falls to 0
			SetGameState(EBatteryPlayState::EGameOver); // GAME OVER :(
		}
	}
}

float ABatteryCollector2GameMode::GetPowerToWin() const
{
	return powerToWin;
}
