// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BumperKartAIController.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API ABumperKartAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, Category = Controller)
		class UBehaviorTree* CarBehavior = nullptr;

	UPROPERTY(EditAnywhere, Category = Controller)
		FName	TargetCheckpointKey = "TargetCheckpoint";
};
