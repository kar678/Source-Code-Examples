// Copyright Cogwheel Interactive 2018

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "ShooterConquestCaptureZone.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterConquestCaptureZone : public ATriggerSphere
{
	GENERATED_BODY()
	
	UFUNCTION()
		void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Game|Flag")
		void FlagLoop();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Game|Flag")
		void FlagAddPointsLoop();

	AShooterConquestCaptureZone(const class FObjectInitializer & ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		int32 GetTeamOwningFlag() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		float GetTeam0Percent() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		float GetTeam1Percent() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		bool GetIsContested() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		float GetCapInterval() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		FString GetFlagName() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Flag")
		int32 GetTeamGoingTowards() const;

	FTimerHandle TimerHandle;
	FTimerHandle TimerHandle2;

	/**How many people from team 0 are in the zone */
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Gameplay")
		int32 TeamNum0;

	/**How many people from team 1 are in the zone */
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Gameplay")
		int32 TeamNum1;

	/**Team that has this flag, even at the start of the match */
	UPROPERTY(EditAnywhere, Replicated, Category = "Gameplay")
		int32 Team;

	/**What Team the flag is going towards */
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Gameplay")
		int32 GoingToTeam;

	/**How often this flag grants points */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float UpdateTime;

	/**How many points this flag gives to the owning team */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		int32 Points;

	/**How Long it takes to go from one team to the other */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float TotalCaptureTime;

	/**The name that will be shown on the user interface */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FString CapturePointName;

	UPROPERTY()
		float PercentInterval;

	UPROPERTY()
		float PercentAddValue;

	/**How much TeamNum0 owns the flag 0-100% */
	UPROPERTY(Replicated, meta = (ClampMin = "0", ClampMax = "100"))
		float Team0Percent;

	/**How much TeamNum1 owns the flag 0-100% */
	UPROPERTY(Replicated, meta = (ClampMin = "0", ClampMax = "100"))
		float Team1Percent;

	UPROPERTY()
		float TeamMaxPercent;

	/**Tell us if the flag is contested */
	UPROPERTY(VisibleAnywhere)
		bool IsContested;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game end or when destoryed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void StartLoop();
	
};
