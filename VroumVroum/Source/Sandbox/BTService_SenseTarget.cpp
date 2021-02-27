// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SenseTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Bonus/RemoteControlledCar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Checkpoint.h"
#include "Circuit.h"
#include "BumperKart.h"

#define DISTANCE_TARGET_SIGHT 2000

void UBTService_SenseTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MyController = OwnerComp.GetAIOwner();

    ARemoteControlledCar* ControlledCar = Cast<ARemoteControlledCar>(MyController->GetPawn());

    if (!ControlledCar)
        return;

    FCircuitPlayer* Target = ControlledCar->GetTarget();

    if(Target != nullptr && Target->NextCheckpoint != nullptr && ControlledCar->GetNextCheckpoint() != nullptr)
    {
        if(Target->NextCheckpoint->GetID() == ControlledCar->GetNextCheckpoint()->GetID())
        {
            // Target Actor is right next to the remote car
            OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), Target->Actor);
        }
        else if (Target->NextCheckpoint->GetID()+1 == ControlledCar->GetNextCheckpoint()->GetID() &&
            UKismetMathLibrary::Vector_Distance(ControlledCar->GetActorLocation(), Target->Actor->GetActorLocation()) < DISTANCE_TARGET_SIGHT)
        {
            // Target Actor is right behind the remote car, so no need to cross the whole race
            OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), Target->Actor);
        }
        else 
        {
            // Target Actor unattainable, the remote car needs to continue along the race
            OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
        }
    }
}

void UBTService_SenseTarget::OnGameplayTaskActivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskActivated(Task);
    // Won't compile if this function is not override
}

void UBTService_SenseTarget::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskDeactivated(Task);
    // Won't compile if this function is not override
}
