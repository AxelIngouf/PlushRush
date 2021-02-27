// Fill out your copyright notice in the Description page of Project Settings.


#include "BumperKartPawnMovementComponent.h"
#include "BumperKart.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

void UBumperKartPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ControlledKart = Cast<ABumperKart>(GetOwner());
}

void UBumperKartPawnMovementComponent::RequestDirectMove_Implementation(const FVector& MoveVelocity,
    bool bForceMaxSpeed)
{
	Super::RequestDirectMove(MoveVelocity, bForceMaxSpeed);

	const FVector VehicleLocation = GetOwner()->GetActorLocation();
	const FVector Destination = VehicleLocation + MoveVelocity * GetWorld()->GetDeltaSeconds();
	//DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), Destination, FColor::Red, false, 1.f, 0, 3.f);

	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(VehicleLocation, Destination);

	const float YawAngle = abs(GetOwner()->GetActorRotation().Yaw - TargetRotation.Yaw);

	if (YawAngle > 180 && GetOwner()->GetActorRotation().Yaw > TargetRotation.Yaw ||
		YawAngle < 180 && GetOwner()->GetActorRotation().Yaw < TargetRotation.Yaw)
	{
		ControlledKart->AttemptToTurn(1.f);
	}
	else
	{
		ControlledKart->AttemptToTurn(-1.f);
	}

	if (YawAngle > 10)
	{
		ControlledKart->AttemptToAccelerate(0.5f);
	}
	else
	{
		ControlledKart->AttemptToAccelerate(1.f);
	}
}
