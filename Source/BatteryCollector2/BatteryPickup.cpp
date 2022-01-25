// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryPickup.h"


ABatteryPickup::ABatteryPickup()
{
	GetMesh()->SetSimulatePhysics(true);

	// Starting power
	power = 150.f;
}

float ABatteryPickup::GetPower() // How much power it stores
{
	return power;
}

void ABatteryPickup::Collect_Implementation()
{
	// Call the parent function (logs for DEBUG)
	Super::Collect_Implementation();

	// Destroy the battery
	Destroy();
}
