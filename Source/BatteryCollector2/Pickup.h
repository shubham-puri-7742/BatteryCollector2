// Fill out your copyright notice in the Description page of Project Settings.
// BASE PICKUP CLASS. CAN BE SUBCLASSED

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BATTERYCOLLECTOR2_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Whether the pickup can be used
	bool bActive;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Mesh getter */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { // Just get the mesh. No modificitaions
		return PickupMesh;
	}

	/** Returns if the pickup is active */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive();

	/** To change the state of the pickup */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool PickupState);

	/** For when the pickup is collected */
	UFUNCTION(BlueprintNativeEvent) // Some behaviours in code, some in BP
	void Collect();
	virtual void Collect_Implementation(); // The version that child classes can override

private:

	/** Pickup mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PickupMesh;

};
