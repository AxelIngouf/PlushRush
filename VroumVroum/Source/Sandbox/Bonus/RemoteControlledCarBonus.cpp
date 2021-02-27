// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteControlledCarBonus.h"
#include "RemoteControlledCar.h"
#include "Sandbox/BumperKart.h"
#include "../Circuit.h"

URemoteControlledCarBonus::URemoteControlledCarBonus()
{
    BonusType = EBonusType::REMOTE_CONTROLLED_CAR;
}

void URemoteControlledCarBonus::ActivateBonus(AActor* Owner)
{
    const FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 250;

    ABumperKart* BumperKart = Cast<ABumperKart>(Owner);

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParameters.Owner = Owner;

    ARemoteControlledCar* RemoteControlledCar = Owner->GetWorld()->SpawnActor<ARemoteControlledCar>(BumperKart->GetRCCarStaticClass(), SpawnLocation, Owner->GetActorRotation(), SpawnParameters);
    RemoteControlledCar->SetNextCheckpoint(BumperKart->GetCircuit()->GetPlayer(BumperKart->GetPlayerID())->NextCheckpoint);
    RemoteControlledCar->SetEmitter(BumperKart);
}


