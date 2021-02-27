#include "RCCarAIController.h"


#include "CandyPinata.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Circuit.h"
#include "Checkpoint.h"
#include "BumperKart.h"

void ARCCarAIController::BeginPlay()
{
    Super::BeginPlay();

    if (!RunBehaviorTree(CarBehavior))
    {
        UE_LOG(LogTemp, Warning, TEXT("Car Behavior could not be run."));
        return;
    }

    ACircuit* Circuit = Cast<ACircuit>(UGameplayStatics::GetActorOfClass(this, ACircuit::StaticClass()));

    if (!Circuit)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Circuit was found on the map."));
        return;
    }

    // Get the first player still in the race.
    FCircuitPlayer* FirstPlayer = Circuit->GetFirstPlayerInRace();

    if (FirstPlayer && FirstPlayer->Actor != nullptr)
    {
        //GetBlackboardComponent()->SetValueAsObject(TargetPlayerKey, FirstPlayer->Actor);
        //GetBlackboardComponent()->SetValueAsObject(TargetCheckpointKey, FirstPlayer->NextCheckpoint);
    }
}