#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RCCarAIController.generated.h"

/**
 *
 */
UCLASS()
class SANDBOX_API ARCCarAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = Controller)
	class UBehaviorTree* CarBehavior = nullptr;

	UPROPERTY(EditAnywhere, Category = Controller)
	FName	TargetPlayerKey = "TargetActor";

	UPROPERTY(EditAnywhere, Category = Controller)
	FName	TargetCheckpointKey = "TargetCheckpoint";
};
