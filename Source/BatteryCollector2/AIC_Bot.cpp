// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Bot.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BatteryPickup.h"

void AAIC_Bot::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true; // Enable tick
	Super::BeginPlay();

	// Required for starting the first move
	start = true;
}

void AAIC_Bot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds); // parent tick
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABatteryPickup::StaticClass(), Waypoints); // get all battery pickups and store in waypoints
	if (Waypoints.Num() && start) { // if not empty and starting
		GoToRandomWaypoint(); // make the first move
		start = false; // repetition through OnMoveCompleted
	}
}

void AAIC_Bot::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAIC_Bot::GoToRandomWaypoint, 0.5f, false); // go to another random waypoint every 0.5s
}

ABatteryPickup* AAIC_Bot::GetRandomWaypoint()
{
	auto index = FMath::RandRange(0, Waypoints.Num() - 1); // get a random index in the valid range
	return Cast<ABatteryPickup>(Waypoints[index]); // return a reference to a battery pickup actor at that index
}

void AAIC_Bot::GoToRandomWaypoint()
{
	MoveToActor(GetRandomWaypoint()); // move to a random waypoint
}
