#pragma once

#include "CoreMinimal.h"

#include <random>

#include "GameFramework/Pawn.h"
#include "BumperKart.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateDrift);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateRain);

UCLASS()
class SANDBOX_API ABumperKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
    explicit ABumperKart(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxColl = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* FrontLeft = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* FrontRight = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* BackLeft = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* BackRight = nullptr;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float VelocityCap = 42.f;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float MaxSpeed = 2000.f;	//m.s-1

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float BounceForce = 1000.f;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float SuspensionResistanceK = 85420.69f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float GravityRate = 1.f;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float DragRate = 2.f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float TurnRate = 8.f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float HoverDistance = 75.f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float MaxTilt = 12.5f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
		float DriftRate = 42000.f;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly)
		bool bFinished = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float MaxBoostTime = 4.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float MaxFOV = 120.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bDrifting;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bBoostDrifting;

	//enum class ResistanceTerrain
	//{
	//	Concrete = 0.015f,
	//	Sand = 0.3f,
	//	Ketchup = 0.3f
	//};

	void	PickUpItem();

    /**
	 * Use current BonusObject if it's not null.
	 */
	void	UseItem();

    /**
	 * Use current BonusObject if it's not null.
	 * Some Bonuses will be used behind the player,
	 * but others will keep their default behavior.
	 */
	void	UseItemBehind();

    /**
	 * Plays a knock over animation on the kart, making it uncontrollable for a short period of time.
	 * BumperKart will also lose some of its candies.
	 */
	void	KnockOver(int KnockOverTime);

	void	KnockOverStop();

    /**
	 * Increment candy number by one.
	 */
	void	AddCandy();

    /**
	 * Used by Circuit/Server to inform the current BumperKart of its place in the leaderboard.
	 *
	 * @param Place - Player place in the race's leaderboard.
	 */
	void	SetPlayerLeaderboard(const int Place) { LeaderboardPlace = Place; }

	int		GetPlayerID() const { return PlayerID; }
	void	SetPlayerID(const int ID) { PlayerID = ID; }

	int		GetCurrentBonus() const { return CurrentBonus; }

    /**
	 * Returns BumperKart's Circuit.
	 *
	 * @return BumperKart's Circuit.
	 */
	UFUNCTION(BlueprintCallable)
	class ACircuit*		GetCircuit() const { return Circuit; }

	TSubclassOf<class ARemoteControlledCar> GetRCCarStaticClass() const { return RCCarBonusType; }

	void ApplyNextPosition(float DeltaTime);
	void Accelerate(float AxisValue);
	void Decelerate();
	void Turn(float AxisValue);
	void Drift();
	void DriftStop();
	void HoverGround();
	void LockRotation();
	void CameraMovement();
	void ResetCamera();
	void DriftBoost();

	FVector& GetVelocityVec() { return Velocity; }

	//TODO: Delete later (Used for ALPHA only)
	void GoToResult();

	/* NETWORK */

	void AttemptToAccelerate(float AxisValue);

	void AttemptToTurn(float AxisValue);

	void AttemptToDrift();

	void AttemptToStopDrift();

	void AttemptToUseItem();

	void AttemptToUseItemBehind();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAccelerate(float AxisValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTurn(float AxisValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrift();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopDrift();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItemBehind();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

    /**
	 * Place the Bumper Kart back on the track. Called when the player fell out.
	 */
	void	OutOfBound();

    /**
	 * Maximum number of candies the kart will drop when knocked over by a trap.
	 */
	UPROPERTY(EditAnywhere)
	int	MaxCandiesDropped = 3;

	/**
	 * Maximum number of candies the kart can carry.
	 */
	UPROPERTY(EditAnywhere)
	int MaxCandies = 10;

    /**
	 * Current Leaderboard place. Updated by Circuit/Server. 
	 */
	UPROPERTY(VisibleAnywhere)
	int LeaderboardPlace = 0;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PistolMesh = nullptr;

    /**
	 * When the BumperKart will use a RemoteControlledCar, this the static class that will be spawned.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARemoteControlledCar> RCCarBonusType;

	UPROPERTY(VisibleAnywhere)
	int		CurrentBonus;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateItem	OnUpdateItem;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateDrift	OnUpdateDrift;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateDrift	OnUpdateRain;

private:

    /**
	 * Returns Static class of the specified bonus.
	 *
	 * @param BonusType - Index of the Bonus.
	 * @return Static class of the specified bonus.
	 */
	TSubclassOf<class UBonusObject>	GetBonusStaticClass(const int BonusType) { return BonusTypes[BonusType]; }

    /**
	 * Get a random bonus index depending on the player leaderboard.
	 *
	 * @return BonusTypes array index of a random bonus.
	 */
	int		GetRandomBonus() const;

	void	DestroyCurrentBonus();

	UFUNCTION()
	void	UpdatePistolVisibility();

	UFUNCTION()
	void	UpdateTerrainResistance();

	void	ConstructSounds();

	void	InitSounds();

	UPROPERTY(Replicated)
	FVector Velocity = FVector::ZeroVector;
	FVector Extent = FVector::ZeroVector;
	FHitResult CenterOutHit;

	float MaxEngineForce = 10000.f;		//N
	float MaxTurningDegrees = 90.f;		//deg
	float Mass = 1000.f;				//Kg
	UPROPERTY(Replicated)
	float Throttle = 0.f;
	UPROPERTY(Replicated)
	float Steering = 0.f;
	UPROPERTY(Replicated)
	float TerrainResistanceK = 0.45f;
	float MaxVelCam = 60.f;
	float LerpTimer = 0.f;
	float LastFrameFOV = 0.f;
	float MinFov = 60.f;
	float DriftHold = 0.f;

	float	DefaultTerrainResistanceK = 0.45f;
	float	RainTerrainResistanceK = 0.3f;

	bool	bObstacle = false;

	UPROPERTY(VisibleAnywhere)
	bool	bWallRiding = false;
	UPROPERTY(VisibleAnywhere)
	bool	bOnGround = false;
	UPROPERTY(VisibleAnywhere)
	FVector	GroundNormal;
	//UPROPERTY(VisibleAnywhere)
	//FRotator	GroundRotation;

	int ItemID = 0;

	bool bFirstCall = false;

	FTimerHandle CamTimer;
	FTimerHandle BoostTimer;

    /**
	 * Current number of candies.
	 */
	UPROPERTY(VisibleAnywhere)
	int	NumberOfCandies = 0;

	UPROPERTY(EditAnywhere)
	class UBumperKartPawnMovementComponent*	BumperKartMovementComponent = nullptr;

	UPROPERTY(EditAnywhere)
    class UCandyEmitter* CandyEmitter = nullptr;

	ACircuit* Circuit = nullptr;

    /**
	 * Player ID given by the server.
	 */
	UPROPERTY(VisibleAnywhere)
	int PlayerID;

	static std::random_device Rd;
	static std::mt19937 Gen;

	TArray<TSubclassOf<UBonusObject>> BonusTypes;

	bool			bKnockedOver = false;
	FTimerHandle	TimerHandle_KnockOver;

	/* SOUNDS */

	UAudioComponent* MotorSound = nullptr;
	UAudioComponent* MotorStopSound = nullptr;

	UPROPERTY(EditAnywhere)
	USoundWave*	MotorSoundWave = nullptr;
	UPROPERTY(EditAnywhere)
	USoundWave* MotorStopSoundWave = nullptr;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};