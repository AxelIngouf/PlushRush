#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Explode.generated.h"

/**
 *
 */
UCLASS()
class SANDBOX_API UBTTask_Explode : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}

};
