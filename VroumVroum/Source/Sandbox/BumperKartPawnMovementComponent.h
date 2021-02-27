// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BumperKartPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UBumperKartPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	UFUNCTION(Server, Reliable)
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

	class ABumperKart* ControlledKart = nullptr;
};
