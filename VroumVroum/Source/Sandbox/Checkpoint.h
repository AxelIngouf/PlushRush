// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Checkpoint.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API ACheckpoint final : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ACheckpoint() = default;

	virtual void BeginPlay() override;

	UFUNCTION()
	void	BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	int		GetID() const { return CheckpointID; }

protected:
	UPROPERTY(EditAnywhere, Category=Checkpoint)
	int	CheckpointID;

	class ACircuit* Circuit;
};
