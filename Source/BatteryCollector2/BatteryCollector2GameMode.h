// Copyright Epic Games, Inc. All Rights Reserved.
// Intended as a fork of the Battery Collector game:
// The player's goal is to charge up his character
// While another AI-controlled bot is also competing for the batteries

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BatteryCollector2GameMode.generated.h"

// Game state enum
UENUM(BlueprintType)
enum class EBatteryPlayState : uint8 {
	EPlaying, EGameOver, EWon, EUnknown
};

UCLASS(minimalapi)
class ABatteryCollector2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABatteryCollector2GameMode();

	virtual void Tick(float DeltaTime) override;

	// Returns power to win (for the HUD)
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;

	// Gets the game state
	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetGameState() const;

	// Sets a new game state
	void SetGameState(EBatteryPlayState newState);

protected:

	// Decay rate for the player's power
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float decayRate;

	// POwer to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"));
	float powerToWin;

	class ABatteryCollector2Character* character;

	// Widget class for the HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"));
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	// The HUD instance
	UPROPERTY()
	class UUserWidget* CurrentWidget;

private:
	EBatteryPlayState gameState; // the current state

	// An array of all spawn volumes
	TArray<class ASpawnVolume*> SpawnVolumes;

	/** Handles any function calls on changing the state */
	void HandleNewState(EBatteryPlayState newState);
};



