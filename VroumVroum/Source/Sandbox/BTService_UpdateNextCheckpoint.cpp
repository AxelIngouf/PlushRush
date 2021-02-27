// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateNextCheckpoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BumperKart.h"
#include "Circuit.h"
#include "Checkpoint.h"

void UBTService_UpdateNextCheckpoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* MyController = OwnerComp.GetAIOwner();

    ABumperKart* ControlledKart = Cast<ABumperKart>(MyController->GetPawn());

    if(ControlledKart && ControlledKart->GetCircuit() && ControlledKart->GetPlayerID() >= 0 )
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), ControlledKart->GetCircuit()->GetPlayer(ControlledKart->GetPlayerID())->NextCheckpoint);
    }
}

void UBTService_UpdateNextCheckpoint::OnGameplayTaskActivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskActivated(Task);
}

void UBTService_UpdateNextCheckpoint::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
    Super::OnGameplayTaskDeactivated(Task);
}
