// Fill out your copyright notice in the Description page of Project Settings.
// SUBCLASS OF PICKUP FOR BATTERY. SUBCLASSED IN BP AS A FAKE (DEAD) BATTERY

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR2_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABatteryPickup();

	/** Override the collect function */
	virtual void Collect_Implementation() override;

	/** Gets the battery power */
	float GetPower();

protected:
	/** The amount of power the battery gives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float power;

};
