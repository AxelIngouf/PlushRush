#include "RemoteControlledCar.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sandbox/BumperKart.h"
#include "Sandbox/Checkpoint.h"
#include "Sandbox/Circuit.h"

#define REFRESH_TRANSFORM_INTERVAL 0.03f

ARemoteControlledCar::ARemoteControlledCar()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetCapsuleComponent());

	SetReplicates(true);
}

void ARemoteControlledCar::Explode()
{
	TArray<AActor*> OverlappedActors;

	this->GetOverlappingActors(OverlappedActors);

	for(AActor* Actor : OverlappedActors)
	{
		ABumperKart* BumperKart = Cast<ABumperKart>(Actor);

		if(BumperKart)
		{
			BumperKart->KnockOver(KnockOverPlayerTime);

			if(CarEmitter != nullptr && BumperKart->GetPlayerID() != CarEmitter->GetPlayerID())
			{
				BumperKart->GetCircuit()->GetPlayer(CarEmitter->GetPlayerID())->PlayerStats.PlayerTrappedWithRCCar++;
			}
		}
	}

	Destroy();
}

void ARemoteControlledCar::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_TransformRefresh, this, &ARemoteControlledCar::RefreshTransform,
                                    REFRESH_TRANSFORM_INTERVAL, true);

	Circuit = Cast<ACircuit>(UGameplayStatics::GetActorOfClass(this, ACircuit::StaticClass()));

	if(Circuit && Circuit->GetNumberOfCheckpoints() > 0)
	{
		NextCheckpoint = Circuit->GetCheckpointAt(0);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARemoteControlledCar::BeginOverlap);
}

void ARemoteControlledCar::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACheckpoint* Checkpoint = Cast<ACheckpoint>(OtherActor);

	if(Checkpoint && Checkpoint->GetID() == NextCheckpoint->GetID())
	{
		NextCheckpoint = Circuit->GetCheckpointAt((NextCheckpoint->GetID() + 1) % Circuit->GetNumberOfCheckpoints());
	}
}

FCircuitPlayer* ARemoteControlledCar::GetTarget() const
{
	return Circuit->GetFirstPlayerInRace();
}

void ARemoteControlledCar::SetNextCheckpoint(ACheckpoint* Checkpoint)
{
	NextCheckpoint = Checkpoint;
}

void ARemoteControlledCar::SetEmitter(ABumperKart* Emitter)
{
	CarEmitter = Emitter;
}

void ARemoteControlledCar::RefreshTransform()
{
	SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 35, REFRESH_TRANSFORM_INTERVAL, 500));
}

