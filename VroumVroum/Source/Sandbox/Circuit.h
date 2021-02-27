// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Circuit.generated.h"

class ABumperKart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRaceFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewPlayerRegistered, ABumperKart*, PlayerKart);


class UWeatherConfig;

USTRUCT()
struct FCircuitTimer
{
	GENERATED_BODY()

	FCircuitTimer() = default;
	FCircuitTimer(const int M, const int S, const int  MS)
	{
		Minutes = M;
		Seconds = S;
		Milliseconds = MS;
	}

	bool			operator<(const FCircuitTimer& Other) const;
	FCircuitTimer	operator-(const FCircuitTimer& Other) const;

	FString TimerToString() const;

	UPROPERTY(VisibleAnywhere)
	int		Minutes = 0;
	UPROPERTY(VisibleAnywhere)
	int		Seconds = 0;
	UPROPERTY(VisibleAnywhere)
	float	Milliseconds = 0.f;
};

USTRUCT()
struct FCircuitPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int				Leaderboard = 0;
	UPROPERTY(VisibleAnywhere)
	int				BonusUsed = 0;
	UPROPERTY(VisibleAnywhere)
	int				FellInTrap = 0;
	UPROPERTY(VisibleAnywhere)
	int				PlayerTrappedWithMonkey = 0;
	UPROPERTY(VisibleAnywhere)
	int				PlayerTrappedWithBullet = 0;
	UPROPERTY(VisibleAnywhere)
	int				PlayerTrappedWithRCCar = 0;
	UPROPERTY(VisibleAnywhere)
	int				TotalCandies = 0;
	UPROPERTY(VisibleAnywhere)
	FCircuitTimer	BestLapTimer;
	UPROPERTY(VisibleAnywhere)
	FCircuitTimer	LastLapTotalTimer;

	FName			PlayerName;
};


USTRUCT()
struct FCircuitPlayer
{
	GENERATED_BODY()

	FCircuitPlayer() = default;
	FCircuitPlayer(ABumperKart* A, class ACheckpoint* FirstCheckpoint);

    bool	IsBehind(const FCircuitPlayer& OtherPlayer) const;

	ABumperKart*	Actor = nullptr;
	UPROPERTY(VisibleAnywhere)
	int		Lap = 1;
	UPROPERTY(VisibleAnywhere)
	bool	bFinished = false;

	UPROPERTY(VisibleAnywhere)
	bool	bWrongWay = false;

	UPROPERTY(VisibleAnywhere)
	FCircuitPlayerStats PlayerStats;

	UPROPERTY(VisibleAnywhere)
	float	CurrentDistanceToNextCheckpoint = 0;

	UPROPERTY(VisibleAnywhere)
	float	NearestDistanceToNextCheckpoint = -1;

	UPROPERTY(VisibleAnywhere)
	ACheckpoint*	NextCheckpoint = nullptr;
};

UCLASS()
class SANDBOX_API ACircuit final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACircuit();

    /**
	 * Register a new Player to the Race.
	 * This function should be called by each BumperKart on BeginPlay.
	 *
	 * @param NewPlayer - New Player to register.
	 */
	void	RegisterPlayer(ABumperKart* NewPlayer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void	AttemptPlayerReachedCheckpoint(int PlayerID, int CheckpointID);

	UFUNCTION(Server, Reliable, WithValidation)
	void	ServerPlayerReachedCheckpoint(int PlayerID, int CheckpointID);

	void	PlayerReachedCheckpoint(int PlayerID, int CheckpointID);

    /**
     * Removes the specified player from the Players array.
	 * Called when a BumperKart is unexpectedly destroyed.
	 * 
	 * @param DestroyedPlayer - Player to delete from the Players array.
	 */
	void	DeletePlayer(AActor* DestroyedPlayer);

    /**
	 * Returns the Circuit Player at the specified place in the leaderboard.
	 * If the second argument is false, it will return the first found player,
	 * that has not finished the race, at the given leaderboard place or after.
	 *
	 * @param LeaderBoard - Place in the leaderboard
	 * @param bIncludeFinishedPlayers - Should return a player that finished the race or not
	 * @return The player at the given place in the leaderboard
	 */
	FCircuitPlayer* GetPlayerInLeaderboard(int LeaderBoard, bool bIncludeFinishedPlayers = true);

    /**
	 * Returns the first Circuit Player in race. The first player is updated every frames.
	 *
	 * @return First Circuit Player in race.
	 */
	FCircuitPlayer* GetFirstPlayerInRace() const { return FirstPlayerStillInRace; }

    /**
	 * Returns the Circuit Player with the specified ID.
	 * 
	 * @param PlayerID - ID of the Circuit Player to retrieve.
	 * @return Circuit Player with the specified ID.
	 */
	FCircuitPlayer* GetPlayer(const int PlayerID) { return &Players[PlayerID]; }

    /**
	 * Returns the Checkpoint with the specified ID.
	 * 
	 * @param Index - ID of the Checkpoint to retrieve.
	 * @return Checkpoint with the specified ID.
	 */
	ACheckpoint*	GetCheckpointAt(const int Index) const { return Checkpoints[Index]; }

    /**
	 * Get the total amount of in the Circuit.
	 * 
	 * @return Number of Checkpoints in the map.
	 */
	int				GetNumberOfCheckpoints() const { return Checkpoints.Num(); }

	int				GetNumberOfPlayers() const { return Players.Num(); }

	UFUNCTION(BlueprintCallable)
	UWeatherConfig* GetWeatherConfig() const { return WeatherComponent; }

    /**
	 * End Rac delegate. Broadcasts once when all the players crossed the finish line.
	 */
	UPROPERTY(BlueprintAssignable, Category = Circuit)
	FRaceFinished		OnRaceFinished;

	UPROPERTY(BlueprintAssignable, Category = Circuit)
	FOnNewPlayerRegistered	OnNewPlayerRegisteredDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

    /**
	 * Total number of laps to finish the race.
	 */
	UPROPERTY(EditAnywhere, Category=Circuit)
	int		NumberOfLaps = 3;

private:
    /**
	 * Finds all the Checkpoints on the map and put them, by ID order,
	 * in the Checkpoints Array of the current Circuit.
	 */
	void	RetrieveCheckpointsInOrder();

    /**
	 * Finds all the BumperKarts on the map and put them in the Players Array
	 * of the current Circuit while assigning each of them a unique ID.
	 */
	void	RetrievePlayers();

    /**
	 * Called when a Player finished a Circuit lap.
	 * 
	 * @param PlayerID - Player who finished the lap.
	 */
	void	PlayerFinishedLap(int PlayerID);

    /**
	 * Called when a Player finished the race.
	 * 
	 * @param PlayerID - Player who finished the race.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void	PlayerFinishedRace(int PlayerID);

    /**
	 * Called every frame to update the race timer.
	 */
	void	UpdateTimer();

    /**
	 * Called every frame to update the player distance to the next checkpoint.
	 * Distance is used to determine Players leaderboard when some of them are close to each others.
	 */
	void	UpdatePlayerDistance();

    /**
	 * Called every frame to update the current race Leaderboard.
	 */
	void	UpdateLeaderboard();

    /**
	 * Array of Checkpoints sorted by order in the Circuit.
	 */
	UPROPERTY(EditAnywhere, Category=Circuit)
	TArray<ACheckpoint*>	Checkpoints;

    /**
	 * Array of Circuit Players, containing the BumperKart they are referring to,
	 * and each with a different ID (corresponding to their index in the array).
	 */
	UPROPERTY(EditAnywhere, Replicated, Category = Circuit)
	TArray<FCircuitPlayer>	Players;

	bool	bStarted = false;

    /**
	 * Current Circuit timer.
	 */
	FCircuitTimer	CircuitTimer;
	float	LastRecordedTime = 0.f;

    /**
	 * Number of Players that reached the finished line.
	 */
	UPROPERTY(EditAnywhere, Replicated, Category = Circuit)
	int		NumberOfFinishedPlayers = 0;

    /**
	 * Pointer to the first player of the race. Updated every frame.
	 */
	FCircuitPlayer* FirstPlayerStillInRace;

	UPROPERTY(EditAnywhere)
	UWeatherConfig* WeatherComponent = nullptr;
};
