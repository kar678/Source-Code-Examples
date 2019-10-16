// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/ShooterGameMode.h"
#include "ShooterGame_KingOfTheHill.generated.h"

class AShooterPlayerState;
class AShooterAIController;
class AShooterKOTHCaptureZone;
/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGame_KingOfTheHill : public AShooterGameMode
{
	GENERATED_BODY()
	
	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;

	FTimerHandle TimerHandle;

	/** can players damage each other? */
	virtual bool CanDealDamage(AShooterPlayerState* DamageInstigator, AShooterPlayerState* DamagedPlayer) const override;

	virtual void CheckWinConditions();
	
protected:

	/** number of teams */
	int32 NumTeams;

	/** best team */
	int32 WinnerTeam;

	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(AShooterPlayerState* ForPlayerState) const;

	/** check who won */
	virtual void DetermineMatchWinner() override;

	virtual void HandleMatchHasStarted() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(AShooterPlayerState* PlayerState) const override;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** initialization for bot after spawning */
	virtual void InitBot(AShooterAIController* AIC, int32 BotNum) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(EditDefaultsOnly, Category = GameMode)
		bool TeamAData;

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
		bool TeamBData;

public:

	AShooterGame_KingOfTheHill(const FObjectInitializer & ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Gamemode Logic")
		virtual void FlagAddScore(AShooterKOTHCaptureZone* CaptureFlag, int32 OwningTeam, int32 Points);

};
