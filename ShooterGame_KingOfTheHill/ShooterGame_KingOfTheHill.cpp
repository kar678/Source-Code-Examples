// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTeamStart.h"
#include "ShooterGame_KingOfTheHill.h"
#include "Online/ShooterPlayerState.h"
#include "Bots/ShooterAIController.h"
#include "ShooterKOTHCaptureZone.h"
#include "EngineUtils.h"


AShooterGame_KingOfTheHill::AShooterGame_KingOfTheHill(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumTeams = 2;
	bDelayedStart = true;
	GameType = "KOTH";
}

void AShooterGame_KingOfTheHill::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	AShooterPlayerState* NewPlayerState = CastChecked<AShooterPlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
}

void AShooterGame_KingOfTheHill::InitGameState()
{
	Super::InitGameState();

	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
		MyGameState->TeamTargetScore = TargetScore;
		MyGameState->RespawnDelay = MinRespawnDelay;
	}
}

void AShooterGame_KingOfTheHill::BeginPlay()
{
	if (bUseMatchWinConditions)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterGame_KingOfTheHill::CheckWinConditions, CheckWinConditionsInterval, true, 3.0);
	}
}

void AShooterGame_KingOfTheHill::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (TActorIterator<AShooterKOTHCaptureZone> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AShooterKOTHCaptureZone* CP = Cast<AShooterKOTHCaptureZone>(*ActorItr);
		if (CP)
		{
			CP->StartLoop();
		}
	}
}

void AShooterGame_KingOfTheHill::CheckWinConditions()
{
	AShooterGameState const* const MyGameState = Cast<AShooterGameState>(GameState);
	int32 BestScore = TargetScore;

	for (int32 i = 0; i < MyGameState->TeamScoresKOTH.Num(); i++)
	{
		const int32 TeamScore = MyGameState->TeamScoresKOTH[i];
		if (BestScore < TeamScore)
		{
			FinishMatch();
		}
		else if (BestScore == TeamScore)
		{
			FinishMatch();
		}
	}

}

bool AShooterGame_KingOfTheHill::CanDealDamage(AShooterPlayerState* DamageInstigator, class AShooterPlayerState* DamagedPlayer) const
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		return false;
	}
	else
	{
		return DamageInstigator && DamagedPlayer && (DamagedPlayer == DamageInstigator || DamagedPlayer->GetTeamNum() != DamageInstigator->GetTeamNum());
	}
}

int32 AShooterGame_KingOfTheHill::ChooseTeam(AShooterPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		AShooterPlayerState const* const TestPlayerState = Cast<AShooterPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNum()))
		{
			TeamBalance[TestPlayerState->GetTeamNum()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}

void AShooterGame_KingOfTheHill::DetermineMatchWinner()
{
	AShooterGameState const* const MyGameState = Cast<AShooterGameState>(GameState);
	int32 BestScore = MAX_uint32;
	int32 BestTeam = -1;
	int32 NumBestTeams = 1;

	for (int32 i = 0; i < MyGameState->TeamScoresKOTH.Num(); i++)
	{
		const int32 TeamScore = MyGameState->TeamScoresKOTH[i];
		if (BestScore < TeamScore)
		{
			BestScore = TeamScore;
			BestTeam = i;
			NumBestTeams = 1;
		}
		else if (BestScore == TeamScore)
		{
			NumBestTeams++;
		}
	}

	WinnerTeam = (NumBestTeams == 1) ? BestTeam : NumTeams;
}

bool AShooterGame_KingOfTheHill::IsWinner(AShooterPlayerState* PlayerState) const
{
	return PlayerState && !PlayerState->IsQuitter() && PlayerState->GetTeamNum() == WinnerTeam;
}

bool AShooterGame_KingOfTheHill::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		AShooterTeamStart* TeamStart = Cast<AShooterTeamStart>(SpawnPoint);
		AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

void AShooterGame_KingOfTheHill::InitBot(AShooterAIController* AIC, int32 BotNum)
{
	AShooterPlayerState* BotPlayerState = CastChecked<AShooterPlayerState>(AIC->PlayerState);
	const int32 TeamNum = ChooseTeam(BotPlayerState);
	BotPlayerState->SetTeamNum(TeamNum);

	Super::InitBot(AIC, BotNum);
}

void AShooterGame_KingOfTheHill::FlagAddScore(AShooterKOTHCaptureZone* CaptureFlag, int32 OwningTeam, int32 Points)
{
	if (CaptureFlag)
	{
		AShooterGameState* const MyGameState = GetWorld()->GetGameState<AShooterGameState>();
		if (MyGameState && OwningTeam >= 0)
		{
			if (OwningTeam >= MyGameState->TeamScoresKOTH.Num())
			{
				MyGameState->TeamScoresKOTH.AddZeroed(OwningTeam - MyGameState->TeamScoresKOTH.Num() + 1);
			}

			MyGameState->TeamScoresKOTH[OwningTeam] += Points;
		}
	}
}
