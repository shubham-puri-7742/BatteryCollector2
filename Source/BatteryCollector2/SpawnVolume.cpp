// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickup.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // Make it false to improve performance.

	// Initialise the spawn volume
	SpawnLoc = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnLocation"));
	RootComponent = SpawnLoc; // Make it the root

	// Set the spawn delays
	minSpawnDelay = 1.0f;
	maxSpawnDelay = 4.5f;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetRandomPointInVolume()
{
	// Get a random point at a certain distance from the origin
	FVector SpawnOrigin = SpawnLoc->Bounds.Origin;
	FVector SpawnExtent = SpawnLoc->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SetSpawningActive(bool spawning)
{
	if (spawning) {
		// Set the timer
		// Set a random spawn delay between min and max
		spawnDelay = FMath::FRandRange(minSpawnDelay, maxSpawnDelay);

		// Call spawn pickup after a random time interval between the min and max delays
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, spawnDelay, false);

	}
	else { // Turn off spawning
		// Clear the timer
		GetWorldTimerManager().ClearTimer(SpawnTimer);

	}
}

void ASpawnVolume::SpawnPickup()
{
	// If the object is set
	if (SpawnedObject != NULL) {
		// Check if valid world
		UWorld* const World = GetWorld();
		if (World) {
			// Set spawn params
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // The spawnvolume is the owner
			SpawnParams.Instigator = GetInstigator();

			// Get a random spawnpoint and rotation
			FVector SpawnLocation = GetRandomPointInVolume();
			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;

			// Spawn the pickups
			APickup* const SpawnedPickup = World->SpawnActor<APickup>(SpawnedObject, SpawnLocation, SpawnRotation, SpawnParams);

			// Set a random spawn delay between min and max
			spawnDelay = FMath::FRandRange(minSpawnDelay, maxSpawnDelay);

			// Call spawn pickup after a random time interval between the min and max delays
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, spawnDelay, false);
		}
	}
}

