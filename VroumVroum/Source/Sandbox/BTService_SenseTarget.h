// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SenseTarget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UBTService_SenseTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
};
