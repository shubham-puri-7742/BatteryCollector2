// Fill out your copyright notice in the Description page of Project Settings.
// GAMEPLAY FEATURE 01: PICKUPS (BATTERIES) TO CHARGE THE PLAYER'S 'ENERGY' (HEALTH). SUBCLASSED in BP BY A FAKE (DEAD) BATTERY

#include "Pickup.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // Make it false to improve performance.

	// Start active
	bActive = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh")); // Initialise the mesh
	RootComponent = PickupMesh; // Make it the root
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APickup::IsActive() // Because pickups can only be collected if active.
{
	return bActive;
}

void APickup::SetActive(bool PickupState) // Activate/deactivate
{
	bActive = PickupState;
}

void APickup::Collect_Implementation() // Override in child classes/BPs
{
	// DEBUG ONLY
	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("Collected a %s"), *PickupDebugString);
}

