// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateRCCarCheckpoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Checkpoint.h"
#include "Bonus/RemoteControlledCar.h"

void UBTService_UpdateRCCarCheckpoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MyController = OwnerComp.GetAIOwner();

    ARemoteControlledCar* ControlledCar = Cast<ARemoteControlledCar>(MyController->GetPawn());

    if (ControlledCar)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), ControlledCar->GetNextCheckpoint());
    }
}

void UBTService_UpdateRCCarCheckpoint::OnGameplayTaskActivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskActivated(Task);
}

void UBTService_UpdateRCCarCheckpoint::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskDeactivated(Task);
}