#include "BumperKart.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/AudioComponent.h"

#include "BumperKartPawnMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"

#include "Circuit.h"
#include "Checkpoint.h"
#include "WeatherConfig.h"
#include "Bonus/BonusObject.h"
#include "CandyEmitter.h"
#include "Kismet/GameplayStatics.h"

#include "PlushRushInstance.h"

#include "Bonus/ProjectileBonus.h"
#include "Bonus/RemoteControlledCarBonus.h"
#include "Bonus/StaticTrapBonus.h"

#include "Net/UnrealNetwork.h"

std::random_device ABumperKart::Rd;
std::mt19937 ABumperKart::Gen(Rd());

// Sets default values
ABumperKart::ABumperKart(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UBumperKartPawnMovementComponent>("PawnMovementComponent"))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxColl = CreateDefaultSubobject<UBoxComponent>(FName("MyBoxColl"));
	if (BoxColl != nullptr)
	{
		RootComponent = BoxColl;

		FrontLeft = CreateDefaultSubobject<USceneComponent>(FName("FrontLeft"));
		if (FrontLeft != nullptr)
			FrontLeft->SetupAttachment(BoxColl);
		FrontRight = CreateDefaultSubobject<USceneComponent>(FName("FrontRight"));
		if (FrontRight != nullptr)
			FrontRight->SetupAttachment(BoxColl);
		BackLeft = CreateDefaultSubobject<USceneComponent>(FName("BackLeft"));
		if (BackLeft != nullptr)
			BackLeft->SetupAttachment(BoxColl);
		BackRight = CreateDefaultSubobject<USceneComponent>(FName("BackRight"));
		if (BackRight != nullptr)
			BackRight->SetupAttachment(BoxColl);
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PistolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetRootComponent());
	PistolMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Assets/Meshes/Bonus/SM_Pistol.SM_Pistol")).Object);
	PistolMesh->SetCollisionProfileName(TEXT("IgnoreAll"));
	PistolMesh->SetVisibility(false);
	PistolMesh->SetRelativeLocation({ 20, 60, 0 });
	PistolMesh->SetRelativeRotation({ 0, -90, 0 });
	PistolMesh->SetRelativeScale3D({ 0.2, 0.2, 0.2 });

	CurrentBonus = static_cast<int>(EBonusType::EMPTY);

	BumperKartMovementComponent = CreateDefaultSubobject<UBumperKartPawnMovementComponent>(TEXT("BumperKartMovementComponent"));

	CandyEmitter = CreateDefaultSubobject<UCandyEmitter>(TEXT("CandyEmitter"));

	BonusTypes = { UProjectileBonus::StaticClass() , UProjectileBonus::StaticClass() , UStaticTrapBonus::StaticClass() , URemoteControlledCarBonus::StaticClass() };

	ConstructSounds();
}

// Called when the game starts or when spawned
void ABumperKart::BeginPlay()
{
	Super::BeginPlay();

	//Settings suspension
	if (BoxColl != nullptr)
	{
		Extent = BoxColl->GetScaledBoxExtent();
		FrontLeft->SetRelativeLocation({ Extent.X, -Extent.Y, -Extent.Z });
		FrontRight->SetRelativeLocation({ Extent.X, Extent.Y, -Extent.Z });
		BackLeft->SetRelativeLocation({ -Extent.X, -Extent.Y, -Extent.Z });
		BackRight->SetRelativeLocation({ -Extent.X, Extent.Y, -Extent.Z });
	}

	Circuit = Cast<ACircuit>(UGameplayStatics::GetActorOfClass(this, ACircuit::StaticClass()));

	if (!Circuit)
	{
		Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(this))->GenerateCircuit();
		Circuit = Cast<ACircuit>(UGameplayStatics::GetActorOfClass(this, ACircuit::StaticClass()));
	}

	if (Circuit)
	{
		Circuit->GetWeatherConfig()->OnRainUpdate.AddDynamic(this, &ABumperKart::UpdateTerrainResistance);
		Circuit->RegisterPlayer(this);
	}

	OnUpdateItem.AddDynamic(this, &ABumperKart::UpdatePistolVisibility);

	InitSounds();
}

void ABumperKart::FellOutOfWorld(const UDamageType& dmgType)
{
	OutOfBound();
}

void ABumperKart::OutOfBound()
{
	ACheckpoint* NextCheckpoint = Circuit->GetPlayer(PlayerID)->NextCheckpoint;
	if (NextCheckpoint != nullptr)
	{
		const int PreviousCheckpointID = NextCheckpoint->GetID() == 0 ? Circuit->GetNumberOfCheckpoints() - 1 : NextCheckpoint->GetID() - 1;
		SetActorLocation(Circuit->GetCheckpointAt(PreviousCheckpointID)->GetActorLocation());
		SetActorRotation(Circuit->GetCheckpointAt(PreviousCheckpointID)->GetActorRotation());

		Velocity = FVector::OneVector;
		Throttle = 0.f;
		Steering = 0.f;
	}
}

int ABumperKart::GetRandomBonus() const
{
	// #	PR3	PR1	STT	RCC
	// 1 :	5	50	50	5
	// 2 :	10	40	40	5
	// 3 :	15	30	30	5
	// 4 :	20	20	20	5
	// 5 :	25	10	10	5
	// 6 :	20	10	10	5
	// 7 :	15	10	10	5
	// 8 :	10	10	10	5

	int	Probability[static_cast<int>(EBonusType::EMPTY)]{ 10 };

	Probability[static_cast<int>(EBonusType::STATIC_TRAP)] += (5 - LeaderboardPlace) * 10;
	Probability[static_cast<int>(EBonusType::PROJECTILE_ONE)] += (5 - LeaderboardPlace) * 10;
	Probability[static_cast<int>(EBonusType::REMOTE_CONTROLLED_CAR)] = 5;

	Probability[static_cast<int>(EBonusType::PROJECTILE_THREE)] = 5 + (4 - abs(LeaderboardPlace - 5)) * 5;

	int TotalProbabilities = 0;
	for (int i = 0; i < static_cast<int>(EBonusType::EMPTY); i++)
	{
		TotalProbabilities += Probability[i];
	}

	const std::uniform_int_distribution<int> Dis(0, TotalProbabilities - 1);

	int	ChosenProbability = Dis(Gen);
	int ChosenIndex = 0;
	while (ChosenProbability > 0)
	{
		ChosenProbability -= Probability[ChosenIndex];
		if (ChosenProbability > 0)
			ChosenIndex++;
	}

	return ChosenIndex;
}

void ABumperKart::DestroyCurrentBonus()
{
	switch (CurrentBonus)
	{
		case static_cast<int>(EBonusType::PROJECTILE_THREE) :
			CurrentBonus = static_cast<int>(EBonusType::PROJECTILE_TWO);
			break;
		case static_cast<int>(EBonusType::PROJECTILE_TWO) :
			CurrentBonus = static_cast<int>(EBonusType::PROJECTILE_ONE);
			break;
		default:
			CurrentBonus = static_cast<int>(EBonusType::EMPTY);
			break;
	}
}

void ABumperKart::UpdatePistolVisibility()
{
	if (CurrentBonus == static_cast<int>(EBonusType::EMPTY))
	{
		PistolMesh->SetVisibility(false);
	}
	else if (CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_ONE) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_TWO) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_THREE))
	{
		PistolMesh->SetVisibility(true);
	}
	else
	{
		PistolMesh->SetVisibility(false);
	}
}

void ABumperKart::UpdateTerrainResistance()
{
	if (Circuit->GetWeatherConfig()->IsRaining())
	{
		TerrainResistanceK = RainTerrainResistanceK;
	}
	else
	{
		TerrainResistanceK = DefaultTerrainResistanceK;
	}

	OnUpdateRain.Broadcast();
}

void ABumperKart::ConstructSounds()
{
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundMotorHover(TEXT("SoundWave'/Game/Audio/S_KartHover.S_KartHover'"));
	MotorSoundWave = SoundMotorHover.Object;

	static ConstructorHelpers::FObjectFinder<USoundWave> SoundMotorStop(TEXT("SoundWave'/Game/Audio/S_KartKnockOver.S_KartKnockOver'"));
	MotorStopSoundWave = SoundMotorStop.Object;
}

void ABumperKart::InitSounds()
{
	if (MotorSoundWave)
	{
		MotorSound = UGameplayStatics::SpawnSoundAttached(MotorSoundWave, RootComponent);
	}
}

// Called to bind functionality to input
void ABumperKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent != nullptr)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &ABumperKart::AttemptToAccelerate);
		PlayerInputComponent->BindAxis("MoveRight", this, &ABumperKart::AttemptToTurn);
		PlayerInputComponent->BindAction("Handbrake", IE_Repeat, this, &ABumperKart::AttemptToDrift);
		PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ABumperKart::AttemptToStopDrift);

		PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &ABumperKart::AttemptToUseItem);
		PlayerInputComponent->BindAction("UseItemBehind", IE_Pressed, this, &ABumperKart::AttemptToUseItemBehind);
		PlayerInputComponent->BindAction("GoToResult", IE_Pressed, this, &ABumperKart::GoToResult);
	}
}

// Called every frame
void ABumperKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LerpTimer += DeltaTime;

	if (bDrifting)
	{
		DriftHold += DeltaTime;
	}
	
	LockRotation();
	ApplyNextPosition(DeltaTime);
	HoverGround();
	CameraMovement();
}

void ABumperKart::ApplyNextPosition(float DeltaTime)
{
	if (bObstacle)
		Throttle *= 0.2;

	if (bOnGround)
		MaxTilt = 360.f;
	else
		MaxTilt = 25.f;

	FVector Force = GetActorForwardVector() * MaxEngineForce * Throttle;
	const FVector NegNormalVel = -Velocity.GetSafeNormal();

	//Air resistance

	const FVector AirResistance = bOnGround ? NegNormalVel * Velocity.SizeSquared() * DragRate : NegNormalVel * Velocity.SizeSquared() * DragRate * 2;
	Force += AirResistance;

	//Gravity constraint
	float GravityAccelerationX = 0.f;
	float GravityAccelerationY = 0.f;
	float GravityAccelerationZ = GetWorld()->GetGravityZ();
	const float NormalForce = Mass * -GravityAccelerationZ / 100;

	//Rolling friction
	const FVector RollingResistance = NegNormalVel * NormalForce * TerrainResistanceK;
	Force += bDrifting ? RollingResistance * 1.5 : RollingResistance;
	if (!bOnGround)
	{
		Force.X = 0;
		Force.Y = 0;
	}
	//Determine acceleration vector
	const FVector Acceleration = Force / Mass;

	//Diff velocity quantity
	const FVector DeltaVelocity = Acceleration * DeltaTime;
	Velocity += DeltaVelocity;

	if (Velocity.Size() > 0.f)
	{
		if (!bOnGround)
		{
			Steering *= 0.5f;
		}
		//Diff movement rotation
		float RotationAngle = MaxTurningDegrees * Steering * DeltaTime;
		if (FVector::DotProduct(GetActorForwardVector(), Velocity) < 0)
			RotationAngle *= -1;

		const FQuat DeltaRotation(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
		AddActorWorldRotation(DeltaRotation);

		Velocity = DeltaRotation.RotateVector(Velocity);
	}

	if (Velocity.Size() > VelocityCap)
	{
		float Div = VelocityCap / Velocity.Size();
		Velocity *= Div;
	}

	if (bOnGround)
	{
		GravityAccelerationX = GroundNormal.X * GroundNormal.Z;
		GravityAccelerationY = GroundNormal.Y * GroundNormal.Z;
		GravityAccelerationZ *= GroundNormal.Z;
	}
	else
	{
		GravityAccelerationZ *= GravityRate;
	}
	//Diff movement location
	FVector DeltaLocation = Velocity * DeltaTime * 100.f;
	DeltaLocation.X += GravityAccelerationX * DeltaTime;
	DeltaLocation.Y += GravityAccelerationY * DeltaTime;
	DeltaLocation.Z += GravityAccelerationZ * DeltaTime;

	FHitResult HitRes;
	AddActorWorldOffset(DeltaLocation, true, &HitRes);

	if (HitRes.IsValidBlockingHit() && !bWallRiding)
	{
		if (HitRes.Actor != nullptr)
		{
			if (HitRes.ImpactPoint.Z < GetActorLocation().Z - Extent.Z)
			{
				return;
			}
		}

		BoxColl->AddImpulse(HitRes.Normal * BounceForce);
		Throttle = 0;
		LastFrameFOV = FollowCamera->FieldOfView;

		if (!CamTimer.IsValid())
			bFirstCall = true;

		GetWorldTimerManager().SetTimer(CamTimer, this, &ABumperKart::ResetCamera, DeltaTime, true, 0.f);
	}

	if (MotorSound && MotorSound->IsPlaying())
	{
		MotorSound->SetPitchMultiplier(UKismetMathLibrary::MapRangeClamped(Velocity.Size(), 0, 50, 0.5, 2.5));
	}
}

void ABumperKart::Accelerate(float AxisValue)
{
	if (bKnockedOver || bFinished || !bOnGround)
	{
		Throttle = 0.f;
	}
	else
	{
		Throttle = AxisValue;
	}
}

void ABumperKart::Decelerate()
{
	Velocity /= DragRate;
}

void ABumperKart::Turn(float AxisValue)
{
	if (bKnockedOver || bFinished)
		Steering = 0.f;
	else
		Steering = AxisValue;
}

void ABumperKart::Drift()
{
	if (!bOnGround)
		return;
	bDrifting = true;
	FVector Impulse = GetActorRightVector();
	Impulse *= -Steering;
	Impulse *= DriftRate;

	if (Steering > 0)
	{
		BoxColl->AddForceAtLocation(Impulse, BackRight->GetComponentLocation());
	}
	else
	{
		BoxColl->AddForceAtLocation(Impulse, BackLeft->GetComponentLocation());
	}

	OnUpdateDrift.Broadcast();
}

void ABumperKart::DriftStop()
{
	bDrifting = false;

	float VelSize = Velocity.Size();
	Velocity = GetActorForwardVector() * VelSize;

	if (DriftHold > 0.f)
	{
		if (DriftHold > MaxBoostTime)
		{
			DriftHold = MaxBoostTime;
		}
		bBoostDrifting = true;
		GetWorldTimerManager().SetTimer(BoostTimer, this, &ABumperKart::DriftBoost, GetWorld()->DeltaTimeSeconds, true, 0.f);
	}

	if (!BoostTimer.IsValid())
	{
		DriftHold = 0.f;
	}

	OnUpdateDrift.Broadcast();
}

void ABumperKart::DriftBoost()
{
	if (DriftHold <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(BoostTimer);
		DriftHold = 0.f;
		bBoostDrifting = false;
	}

	FVector Impulse = GetActorForwardVector();
	Impulse *= DriftRate;
	BoxColl->AddImpulse(Impulse);

	DriftHold -= GetWorld()->DeltaTimeSeconds;

	OnUpdateDrift.Broadcast();
}

void ABumperKart::HoverGround()
{
	FVector Center;
	FVector Loc = GetActorLocation();
	Center = Loc;
	Center.Z -= Extent.Z;

	float GravityForce = GetWorld()->GetGravityZ();
	FHitResult OutHit;
	FVector FrontL, FrontR, BackL, BackR;

	FrontL = FrontLeft->GetComponentLocation();
	FrontR = FrontRight->GetComponentLocation();
	BackL = BackLeft->GetComponentLocation();
	BackR = BackRight->GetComponentLocation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredComponent(BoxColl);

	const FVector Down = -GetActorUpVector();
	const FVector End = Down * (HoverDistance)+Center;
	const FVector EndFL = Down * HoverDistance + FrontL;
	const FVector EndFR = Down * HoverDistance + FrontR;
	const FVector EndBL = Down * HoverDistance + BackL;
	const FVector EndBR = Down * HoverDistance + BackR;

	if (GetWorld()->LineTraceSingleByChannel(CenterOutHit, Center, End, ECC_WorldStatic, CollisionParams))
	{
		if (CenterOutHit.Distance < HoverDistance)
		{
			//float delta = (HoverDistance)-OutHit.Distance;
			//SetActorLocation({ Loc.X, Loc.Y, Loc.Z + delta });
			bOnGround = true;
			if (GroundNormal != CenterOutHit.Normal)
			{
				GroundNormal = CenterOutHit.Normal;
				/*GroundRotation = UKismetMathLibrary::RInterpTo(
					GetActorRotation(),
					UKismetMathLibrary::MakeRotator(
						UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), GroundNormal).Roll,
						UKismetMathLibrary::MakeRotFromYZ(GetActorRightVector(), GroundNormal).Pitch,
						GetActorRotation().Yaw),
					1,
					1
				);*/
			}


			//DrawDebugLine(GetWorld(), GetActorLocation(), End, FColor::Red, false, 15.f, 0, 3.f);
			//UE_LOG(LogTemp, Warning, TEXT("PITCH: %f"), UKismetMathLibrary::FindLookAtRotation(Center, End).Pitch);
			if (UKismetMathLibrary::FindLookAtRotation(Center, End).Pitch >= -65.f)
			{
				bWallRiding = true;
			}
			else
			{
				bWallRiding = false;
			}
		}
	}
	else
	{
		bOnGround = false;
		bWallRiding = false;
	}

	if (GetWorld()->LineTraceSingleByChannel(OutHit, FrontL, EndFL, ECC_WorldStatic, CollisionParams))
	{
		bOnGround = true;
		if (OutHit.Distance < HoverDistance)
		{
			FVector SuspensionForce = OutHit.ImpactNormal * FMath::Lerp(SuspensionResistanceK, 0.f, OutHit.Distance / HoverDistance);

			BoxColl->AddForceAtLocation(SuspensionForce, FrontL);
		}
	}
	else if (!bWallRiding)
	{
		if (GetActorLocation().Y > FrontL.Y)
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * GravityForce, FrontL);
		}
		else
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * -GravityForce, FrontL);
		}
	}
	if (GetWorld()->LineTraceSingleByChannel(OutHit, FrontR, EndFR, ECC_WorldStatic, CollisionParams))
	{
		bOnGround = true;
		if (OutHit.Distance < HoverDistance)
		{
			FVector SuspensionForce = OutHit.ImpactNormal * FMath::Lerp(SuspensionResistanceK, 0.f, OutHit.Distance / HoverDistance);

			BoxColl->AddForceAtLocation(SuspensionForce, FrontR);
		}
	}
	else if (!bWallRiding)
	{
		if (GetActorLocation().Y > FrontR.Y)
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * GravityForce, FrontR);
		}
		else
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * -GravityForce, FrontR);
		}
	}
	if (GetWorld()->LineTraceSingleByChannel(OutHit, BackL, EndBL, ECC_WorldStatic, CollisionParams))
	{
		bOnGround = true;
		if (OutHit.Distance < HoverDistance)
		{
			FVector SuspensionForce = OutHit.ImpactNormal * FMath::Lerp(SuspensionResistanceK, 0.f, OutHit.Distance / HoverDistance);

			BoxColl->AddForceAtLocation(SuspensionForce, BackL);
		}
	}
	else if (!bWallRiding)
	{
		if (GetActorLocation().Y > BackL.Y)
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * GravityForce, BackL);
		}
		else
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * -GravityForce, BackL);
		}
	}
	if (GetWorld()->LineTraceSingleByChannel(OutHit, BackR, EndBR, ECC_WorldStatic, CollisionParams))
	{
		bOnGround = true;
		if (OutHit.Distance < HoverDistance)
		{
			FVector SuspensionForce = OutHit.ImpactNormal * FMath::Lerp(SuspensionResistanceK, 0.f, OutHit.Distance / HoverDistance);

			BoxColl->AddForceAtLocation(SuspensionForce, BackR);
		}
	}
	else if (!bWallRiding)
	{
		if (GetActorLocation().Y > BackR.Y)
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * GravityForce, BackR);
		}
		else
		{
			BoxColl->AddForceAtLocation(GetActorUpVector() * -GravityForce, BackR);
		}
	}
}

void ABumperKart::LockRotation()
{
	FRotator Rotation = GetActorRotation();
	if (!bWallRiding)
	{
		if (Rotation.Roll > MaxTilt)
			Rotation.Roll = MaxTilt;
		else if (Rotation.Roll < -MaxTilt)
			Rotation.Roll = -MaxTilt;
		if (Rotation.Pitch > MaxTilt)
			Rotation.Pitch = MaxTilt;
		else if (Rotation.Pitch < -MaxTilt)
			Rotation.Pitch = -MaxTilt;

		SetActorRotation(Rotation);
	}
}

void ABumperKart::CameraMovement()
{
	float Alpha = Velocity.Size() / MaxVelCam;

	if (Velocity.Size() > MaxVelCam)
		Alpha = 1.f;

	FollowCamera->FieldOfView = FMath::Lerp(MinFov, MaxFOV, Alpha);
}

void ABumperKart::ResetCamera()
{
	if (bFirstCall)
	{
		LerpTimer = 0;
		bFirstCall = false;
	}

	FollowCamera->FieldOfView = FMath::Lerp(LastFrameFOV, MinFov, LerpTimer / 1.5f);

	if (FollowCamera->FieldOfView >= MinFov)
	{
		GetWorldTimerManager().ClearTimer(CamTimer);
	}
}
//TODO: Delete Later (Used for ALPHA only)
void ABumperKart::GoToResult()
{
	TArray<AActor*> PlayersFound;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Kart"), PlayersFound);

	if (PlayersFound.Num() > 0)
	{
		Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->NumberOfPlayersInRace = PlayersFound.Num();
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("UIEndGameLevel"));
}
void ABumperKart::AttemptToAccelerate(const float AxisValue)
{
	if (GetLocalRole() == ROLE_AutonomousProxy && IsPlayerControlled())
	{
		ServerAccelerate(AxisValue);
	}
	else
	{
		Accelerate(AxisValue);
	}
}

void ABumperKart::AttemptToTurn(const float AxisValue)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerTurn(AxisValue);
	}
	else
	{
		Turn(AxisValue);
	}
}

void ABumperKart::AttemptToDrift()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerDrift();
	}
	else
	{
		Drift();
	}
}

void ABumperKart::AttemptToStopDrift()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerStopDrift();
	}
	else
	{
		DriftStop();
	}
}

void ABumperKart::AttemptToUseItem()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerUseItem();
	}
	else
	{
		UseItem();
	}
}

void ABumperKart::AttemptToUseItemBehind()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerUseItemBehind();
	}
	else
	{
		UseItemBehind();
	}
}

void ABumperKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABumperKart, MaxSpeed);
	DOREPLIFETIME(ABumperKart, SuspensionResistanceK);
	DOREPLIFETIME(ABumperKart, DragRate);
	DOREPLIFETIME(ABumperKart, TurnRate);
	DOREPLIFETIME(ABumperKart, HoverDistance);
	DOREPLIFETIME(ABumperKart, MaxTilt);
	DOREPLIFETIME(ABumperKart, bFinished);
	DOREPLIFETIME(ABumperKart, GravityRate);
	DOREPLIFETIME(ABumperKart, BounceForce);

	DOREPLIFETIME(ABumperKart, Velocity);
	DOREPLIFETIME(ABumperKart, Throttle);
	DOREPLIFETIME(ABumperKart, Steering);
	DOREPLIFETIME(ABumperKart, TerrainResistanceK);
	DOREPLIFETIME(ABumperKart, DriftRate);
}

void ABumperKart::ServerUseItem_Implementation()
{
	UseItem();
}

bool ABumperKart::ServerUseItem_Validate()
{
	return true;
}

void ABumperKart::ServerUseItemBehind_Implementation()
{
	UseItemBehind();
}

bool ABumperKart::ServerUseItemBehind_Validate()
{
	return true;
}

void ABumperKart::ServerAccelerate_Implementation(const float AxisValue)
{
	Accelerate(AxisValue);
}

bool ABumperKart::ServerAccelerate_Validate(const float AxisValue)
{
	return true;
}

void ABumperKart::ServerDrift_Implementation()
{
	Drift();
}

bool ABumperKart::ServerDrift_Validate()
{
	return true;
}

void ABumperKart::ServerStopDrift_Implementation()
{
	DriftStop();
}

bool ABumperKart::ServerStopDrift_Validate()
{
	return true;
}

void ABumperKart::ServerTurn_Implementation(const float AxisValue)
{
	Turn(AxisValue);
}

bool ABumperKart::ServerTurn_Validate(const float AxisValue)
{
	return true;
}

void ABumperKart::PickUpItem()
{
	if (CurrentBonus == static_cast<int>(EBonusType::EMPTY))
	{
		CurrentBonus = GetRandomBonus();

		OnUpdateItem.Broadcast();
	}
}

void ABumperKart::UseItem()
{
	if (CurrentBonus != static_cast<int>(EBonusType::EMPTY) && !bFinished && !bKnockedOver)
	{
		if (CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_ONE) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_TWO) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_THREE))
			Cast<UProjectileBonus>(UProjectileBonus::StaticClass()->GetDefaultObject())->ActivateBonus(this);
		else if (CurrentBonus == static_cast<int>(EBonusType::STATIC_TRAP))
			Cast<UStaticTrapBonus>(UStaticTrapBonus::StaticClass()->GetDefaultObject())->ActivateBonus(this);
		else if (CurrentBonus == static_cast<int>(EBonusType::REMOTE_CONTROLLED_CAR))
			Cast<URemoteControlledCarBonus>(URemoteControlledCarBonus::StaticClass()->GetDefaultObject())->ActivateBonus(this);

		DestroyCurrentBonus();
		OnUpdateItem.Broadcast();
		Circuit->GetPlayer(PlayerID)->PlayerStats.BonusUsed++;
	}
}

void ABumperKart::UseItemBehind()
{
	if (CurrentBonus != static_cast<int>(EBonusType::EMPTY) && !bFinished && !bKnockedOver)
	{
		if (CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_ONE) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_TWO) || CurrentBonus == static_cast<int>(EBonusType::PROJECTILE_THREE))
			Cast<UProjectileBonus>(UProjectileBonus::StaticClass()->GetDefaultObject())->ActivateBonusBehind(this);
		else if (CurrentBonus == static_cast<int>(EBonusType::STATIC_TRAP))
			Cast<UStaticTrapBonus>(UStaticTrapBonus::StaticClass()->GetDefaultObject())->ActivateBonusBehind(this);
		else if (CurrentBonus == static_cast<int>(EBonusType::REMOTE_CONTROLLED_CAR))
			Cast<URemoteControlledCarBonus>(URemoteControlledCarBonus::StaticClass()->GetDefaultObject())->ActivateBonusBehind(this);

		DestroyCurrentBonus();
		OnUpdateItem.Broadcast();
		Circuit->GetPlayer(PlayerID)->PlayerStats.BonusUsed++;
	}
}

void ABumperKart::KnockOver(const int KnockOverTime)
{
	GetWorldTimerManager().SetTimer(TimerHandle_KnockOver, this, &ABumperKart::KnockOverStop, KnockOverTime);

	bKnockedOver = true;

	const int DroppedCandies = UKismetMathLibrary::Min(NumberOfCandies, MaxCandiesDropped);
	CandyEmitter->EmitCandies(DroppedCandies);
	NumberOfCandies -= DroppedCandies;

	Circuit->GetPlayer(PlayerID)->PlayerStats.FellInTrap++;

	if (MotorSound && MotorSound->IsPlaying())
	{
		MotorSound->SetPaused(true);
	}

	if (MotorStopSoundWave)
	{
		MotorStopSound = UGameplayStatics::SpawnSoundAttached(MotorStopSoundWave, RootComponent);
	}
}

void ABumperKart::KnockOverStop()
{
	bKnockedOver = false;

	if (MotorSound)
	{
		MotorSound->SetPaused(false);
	}

}


void ABumperKart::AddCandy()
{
	if (NumberOfCandies < MaxCandies)
		NumberOfCandies++;

	Circuit->GetPlayer(PlayerID)->PlayerStats.TotalCandies++;

}

