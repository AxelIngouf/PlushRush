// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_UseItem.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UBTTask_UseItem final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}

    void	UseProjectile() const;
    void	UseStaticTrap() const;
    void	UseRemoteControlledCar() const;

	class ABumperKart* Kart = nullptr;
	
};
