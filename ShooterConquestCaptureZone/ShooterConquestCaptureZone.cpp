// Copyright Cogwheel Interactive 2018

#include "ShooterGame.h"
#include "ShooterConquestCaptureZone.h"
#include "Online/ShooterPlayerState.h"
#include "Online/ShooterGame_Conquest.h"


AShooterConquestCaptureZone::AShooterConquestCaptureZone(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	Team = 2;
	TeamNum0 = 0;
	TeamNum1 = 0;
	UpdateTime = 8.0f;
	Points = 1;
	TotalCaptureTime = 120.0f;
	Team0Percent = 0.0f;
	Team1Percent = 0.0f;
	TeamMaxPercent = 100.0f;
	IsContested = false;
}

void AShooterConquestCaptureZone::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterConquestCaptureZone, TeamNum0);
	DOREPLIFETIME(AShooterConquestCaptureZone, TeamNum1);
	DOREPLIFETIME(AShooterConquestCaptureZone, GoingToTeam);
	DOREPLIFETIME(AShooterConquestCaptureZone, Team1Percent);
	DOREPLIFETIME(AShooterConquestCaptureZone, Team0Percent);
	DOREPLIFETIME(AShooterConquestCaptureZone, Team);
}

int32 AShooterConquestCaptureZone::GetTeamOwningFlag() const
{
	return Team;
}

float AShooterConquestCaptureZone::GetTeam0Percent() const
{
	return Team0Percent;
}

float AShooterConquestCaptureZone::GetTeam1Percent() const
{
	return Team1Percent;
}

bool AShooterConquestCaptureZone::GetIsContested() const
{
	return IsContested;
}

float AShooterConquestCaptureZone::GetCapInterval() const
{
	return TotalCaptureTime;
}

FString AShooterConquestCaptureZone::GetFlagName() const
{
	return CapturePointName;
}

int32 AShooterConquestCaptureZone::GetTeamGoingTowards() const
{
	return GoingToTeam;
}


void AShooterConquestCaptureZone::BeginPlay()
{
	Super::BeginPlay();
	float A = TotalCaptureTime / 2.0f;
	PercentAddValue = 100.0f / A;

	// set up a notification for when this component overlaps something //CollisionComponent 
	OnActorBeginOverlap.AddDynamic(this, &AShooterConquestCaptureZone::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AShooterConquestCaptureZone::OnOverlapEnd);
}

void AShooterConquestCaptureZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().ClearTimer(TimerHandle2);
}

void AShooterConquestCaptureZone::StartLoop()
{
	if (Role = ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterConquestCaptureZone::FlagAddPointsLoop, UpdateTime, true, 3.0);
		//GetWorldTimerManager().SetTimer(TimerHandle2, this, &AShooterConquestCaptureZone::FlagLoop, 1.0f, true, 3.0);
	}
}

void AShooterConquestCaptureZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Team0Percent >= 100.0f)
	{
		Team = 0;
		if (Team0Percent > 100.0f)
		{
			Team0Percent = 100.0f;
		}
	}
	if (Team1Percent >= 100.0f)
	{
		Team = 1;
		if (Team1Percent > 100.0f)
		{
			Team1Percent = 100.0f;
		}
	}
	if (Team0Percent <= 0.0f && Team1Percent <= 0.0f)
	{
		Team = 2;
		GoingToTeam = 2;
	}
	if (Team0Percent > 0.0f && Team1Percent <= 0.0f)
	{
		GoingToTeam = 0;
	}
	if (Team1Percent > 0.0f && Team0Percent <= 0.0f)
	{
		GoingToTeam = 1;
	}

	if (Team0Percent < TeamMaxPercent)
	{
		if (TeamNum0 > 0)
		{
			if (Team0Percent > TeamMaxPercent)
			{
				Team0Percent = TeamMaxPercent;
			}

			if (Team1Percent > 0)
			{
				Team1Percent -= PercentAddValue * DeltaTime;
			}
			else if (Team0Percent < TeamMaxPercent)
			{
				Team0Percent += PercentAddValue * DeltaTime;
			}
		}
	}

	if (Team1Percent < TeamMaxPercent)
	{
		if (TeamNum1 > 0)
		{
			if (Team1Percent > TeamMaxPercent)
			{
				Team1Percent = TeamMaxPercent;
			}

			if (Team0Percent > 0)
			{
				Team0Percent -= PercentAddValue * DeltaTime;
			}
			else if (Team1Percent < TeamMaxPercent)
			{
				Team1Percent += PercentAddValue * DeltaTime;
			}
		}
	}
}

void AShooterConquestCaptureZone::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  //OtherActorPlayerState->GetTeamNum;
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AShooterCharacter* OtherActorCharacter = Cast<AShooterCharacter>(OtherActor);
		bool bDying = OtherActorCharacter->bIsDying;

		if (OtherActorCharacter && !bDying)
		{
			AShooterPlayerState* OtherActorPlayerState = Cast<AShooterPlayerState>(OtherActorCharacter->GetPlayerState());

			if (OtherActorPlayerState)
			{
				int32 OtherActorTeam = OtherActorPlayerState->GetTeamNum();

				if (OtherActorTeam == 0)
				{
					TeamNum0 += 1;
				}

				if (OtherActorTeam == 1)
				{
					TeamNum1 += 1;
				}
			}
		}

	}
}

void AShooterConquestCaptureZone::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		AShooterCharacter* OtherActorCharacter = Cast<AShooterCharacter>(OtherActor);

		bool bDying = OtherActorCharacter->bIsDying;

		if (OtherActorCharacter)
		{
			AShooterPlayerState* OtherActorPlayerState = Cast<AShooterPlayerState>(OtherActorCharacter->GetPlayerState());

			if (OtherActorPlayerState)
			{
				int32 OtherActorTeam = OtherActorPlayerState->GetTeamNum();

				if (OtherActorTeam == 0)
				{
					TeamNum0 -= 1;
				}

				if (OtherActorTeam == 1)
				{
					TeamNum1 -= 1;
				}
			}
		}
	}
}

void AShooterConquestCaptureZone::FlagLoop_Implementation()
{
	if (TeamNum0 == TeamNum1 && Team0Percent != 0 && Team1Percent != 0)
	{
		IsContested = true;
	}
	if (TeamNum0 > TeamNum1)
	{
		IsContested = false;

		if (GoingToTeam == 1)
		{
			Team1Percent -= PercentAddValue;
		}
		if (GoingToTeam == 2)
		{
			Team0Percent += PercentAddValue;
		}
		if (GoingToTeam == 0)
		{
			if (Team0Percent < 100.0f)
			{
				Team0Percent += PercentAddValue;
			}
		}
	}
	if (TeamNum1 > TeamNum0)
	{
		IsContested = false;

		if (GoingToTeam == 0)
		{
			Team0Percent -= PercentAddValue;
		}
		if (GoingToTeam == 2)
		{
			Team1Percent += PercentAddValue;
		}
		if (GoingToTeam == 1)
		{
			if (Team1Percent < 100.0f)
			{
				Team1Percent += PercentAddValue;
			}
		}
	}
}

void AShooterConquestCaptureZone::FlagAddPointsLoop_Implementation()
{
	if (Team == 0)
	{
		AShooterGame_Conquest* Gamemode = GetWorld()->GetAuthGameMode<AShooterGame_Conquest>();

		if (Gamemode)
		{
			Gamemode->FlagAddScore(this, 0, Points);
		}
	}
	if (Team == 1)
	{
		AShooterGame_Conquest* Gamemode = GetWorld()->GetAuthGameMode<AShooterGame_Conquest>();

		if (Gamemode)
		{
			Gamemode->FlagAddScore(this, 1, Points);
		}
	}
}

bool AShooterConquestCaptureZone::FlagLoop_Validate()
{
	return true;
}

bool AShooterConquestCaptureZone::FlagAddPointsLoop_Validate()
{
	return true;
}
