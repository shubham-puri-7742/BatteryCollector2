// Fill out your copyright notice in the Description page of Project Settings.
// SPAWN VOLUME: SPAWNS A SPECIFIED CLASS AT A RANDOM POINT INSIDE IT

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYCOLLECTOR2_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The pickup to spawn */
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> SpawnedObject;

	// Spawn timer
	FTimerHandle SpawnTimer;

	// Min and max spawn delays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float minSpawnDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float maxSpawnDelay;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UBoxComponent* GetSpawnLocation() const { // gets a reference to the spawn point
		return SpawnLoc;
	}

	/** Gets a random spawnpoint in the volume */
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	// Toggles spawning
	UFUNCTION (BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool spawning);

private:
	/** Where to spawn the object */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnLoc;

	/** Spawns a new pickup */
	void SpawnPickup();

	/** Current spawn delay */
	float spawnDelay;

};
