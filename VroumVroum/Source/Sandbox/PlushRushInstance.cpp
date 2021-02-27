// Fill out your copyright notice in the Description page of Project Settings.


#include "PlushRushInstance.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

#include "Circuit.h"
#include "Kismet/GameplayStatics.h"

FPlayerAchievements::FPlayerAchievements(FString Name, FString Value, EAchievementType Type, ABumperKart* Player)
{
    AchievementName = Name;
    AchievementValue = Value;
    AchievementType = Type;
    PlayerActor = Player;
}

void UPlushRushInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();

    if(OnlineSubSystem == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Online subsystem not found"));
        return;
    }

    SessionInterface = OnlineSubSystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Online session interface not found"));
        return;
    }

    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPlushRushInstance::OnCreateSessionComplete);
    SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPlushRushInstance::OnDestroySessionComplete);
    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPlushRushInstance::OnFindSessionComplete);
    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPlushRushInstance::OnJoinSessionComplete);

    GenerateCircuit();
}

void UPlushRushInstance::Shutdown()
{
    Super::Shutdown();

    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Online session interface not found"));
        return;
    }

    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(OnlineSessionName);
    if(ExistingSession)
    {
        if(!SessionInterface->DestroySession(OnlineSessionName))
        {
            UE_LOG(LogTemp, Warning, TEXT("Couldn't destroy session"));
        }
    }

}

void UPlushRushInstance::CreateServer()
{
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    if(IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
        SessionSettings.bIsLANMatch = false;
    else
        SessionSettings.bIsLANMatch = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.NumPublicConnections = 8;

    if (!SessionInterface->CreateSession(0, OnlineSessionName, SessionSettings))
    {
        UE_LOG(LogTemp, Warning, TEXT("Couldn't create the online session"));
        return;
    }
}

void UPlushRushInstance::JoinServer()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
        SessionSearch->bIsLanQuery = false;
    else
        SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = 10000;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UPlushRushInstance::GenerateCircuit_Implementation()
{
    if (UGameplayStatics::GetCurrentLevelName(this) == "PlaceHolder" || UGameplayStatics::GetCurrentLevelName(this) == "Axel")
        Circuit = Cast<ACircuit>(GetWorld()->SpawnActor(ACircuit::StaticClass()));
}

void UPlushRushInstance::EndRace()
{
    if(Players.Num() > 0)
    {
        FCircuitPlayer* SniperPlayer = nullptr;
        FCircuitPlayer* NukeThemPlayer = nullptr;
        FCircuitPlayer* NopePlayer = nullptr;
        FCircuitPlayer* ExecutorPlayer = nullptr;
        FCircuitPlayer* VictimPlayer = nullptr;
        FCircuitPlayer* GourmandPlayer = nullptr;
        FCircuitPlayer* PacifistPlayer = nullptr;
        FCircuitPlayer* BestLapPlayer = nullptr;
        FCircuitPlayer* MostBonusPlayer = nullptr;

        for(FCircuitPlayer Player : Players)
        {

            if (Player.PlayerStats.PlayerTrappedWithBullet > 0 && (SniperPlayer == nullptr || Player.PlayerStats.
                PlayerTrappedWithBullet > SniperPlayer->PlayerStats.PlayerTrappedWithBullet))
            {
                SniperPlayer = &Player;
            }

            if (Player.PlayerStats.PlayerTrappedWithRCCar > 0 && (NukeThemPlayer == nullptr || Player.PlayerStats.
                PlayerTrappedWithRCCar > NukeThemPlayer->PlayerStats.PlayerTrappedWithRCCar))
            {
                NukeThemPlayer = &Player;
            }

            if (Player.PlayerStats.PlayerTrappedWithMonkey > 0 && (NopePlayer == nullptr || Player.PlayerStats.
                PlayerTrappedWithMonkey > NopePlayer->PlayerStats.PlayerTrappedWithMonkey))
            {
                NopePlayer = &Player;
            }

            if (Player.PlayerStats.PlayerTrappedWithMonkey + Player.PlayerStats.PlayerTrappedWithBullet + Player.
                PlayerStats.PlayerTrappedWithRCCar > 0 && (ExecutorPlayer == nullptr || Player.PlayerStats.
                    PlayerTrappedWithMonkey + Player.PlayerStats.PlayerTrappedWithBullet + Player.PlayerStats.
                    PlayerTrappedWithRCCar > ExecutorPlayer->PlayerStats.PlayerTrappedWithMonkey + ExecutorPlayer->
                    PlayerStats.PlayerTrappedWithBullet + ExecutorPlayer->PlayerStats.PlayerTrappedWithRCCar))
            {
                ExecutorPlayer = &Player;
            }

            if (Player.PlayerStats.FellInTrap > 0 && (VictimPlayer == nullptr || Player.PlayerStats.
                FellInTrap > VictimPlayer->PlayerStats.FellInTrap))
            {
                VictimPlayer = &Player;
            }

            if (Player.PlayerStats.TotalCandies > 0 && (GourmandPlayer == nullptr || Player.PlayerStats.
                TotalCandies > GourmandPlayer->PlayerStats.TotalCandies))
            {
                GourmandPlayer = &Player;
            }

            if (PacifistPlayer == nullptr || Player.PlayerStats.
                BonusUsed < PacifistPlayer->PlayerStats.BonusUsed)
            {
                PacifistPlayer = &Player;
            }

            if (BestLapPlayer == nullptr || Player.PlayerStats.BestLapTimer < BestLapPlayer->PlayerStats.BestLapTimer)
            {
                BestLapPlayer = &Player;
            }

            if (Player.PlayerStats.BonusUsed > 0 && (MostBonusPlayer == nullptr || Player.PlayerStats.
                BonusUsed > MostBonusPlayer->PlayerStats.BonusUsed))
            {
                MostBonusPlayer = &Player;
            }
        }

        if(SniperPlayer != nullptr)
        {
            Achievements.Emplace("Sniper", FString(SniperPlayer->PlayerStats.PlayerTrappedWithBullet + ""), EAchievementType::SNIPER, SniperPlayer->Actor);
        }

        if (NukeThemPlayer != nullptr)
        {
            Achievements.Emplace("NUKE THEM !", FString(NukeThemPlayer->PlayerStats.PlayerTrappedWithRCCar + ""), EAchievementType::NUKE_THEM, NukeThemPlayer->Actor);
        }

        if (NopePlayer != nullptr)
        {
            Achievements.Emplace("Nope", FString(NopePlayer->PlayerStats.PlayerTrappedWithMonkey + ""), EAchievementType::NOPE, NopePlayer->Actor);
        }

        if (ExecutorPlayer != nullptr)
        {
            Achievements.Emplace(
                "Executor", FString(
                    ExecutorPlayer->PlayerStats.PlayerTrappedWithMonkey + ExecutorPlayer->PlayerStats.
                    PlayerTrappedWithBullet + ExecutorPlayer->PlayerStats.PlayerTrappedWithRCCar + ""),
                EAchievementType::EXECUTOR, ExecutorPlayer->Actor);
        }

        if (VictimPlayer != nullptr)
        {
            Achievements.Emplace("Victime boloss", FString(VictimPlayer->PlayerStats.FellInTrap + ""), EAchievementType::VICTIM, VictimPlayer->Actor);
        }

        if (GourmandPlayer != nullptr)
        {
            Achievements.Emplace("Gourmand", FString(GourmandPlayer->PlayerStats.TotalCandies + ""), EAchievementType::GOURMAND, GourmandPlayer->Actor);
        }

        if (PacifistPlayer != nullptr)
        {
            Achievements.Emplace("Pacifist", FString(PacifistPlayer->PlayerStats.BonusUsed + ""), EAchievementType::PACIFIST, PacifistPlayer->Actor);
        }

        if (BestLapPlayer != nullptr)
        {
            Achievements.Emplace("Chrono", BestLapPlayer->PlayerStats.BestLapTimer.TimerToString(), EAchievementType::CHRONO, BestLapPlayer->Actor);
        }

        if (MostBonusPlayer != nullptr)
        {
            Achievements.Emplace("Trigger-happy", FString(MostBonusPlayer->PlayerStats.BonusUsed+""), EAchievementType::MOST_BONUS, MostBonusPlayer->Actor);
        }
    }
}

void UPlushRushInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if(!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Creating session %s failed"), *SessionName.ToString());
        return;
    }
    GetWorld()->ServerTravel("/Game/Maps/Dev/Axel?listen");

    GetWorld()->GetTimerManager().SetTimer(TimerHandle_StartRace, this, &UPlushRushInstance::TravelToRace, LobbyTimer);
}

void UPlushRushInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Destroying session %s failed"), *SessionName.ToString());
        return;
    }
}

void UPlushRushInstance::OnFindSessionComplete(const bool bWasSuccessful)
{
    if(!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Online session was not successfuly found"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Online session found"));

    TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

    if(SearchResults.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Joining server"));
        SessionInterface->JoinSession(0, OnlineSessionName, SearchResults[0]);
    }

    UE_LOG(LogTemp, Warning, TEXT("Found %d online sessions"), SearchResults.Num());

}

void UPlushRushInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Online session joined"));

    if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        FString JoinAddress;
        if(SessionInterface->GetResolvedConnectString(SessionName, JoinAddress) && JoinAddress != "")
        {
            PlayerController->ClientTravel(JoinAddress, TRAVEL_Absolute);
        }
    }
}

void UPlushRushInstance::TravelToRace()
{
    GetWorld()->ServerTravel("/Game/Maps/PlaceHolder?listen");

    GenerateCircuit();
}
