// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBonus.h"
#include "Projectile.h"
#include "Sandbox/BumperKart.h"
#include "Kismet/KismetMathLibrary.h"

UProjectileBonus::UProjectileBonus()
{
    BonusType = EBonusType::PROJECTILE_ONE;

    ProjectileType = AProjectile::StaticClass();
}

void UProjectileBonus::ActivateBonus(AActor* Owner)
{
    const FVector OwnerPosition = Owner->GetActorLocation();
    const FVector OwnerDirection = Owner->GetActorForwardVector();

    const FVector ProjectileSpawnPosition(OwnerPosition.X + OwnerDirection.X * 250,
                                          OwnerPosition.Y + OwnerDirection.Y * 250, OwnerPosition.Z);


    SpawnProjectile(ProjectileSpawnPosition, Owner->GetActorRotation(), Owner);
}

void UProjectileBonus::ActivateBonusBehind(AActor* Owner)
{
    const FVector OwnerPosition = Owner->GetActorLocation();
    const FVector OwnerInverseDirection = Owner->GetActorForwardVector() * -1;

    const FVector ProjectileSpawnPosition(OwnerPosition.X + OwnerInverseDirection.X * 200,
        OwnerPosition.Y + OwnerInverseDirection.Y * 200, OwnerPosition.Z);

    FRotator ProjectileSpawnRotation(Owner->GetActorRotation());
    ProjectileSpawnRotation.Yaw -= 180;

    SpawnProjectile(ProjectileSpawnPosition, ProjectileSpawnRotation, Owner);
}

void UProjectileBonus::SpawnProjectile(const FVector Location, const FRotator Rotation, AActor* Owner) const
{
    const FTransform TrapTransform = UKismetMathLibrary::MakeTransform(Location, Rotation, FVector::OneVector);

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParameters.Owner = Owner;

    AProjectile* SpawnedTrap = Owner->GetWorld()->SpawnActor<AProjectile>(ProjectileType, TrapTransform, SpawnParameters);
    SpawnedTrap->SetEmitter(Cast<ABumperKart>(Owner));
}
