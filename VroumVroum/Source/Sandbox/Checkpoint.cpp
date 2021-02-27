#include "Checkpoint.h"

#include "Kismet/GameplayStatics.h"
#include <Sandbox/Circuit.h>

#include "BumperKart.h"

void ACheckpoint::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::BeginOverlap);


    Circuit = Cast<ACircuit>(UGameplayStatics::GetActorOfClass(this, ACircuit::StaticClass()));
}

void ACheckpoint::BeginOverlap(AActor*, AActor* OtherActor)
{
    ABumperKart* BumperKart = Cast<ABumperKart>(OtherActor);
    if(Circuit && BumperKart && HasAuthority())
    {
        Circuit->ServerPlayerReachedCheckpoint(BumperKart->GetPlayerID(), CheckpointID);
    }
}
