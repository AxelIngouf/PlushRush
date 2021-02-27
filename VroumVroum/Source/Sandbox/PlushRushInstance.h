// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "PlushRushInstance.generated.h"

class ABumperKart;

UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	SNIPER		UMETA(DisplayName = "Sniper"),
	NUKE_THEM	UMETA(DisplayName = "Nuke Them"),
	NOPE		UMETA(DisplayName = "Nope"),
	EXECUTOR	UMETA(DisplayName = "Executor"),
	VICTIM		UMETA(DisplayName = "Victim"),
	GOURMAND	UMETA(DisplayName = "Gourmand"),
	PACIFIST	UMETA(DisplayName = "Pacifist"),
	CHRONO		UMETA(DisplayName = "Best Lap"),
	MOST_BONUS	UMETA(DisplayName = "Most Bonus")
};

USTRUCT()
struct FPlayerAchievements
{
	GENERATED_BODY()

	FPlayerAchievements() = default;
	FPlayerAchievements(FString Name, FString Value, EAchievementType Type, ABumperKart* Player);

	FString				AchievementName;
	FString				AchievementValue;
	EAchievementType	AchievementType;
	ABumperKart*		PlayerActor;
};

/**
 * 
 */
UCLASS()
class SANDBOX_API UPlushRushInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void	Init() override;

	virtual void	Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void	CreateServer();

	UFUNCTION(BlueprintCallable)
	void	JoinServer();

	void	EndRace();

	UPROPERTY(VisibleAnywhere)
	int NumberOfPlayersInRace;

	UPROPERTY(VisibleAnywhere)
	TArray<struct FCircuitPlayer>	Players;

	TArray<FPlayerAchievements>		Achievements;

private:

	UFUNCTION()
	void	OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void	OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void	OnFindSessionComplete(bool bWasSuccessful);

	void	OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void	TravelToRace();

	UPROPERTY(EditAnywhere)
	float	LobbyTimer = 30.f;

	IOnlineSessionPtr	SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FName OnlineSessionName = "Circuit Session";

	FTimerHandle TimerHandle_StartRace;
};