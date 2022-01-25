// Fill out your copyright notice in the Description page of Project Settings.

// BOT AI (COMPETES WITH THE PLAYER FOR THE FALLING BATTERIES)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
// #include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "AIC_Bot.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR2_API AAIC_Bot : public AAIController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// To run on completion (starts a new move after 0.5s)
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	UPROPERTY()
	TArray<AActor*> Waypoints; // stores waypoints (locations of batteries)

	UFUNCTION()
	class ABatteryPickup* GetRandomWaypoint(); // Gets a random waypoint - Type changed from TargetPoint to BatteryPickup

	UFUNCTION()
	void GoToRandomWaypoint();

	FTimerHandle TimerHandle; // For repeating the move every 0.5s

	bool start; // for triggering the initial move
	
};
