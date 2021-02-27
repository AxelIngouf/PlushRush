#include "BTTask_Explode.h"

#include "Bonus/RemoteControlledCar.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_Explode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ARemoteControlledCar* Car = Cast<ARemoteControlledCar>(OwnerComp.GetAIOwner()->GetPawn());

    if (Car)
    {
        Car->Explode();
    }

    return EBTNodeResult::Succeeded;
}