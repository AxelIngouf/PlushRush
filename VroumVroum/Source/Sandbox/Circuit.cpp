#include "Circuit.h"

#include "BumperKart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlushRushInstance.h"
#include "Checkpoint.h"

#include "WeatherConfig.h"
#include "Net/UnrealNetwork.h"

#define DISTANCE_BEFORE_WRONG_WAY 2000

ACircuit::ACircuit()
{
	PrimaryActorTick.bCanEverTick = true;

	WeatherComponent = CreateDefaultSubobject<UWeatherConfig>(TEXT("WeatherComponent"));

	bAlwaysRelevant = true;
	SetReplicates(true);
}

void ACircuit::RegisterPlayer(ABumperKart* NewPlayer)
{
	RetrievePlayers();
	if(NewPlayer->GetPlayerID() < 0)
	{
		NewPlayer->SetPlayerID(Players.Num());
		Players.Emplace(NewPlayer, Checkpoints[0]);
		OnNewPlayerRegisteredDelegate.Broadcast(NewPlayer);
	}
}

void ACircuit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACircuit, NumberOfFinishedPlayers);
	DOREPLIFETIME(ACircuit, Players);
}

void ACircuit::AttemptPlayerReachedCheckpoint(const int PlayerID, const int CheckpointID)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerPlayerReachedCheckpoint(PlayerID, CheckpointID);
	}
	else
	{
		PlayerReachedCheckpoint(PlayerID, CheckpointID);
	}
}

void ACircuit::ServerPlayerReachedCheckpoint_Implementation(const int PlayerID, const int CheckpointID)
{
	PlayerReachedCheckpoint(PlayerID, CheckpointID);
}

bool ACircuit::ServerPlayerReachedCheckpoint_Validate(const int PlayerID, const int CheckpointID)
{
	return true;
}

void ACircuit::PlayerReachedCheckpoint(const int PlayerID, const int CheckpointID)
{
	if(PlayerID < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unknown player crossed a checkpoint."));
		return;
	}


	FCircuitPlayer* Player = &Players[PlayerID];

	if (Player->bFinished)
		return;

	if (Player->NextCheckpoint->GetID() == CheckpointID)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), CheckpointID);
		UE_LOG(LogTemp, Warning, TEXT("%d"), Checkpoints.Num() - 1);
		if (CheckpointID == Checkpoints.Num() - 1)
		{
			PlayerFinishedLap(PlayerID);
		}
		else
		{
			Player->NextCheckpoint = Checkpoints[CheckpointID + 1];
		}
		Player->NearestDistanceToNextCheckpoint = -1;
	}
}

void ACircuit::DeletePlayer(AActor* DestroyedPlayer)
{
	ABumperKart* BumperKart = Cast<ABumperKart>(DestroyedPlayer);

	if (BumperKart == nullptr)
		return;

	const int PlayerID = BumperKart->GetPlayerID();

	Players.RemoveAt(PlayerID);

	if(Players.Num() < PlayerID)
	{
	    for(int32 i = PlayerID; i < Players.Num(); i++)
	    {
			Players[i].Actor->SetPlayerID(i);
	    }
	}
}

FCircuitPlayer* ACircuit::GetPlayerInLeaderboard(int LeaderBoard, const bool bIncludeFinishedPlayers)
{
	if(!bIncludeFinishedPlayers)
	{
		LeaderBoard += NumberOfFinishedPlayers;
	}

	if (LeaderBoard > this->Players.Num())
		return nullptr;

	for (int iPlayer = 0; iPlayer < Players.Num(); iPlayer++)
	{
		if (Players[iPlayer].PlayerStats.Leaderboard == LeaderBoard)
		{
			return &Players[iPlayer];
		}
	}
	return nullptr;
}

void ACircuit::BeginPlay()
{
	Super::BeginPlay();

	RetrieveCheckpointsInOrder();

	if(Checkpoints.Num() > 0)
		RetrievePlayers();
}

void ACircuit::UpdatePlayerDistance()
{
	for(FCircuitPlayer& Player : Players)
	{
		const float NewDistance = UKismetMathLibrary::Vector_Distance(Player.NextCheckpoint->GetActorLocation(), Player.Actor->GetActorLocation());

		if (Player.NearestDistanceToNextCheckpoint +
			DISTANCE_BEFORE_WRONG_WAY < NewDistance && Player.NearestDistanceToNextCheckpoint >= 0)
		{
			if(NewDistance - Player.NearestDistanceToNextCheckpoint > DISTANCE_BEFORE_WRONG_WAY / 2)
			{
				Player.bWrongWay = true;
				if(NewDistance - Player.NearestDistanceToNextCheckpoint > DISTANCE_BEFORE_WRONG_WAY * 1.5)
					Player.NearestDistanceToNextCheckpoint = NewDistance - DISTANCE_BEFORE_WRONG_WAY;
			}
			else
			{
				Player.bWrongWay = false;
			}
		}
		else
		{
			Player.bWrongWay = false;
			if(NewDistance < Player.NearestDistanceToNextCheckpoint || Player.NearestDistanceToNextCheckpoint < 0)
				Player.NearestDistanceToNextCheckpoint = NewDistance;
		}
		Player.CurrentDistanceToNextCheckpoint = NewDistance;
	}
}

void ACircuit::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTimer();
	UpdatePlayerDistance();
	UpdateLeaderboard();
}

void ACircuit::RetrieveCheckpointsInOrder()
{
	TArray<AActor*> MapCheckpoints;
	UGameplayStatics::GetAllActorsOfClass(this, ACheckpoint::StaticClass(), MapCheckpoints);

	Checkpoints.Reserve(MapCheckpoints.Num());

	for(int i = 0; i < MapCheckpoints.Num(); i++)
	{
		for (AActor* Actor : MapCheckpoints)
		{
			ACheckpoint* Checkpoint = Cast<ACheckpoint>(Actor);
			if (Checkpoint && Checkpoint->GetID() == i)
			{
				Checkpoints.Add(Checkpoint);
				break;
			}
		}
	}
}

void ACircuit::RetrievePlayers()
{
	TArray<AActor*> MapPlayers;
	UGameplayStatics::GetAllActorsOfClass(this, ABumperKart::StaticClass(), MapPlayers);

	if (Checkpoints.Num() <= 0)
		RetrieveCheckpointsInOrder();

    for(int i = 0; i < MapPlayers.Num(); i++)
	{
		ABumperKart* BumperKart = Cast<ABumperKart>(MapPlayers[i]);
		if(BumperKart)
		{
			bool Stop = false;
			for(const FCircuitPlayer& P : Players)
			{
			    if(P.Actor == BumperKart)
			    {
					Stop = true;
			    }
			}
			if(!Stop)
			{
				BumperKart->SetPlayerID(i);
				Players.Emplace(BumperKart, Checkpoints[0]);
			}
		}
	}
}

void ACircuit::PlayerFinishedLap(const int PlayerID)
{
	FCircuitPlayer& Player = Players[PlayerID];
	if(!Player.bFinished)
	{
		const FCircuitTimer CurrentTotalTimer = CircuitTimer;
		const FCircuitTimer CurrentLapTimer = CurrentTotalTimer - Player.PlayerStats.LastLapTotalTimer;
		if (Player.Lap == 1 || CurrentLapTimer < Player.PlayerStats.BestLapTimer)
		{
			Player.PlayerStats.BestLapTimer = CurrentLapTimer;
		}
		UE_LOG(LogTemp, Warning, TEXT("FinishedLap"));
		Player.Lap++;
		if (Player.Lap > NumberOfLaps)
		{
			PlayerFinishedRace(PlayerID);
		}
		Player.PlayerStats.LastLapTotalTimer = CurrentTotalTimer;
	}
	Player.NextCheckpoint = Checkpoints[0];
}

void ACircuit::PlayerFinishedRace_Implementation(const int PlayerID)
{
	Players[PlayerID].bFinished = true;
	NumberOfFinishedPlayers++;
	if (NumberOfFinishedPlayers >= Players.Num())
	{
		OnRaceFinished.Broadcast();

		//TODO: Move all following code in the delegate
		Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->NumberOfPlayersInRace = Players.Num();
		Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->Players = Players;
		Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->EndRace();

		UGameplayStatics::OpenLevel(GetWorld(), "UIEndGameLevel");
	}
}

bool ACircuit::PlayerFinishedRace_Validate(int PlayerID)
{
	if(Players[PlayerID].Lap <= NumberOfLaps)
	{
		return false;
	}
	return true;
}

void ACircuit::UpdateLeaderboard()
{
	std::vector<FCircuitPlayer> PlayersLeaderboard;
	for(const FCircuitPlayer& Player : Players)
	{
		std::vector<FCircuitPlayer>::iterator It;
		for (It = PlayersLeaderboard.begin(); It != PlayersLeaderboard.end(); ++It)
		{
			if (Player.IsBehind(*It))
			{
				break;
			}
		}
		PlayersLeaderboard.insert(It, Player);
	}

	for(int iPlayer = 0; iPlayer < PlayersLeaderboard.size(); iPlayer++)
	{
	    if(!PlayersLeaderboard[iPlayer].bFinished)
	    {
			Players[PlayersLeaderboard[iPlayer].Actor->GetPlayerID()].PlayerStats.Leaderboard = 1 + iPlayer;
			Players[PlayersLeaderboard[iPlayer].Actor->GetPlayerID()].Actor->SetPlayerLeaderboard(1 + iPlayer);
	    }
	}

	if(Players.Num() > 0)
	    FirstPlayerStillInRace = &Players[PlayersLeaderboard[0].Actor->GetPlayerID()];
}

void ACircuit::UpdateTimer()
{
    const float	TimeSinceCreation = GetGameTimeSinceCreation();
    const float	SecondsToAdd = TimeSinceCreation - LastRecordedTime;

	CircuitTimer.Milliseconds += SecondsToAdd * 1000;
	while (CircuitTimer.Milliseconds >= 1000)
	{
		CircuitTimer.Milliseconds -= 1000;
		CircuitTimer.Seconds++;
	}
	while (CircuitTimer.Seconds >= 60)
	{
		CircuitTimer.Seconds -= 60;
		CircuitTimer.Minutes++;
	}

	LastRecordedTime = TimeSinceCreation;
}

FCircuitPlayer::FCircuitPlayer(ABumperKart* A, ACheckpoint* FirstCheckpoint)
{
	Actor = A;
	NextCheckpoint = FirstCheckpoint;
	PlayerStats.PlayerName = "Player" + A->GetPlayerID();
}

bool FCircuitPlayer::IsBehind(const FCircuitPlayer& OtherPlayer) const
{
	if (OtherPlayer.bFinished)
		return true;

	if (this->Lap > OtherPlayer.Lap)
		return true;

	if (this->Lap == OtherPlayer.Lap)
	{
		if (this->NextCheckpoint->GetID() > OtherPlayer.NextCheckpoint->GetID())
			return true;

		if (this->NextCheckpoint->GetID() == OtherPlayer.NextCheckpoint->GetID())
		{
			const float ThisPlayerDistance = this->CurrentDistanceToNextCheckpoint;
			const float OtherPlayerDistance = this->CurrentDistanceToNextCheckpoint;

			if (ThisPlayerDistance > OtherPlayerDistance)
				return true;
		}
	}
	return false;
}

bool FCircuitTimer::operator<(const FCircuitTimer& Other) const
{
	if (Minutes < Other.Minutes)
		return true;

	if (Minutes == Other.Minutes)
	{
		if (Seconds < Other.Seconds)
			return true;

		if (Seconds == Other.Seconds)
		{
			if (Milliseconds < Other.Milliseconds)
				return true;
		}
	}

	return false;
}

FCircuitTimer FCircuitTimer::operator-(const FCircuitTimer& Other) const
{
	FCircuitTimer Ret;

	Ret.Milliseconds = Milliseconds - Other.Milliseconds;
	Ret.Seconds = Seconds - Other.Seconds;
	Ret.Minutes = Minutes - Other.Minutes;

	if (Ret.Milliseconds < 0)
	{
		Ret.Milliseconds = 1000 + Ret.Milliseconds;
		Ret.Seconds--;
	}
	if (Ret.Seconds < 0)
	{
		Ret.Seconds = 60 + Ret.Seconds;
		Ret.Minutes--;
	}

	return Ret;
}

FString FCircuitTimer::TimerToString() const
{
	FString String(Minutes < 10 ? "0" + Minutes : Minutes + ":");
	if (Seconds < 10)
		String.Append("0");
	String.AppendInt(Seconds);
	String.Append(":");
	if (Seconds < 10)
		String.Append("0");
	String.AppendInt(Milliseconds);

	return String;
}