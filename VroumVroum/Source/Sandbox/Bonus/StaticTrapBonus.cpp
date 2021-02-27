// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticTrapBonus.h"
#include "StaticTrap.h"
#include "Sandbox/BumperKart.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

UStaticTrapBonus::UStaticTrapBonus()
{
    BonusType = EBonusType::STATIC_TRAP;

    ProjectileType = AStaticTrap::StaticClass();
}

void UStaticTrapBonus::ActivateBonus(AActor* Owner)
{
    const FVector OwnerPosition = Owner->GetActorLocation();
    const FVector OwnerDirection = Owner->GetActorForwardVector();

    const FVector ProjectileSpawnPosition(OwnerPosition.X + OwnerDirection.X * 250,
                                          OwnerPosition.Y + OwnerDirection.Y * 250, OwnerPosition.Z + 20);

    FRotator ProjectileSpawnRotation = Owner->GetActorRotation();
    ProjectileSpawnRotation.Pitch += 25;

    AStaticTrap* SpawnedTrap = Owner->GetWorld()->SpawnActor<AStaticTrap>(ProjectileType, ProjectileSpawnPosition, ProjectileSpawnRotation);
    SpawnedTrap->SetEmitter(Cast<ABumperKart>(Owner));
}

void UStaticTrapBonus::ActivateBonusBehind(AActor* Owner)
{
    const FVector OwnerPosition = Owner->GetActorLocation();
    const FVector OwnerInverseDirection = Owner->GetActorForwardVector() * -1;

    const FVector ProjectileSpawnPosition(OwnerPosition.X + OwnerInverseDirection.X * 200,
        OwnerPosition.Y + OwnerInverseDirection.Y * 200, OwnerPosition.Z);

    const FTransform TrapTransform = UKismetMathLibrary::MakeTransform(ProjectileSpawnPosition,
                                                                        Owner->GetActorRotation(),
                                                                       FVector::OneVector);

    AStaticTrap* SpawnedTrap = Owner->GetWorld()->SpawnActorDeferred<AStaticTrap>(ProjectileType, TrapTransform);

    SpawnedTrap->ProjectileMovement->InitialSpeed = 0;
    SpawnedTrap->SetEmitter(Cast<ABumperKart>(Owner));

    SpawnedTrap->FinishSpawning(TrapTransform);
}
